from Ruminate import *

class TypeImpl(Type):
	def __init__(self, sbtype):
		self.sbtype = sbtype

	def getName(self, current = None):
		return self.sbtype.GetName()
