/*************************************************************************/
/* Wrapper for PID protocol 2.0 to be accessible by Godot scription      */
/*************************************************************************/

#include "register_types.h"
#include "class_db.h"
#include "IB_video_protocol.h"

void register_IB_video_types() {

	ClassDB::register_class<ProtocolPID2>();
	ClassDB::register_class<ProtocolPID2Connection>();
}

void unregister_IB_video_types() {
}
