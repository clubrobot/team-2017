#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import igraph
from geogebra import GeoGebra

import math


class RoadMap:

	def __init__(self, vertices=list(), edges=set()):
		self.graph = igraph.Graph()
		self.graph.add_vertices(len(vertices))
		self.graph.vs['coords'] = vertices
		self.graph.add_edges(edges)
		self.reset_edges()

	def reset_edges(self):
		for edge in self.graph.es:
			vertex = self.graph.vs[edge.target]['coords']
			edge['weight'] = self.get_vertex_distance(edge.source, vertex)

	def cut_edges(self, cutline):
		(xB, yB), (xB2, yB2) = cutline
		dxB, dyB = xB2 - xB, yB2 - yB
		for edge in self.graph.es:
			xA,  yA  = self.graph.vs[edge.source]['coords']
			xA2, yA2 = self.graph.vs[edge.target]['coords']
			dxA, dyA = xA2 - xA, yA2 - yA
			det = dyA * dxB - dxA * dyB
			if det != 0:
				tA = (-dyB * (xB - xA) + dxB * (yB - yA)) / det
				tB = (-dyA * (xB - xA) + dxA * (yB - yA)) / det
				if tA > 0 and tA < 1 and tB > 0 and tB < 1:
					edge['weight'] = math.inf

	def get_vertex_distance(self, vid, vertex):
		x0, y0 = self.graph.vs[vid]['coords']
		x1, y1 = vertex
		return math.hypot(x1 - x0, y1 - y0)

	def get_closest_vertex(self, vertex):
		def key(v): return self.get_vertex_distance(v.index, vertex)
		return min(self.graph.vs, key=key).index

	def add_vertex(self, vertex):
		closest = self.get_closest_vertex(vertex)
		self.graph.add_vertices(1)
		vid = self.graph.vcount() - 1
		self.graph.vs[vid]['coords'] = vertex
		for target in [closest] + self.graph.neighbors(closest):
			self.graph.add_edge(vid, target, weight=self.get_vertex_distance(target, vertex))
		return vid

	def get_shortest_path(self, source, target):
		v = self.add_vertex(source)
		to = self.add_vertex(target)
		indexes = [v]
		for eid in self.graph.get_shortest_paths(v, to, weights='weight', output='epath')[0]:
			edge = self.graph.es[eid]
			if edge['weight'] == math.inf:
				raise RuntimeError('path not found')
			if indexes[-1] != edge.source:
				indexes.append(edge.source)
			else:
				indexes.append(edge.target)
		path = [self.graph.vs[vid]['coords'] for vid in indexes]
		self.graph.delete_vertices([v, to])
		return path

	@staticmethod # Factory function
	def load(geogebra, pattern='roadmap_{\s*\d+\s*,\s*\d+\s*}'):
		# `geogebra` may be a GeoGebra file or a .ggb filename
		if isinstance(geogebra, str):
			geogebra = GeoGebra(geogebra)
			geogebra.close()
		
		# Parse the geogebra file to get a list of segments which match the `pattern` regular expression
		segments = geogebra.getall(pattern) # Takes a while

		# Deduce the graph vertices from the previous list of segmenets
		vertices = set()
		for segment in segments:
			vertices.add(segment[0])
			vertices.add(segment[1])
		
		# Convert the above set in a list so that vertices are now indexed
		vertices = list(vertices)

		# Compute the graph edges using the above vertices list
		edges = set()
		for segment in segments:
			i = vertices.index(segment[0])
			j = vertices.index(segment[1])
			edges.add((min(i, j), max(i, j)))
		
		# Give the hand to the RoadMap constructor
		roadmap = RoadMap(vertices, edges)
		return roadmap
