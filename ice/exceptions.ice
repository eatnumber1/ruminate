#ifndef _EXCEPTIONS_ICE_
#define _EXCEPTIONS_ICE_

module RuminateBackend {
	exception RuminateException {};
	exception LLDBException extends RuminateException {};
	exception SBInvalidException extends LLDBException {};
	exception SBErrorException extends LLDBException {
		string message;
	};
	exception InvalidAddressException extends RuminateException {
		long address;
	};
};

#endif
