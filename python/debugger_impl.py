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

			# TODO: This might be racy. Revisit later
			self.type_factory = TypeImplFactory()
			self.type_factory.debugger = self
			self.type_factory.thread_stop = StoppedThreadFactory(self.em, self.process)

			return self.process

		self.em = LLDBEventMachine(procgen)

		running = threading.Event()
		def onRunning(event):
			self.em.removeCallback(lldb.eStateRunning, onRunning)
			running.set()
		self.em.addCallback(lldb.eStateRunning, onRunning)

		def onStopped(event):
			self.em.removeCallback(lldb.eStateStopped, onStopped)

			process = SBProcess.GetProcessFromEvent(event)

			unix_signals = validate(process.GetUnixSignals())
			# This may not need to be here, we might be able to put it
			# outside the onStopped func
			for sig in unix_signals.signals:
				unix_signals.SetShouldSuppress(sig, False)
				unix_signals.SetShouldStop(sig, False)
				unix_signals.SetShouldNotify(sig, False)

			#unix_signals = validate(process.GetUnixSignals())
			#for sig in unix_signals.signals:
			#	self.debugger.HandleCommand("process handle -n false -p true -s false " + unix_signals.GetSignalAsCString(sig))

			process.Continue()
		self.em.addCallback(lldb.eStateStopped, onStopped)

		def doShutdown(event):
			shutdown()
		self.em.addCallback(lldb.eStateCrashed, doShutdown)
		self.em.addCallback(lldb.eStateExited, doShutdown)

		self.em.begin()
		running.wait()

	def shutdown(self):
		self.em.shutdown()

	def getTypeByVariableName(self, variable, tid, current):
		with StoppedThread(self.em, self.process, tid) as thread:
			frame = validate(thread.frame[2])
			value = validate(frame.FindVariable(variable))
			t = validate(value.type)
			return self.type_factory.proxy(
				sbtype = t,
				current = current
			)

	def getBacktrace(self, tid, current):
		with StoppedThread(self.em, self.process, tid) as thread:
			frame_list = []

			for sbframe in thread.frames:
				frame = Frame()
				# In order for this to work with inlined functions, sbframe.function.name won't work.
				frame.functionName = sbframe.name.split("(")[0]
				frame.moduleName = sbframe.module.file.basename
				frame.compileUnitName = sbframe.compile_unit.file.basename
				frame.functionType = self.type_factory.proxy(
					sbtype = sbframe.function.type,
					current = current
				)
				frame.line = sbframe.line_entry.line
				frame_list.append(frame)
			return frame_list

	def getTypesByName(self, name, current):
		ret = []
		for type in self.target.FindTypes(name):
			ret.append(
				self.type_factory.proxy(
					sbtype = type,
					current = current
				)
			)
		return ret

	def getFunctionName(self, addr, current):
		return lldb.SBAddress(addr, self.target).function.name

	def getArrayMemberTypeAndLength(self, sbtype):
		ptnam = sbtype.GetPointerType().name
		# TODO: Check for errors here
		array = self.target.EvaluateExpression(
			(
				"char a[sizeof((*((%s) NULL))[0])];"
				"__typeof__(&a) ap = &a;"
				"(%s) ap"
			) % (ptnam, ptnam),
			SBExpressionOptions()
		).Dereference()
		return (array.GetChildAtIndex(0, lldb.eDynamicDontRunTarget, False).type, array.num_children)
