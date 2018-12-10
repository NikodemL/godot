/*************************************************************************/
/* Wrapper for PID protocol 2.0 to be accessible by Godot scription      */
/*************************************************************************/

#include "register_types.h"
#include "core/class_db.h"
#include "casino_PID2_protocol.h"

void register_casino_PID2_protocol_types() {

	ClassDB::register_class<ProtocolPID2>();
	ClassDB::register_class<ProtocolPID2Connection>();
}

void unregister_casino_PID2_protocol_types() {
}
