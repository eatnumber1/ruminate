from __future__ import print_function, absolute_import, nested_scopes, with_statement

from lldb import *

from RuminateBackend import *
from lldb_utils import *
from type_impl_factory import *
from lldb_em import *
from stopped_thread import *

import threading

import sys # For debugging only

class DebuggerImpl(Debugger):
	def __init__(self, options, shutdown, debugee):
		self.debugger = validate(SBDebugger.Create())
		self.debugger.SetAsync(True)
		self.debugee = debugee

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

			# TODO: This might be racy. Revisit later
			self.type_factory = TypeImplFactory()
			self.type_factory.debugger = self
			self.type_factory.thread_stop = StoppedThreadFactory(self.em, self.process)

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

			#unix_signals = validate(process.GetUnixSignals())
			## This may not need to be here, we might be able to put it
			## outside the onStopped func
			#for sig in unix_signals.signals:
			#	unix_signals.SetShouldSuppress(sig, False)
			#	unix_signals.SetShouldStop(sig, False)
			#	unix_signals.SetShouldNotify(sig, False)

			# A temporary hack since the above is racy.
			unix_signals = validate(process.GetUnixSignals())
			for sig in unix_signals.signals:
				self.debugger.HandleCommand("process handle -n false -p true -s false " + unix_signals.GetSignalAsCString(sig))

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

	def getTypeByVariableName(self, variable, tid, current):
		print("getTypeByVariableName: begin")

		try:
			with StoppedThread(self.em, self.process, tid) as thread:
				frame = validate(thread.frame[1])
				print(frame)

				value = validate(frame.FindVariable(variable))
				print(value)

				t = validate(value.type)
				print(t)

				return self.type_factory.proxy(
					sbtype = t,
					current = current,
					address = value.address_of.unsigned
				)
		finally:
			print("getTypeByVariableName: exit")

	def getBacktrace(self, tid, current):
		print("getBacktrace: begin")
		try:
			with StoppedThread(self.em, self.process, tid) as thread:
				frame_list = []

				for sbframe in thread.frames:
					frame = Frame()
					frame.functionName = sbframe.name
					frame.moduleName = sbframe.module.file.basename
					frame.compileUnitName = sbframe.compile_unit.file.basename
					frame.functionType = self.type_factory.proxy(sbframe.function.type, current)
					frame.line = sbframe.line_entry.line
					frame_list.append(frame)

				return frame_list
		finally:
			print("getBacktrace: end")

	def createSBValueFor(self, sbtype, address):
		# TODO: Check for errors here
		return self.target.EvaluateExpression(
			"(%s) 0x%x" % (sbtype.GetPointerType().name, address),
			SBExpressionOptions()
		).Dereference()

	def createSBAddressFor(self, address):
		return lldb.SBAddress(address, self.target)
