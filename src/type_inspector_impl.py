from __future__ import print_function, absolute_import
from Ruminate import *
from type_impl import TypeImpl
from lldb_utils import *
from lldb import *

class TypeInspectorImpl(TypeInspector):
	def __init__(self, target, addr, attacher):
		self.target = target
		self.addr = addr
		self.attacher = attacher
		self.process = attacher.attach()
		waitForState(self.process, eStateRunning)
		# TODO: Don't need to stop here.
		print("Stopping...")
		self.process.Stop()
		try:
			waitForState(self.process, eStateStopped)
			self.breakpoint = validate(self.target.BreakpointCreateByAddress(addr))
		finally:
			self.process.Continue()
		print("Created breakpoint on address 0x%x" % addr)
		print(self.breakpoint)
		print(self.breakpoint.IsEnabled())
		print(getDescription(self.breakpoint.FindLocationByAddress(addr), eDescriptionLevelFull))
		if self.breakpoint.GetNumResolvedLocations() == 0:
			raise InvalidAddressException(addr)

	def __del__(self):
		self.target.BreakpointDelete(self.breakpoint.GetID())
		self.attacher.detach()

	def getBreakpointAddress(self, current):
		return self.addr

	def getTypeByVariableName(self, variable, tid, current):
		try:
			print("getTypeByVariableName: begin")
			thread = validate(self.process.GetThreadByID(tid))

			#listener = self.target.debugger.GetListener()
			listener = validate(SBListener("TypeInspectorImpl"))
			broadcaster = self.process.broadcaster
			broadcaster.AddListener(listener, SBProcess.eBroadcastBitStateChanged)
			print("Address = 0x%x" % self.addr)

			while True:
				print("Waiting...")
				event = SBEvent()
				listener.WaitForEventForBroadcasterWithType(
					2**32-1,
					broadcaster,
					SBProcess.eBroadcastBitStateChanged,
					event
				)
				validate(event)
				print("Got event " + getDescription(event))

				if thread.stop_reason == eStopReasonBreakpoint and thread.frame[0].pc == self.addr:
					break

			frame = validate(thread.frame[1])
			print(frame)

			value = validate(frame.FindVariable(variable))
			print(value)

			typ = validate(value.GetType())
			print(typ)

			return TypePrx.uncheckedCast(current.adapter.addWithUUID(TypeImpl(typ)))
		finally:
			self.process.Continue()
			print("Process continued")
			print("getTypeByVariableName: exit")
