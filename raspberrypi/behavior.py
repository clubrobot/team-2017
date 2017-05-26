#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import time
from threading import Thread, Event, current_thread

from components import Manager


class AccessDenied(Exception): pass

class Behavior(Manager):

	def __init__(self, *args, timelimit=None, **kwargs):
		Manager.__init__(self, *args, **kwargs)
		self.timelimit = timelimit
		self.whitelist = set()
		self.blacklist = set()
		self.outputs = dict()
		self.stop_event = Event()

	def perform(self, procedure, args=(), kwargs={}, timelimit=True):
		thread = Thread(args=args, kwargs=kwargs, daemon=True)
		thread_id = id(thread)
		def target(*args, **kwargs):
			try:
				output = procedure(*args, **kwargs)
				self.outputs[thread_id] = output
			except AccessDenied:
				self.outputs[thread_id] = None
			except:
				self.outputs[thread_id] = None
				raise
			finally:
				if thread_id in self.blacklist: self.blacklist.remove(thread_id)
				if thread_id in self.whitelist: self.whitelist.remove(thread_id)
		if not timelimit: self.whitelist.add(thread_id)
		thread._target = target
		thread.start()
		return thread

	def interrupt(self, thread):
		if thread.is_alive():
			thread_id = id(thread)
			self.blacklist.add(thread_id)

	def get(self, thread, timeout=None):
		thread.join(timeout=timeout)
		if thread.is_alive():
			raise TimeoutError('timeout exceeded')
		thread_id = id(thread)
		output = self.outputs[thread_id]
		del self.outputs[thread_id]
		return output

	def send(self, *args, **kwargs):
		thread_id = id(current_thread())
		denyaccess = thread_id in self.blacklist
		if not thread_id in self.whitelist:
			if self.timelimit is not None:
				denyaccess |= (self.get_elapsed_time() > self.timelimit)
			denyaccess |= self.stop_event.is_set()
		if denyaccess:
			raise AccessDenied(thread_id)
		else:
			return Manager.send(self, *args, **kwargs)

	def make_decision(self):
		raise RuntimeError("the 'make_decision' method must be overriden")

	def goto_procedure(self, destination):
		raise RuntimeError("the 'goto_procedure' method must be overriden")

	def start_procedure(self):
		pass

	def stop_procedure(self):
		pass

	def start(self):
		self.starttime = time.monotonic()
		self.stop_event.clear()
		self.log('start')
		try:
			start = self.perform(self.start_procedure, timelimit=False)
			while (self.timelimit is None or self.get_elapsed_time() < self.timelimit) and not self.stop_event.is_set():
				decision = self.make_decision()
				procedure, args, kwargs, (location, thresholds) = decision
				if procedure is None:
					self.log('no decision')
					time.sleep(1)
					continue
				if location is not None:
					if location[2] is not None:
						self.log('goto: ({0[0]:.0f}, {0[1]:.0f}, {0[2]:.2f})'.format(location))
					else:
						self.log('goto: ({0[0]:.0f}, {0[1]:.0f})'.format(location))
					goto = self.perform(self.goto_procedure, args=(location, thresholds))
					success = self.get(goto)
				else:
					success = True
				if success:
					action = self.perform(procedure, args=args, kwargs=kwargs)
					self.get(action)
				else:
					self.log('goto failed')
		except:
			self.whitelist.clear()
			raise
		finally:
			self.stop()
			self.get(start)
			self.whitelist.add(id(current_thread()))

	def stop(self):
		self.log('stop')
		self.perform(self.stop_procedure, timelimit=False)
		self.stop_event.set()

	def get_elapsed_time(self):
		if hasattr(self, 'starttime'):
			return time.monotonic() - self.starttime
		else:
			return 0

	def log(self, message):
		pass#print('[{:>8.2f}] {}'.format(self.get_elapsed_time(), message))
