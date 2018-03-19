/*************************************************************************/
/* Wrapper for PID protocol 2.0 to be accessible by Godot scription      */
/*************************************************************************/

#include "register_types.h"
#include "class_db.h"
#include "IB_video_protocol.h"

static VideoStreamIBSingleton *_streamIBVideoSingleton = NULL;

void register_IB_video_types() {

	ClassDB::register_class<ProtocolPID2>();
	ClassDB::register_class<ProtocolPID2Connection>();

	_streamIBVideoSingleton = memnew(VideoStreamIBSingleton());
}

void unregister_IB_video_types() {
}
