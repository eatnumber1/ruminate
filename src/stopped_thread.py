from lldb import *
from lldb_utils import *

import threading

class StoppedThreadFactory(object):
	def __init__(self, em, process):
		self.em
		self.process

	def produce(tid):
		return StoppedThread(self.em, self.process, tid)

	def stop(tid):
		validate(self.process.Stop())
		return StoppedThread(self.em, self.process, tid)

class StoppedThread(object):
	def __init__(self, em, process, tid):
		self.tid = tid
		self.em = em
		self.process = process

	def __enter__(self):
		thread = validate(self.process.GetThreadByID(self.tid))
		stoppedAtBreakpoint = threading.Event()
		def onStopped(event):
			if thread.stop_reason == eStopReasonBreakpoint:
				stoppedAtBreakpoint.set()
				self.em.removeCallback(lldb.eStateStopped, onStopped)
		self.em.addCallback(lldb.eStateStopped, onStopped)
		stoppedAtBreakpoint.wait()
		return thread

	def __exit__(self, *args):
		self.process.Continue()
		return False
