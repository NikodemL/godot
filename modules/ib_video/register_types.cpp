/*************************************************************************/
/* Wrapper for PID protocol 2.0 to be accessible by Godot scription      */
/*************************************************************************/

#include "register_types.h"
#include "core/class_db.h"
#include "ib_video_stream.h"

void register_ib_video_types() {

	ClassDB::register_class<VideoStreamIBManager>();
	ClassDB::register_class<DirectXIBVideoTexture>();
}

void unregister_ib_video_types() {
}
