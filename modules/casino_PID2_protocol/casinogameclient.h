/////////////////////////////////////////////////////////////////
//
// CasinoGameClient
// Created 27.6.2013
// (c)2013 IBZoot
//
/////////////////////////////////////////////////////////////////
#ifndef __CASINOGAMECLIENT__
#define __CASINOGAMECLEINT__

/////////////////////////////////////////////////////////////////
// Forward declarations
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// Commands & Variables
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
// EGame
/////////////////////////////////////////////////////////////////
enum ECasinoGameClientGame {
   cgcgInvalid = 0,
   cgcgSicBo = 1,
   cgcgRoulette0 = 2,
   cgcgRoulette00 = 3,
   cgcgBaccarat = 4,
   cgcgBlackJack = 5,
   cgcgCraps = 6,
   cgcgKeno = 7,
   cgcgHorse = 8,
   cgcgFSC = 9,
   cgcgBig3Six = 10,
   cgcgPopNPoker = 11,
   cgcgMultihandBlackJack = 12,
   cgcgLast,
};

/////////////////////////////////////////////////////////////////
// ECasinoGameClientCallback
/////////////////////////////////////////////////////////////////
enum ECasinoGameClientCallback {
   cgccLog = 0,
   cgccError,
   cgccAbort,
   /////////////////////////////////////////////////////////////////
   // Common
   /////////////////////////////////////////////////////////////////
   cgccOnInit,
   cgccOnLastBets,
   cgccOnNoMoreBets,
   cgccOnErrorMessageDEPRECATED, // Deprecated, but slot left alone for version compatibility
   cgccOnInfoMessage,
   cgccOnResultDisplayFinished,
   cgccOnCenterData,
   cgccOnCenterColor,
   cgccOnResultDelay,
   cgccOnGameTime,
   cgccOnBetDEPRECATED, // Deprecated, but slot left alone for version compatibility
   cgccOnShutdown,
   cgccOnFerrariTicket,
   cgccOnTableClosedDEPRECATED, // Deprecated, but slot left alone for version compatibility
   cgccOnSideBetResult,
   cgccOnJackpotType,
   cgccOnJackpotState,
   cgccOnWinningPlaystations,
   /////////////////////////////////////////////////////////////////
   // Baccarat
   /////////////////////////////////////////////////////////////////
   cgccOnBaccaratCard,
   cgccOnBaccaratFinalResult,
   cgccOnBaccaratPuck,
   cgccOnBaccaratDecisionDone,
   cgccOnBaccaratClearTableDEPRECATED, // Deprecated, but slot left alone for version compatibility
   cgccOnBaccaratCenterHistoryDEPRECATED, // Deprecated, but slot left alone for version compatibility
   /////////////////////////////////////////////////////////////////
   // BlackJack
   /////////////////////////////////////////////////////////////////
   cgccOnBlackJackCard,
   cgccOnBlackJackFinalResult,
   cgccOnBlackJackPlayerDecision,
   cgccOnBlackJackSetPropertiesDEPRECATED,  // Deprecated, but slot left alone for version compatibility
   cgccOnBlackJackActivePlayer,
   cgccOnBlackJackClearCardsDEPRECATED,  // Deprecated, but slot left alone for version compatibility
   cgccOnBlackJackPlayerDone,
   /////////////////////////////////////////////////////////////////
   // Craps
   /////////////////////////////////////////////////////////////////
   cgccOnCrapsDiceResultDEPRECATED,  // Deprecated, but slot left alone for version compatibility
   cgccOnCrapsHardways,
   cgccOnCrapsCenterHistoryDEPRECATED,  // Deprecated, but slot left alone for version compatibility
   /////////////////////////////////////////////////////////////////
   // Roulette
   /////////////////////////////////////////////////////////////////
   cgccOnRouletteResult,
   cgccOnRouletteCenterHistory,
   /////////////////////////////////////////////////////////////////
   // Big3Six
   /////////////////////////////////////////////////////////////////
   cgccOnBig3SixWheelResult,
   cgccOnBig3SixSpinWheel,
   cgccOnBig3SixCenterHistory,
   /////////////////////////////////////////////////////////////////
   // Keno
   /////////////////////////////////////////////////////////////////
   cgccOnKenoResult,
   cgccOnKenoCenterHistory,
   /////////////////////////////////////////////////////////////////
   // PopNPoker
   /////////////////////////////////////////////////////////////////
   cgccOnPopNPokerResult,
   cgccOnPopNPokerCenterHistory,
   /////////////////////////////////////////////////////////////////
   // Touchdown/Goal roulette
   /////////////////////////////////////////////////////////////////
   cgccOnRouletteTouchdown,
   cgccOnRouletteGoal,
   /////////////////////////////////////////////////////////////////
   // Golden chip stuff
   /////////////////////////////////////////////////////////////////
   cgccOnJackpotValue,
   cgccOnJackpotWinConditionDEPRECATED,  // Deprecated, but slot left alone for version compatibility
	/////////////////////////////////////////////////////////////////
	// New callbacks
	/////////////////////////////////////////////////////////////////
	cgccOnErrorMessage,
	cgccOnTableClosed,
   cgccOnBlackJackClearCards,
   cgccOnBlackJackSetProperties,
   cgccOnBaccaratClearTable,
   cgccOnBaccaratCenterHistory,
   cgccOnBet,
   cgccOnJackpotWinCondition,
   /////////////////////////////////////////////////////////////////
   // Unity update
   /////////////////////////////////////////////////////////////////
   cgccOnCrapsDiceResult,
   cgccOnCrapsCenterHistory,
	/////////////////////////////////////////////////////////////////
	// protocol update 20.5.2015
	/////////////////////////////////////////////////////////////////
	cgccOnBlackJackCenterHistory,
	cgccOnRouletteGoldenBall,
	/////////////////////////////////////////////////////////////////
	// protocol version 2.0
	/////////////////////////////////////////////////////////////////
	cgccOnProtocolVersion,
	cgccOnSerialNumber,
	cgccOnRNGType,
	cgccOnPing,
	cgccOnPong,
	cgccOnPlaystationEventPlayerCame,
	cgccOnPlaystationEventPlayerLeft,
	cgccOnPlaystationEventActivityDetected,
	cgccOnPlaystationEventCallAttendent,
	cgccOnPlaystationEventOrderDrink,
	cgccOnPlaystationEventHelpRequested,
	cgccOnPlaystationEventGameActivated,
	cgccOnPlaystationEventGameDeactivated,
	cgccOnPlaystationEventThrowObject,
	cgccOnPlaystationEventCallAttendentWithdrawn,
	cgccOnCenterEventCroupierLogin,
	cgccOnCenterEventCroupierLogout,
	cgccOnCenterEventTip,
	cgccOnCenterEventManualResult,
	cgccOnCenterEventGameCanceled,
	cgccOnStationData,
	cgccOnConnected,
	cgccOnDisconnected,
	cgccOnCenterEventBurntCard,
	cgccOnCenterEventShoeChanged,
	cgccOnCardsDealt,
	cgccOnWinnings,
	cgccOnPlaystationEventLanguageChanged,
	cgccOnCenterEventRFIDCardData,
	cgccOnCenterEventGameChanged,
	cgccOnCenterEventCutCard,
	cgccOnKenoDrawID,
	cgccOnRouletteResultwStatus,
	cgccOnRouletteCenterHistorywStatus,
	cgccOnPlaystationEventGameFocused,
	cgccOnPlaystationEventGameUnfocused,
	cgccOnDiceYouAreTheShooter,
	cgccOnDiceSelected,
	cgccOnDiceShootTheDice,
	cgccOnDiceShot,
	cgccOnDetailedWin,
	cgccOnBlackJackSideBetsLastWinInfo,
	cgccOnPlaystationSettingSoundVolume,
	/////////////////////////////////////////////////////////////////
   // Last
   /////////////////////////////////////////////////////////////////
   cgccLast,
};

/////////////////////////////////////////////////////////////////
// ECasinoGameClientLog
/////////////////////////////////////////////////////////////////
enum ECasinoGameClientLog {
   cgclDev = 0,
   cgclInfo,
   cgclWarning,
   cgclError,
};


/////////////////////////////////////////////////////////////////
// Callbacks
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// System
/////////////////////////////////////////////////////////////////
typedef void (*FLog)(void *callbackUserData, const char *str);
typedef void (*FError)(void *callbackUserData, const char *str);
typedef void (*FAbort)(void *callbackUserData, const char *str);
/////////////////////////////////////////////////////////////////
// Common
/////////////////////////////////////////////////////////////////
typedef void(*FOnInit)(void *callbackUserData, ECasinoGameClientGame gameType, int playStationWinningThreshold, char playStationWinningCount, char *centerVersion);
typedef void (*FOnLastBets)(void *callbackUserData);
typedef void (*FOnNoMoreBets)(void *callbackUserData);
typedef void (*FOnErrorMessage)(void *callbackUserData, int errorLevel, const char *message);
typedef void (*FOnInfoMessage)(void *callbackUserData, const char *message);
typedef void (*FOnResultDisplayFinished)(void *callbackUserData);
typedef void (*FOnCenterData)(void *callbackUserData, int count, float *aColorRGB);
typedef void (*FOnStationData)(void *callbackUserData, int count, unsigned char *aStationData);
typedef void (*FOnCenterColor)(void *callbackUserData, float *color);
typedef void (*FOnRouletteTouchdown)(void *callbackUserData, unsigned char touchdownStage, unsigned char yards);
typedef void (*FOnRouletteGoal)(void *callbackUserData, unsigned char goalStage, unsigned char meters);
typedef void (*FOnResultDelay)(void *callbackUserData, int delay);
typedef void (*FOnGameTime)(void *callbackUserData, int remainingTime, int gameTime, int lastBetsTime);
typedef void (*FOnBet)(void *callbackUserData, int player, int denomination, int betCount, int *bet);
typedef void (*FOnShutdown)(void *callbackUserData, int msg);
typedef void (*FOnFerrariTicket)(void *callbackUserData, unsigned char *astation, int stationCount);
typedef void (*FOnTableClosed)(void *callbackUserData, int status);
typedef void (*FOnSideBetResult)(void *callbackUserData, int result);
typedef void (*FOnJackpotValue)(void *callbackUserData, int type, long long value, int hit);
typedef void (*FOnJackpotType)(void *callbackUserData, int type);
typedef void (*FOnJackpotState)(void *callbackUserData, int state, int additionData);
typedef void (*FOnJackpotWinCondition)(void *callbackUserData, int station, unsigned char *winCondition, int winConditionCount);
typedef void (*FOnWinningPlaystations)(void *callbackUserData, unsigned char *astation, int stationCount);
typedef void (*FOnRouletteGoldenBall)(void *callbackUserData, unsigned char enabled, unsigned char ballColor);
typedef void (*FOnProtocolVersion)(void *callbackUserData, unsigned short version);
typedef void (*FOnSerialNumber)(void *callbackUserData, const char *serial);
typedef void (*FOnRNGType)(void *callbackUserData, unsigned char rngType);
typedef void (*FOnPing)(void *callbackUserData);
typedef void (*FOnPong)(void *callbackUserData);
typedef void (*FOnConnected)(void *callbackUserData);
typedef void (*FOnDisconnected)(void *callbackUserData);
typedef void (*FOnCardsDealt)(void *callbackUserData, int numberOfCardsDealt, int numberOfCardsBurnt);
typedef void(*FOnWinnings)(void *callbackUserData, __int64 totalWin, __int64 maxWin);
typedef void(*FOnDetailedWin)(void *callbackUserData, unsigned char player, unsigned char nrOfBytesForWinAmmount, int dataSize, unsigned char *data);
/////////////////////////////////////////////////////////////////
// Baccarat
/////////////////////////////////////////////////////////////////
typedef void (*FOnBaccaratCard)(void *callbackUserData, int hand, int card, void *cardTexture, int cardIndex, int cardDown, int cardTurn, int ticketID);
typedef void (*FOnBaccaratFinalResult)(void *callbackUserData, int result);
typedef void (*FOnBaccaratPuck)(void *callbackUserData, int player, int banker);
typedef void (*FOnBaccaratDecisionDone)(void *callbackUserData, int value);
typedef void (*FOnBaccaratClearTable)(void *callbackUserData);
typedef void (*FOnBaccaratCenterHistory)(void *callbackUserData, unsigned char *result, int resultCount, int gameNumber);
/////////////////////////////////////////////////////////////////
// BlackJack
/////////////////////////////////////////////////////////////////
typedef void (*FOnBlackJackCard)(void *callbackUserData, int hand, int card, void *cardTexture, int index, int ticketID);
typedef void (*FOnBlackJackFinalResult)(void *callbackUserData, int result);
typedef void(*FOnBlackJackPlayerDecision)(void *callbackUserData, int player, int decision);
typedef void(*FOnBlackJackSetProperties)(void *callbackUserData, int decision, int dealer17, int dealerFaceDown, int hit, int dealerCheckBlackJack, int pays, int doubleAfterSplit, int doubleAllowed);
typedef void(*FOnBlackJackSideBetsLastWinInfo)(void *callbackUserData, int *LastWinTable, int resultCount);
typedef void (*FOnBlackJackActivePlayer)(void *callbackUserData, int player);
typedef void (*FOnBlackJackClearCards)(void *callbackUserData);
typedef void (*FOnBlackJackPlayerDone)(void *callbackUserData, int player);
typedef void (*FOnBlackJackCenterHistory)(void *callbackUserData, unsigned char *result, int resultCount);
/////////////////////////////////////////////////////////////////
// Craps
/////////////////////////////////////////////////////////////////
typedef void (*FOnCrapsDiceResult)(void *callbackUserData, int dice1, int dice2, int dice3, int point);
typedef void(*FOnCrapsHardways)(void *callbackUserData, int gamesSinceLastHard4, int gamesSinceLastHard6, int gamesSinceLastHard8, int gamesSinceLastHard10);
typedef void(*FOnCrapsCenterHistory)(void *callbackUserData, int *result, int resultCount);
typedef void(*FOnDiceYouAreTheShooter)(void *callbackUserData, int player, int timeout);
typedef void(*FOnDiceSelected)(void *callbackUserData, unsigned char selectedDice);
typedef void(*FOnDiceShootTheDice)(void *callbackUserData, int player, int timeout);
typedef void(*FOnDiceShot)(void *callbackUserData);
/////////////////////////////////////////////////////////////////
// Roulette
/////////////////////////////////////////////////////////////////
typedef void(*FOnRouletteResult)(void *callbackUserData, char result);
typedef void(*FOnRouletteResultwStatus)(void *callbackUserData, char result, unsigned char status);
typedef void(*FOnRouletteCenterHistory)(void *callbackUserData, unsigned char *result, int resultCount);
typedef void(*FOnRouletteCenterHistorywStatus)(void *callbackUserData, unsigned char *result, int resultCount);
/////////////////////////////////////////////////////////////////
// Big3Six
/////////////////////////////////////////////////////////////////
typedef void (*FOnBig3SixWheelResult)(void *callbackUserData, int wheel, int result);
typedef void (*FOnBig3SixSpinWheel)(void *callbackUserData, int wheel);
typedef void (*FOnBig3SixCenterHistory)(void *callbackUserData, unsigned char *result, int resultCount);
/////////////////////////////////////////////////////////////////
// Keno
/////////////////////////////////////////////////////////////////
typedef void (*FOnKenoResult)(void *callbackUserData, char result);
typedef void (*FOnKenoDrawID)(void *callbackUserData, const char *drawID);
typedef void (*FOnKenoCenterHistory)(void *callbackUserData, unsigned char *result, int resultCount);
/////////////////////////////////////////////////////////////////
// PopNPoker
/////////////////////////////////////////////////////////////////
typedef void (*FOnPopNPokerResult)(void *callbackUserData, char result);
typedef void (*FOnPopNPokerCenterHistory)(void *callbackUserData, unsigned char *result, int resultCount);
/////////////////////////////////////////////////////////////////
// Playstation Events
/////////////////////////////////////////////////////////////////
typedef void(*FOnPlaystationEventPlayerCame)(void *callbackUserData, unsigned char playstation);
typedef void(*FOnPlaystationEventPlayerLeft)(void *callbackUserData, unsigned char playstation);
typedef void(*FOnPlaystationEventActivityDetected)(void *callbackUserData, unsigned char playstation);
typedef void(*FOnPlaystationEventCallAttendent)(void *callbackUserData, unsigned char playstation);
typedef void(*FOnPlaystationEventOrderDrink)(void *callbackUserData, unsigned char playstation);
typedef void(*FOnPlaystationEventHelpRequested)(void *callbackUserData, unsigned char playstation);
typedef void(*FOnPlaystationEventGameActivated)(void *callbackUserData, unsigned char playstation, unsigned char gameType);
typedef void(*FOnPlaystationEventGameDeactivated)(void *callbackUserData, unsigned char playstation, unsigned char gameType);
typedef void(*FOnPlaystationEventThrowObject)(void *callbackUserData, unsigned char playstation, unsigned char objectID);
typedef void(*FOnPlaystationEventCallAttendentWithdrawn)(void *callbackUserData, unsigned char playstation);
typedef void(*FOnPlaystationEventLanguageChanged)(void *callbackUserData, unsigned char playstation, unsigned char languageID);
typedef void(*FOnPlaystationEventGameFocused)(void *callbackUserData, unsigned char playstation, unsigned char gameType);
typedef void(*FOnPlaystationEventGameUnfocused)(void *callbackUserData, unsigned char playstation, unsigned char gameType);
/////////////////////////////////////////////////////////////////
// Playstation Setting Values Events
/////////////////////////////////////////////////////////////////
typedef void(*FOnPlaystationSettingSoundVolume) (void *callbackUserData, unsigned char valueSize, int nrOfValues, unsigned char *aVolumeSettings);
/////////////////////////////////////////////////////////////////
// Center Events
/////////////////////////////////////////////////////////////////
typedef void(*FOnCenterEventCroupierLogin)(void *callbackUserData, unsigned char *croupierID, int croupierIDLength, const char *croupierName);
typedef void(*FOnCenterEventCroupierLogout)(void *callbackUserData, unsigned char *croupierID, int croupierIDLength, const char *croupierName);
typedef void(*FOnCenterEventTip)(void *callbackUserData, unsigned char playstation, unsigned char *croupierID, int croupierIDLength, int tip);
typedef void(*FOnCenterEventManualResult)(void *callbackUserData);
typedef void(*FOnCenterEventGameCanceled)(void *callbackUserData);
typedef void(*FOnCenterEventBurntCard)(void *callbackUserData, int card, int numberOfCards);
typedef void(*FOnCenterEventShoeChanged)(void *callbackUserData);
typedef void(*FOnCenterEventRFIDCardData)(void *callbackUserData, unsigned char actionType, unsigned char croupierLevel, unsigned char *croupierID, int croupierIDLength,
														unsigned char *password, int passwordLength, const char *lastUpdateDateTime, unsigned char *lastCroupierID, int lastCroupierIDLength, const char *croupierName);
typedef void(*FOnCenterEventGameChanged)(void *callbackUserData, ECasinoGameClientGame gameType, unsigned char switchMode);
typedef void(*FOnCenterEventCutCard)(void *callbackUserData);


/////////////////////////////////////////////////////////////////
//
// CasinoGameClient interface
//
/////////////////////////////////////////////////////////////////
typedef int (*FCasinoGameClientInitialize)();
typedef int (*FCasinoGameClientFinalize)();
typedef void (*FCasinoGameClientUpdate)();
typedef void (*FCasinoGameClientSetCallback)(ECasinoGameClientCallback callback, void *callbackFunction);
typedef void (*FCasinoGameClientSetTCPTimeout)(int timeoutMs);
typedef void *(*FCasinoGameClientCreate)(ECasinoGameClientGame game, const char *ip, int port, int isHologram, const char *name, void *callbackUserData);
typedef void (*FCasinoGameClientDelete)(void *handle);
typedef int (*FCasinoGameClientIsConnected)(void *handle);
typedef void (*FCasinoGameClientSendTicketID)(void *handle, int ticketID);
typedef void (*FCasinoGameClientSendInit)(void *handle);
typedef void(*FCasinoGameClientSetHistorySize)(void *handle, unsigned short historySize);
typedef void(*FCasinoGameClientAskForHistory)(void *handle);
typedef void(*FCasinoGameClientSideBetsLastWin)(void *handle);
typedef void (*FCasinoGameClientSetLogLevel)(ECasinoGameClientLog _logLevel);
typedef void (*FCasinoGameClientIsDeviceSet)();
typedef void (*FCasinoGameClientSubscribe)(void *handle, unsigned char command, unsigned char maskLength, unsigned char *mask);
typedef void (*FCasinoGameClientPing)(void *handle);
/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
static const char *getCasinoGameName(ECasinoGameClientGame game)
{
   switch( game )
   {
      case cgcgSicBo:
         return "sicBo";
      case cgcgRoulette0:
         return "roulette0";
      case cgcgRoulette00:
         return "roulette00";
      case cgcgBaccarat:
         return "baccarat";
      case cgcgBlackJack:
         return "blackJack";
      case cgcgCraps:
         return "craps";
      case cgcgKeno:
         return "keno";
      case cgcgHorse:
         return "horse";
      case cgcgFSC:
         return "fsc";
      case cgcgBig3Six:
         return "big3Six";
      case cgcgPopNPoker:
         return "popNPoker";
      case cgcgMultihandBlackJack:
         return "multihandBlackJack";
   };
   return "";
}

#endif
/////////////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////////
