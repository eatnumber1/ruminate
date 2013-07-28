from __future__ import print_function, absolute_import, nested_scopes, with_statement

from lldb import *

from Ruminate import *
from lldb_utils import *
from type_impl import *
from lldb_em import *

import threading

import sys # For debugging only

class DebuggerImpl(Debugger):
	def __init__(self, options, shutdown):
		self.debugger = validate(SBDebugger.Create())
		self.debugger.SetAsync(True)

		#self.debugger.SetLoggingCallback(lambda msg: sys.stdout.write(msg))
		#self.debugger.EnableLog("lldb", ["all"])

		self.target = validate(self.debugger.CreateTarget(options.exename))
		self.breakpoint = validate(self.target.BreakpointCreateByAddress(options.breakpointAddress))

		def procgen(listener):
			err = SBError()
			self.process = validate(self.target.AttachToProcessWithID(
				listener,
				options.pid,
				err
			))
			validate(err)
			return self.process

		self.em = LLDBEventMachine(procgen)

		running = threading.Event()
		def onRunning(event):
			print("onRunning")
			self.em.removeCallback(lldb.eStateRunning, onRunning)
			running.set()
		self.em.addCallback(lldb.eStateRunning, onRunning)

		def onStopped(event):
			print("onStopped")
			self.em.removeCallback(lldb.eStateStopped, onStopped)

			process = SBProcess.GetProcessFromEvent(event)

			unix_signals = validate(process.GetUnixSignals())
			# This may not need to be here, we might be able to put it
			# outside the onStopped func
			for sig in unix_signals.signals:
				unix_signals.SetShouldSuppress(sig, False)
				unix_signals.SetShouldStop(sig, False)
				unix_signals.SetShouldNotify(sig, False)

			process.Continue()
		self.em.addCallback(lldb.eStateStopped, onStopped)

		def doShutdown(event):
			print("doShutdown(" + getDescription(event) + ")")
			shutdown()
		self.em.addCallback(lldb.eStateCrashed, doShutdown)
		self.em.addCallback(lldb.eStateExited, doShutdown)

		self.em.begin()
		running.wait()
		print("__init__: end")

	def shutdown(self):
		self.em.shutdown()

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

	def getTypeByVariableName(self, variable, tid, current):
		print("getTypeByVariableName: begin")

		try:
			with self.StoppedThread(self.em, self.process, tid) as thread:

				frame = validate(thread.frame[1])
				print(frame)

				value = validate(frame.FindVariable(variable))
				print(value)

				typ = validate(value.GetType())
				print(typ)

				return TypeImpl.proxyFor(typ, current)
		finally:
			print("getTypeByVariableName: exit")

	def getBacktrace(self, tid, current):
		print("getBacktrace: begin")
		try:
			with self.StoppedThread(self.em, self.process, tid) as thread:
				frame_list = []

				for sbframe in thread.frames:
					frame = Frame()
					frame.functionName = sbframe.name
					frame.moduleName = sbframe.module.file.basename
					frame.compileUnitName = sbframe.compile_unit.file.basename
					frame.functionType = TypeImpl.proxyFor(sbframe.function.type, current)
					frame.line = sbframe.line_entry.line
					frame_list.append(frame)

				return frame_list
		finally:
			print("getBacktrace: end")
