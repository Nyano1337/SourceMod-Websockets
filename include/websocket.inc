#if defined _websocket_included_
	#endinput
#endif
#define _websocket_included_

#include <websocket/json>
#include <websocket/ws>



/**
 * Do not edit below this line!
 */
public Extension __ext_websocket =
{
	name = "websocket",
	file = "websocket.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};
