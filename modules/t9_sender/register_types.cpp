/*************************************************************************/
/* Wrapper for PID protocol 2.0 to be accessible by Godot scription      */
/*************************************************************************/

#include "register_types.h"
#include "t9_sender.h"

void register_t9_sender_types() {

	ClassDB::register_class<T9Sender>();
}

void unregister_t9_sender_types() {

}
