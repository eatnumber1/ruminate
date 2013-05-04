#ifndef _PRIVATE_PROTOCOL_H_
#define _PRIVATE_PROTOCOL_H_

namespace rumination {

class Protocol {
	enum CommandType {
		COMMAND_TYPE_INVALID = 0,
		COMMAND_TYPE_ERROR
	};

	Protocol();
	Protocol( enum CommandType );
	Protocol( const Protocol & );

	const Protocol &operator=( const Protocol & );

private:
	enum CommandType m_commandType;

}

}

#endif
