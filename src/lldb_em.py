from __future__ import print_function, absolute_import

import threading, sys
import lldb

from lldb_utils import validate, getDescription

class LLDBEventMachine(threading.Thread):
	def __init__(self, procgen):
		super(LLDBEventMachine, self).__init__()
		self.begun = False
		#self.daemon = True
		self.shutdownRequested = False

		self.listener = validate(lldb.SBListener("LLDBEventMachine"))
		self.procgen = procgen

		self.callbacks = {}
		for state in [
			lldb.eStateAttaching,
			lldb.eStateConnected,
			lldb.eStateCrashed,
			lldb.eStateDetached,
			lldb.eStateExited,
			lldb.eStateInvalid,
			lldb.eStateLaunching,
			lldb.eStateRunning,
			lldb.eStateStepping,
			lldb.eStateStopped,
			lldb.eStateSuspended,
			lldb.eStateUnloaded
		]:
			self.callbacks[state] = []

	def begin(self):
		self.process = validate(self.procgen(self.listener))
		self.procgen = None
		self.begun = True

		self.start()

	def shutdown(self):
		self.shutdownRequested = True
		if threading.currentThread() != self:
			self.join()

	def getProcess(self):
		return self.process

	def run(self):
		while not self.shutdownRequested:
			event = lldb.SBEvent()
			#validate(self.listener.WaitForEvent(2**32-1, event))
			if not self.listener.WaitForEvent(1, event):
				continue
			validate(event)
			print("Got event " + getDescription(event))
			self.onEvent(event)

	def onEvent(self, event):
		state = lldb.SBProcess.GetStateFromEvent(event)
		callbacks = self.callbacks[state]
		if callbacks:
			for cb in callbacks:
				cb(event)
		elif state == lldb.eStateStopped:
			# TODO: This is a hack
			self.process.Continue()

	def addCallback(self, state, cb):
		self.callbacks[state].append(cb)

	def removeCallback(self, state, cb):
		self.callbacks[state].remove(cb)
