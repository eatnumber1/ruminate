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
			unix_signals = validate(self.process.GetUnixSignals())
			for sig in unix_signals.signals:
				unix_signals.SetShouldSuppress(sig, False)
				unix_signals.SetShouldStop(sig, False)
				unix_signals.SetShouldNotify(sig, False)
			return self.process

		self.em = LLDBEventMachine(procgen)

		def onStopped(event):
			self.em.removeCallback(lldb.eStateStopped, onStopped)
			SBProcess.GetProcessFromEvent(event).Continue()
		self.em.addCallback(lldb.eStateStopped, onStopped)

		def doShutdown(event):
			print("doShutdown(" + str(event) + ")")
			shutdown()
		self.em.addCallback(lldb.eStateCrashed, doShutdown)
		self.em.addCallback(lldb.eStateExited, doShutdown)

		self.em.begin()

	def shutdown(self):
		self.em.shutdown()

	def getTypeByVariableName(self, variable, tid, current):
		print("getTypeByVariableName: begin")

		try:
			thread = validate(self.process.GetThreadByID(tid))

			stoppedAtBreakpoint = threading.Event()
			def onStopped(event):
				if thread.stop_reason == eStopReasonBreakpoint:
					stoppedAtBreakpoint.set()
					self.em.removeCallback(lldb.eStateStopped, onStopped)
			self.em.addCallback(lldb.eStateStopped, onStopped)

			try:
				stoppedAtBreakpoint.wait()

				frame = validate(thread.frame[1])
				print(frame)

				value = validate(frame.FindVariable(variable))
				print(value)

				typ = validate(value.GetType())
				print(typ)

				return TypePrx.uncheckedCast(current.adapter.addWithUUID(TypeImpl(typ)))
			except:
				try:
					self.em.removeCallback(lldb.eStateStopped, onStopped)
				except:
					pass
		finally:
			self.process.Continue()
			print("Process continued")
			print("getTypeByVariableName: exit")
