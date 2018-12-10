/*************************************************************************/
/* Wrapper for PID protocol 2.0 to be accessible by Godot scription      */
/*************************************************************************/


#ifndef PROTOCOL_PID2_H
#define PROTOCOL_PID2_H

#include <array>
#include "core/reference.h"
#include "casinogameclient.h"

// Additional definitions not in casinogameclient.h
typedef void(*FCasinoGameClientProposeProtocolVersion)(void *handle, short version);

enum Commands
{
	// 1st byte
	commLastBets = 1,
	commNoMoreBets,
	commRouletteResult,
	commDiceResult,
	commJackpotValue,
	commBaccaratResult,
	commDetailedBetInfo,
	commAskForHistory,
	// 2nd byte
	commCenterHistory,
	commCrapsHardways,
	commCenterData,
	commBlackjack,
	commErrorMessage,
	commCenterData2,
	commInfoMessage,
	commKenoResult,
	// 3rd byte
	commBigSix,
	commPopNPokerResult,
	commInit,
	commResultDisplayFinished,
	commGameTime,
	commShutdown,
	commFerrariTicket,
	commTableClosed,
	// 4th byte
	commSidebetResult,
	commJackpotType,
	commJackpotState,
	commWinningPlaystations,
	commJackpotWinCondition,
	commPing,
	commPong,
	commPlaystationEvent,
	// 5th byte
	commCenterEvent,
};

enum InitCommand
{
	cinitPIDType = 1,
	cinitGameType,
	cinitThresholdForWinningPlaystations,
	cinitVersionInfo,
	cinitProtocolVersion,
	cinitSerialNumber,
	cinitSubscription = 8,
	cinitRNGType,
};

enum Shutdown
{
	shutExit = 1,
	shutLogOff,
	shutShutdown,
	shutReboot,
};

enum BaccaratResultEvent
{
	baccCard = 1,
	baccFinalResult,
	baccPuck,
	baccDecisionDone,
	baccClearTable,
};

enum CenterData2
{
	cd2CenterColor = 1,
	cd2RouletteTouchdown,
	cd2RouletteGoal,
	cd2ResultDelay,
	cd2ResultGoldenBall,
	cd2CardsDealt = 7,
};

enum BlackjackResultEvent
{
	bjCard = 1,
	bjFinalResult,
	bjPlayerDecision,
	bjSetProperties,
	bjSetActivePlayer,
	bjClearCards,
	bjPlayerDone,
};

enum BigSixResultEvent
{
	b6Result = 1,
	b6Spin,
};

enum PlayStationEvent
{
	psePlayerCame = 1,
	psePlayerLeft,
	pseActivityDetected,
	pseCallAttendent,
	pseOrderDrink,
	pseHelpRequested,
	pseGameActivated,
	pseGameDeactivated,
	pseThrowObject,
	pseCallAttendentWithdrawn,
};

enum CenterEvent
{
	ceCroupierLogin = 1,
	ceCroupierLogout,
	ceTip,
	ceManualResult,
	ceGameCanceled,
	ceBurntCard,
	ceShoeChanged,
};

class ProtocolPID2Connection;

// As PID protocol sometimes returns "invalid pointer", we use
// checksumming technique when entering data. If two checksums
// match, we can propagate
struct ProtocolUserData
{
	int _check1;
	int _check2;
	ProtocolPID2Connection* _connection;

	ProtocolUserData()
	{
		_check1 = 321313312;
		_check2 = 823132131;
	}

	bool isOK()
	{
		return _check1 == 321313312 &&
				_check2 == 823132131;
	}
};

class ProtocolPID2;

/*
	A connection created on ProtocolPID, used to connect to a center.
*/
class ProtocolPID2Connection : public Reference
{
	GDCLASS(ProtocolPID2Connection, Reference);

	CharString _ID;
	CharString _IP;
	int _port;
	void* _handle;

	ProtocolUserData _userData;

	ProtocolPID2* _PID_client;

	int _protocolVersion;

	// Command masks
	std::array<unsigned char, 5> _CommandMask;
	std::array<unsigned char, 2> _InitCommandMask;
	std::array<unsigned char, 1> _ShutdownCommandMask;
	std::array<unsigned char, 1> _CenterData2CommandMask;
	std::array<unsigned char, 2> _PlaystationEventCommandMask;
	std::array<unsigned char, 1> _CenterEventCommandMask;
	std::array<unsigned char, 1> _BaccaratCommandMask;
	std::array<unsigned char, 1> _BlackjackCommandMask;
	std::array<unsigned char, 1> _BigSixCommandMask;

	// Adds a command to a specific mask
	void set_mask(unsigned char* mask, int command);

protected:
	static void _bind_methods();

	void set_callback(ECasinoGameClientCallback callback, void* funcPtr);

	// Specific connection should add specific events/callbacks here
	virtual void add_commands();
	virtual void set_callbacks();
	virtual void subscribe();


public:
	ProtocolPID2Connection();

	void init(ProtocolPID2* client, String id, String address, int port);

	int get_protocol_version() { return _protocolVersion; }
	bool is_connected();

	void ask_for_history();


	void on_connected();
	void on_log(const char* str);
	void on_disconnected();
	void on_init(ECasinoGameClientGame gameType, int playStationWinningThreshold,
		unsigned char playStationWinningCount, const char* centerVersion);
	void on_protocol_version(int version);
	void on_jackpot_type(int type);

	// common
	void on_no_more_bets();
	void on_last_bets();
	void on_game_time(float remainingTime, float gameTime, float lastBetsTime);


	// roulette
	void on_roulette_result(int result);
	void on_roulette_history(Array history);
};


/* Connects to PID protocol and loads all functions. It also calls Initialize at start
   but everything else is left for connection. */
class ProtocolPID2 : public Reference {
	GDCLASS(ProtocolPID2, Reference);

private:
	void* _pLibrary;


	FCasinoGameClientInitialize _fpInitialize;
	FCasinoGameClientFinalize _fpFinalize;

public:
	// Loaded library's pointers, call directly from connection code
	FCasinoGameClientUpdate _fpUpdate;
	FCasinoGameClientSetCallback _fpSetCallback;
	FCasinoGameClientCreate _fpCreate;
	FCasinoGameClientDelete _fpDelete;

	FCasinoGameClientIsConnected _fpIsConnected;
	FCasinoGameClientSendTicketID _fpSetTicketID;
	FCasinoGameClientSendInit _fpSendInit;
	FCasinoGameClientSetHistorySize _fpSetHistorySize;
	FCasinoGameClientAskForHistory _fpAskForHistory;
	FCasinoGameClientSideBetsLastWin _fpSideBetsLastWin;
	FCasinoGameClientIsDeviceSet _gpIsDeviceSet;
	FCasinoGameClientSubscribe _fpSubscribe;
	FCasinoGameClientPing _fpPing;
	FCasinoGameClientProposeProtocolVersion _fpProposeProtocolVersion;

protected:
	static void _bind_methods();

public:
	ProtocolPID2();
	~ProtocolPID2();

	void update();

	Ref<ProtocolPID2Connection> create_connection(String id, String ip, int port);
};





#endif


