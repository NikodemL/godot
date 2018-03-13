/*************************************************************************/
/* Wrapper for PID protocol 2.0 to be accessible by Godot scription      */
/*************************************************************************/

#include "casino_PID2_protocol.h"
#include "windows.h"

void ProtocolPID2::_bind_methods() {
	ClassDB::bind_method(D_METHOD("update"), &ProtocolPID2::update);
	ClassDB::bind_method(D_METHOD("create_connection", "id", "ip", "port"), &ProtocolPID2::create_connection);
}

ProtocolPID2::ProtocolPID2()
{
	// Initialize the sockets (used by DLL, does not initialize itself)
	WSAData wsaData;
	WSAStartup(0x0202, &wsaData);

	// Load library and save pointer (we do not use .lib for loading because DLL
	// is not meant to be used this way ... somehow)
	HINSTANCE hLibrary = LoadLibrary("casinogameclient.dll");
	if (hLibrary == NULL)
	{

		ERR_PRINTS("Could not load casinogameclient.dll. Make sure it is in the bin directory");
		_pLibrary = NULL;
		return;
	}
	_pLibrary = hLibrary;

	// Load all functions
	_fpInitialize = (FCasinoGameClientInitialize)GetProcAddress(hLibrary, "CasinoGameClientInitialize");
	_fpFinalize = (FCasinoGameClientFinalize)GetProcAddress(hLibrary, "CasinoGameClientFinalize");
	_fpUpdate = (FCasinoGameClientUpdate)GetProcAddress(hLibrary, "CasinoGameClientUpdate");
	_fpSetCallback = (FCasinoGameClientSetCallback)GetProcAddress(hLibrary, "CasinoGameClientSetCallback");
	_fpCreate = (FCasinoGameClientCreate)GetProcAddress(hLibrary, "CasinoGameClientCreate");
	_fpDelete = (FCasinoGameClientDelete)GetProcAddress(hLibrary, "CasinoGameClientDelete");

	_fpIsConnected = (FCasinoGameClientIsConnected)GetProcAddress(hLibrary, "CasinoGameClientIsConnected");
	_fpSetTicketID = (FCasinoGameClientSendTicketID)GetProcAddress(hLibrary, "CasinoGameClientSendTicketID");
	_fpSendInit = (FCasinoGameClientSendInit)GetProcAddress(hLibrary, "CasinoGameClientSendInit");
	_fpSetHistorySize = (FCasinoGameClientSetHistorySize)GetProcAddress(hLibrary, "CasinoGameClientSetHistorySize");
	_fpAskForHistory = (FCasinoGameClientAskForHistory)GetProcAddress(hLibrary, "CasinoGameClientAskForHistory");

	_fpProposeProtocolVersion = (FCasinoGameClientProposeProtocolVersion)GetProcAddress(hLibrary, "CasinoGameClientProposeProtocolVersion");
	_gpIsDeviceSet = (FCasinoGameClientIsDeviceSet)GetProcAddress(hLibrary, "IsDeviceSet");
	_fpSubscribe = (FCasinoGameClientSubscribe)GetProcAddress(hLibrary, "CasinoGameClientSubscribe");

	print_line("Loaded casinogameclient.dll");

	// Initialize the DLL
	_fpInitialize();

	print_line("PID initialized");
}

ProtocolPID2::~ProtocolPID2()
{
	if (_fpFinalize != NULL)
		_fpFinalize();

	print_line("PID finalized");
}


void ProtocolPID2::update()
{
	if (_fpUpdate)
		_fpUpdate();
}

void ProtocolPID2Connection::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("ask_for_history"), &ProtocolPID2Connection::ask_for_history);

	// External signal events
	ADD_SIGNAL(MethodInfo("on_connected"));
	ADD_SIGNAL(MethodInfo("on_disconnected"));
	ADD_SIGNAL(MethodInfo("on_log"));
	ADD_SIGNAL(MethodInfo("on_init"));
	ADD_SIGNAL(MethodInfo("on_protocol_version"));

	ADD_SIGNAL(MethodInfo("on_no_more_bets"));
	ADD_SIGNAL(MethodInfo("on_last_bets"));
	ADD_SIGNAL(MethodInfo("on_game_time", PropertyInfo(Variant::REAL, "remainingTime"),
		PropertyInfo(Variant::REAL, "gameTime"), PropertyInfo(Variant::REAL, "lastBetsTime")));

	ADD_SIGNAL(MethodInfo("on_roulette_result", PropertyInfo(Variant::INT, "result")));

	ADD_SIGNAL(MethodInfo("on_roulette_history", PropertyInfo(Variant::ARRAY, "history")));
}

void ProtocolPID2Connection::add_commands()
{
	print_line("Adding PID commands");

	// First add all common commands
	set_mask(_CommandMask.data(), commLastBets);
	set_mask(_CommandMask.data(), commNoMoreBets);
	set_mask(_CommandMask.data(), commDetailedBetInfo);
	set_mask(_CommandMask.data(), commAskForHistory);
	set_mask(_CommandMask.data(), commCenterHistory);
	set_mask(_CommandMask.data(), commCenterData);
	set_mask(_CommandMask.data(), commErrorMessage);
	set_mask(_CommandMask.data(), commCenterData2);
	set_mask(_CommandMask.data(), commInfoMessage);
	set_mask(_CommandMask.data(), commInit);
	set_mask(_CommandMask.data(), commResultDisplayFinished);
	set_mask(_CommandMask.data(), commGameTime);
	set_mask(_CommandMask.data(), commShutdown);
	set_mask(_CommandMask.data(), commFerrariTicket);
	set_mask(_CommandMask.data(), commTableClosed);
	set_mask(_CommandMask.data(), commSidebetResult);
	set_mask(_CommandMask.data(), commWinningPlaystations);
	set_mask(_CommandMask.data(), commPing);
	set_mask(_CommandMask.data(), commPong);
	set_mask(_CommandMask.data(), commPlaystationEvent);
	set_mask(_CommandMask.data(), commCenterEvent);

	// Roulette specific
	set_mask(_CommandMask.data(), commRouletteResult);
	// And jackpot stuff

	set_mask(_InitCommandMask.data(), cinitPIDType);
	set_mask(_InitCommandMask.data(), cinitGameType);
	set_mask(_InitCommandMask.data(), cinitThresholdForWinningPlaystations);
	set_mask(_InitCommandMask.data(), cinitVersionInfo);
	set_mask(_InitCommandMask.data(), cinitProtocolVersion);
	set_mask(_InitCommandMask.data(), cinitSerialNumber);
	set_mask(_InitCommandMask.data(), cinitSubscription);
	set_mask(_InitCommandMask.data(), cinitRNGType);

	set_mask(_ShutdownCommandMask.data(), shutExit);
	set_mask(_ShutdownCommandMask.data(), shutLogOff);
	set_mask(_ShutdownCommandMask.data(), shutShutdown);
	set_mask(_ShutdownCommandMask.data(), shutReboot);

	set_mask(_CenterData2CommandMask.data(), cd2CenterColor);
	set_mask(_CenterData2CommandMask.data(), cd2ResultDelay);

	// Roulette specific
	set_mask(_CenterData2CommandMask.data(), cd2RouletteGoal);
	set_mask(_CenterData2CommandMask.data(), cd2ResultGoldenBall);
	set_mask(_CenterData2CommandMask.data(), cd2RouletteTouchdown);

	set_mask(_PlaystationEventCommandMask.data(), psePlayerCame);
	set_mask(_PlaystationEventCommandMask.data(), psePlayerLeft);
	set_mask(_PlaystationEventCommandMask.data(), pseActivityDetected);
	set_mask(_PlaystationEventCommandMask.data(), pseCallAttendent);
	set_mask(_PlaystationEventCommandMask.data(), pseOrderDrink);
	set_mask(_PlaystationEventCommandMask.data(), pseHelpRequested);
	set_mask(_PlaystationEventCommandMask.data(), pseGameActivated);
	set_mask(_PlaystationEventCommandMask.data(), pseGameDeactivated);
	set_mask(_PlaystationEventCommandMask.data(), pseThrowObject);
	set_mask(_PlaystationEventCommandMask.data(), pseCallAttendentWithdrawn);

	set_mask(_CenterEventCommandMask.data(), ceCroupierLogin);
	set_mask(_CenterEventCommandMask.data(), ceCroupierLogout);
	set_mask(_CenterEventCommandMask.data(), ceTip);
	set_mask(_CenterEventCommandMask.data(), ceManualResult);
	set_mask(_CenterEventCommandMask.data(), ceGameCanceled);
}

void ProtocolPID2Connection::set_callback(ECasinoGameClientCallback callback, void* funcPtr)
{
	_PID_client->_fpSetCallback(callback, funcPtr);
}

__declspec(cdecl) void on_c_log(void* userdata, const char* str)
{
	print_line(str);

	/*
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		return data->_connection->on_log(str);
	}
	else {
		ERR_PRINTS("Log from invalid source");
	}*/
}

__declspec(cdecl) void on_c_connected(void* userdata)
{
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		return data->_connection->on_connected();
	}
	else {
		ERR_PRINTS("Connected from invalid source");
	}
}

__declspec(cdecl) void on_c_disconnected(void* userdata)
{
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		return data->_connection->on_disconnected();
	}
	else {
		ERR_PRINTS("Disconnected from invalid source");
	}
}

__declspec(cdecl) void on_c_init(void* userdata, ECasinoGameClientGame gameType, int playStationWinningThreshold,
	unsigned char playStationWinningCount, const char* centerVersion)
{
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		return data->_connection->on_init(gameType, playStationWinningThreshold,
			playStationWinningCount, centerVersion);
	}
	else {
		ERR_PRINTS("On Init from invalid source");
	}
}

__declspec(cdecl) void on_c_protocol_version(void* userdata, int version)
{
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		return data->_connection->on_protocol_version(version);
	}
	else {
		ERR_PRINTS("Protocol version from invalid source");
	}
}

__declspec(cdecl) void on_c_roulette_result(void* userdata, unsigned char result)
{
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		return data->_connection->on_roulette_result(result);
	}
	else {
		ERR_PRINTS("On Init from invalid source");
	}
}

__declspec(cdecl) void on_c_last_bets(void* userdata)
{
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		return data->_connection->on_last_bets();
	}
	else {
		ERR_PRINTS("On Init from invalid source");
	}
}

__declspec(cdecl) void on_c_no_more_bets(void* userdata)
{
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		return data->_connection->on_no_more_bets();
	}
	else {
		ERR_PRINTS("On Init from invalid source");
	}
}

__declspec(cdecl) void on_c_game_time(void* userdata, int rem, int gt, int lbt)
{
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		auto conn = data->_connection;
		if (conn->get_protocol_version() > 1)
		{
			return conn->on_game_time(rem / 1000.0f, gt / 1000.0f, lbt / 1000.0f);
		}
		return conn->on_game_time(rem, gt, lbt);
	}
	else {
		ERR_PRINTS("On Init from invalid source");
	}

}

__declspec(cdecl) void on_c_roulette_center_history(void* userdata, unsigned char *result, int resultCount)
{
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		Array arr;
		for (int i = 0; i < resultCount; i++)
		{
			arr.append(Variant(result[i]));
		}

		data->_connection->on_roulette_history(arr);
	}
	else {
		ERR_PRINTS("On Init from invalid source");
	}
}

__declspec(cdecl) void on_c_roulette_center_history_status(void* userdata, unsigned char *result, int resultCount)
{
	ProtocolUserData* data = (ProtocolUserData*)userdata;
	if (data && data->isOK())
	{
		Array arr;
		for (int i = 0; i < resultCount; i++)
		{
			arr.append(Variant(result[i] - 48));
		}

		data->_connection->on_roulette_history(arr);
	}
	else {
		ERR_PRINTS("On Init from invalid source");
	}
}




void ProtocolPID2Connection::set_callbacks()
{
	// DLL uses __declspec(cdeck) for callback, we need to wrap our "thiscall" functions
	set_callback(cgccLog, on_c_log);
	set_callback(cgccOnConnected, on_c_connected);
	set_callback(cgccOnDisconnected, on_c_disconnected);
	set_callback(cgccOnInit, on_c_init);
	set_callback(cgccOnProtocolVersion, on_c_protocol_version);

	// roulette specific
	set_callback(cgccOnRouletteResult, on_c_roulette_result);
	set_callback(cgccOnNoMoreBets, on_c_no_more_bets);
	set_callback(cgccOnLastBets, on_c_last_bets);
	set_callback(cgccOnGameTime, on_c_game_time);

	set_callback(cgccOnRouletteCenterHistory, on_c_roulette_center_history);
	set_callback(cgccOnRouletteCenterHistorywStatus, on_c_roulette_center_history_status);
}

void ProtocolPID2Connection::set_mask(unsigned char* arr, int command)
{
	int realCmd = command - 1;
	int index = realCmd / 8;
	arr[index] |= (unsigned char)(1 << (realCmd % 8));
}

ProtocolPID2Connection::ProtocolPID2Connection()
{

}
void ProtocolPID2Connection::ask_for_history()
{
	print_line("PID asking for history");
	(_PID_client->_fpSetHistorySize)(&_userData, 50);
	(_PID_client->_fpAskForHistory)(&_userData);
}


void ProtocolPID2Connection::subscribe()
{
	print_line("Subscribing");

	// Subscribe to all events
	(_PID_client->_fpProposeProtocolVersion)(&_userData, (unsigned short)1);

	(_PID_client->_fpSubscribe)(&_userData, 0, 5, _CommandMask.data());

	(_PID_client->_fpSubscribe)(&_userData, commCenterData2, 1, _CenterData2CommandMask.data());
	(_PID_client->_fpSubscribe)(&_userData, commInit, 2, _InitCommandMask.data());
	(_PID_client->_fpSubscribe)(&_userData, commShutdown, 1, _ShutdownCommandMask.data());
	(_PID_client->_fpSubscribe)(&_userData, commPlaystationEvent, 2, _PlaystationEventCommandMask.data());
	(_PID_client->_fpSubscribe)(&_userData, commCenterEvent, 1, _CenterEventCommandMask.data());
}

void ProtocolPID2Connection::on_connected()
{
	print_line("PID on connected");
	emit_signal("on_connected");
}

void ProtocolPID2Connection::on_log(const char* str)
{
	print_line("PID on log");
	emit_signal("on_log");
}

void ProtocolPID2Connection::on_disconnected()
{
	print_line("PID on disconnected");
	emit_signal("on_disconnected");
}

void ProtocolPID2Connection::on_init(ECasinoGameClientGame gameType, int playStationWinningThreshold,
	byte playStationWinningCount, const char* centerVersion)
{
	print_line("PID on init");
	subscribe();

	emit_signal("on_init");
}

void ProtocolPID2Connection::on_protocol_version(int version)
{
	print_line("PID protocol version: " + version);
	_protocolVersion = version;
	emit_signal("on_protocol_version", Variant(version));

}

void ProtocolPID2Connection::on_game_time(float remainingTime, float gameTime, float lastBetsTime)
{
	print_line("PID game time");
	emit_signal("on_game_time", Variant(remainingTime), Variant(gameTime), Variant(lastBetsTime));
}

void ProtocolPID2Connection::on_no_more_bets()
{
	print_line("PID no more bets");
	emit_signal("on_no_more_bets");
}

void ProtocolPID2Connection::on_last_bets()
{
	print_line("PID last bets");
	emit_signal("on_last_bets");
}

void ProtocolPID2Connection::on_roulette_result(int result)
{
	print_line("PID roulette result");
	emit_signal("on_roulette_result", Variant(result));
}

void ProtocolPID2Connection::on_roulette_history(Array history)
{
	print_line("PID on_roulette_history result");
	emit_signal("on_roulette_history", Variant(history));
}

void ProtocolPID2Connection::init(ProtocolPID2* protocol, String id, String address, int port)
{
	_PID_client = protocol;

	// Add commands
	add_commands();

	// Copy connect string
	_ID = id;
	_IP = address;
	_port = port;

	// Set all the callback when "update" is called
	set_callbacks();

	_userData._connection = this;

	// Call create
	(_PID_client->_fpCreate)(cgcgInvalid, _IP.ascii().get_data(), port, 0, _ID.ascii().get_data(), &_userData);
}

Ref<ProtocolPID2Connection> ProtocolPID2::create_connection(String id, String ip, int port)
{
	print_line("PID creating connection: id=" + id + ",ip=" + ip);
	Ref<ProtocolPID2Connection> connection = memnew(ProtocolPID2Connection);
	connection->init(this, id, ip, port);
	return connection;
}




