#include "private/protocol.h"

using namespace rumination;

Protocol::Protocol() :
	m_commandType(COMMAND_TYPE_INVALID) {}

Protocol::Protocol( enum CommandType commandType ) :
	m_commandType(commandType) {}

Protocol::Protocol( const Protocol &rhs ) :
	m_commandType(rhs.m_commandType) {}

const Protocol &Protocol::operator=( const Protocol &rhs ) {
	if( this != &rhs ) {
		m_commandType = rhs.m_commandType;
	}
	return *this;
}
