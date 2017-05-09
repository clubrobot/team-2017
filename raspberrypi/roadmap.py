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
			x0, y0 = self.graph.vs[edge.source]['coords']
			x1, y1 = self.graph.vs[edge.target]['coords']
			edge['weight'] = math.hypot(x1 - x0, y1 - y0)

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
