#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
from threading import Thread, current_thread

from components import Manager


class AccessDenied(RuntimeError): pass

class Behavior(Manager):

	def __init__(self, *args, **kwargs):
		Manager.__init__(self, *args, timelimit=None, **kwargs)
		self.timelimit = timelimit
		self.whitelist = set()
		self.blacklist = set()

	def send(self, *args, **kwargs):
		thread_id = id(current_thread())
		elapsed_time = self.get_elapsed_time()
		if thread_id in self.blacklist or (self.timelimit and elapsed_time > self.timelimit and thread_id not in self.whitelist):
			self.blacklist.remove(thread)
			raise AccessDenied()
		else:
			Manager.send(self, *args, **kwargs)

	def start(self):
		self.starttime = time.monotonic()
		# TODO: start actions

	def get_elapsed_time(self):
		return time.monotonic() - self.starttime

	def deny_access(self, threadname):
		self.blacklist.add(threadname)


class Movement(Thread):

	def __init__(self, behavior):
		Thread.__init__(self)
		self.dameon = True
		self.behavior = behavior
	
	def run(self):
		try:
			self.perform()
		except AccessDenied: pass

	def perform(self):
		raise RuntimeError("the 'perform' method must be overriden")

	def interrupt(self, join=False):
		if self.is_alive:
			thread_id = id(self)
			self.behavior.deny_access(thread_id)
			if join:
				self.join()


class Action():
	pass # TODO:
