from __future__ import print_function

from Ruminate import SBInvalidException, SBErrorException, LLDBException
from lldb import SBError, SBStream, SBEvent, SBProcess
import lldb

def validate(obj):
	if isinstance(obj, bool):
		if obj:
			return
		else:
			raise LLDBException()

	if obj and obj.IsValid():
		if isinstance(obj, SBError) and not obj.Success():
			raise SBErrorException(obj.GetCString())
	else:
		raise SBInvalidException()

	return obj

def getDescription(obj, option = None):
	method = getattr(obj, 'GetDescription')
	if not method:
		return None
	tuple = (lldb.SBTarget, lldb.SBBreakpointLocation, lldb.SBWatchpoint)
	if isinstance(obj, tuple):
		if option is None:
			option = lldb.eDescriptionLevelBrief

	stream = lldb.SBStream()
	if option is None:
		success = method(stream)
	else:
		success = method(stream, option)
	if not success:
		return None
	return stream.GetData()

def waitForState(process, state):
	event = SBEvent()
	listener = process.target.debugger.GetListener()
	broadcaster = process.broadcaster
	while process.state != state:
		print("Waiting...")
		listener.WaitForEventForBroadcasterWithType(
			2**32-1,
			broadcaster,
			SBProcess.eBroadcastBitStateChanged,
			event)
		validate(event)
		print("Got event " + getDescription(event))
		# TODO: Check event type

#def ensureState(process, state):
#	if self.process.state != state:
#		waitForState(process, state)
