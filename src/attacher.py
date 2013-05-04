from __future__ import print_function, absolute_import, with_statement

from threading import Lock

from lldb_utils import validate, waitForState
from lldb import *

class Attacher(object):
	def __init__(self, target, attach_info):
		self.refcnt = 0
		self.lock = Lock()
		self.target = target
		self.attach_info = attach_info

	def attach(self):
		with self.lock:
			if self.refcnt == 0:
				err = SBError()
				self.process = validate(
					self.target.Attach(self.attach_info, err)
				)
				validate(err)

				waitForState(self.process, eStateStopped)

				self.process.Continue()
				print("Attached")
			self.refcnt += 1
		return self.process

	def detach(self):
		with self.lock:
			self.refcnt -= 1
			if self.refcnt == 0:
				self.process.Detach()
				self.process = None
				print("Detached")

	def __enter__(self):
		return attach()

	def __exit__(self, type, value, traceback):
		detach()
