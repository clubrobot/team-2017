#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import zipfile
import xml.etree.ElementTree
import re

import math


class GeoGebra:
	
	class Point(tuple):   pass
	class Line(tuple):    pass
	class Circle(tuple):  pass
	class Ellipse(tuple): pass
	class Segment(tuple): pass
	class Vector(tuple):  pass
	class PolyLine(list): pass
	class Polygon(list):  pass
	class Angle(float):   pass
	class Numeric(float): pass
	
	def __init__(self, file):
		self.zipfile = zipfile.ZipFile(file)
		with self.zipfile.open('geogebra.xml') as xmlfile:
			self.root = xml.etree.ElementTree.parse(xmlfile).getroot()

	def __enter__(self):
		return self

	def __exit__(self, exc_type, exc_value, traceback):
		self.close()

	def close(self):
		self.zipfile.close()

	def get(self, label):
		element = self.root.find("./construction/element[@label='{}']".format(label))
		if element is None:
			raise KeyError(label)
		if element.attrib['type'] == 'point':
			return self._parse_point(element)
		elif element.attrib['type'] == 'line':
			return self._parse_line(element)
		elif element.attrib['type'] == 'conic':
			return self._parse_conic(element)
		elif element.attrib['type'] == 'segment':
			return self._parse_segment(element)
		elif element.attrib['type'] == 'vector':
			return self._parse_vector(element)
		elif element.attrib['type'] == 'polyline':
			return self._parse_polyline(element)
		elif element.attrib['type'] == 'polygon':
			return self._parse_polygon(element)
		elif element.attrib['type'] == 'angle':
			return self._parse_angle(element)
		elif element.attrib['type'] == 'numeric':
			return self._parse_numeric(element)
		else:
			raise NotImplementedError("'{}' elements currently not handled".format(element.attrib['type']))
	
	def getall(self, pattern):
		elements = self.root.findall('./construction/element[@label]')
		all_labels = [element.get('label') for element in elements]
		labels = [label for label in all_labels if re.match(pattern, label)]
		return [self.get(label) for label in labels]

	def _parse_point(self, element):
		coords = element.find('coords')
		x = float(coords.get('x'))
		y = float(coords.get('y'))
		return GeoGebra.Point((x, y))

	def _parse_line(self, element):
		coords = element.find('coords')
		a = float(coords.get('x'))
		b = float(coords.get('y'))
		c = float(coords.get('z'))
		try:
			x, y = -c/a, 0
		except ZeroDivisionError:
			x, y = 0, -c/b
		return GeoGebra.Line((x, y, math.pi - math.atan2(a, b)))

	def _parse_conic(self, element):
		matrix = element.find('matrix').attrib
		A =  float(matrix.get('A0'))
		C =  float(matrix.get('A1'))
		F = -float(matrix.get('A2'))
		B = 2 * float(matrix.get('A3'))
		D = 2 * float(matrix.get('A4'))
		E = 2 * float(matrix.get('A5'))
		if A*C - (B/2)**2 < 0:
			raise ValueError('not an ellipse')
		if A == C and B == 0:
			xc = -D / (2*A)
			yc = -E / (2*C)
			radius = math.sqrt(xc**2 + yc**2 + F)
			return GeoGebra.Circle((xc, yc, radius))
		else:
			raise NotImplementedError('ellipses currently not handled')

	def _parse_segment(self, element):
		label = element.attrib['label']
		command = self.root.find("./construction/command[@name='Segment']/output[@a0='{}']/..".format(label))
		if command is not None:
			input = command.find('input')
			a0 = self.get(input.get('a0'))
			a1 = self.get(input.get('a1'))
			return GeoGebra.Segment([a0, a1])
		for command in self.root.findall("./construction/command[@name='Polygon']"):
			if not label in command.find('output').attrib.values():
				continue
			output = command.find('output')
			polygon = self.get(output.get('a0'))
			i = [output.get(key) for key in sorted(output.attrib.keys())].index(label) - 1
			return GeoGebra.Segment((polygon[i], polygon[(i+1) % len(polygon)]))
			
		raise ValueError("inexistant 'Segment' command")

	def _parse_vector(self, element):
		label = element.attrib['label']
		command = self.root.find("./construction/command[@name='Vector']/output[@a0='{}']/..".format(label))
		if command is None:
			raise ValueError("inexistant 'Vector' command")
		input = command.find('input')
		a0 = self.get(input.get('a0'))
		a1 = self.get(input.get('a1'))
		return GeoGebra.Vector([a0, a1])
	
	def _parse_polyline(self, element):
		label = element.attrib['label']
		command = self.root.find("./construction/command[@name='PolyLine']/output[@a0='{}']/..".format(label))
		if command is None:
			raise ValueError("inexistant 'PolyLine' command")
		input = command.find('input')
		labels = [input.get(key) for key in sorted(input.attrib.keys())]
		return GeoGebra.PolyLine([self.get(label) for label in labels])

	def _parse_polygon(self, element):
		label = element.attrib['label']
		command = self.root.find("./construction/command[@name='Polygon']/output[@a0='{}']/..".format(label))
		if command is None:
			raise ValueError("inexistant 'Polygon' command")
		input = command.find('input')
		try:
			numvertices = int(input.get('a2'))
			output = command.find('output')
			labels = [input.get('a0'), input.get('a1')] + [output.get(key) for key in sorted(output.attrib.keys())[2 - numvertices:]]
		except (TypeError, ValueError):
			labels = [input.get(key) for key in sorted(input.attrib.keys())]
		return GeoGebra.Polygon([self.get(label) for label in labels])
	
	def _parse_angle(self, element):
		value = element.find('value')
		val = float(value.get('val'))
		return GeoGebra.Angle(val)

	def _parse_numeric(self, element):
		value = element.find('value')
		val = float(value.get('val'))
		return GeoGebra.Numeric(val)

