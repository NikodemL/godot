#pragma once

#ifndef T9Sender_DLL_EXPORT
	#define T9API __declspec( dllimport ) HRESULT __stdcall 
#else
	#define T9API __declspec( dllexport ) HRESULT __stdcall 
#endif

#define MAX_SCREEN_NUMBER	256
typedef DWORD HRCVPARAM;
typedef DWORD HSCPARAM;


#define SENDCARD_USB_PORT	1025


//Comunication Type define
#define COMUNICATION_TYPE_INVALID	0x00	
#define COMUNICATION_TYPE_NET		0x01	
#define COMUNICATION_TYPE_COM		0x02	//´®żÚ·˘ËÍ
#define COMUNICATION_TYPE_ANY		0xFF	//ČÎŇ»ĐÎĘ˝


//Block define
#define BLOCK_NONE				0
#define BLOCK_ALL				0xffffffff
#define BLOCK_SEND_PIC			0x01
#define BLOCK_SEND_CTRL			0x02
#define BLOCK_ASYN_WRITEFUN		0x04
#define BLOCK_RCV_ANSWER		0x08
#define BLOCK_COEF_PIC			0x10	//ÖđµăĐŁŐýÍĽĆ¬

#define UP_OLD_PROGRAM			0x01
#define UP_NEW_PROGRAM			0x02
#define UP_ALL_PROGRAM			0xff


// ¸÷ÖÖ»Řµ÷ĎűĎ˘¶¨Ňĺ
#define CBMSG_DROPFRAM				1		//Ířż¨·˘ËÍËŮ¶Č¸ú˛»ÉĎŁ¬łöĎÖ¶ŞÖˇŁ¬´ËĘ±lParamÎŢŇâŇĺ

#define CBMSG_RECEIVED_NIC_DATA		2		//´Ó˝ÓĘÜż¨ĘŐµ˝ÁËÍřÂçĘýľÝŁ¬lParamŁ¬ÎŞÖ¸ĎňSReceivedNicDataµÄÖ¸Őë
struct SReceivedNicData
{
	LPBYTE dataAddress;
	int	   dataLen;
};

#define CBMSG_SAVE_HWParamToRcv		3		//±Ł´ć˛ÎĘýµ˝˝ÓĘÜż¨˝ř¶Č±¨¸ćĐĹĎ˘Ł¬
											//lParam¸ß16ÎŞ×´Ě¬ĐĹĎ˘Ł¬0ŐýÔÚ˝řĐĐŁ¬1łÉą¦Ł¬2Ę§°ÜŁ¬µÍ8Î»ÎŞ˝ř¶Č°Ů·Ö±ČˇŁ

#define CBMSG_Read_HWParamFromRcv	4		//´Ó˝ÓĘŐż¨¶ÁČˇ˛ÎĘý˝ř¶Č±¨¸ćĐĹĎ˘Ł¬
											//lParam¸ß8ÎŞ×´Ě¬ĐĹĎ˘Ł¬0ŐýÔÚ˝řĐĐŁ¬1łÉą¦Ł¬2Ę§°ÜˇŁ
											//lParam´Î¸ß8ÎŞż¨ĐÍşĹ
											//lParamµÍ16Î»µÄ¸ß°ËÎ»ÎŞ¸ß°ć±ľşĹŁ¬µÍ8Î»ÎŞµÍ°ć±ľşĹˇŁ

#define CBMSG_Read_FPGAFromRcv		5		//´Ó˝ÓĘŐż¨¶ÁČˇFPGAłĚĐň˝ř¶Č±¨¸ćĐĹĎ˘Ł¬
											//lParam¸ß8Î»×´Ě¬ĐĹĎ˘Ł¬0ŐýÔÚ˝řĐĐŁ¬1łÉą¦Ł¬2Ę§°Ü
											//lParam´Î¸ß8Î»ÎŞż¨ĐÍşĹ
											//lParamµÍ16Î»µÄ¸ß°ËÎ»ÎŞ¸ß°ć±ľşĹŁ¬µÍ8Î»ÎŞµÍ°ć±ľşĹˇŁ

#define CBMSG_Upgrade_FPGAFromRcv	6		//·˘ËÍFPGAłĚĐňµ˝˝ÓĘŐż¨µÄ˝ř¶Č±¨¸ćĐĹĎ˘Ł¬
											//lParam¸ß8Î»ÎŞ×´Ě¬ĐĹĎ˘Ł¬0ŐýÔÚ˝řĐĐŁ¬1łÉą¦Ł¬2Ę§°Ü
											//lParam´Î¸ß8Î»ÎŞ˝ř¶ČĐĹĎ˘Ł¬1żŞĘĽŁ¬2˛ÁłýŁ¬3Đ´ČëŁ¬4ĐŁŃéŁ¬0şÍÇ°Ň»´ÎĎŕÍ¬
											//lParam×îµÍ16Î»ÎŞż¨şĹ

#define CBMSG_Detected_RCV_Card		7		//Ě˝˛âµ˝ÁË˝ÓĘÜż¨Ł¬
											//lParam¸ß16ÎŞ×´Ě¬ĐĹĎ˘Ł¬0żŞĘĽĚ˝˛âŁ¬1łÉą¦Ł¬2Ę§°ÜŁ¬3Ě˝˛âµ˝Ł¬´ÎµÍ°ËÎ»ÎŞ°ć±ľĐĹĎ˘Ł¬µÍ8Î»ÎŞ˛ÎĘýż¨ĐňşĹŁ¨łÉą¦Ę±ÎŞż¨ĘýÁżŁ©

#define CBMSG_AsynWriteFunCard_Rslt	8		//Ňě˛˝Đ´ą¦ÄÜż¨˝áąű
											//lParam¸ßÎŞ×´Ě¬ĐĹĎ˘Ł¬0Ę§°ÜŁ¬1łÉą¦Ł¬2ł¬Ę±

#define CBMSG_SendCorrectCoef2Rcv   9       //·˘ËÍĐŁŐýĘýľÝµ˝˝ÓĘŐż¨µÄ˝ř¶ČĐĹĎ˘Ł¬
                                            //lParam¸ß16Î»ÎŞ×´Ě¬ĐĹĎ˘Ł¬0ŐýÔÚ˝řĐĐŁ¬1łÉą¦Ł¬2Ę§°Ü
                                            //lParamµÍ16Î»ÎŞ˝ř¶ČĐĹĎ˘Ł¬0~100

#define CBMSG_Sending_FPGA_OP		10		//·˘ËÍż¨˛Ů×÷˝ř¶Č±¨¸ćĐĹĎ˘Ł¬
											//lParam¸ß8Î»ÎŞ×´Ě¬ĐĹĎ˘Ł¬0ŐýÔÚ˝řĐĐŁ¬1łÉą¦Ł¬2Ę§°Ü
											//lParam´Î¸ß8Î»ÎŞ˝ř¶ČĐĹĎ˘Ł¬1żŞĘĽŁ¬2˛ÁłýŁ¬3Đ´ČëŁ¬4¶ÁČˇŁ¬0şÍÇ°Ň»´ÎĎŕÍ¬
											//lParam×îµÍ16Î»ÎŢŇâŇĺ

#define CBMSG_Open_WinpcapError		11		//Open WinPcapError, lParamÖ¸Ďňłö´íĚáĘľµÄ×Ö·ű´®(char*)

#define CBMSG_Save_HWParamToSend  12		//·˘ËÍ˛ÎĘýµ˝·˘ËÍż¨µÄ˝ř¶Č±¨¸ćĎűĎ˘
											//lParam¸ß16ÎŞ×´Ě¬ĐĹĎ˘Ł¬0ŐýÔÚ˝řĐĐŁ¬1łÉą¦Ł¬2Ę§°ÜŁ¬µÍ8Î»ÎŞ˝ř¶Č°Ů·Ö±ČˇŁ


/*************************************************
* T9SenderCallback
* ą¦ÄÜËµĂ÷Łş
    »Řµ÷şŻĘýŁ¬˛ÎĽű¸÷ÖÖ»Řµ÷ĎűĎ˘¶¨Ňĺ

* ˛ÎĘýËµĂ÷Łş
.   [in]	wParam
	ĎűĎ˘ŔŕĐÍ
.	[in]	lParam
	ĎűĎ˘Öµ
.	[in]	data
	ĆäËű

* ·µ»ŘÖµËµĂ÷Łş

*************************************************/
typedef HRESULT (__stdcall *T9SenderCallback)(WPARAM wParam, LPARAM lParam, DWORD data);


extern "C"{

/*************************************************
* T9GetNetAdapterCount
* ą¦ÄÜËµĂ÷Łş
    »ńČˇ»úĆ÷ÉĎŇŃľ­°˛×°µÄÍřż¨ĘýÁż

* ˛ÎĘýËµĂ÷Łş
.   [out]	pCount
	·µ»ŘÍřż¨ĘýÁż

* ·µ»ŘÖµËµĂ÷Łş
.   T9_S_OK
	łÉą¦ 
*************************************************/
T9API	T9GetNetAdapterCount(DWORD *pCount);


/*************************************************
* T9GetNetAdapterInfo
* ą¦ÄÜËµĂ÷Łş
    »ńČˇÍřż¨ĐĹĎ˘

* ˛ÎĘýËµĂ÷Łş 
.	[in]	index
	Ířż¨Ë÷ŇýşĹ
.	[out]	pAdapterId
	·µ»Ř¸ĂÍřż¨µÄidŁ¬pAdapterIdÖ¸ĎňµÄÄÚ´ć´óĐˇ˛»µĂĐˇÓÚ260¸ö×Ö˝Ú
.	[in]	adapterIdBufLen
    pAdapterIdĘµĽĘÖ¸ĎňµÄÄÚ´ć´óĐˇ
.   [out]	pAdapterDesp
	·µ»Ř¸ĂÍřż¨µÄĂčĘöĐĹĎ˘Ł¬pAdapterDespÖ¸ĎňµÄÄÚ´ć´óĐˇ˛»µĂĐˇÓÚ260¸ö×Ö˝Ú
.	[in]	adapterDespBufLen
	adapterDespBufLenÖ¸ĎňµÄÄÚ´ć´óĐˇ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_Adapter_Not_Exist
	ÎŢ·¨»ńČˇ¸ĂÍřż¨µÄĐĹĎ˘
.	T9E_Buffer_Overflow
	pAdapterId»ňŐßpAdapterDespÖ¸ĎňµÄÄÚ´ćĚ«Đˇ
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9GetNetAdapterInfo(DWORD	index,
							char	*pAdapterId,
							DWORD	adapterIdBufLen,	//Đč´óÓÚ»ňŐßµČÓÚ260
							char	*pAdapterDesp,
							DWORD	adapterDespBufLen);	//Đč´óÓÚ»ňŐßµČÓÚ260


/*************************************************
* T9CreateScreen
* ą¦ÄÜËµĂ÷Łş
    ´´˝¨Ň»¸öĆÁÄ»

* ˛ÎĘýËµĂ÷Łş 
.   [in]	number
	ĐÂ´´˝¨ĆÁÄ»µÄ±ŕşĹ
.	[in]	width
	ĆÁÄ»żí¶Č					şĎŔíČˇÖµ·¶Î§ 1 - 1024*1024
.	[in]	height
	ĆÁÄ»¸ß¶Č					şĎŔíČˇÖµ·¶Î§ 1 - 1024*1024
.	[in]	bVirtual
    ĘÇ·ńĘÇĐéÄâĆÁ
.	[in]	virtualType
	ĐéÄâĆÁŔŕĐÍ				şĎŔíČˇÖµ·¶Î§0~3       0 vpa_RA_G__B_RB   1 vpa_RA_B__G_RB   2 vpa_G_RA__RB_B   3 vpa_B_RA__RB_G
.	[in]	connectionType
	˝ÓĘŐż¨µÄĽ¶ÁŞ·˝Ďň	şĎŔíČˇÖµ·¶Î§0~3      0 cs_right_to_left    1 cs_left_to_right    2 cs_top_to_bottom  3 cs_bottom_to_top
* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_InvalidNumber
	´íÎóµÄĆÁ±ŕşĹŁ¬şĎŔí±ŕşĹ·¶Î§ÎŞ1µ˝MAX_SCREEN_NUMBER
.	T9E_ScreenAlreadyExist
	¸Ă±ŕşĹµÄĆÁŇŃľ­´´˝¨
.	T9E_NoImplement
	ą¦ÄÜÎ´ĘµĎÖ
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9CreateScreen(DWORD	number,
					   DWORD	width,
					   DWORD	height,
					   BOOL		bVirtual, 
					   int		virtualType, 
					   int		connectionType);


/*************************************************
* T9DestroyScreen
* ą¦ÄÜËµĂ÷Łş
	ÉľłýŇ»¸öĆÁÄ»

* ˛ÎĘýËµĂ÷Łş 
.   [in]	number
	ĐčŇŞÉľłýµÄĆÁÄ»µÄ±ŕşĹ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_InvalidScreen
	Î´Ôř´´˝¨¸Ă±ŕşĹµÄĆÁÄ»
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9DestroyScreen(DWORD	number);

/*************************************************
* T9DestroyAllScreen
* ą¦ÄÜËµĂ÷Łş
	ÉľłýËůÓĐĆÁÄ»

* ˛ÎĘýËµĂ÷Łş 


* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9DestroyAllScreen();

/*************************************************
* T9GetScreenInfo
* ą¦ÄÜËµĂ÷Łş
	¸ůľÝĆÁÄ»idşĹ»ńČˇĆÁÄ»ĐĹĎ˘

* ˛ÎĘýËµĂ÷Łş 
.   [in]	number
	ĐčŇŞ»ńČˇĐĹĎ˘µÄĆÁÄ»±ŕşĹ
.   [out]	pWidth 
	·µ»ŘĆÁÄ»żí¶Č
.   [out]	pHeight
	·µ»ŘĆÁÄ»¸ß¶Č
.   [out]	pbVirtual
	·µ»ŘĆÁÄ»ĘÇ·ńĘÇĐéÄâĆÁ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_InvalidScreen
	Î´Ôř´´˝¨¸Ă±ŕşĹµÄĆÁÄ»
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9GetScreenInfo(DWORD	number,
						DWORD	*pWidth,
						DWORD	*pHeight,
						BOOL	*pbVirtual);


/*************************************************
* T9GetScreenCount
* ą¦ÄÜËµĂ÷Łş
	»ńČˇŇŃľ­´´˝¨µÄĆÁÄ»ĘýÁż

* ˛ÎĘýËµĂ÷Łş 
.   [out]	pCount
	ŇŃľ­´´˝¨µÄĆÁÄ»ĘýÁż

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9GetScreenCount(DWORD *pCount);


/*************************************************
* T9GetScreenNumber
* ą¦ÄÜËµĂ÷Łş
	¸ůľÝĆÁÄ»Ë÷ŇýşĹ»ńČˇĆÁÄ»id

* ˛ÎĘýËµĂ÷Łş 
.   [in]	index
	ĆÁÄ»Ë÷ŇýşĹŁ¬Ë÷ŇýşĹÇřĽäÎŞ[0, ĆÁÄ»ĘýÁżĽő1]
.   [out]	pNumber
	·µ»ŘĆÁÄ»±ŕşĹ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_InvalidIndex
	ÎŢĐ§ĆÁÄ»Ë÷ŇýşĹŁ¬˛ÎĽűindex˛ÎĘýËµĂ÷
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9GetScreenNumber(	DWORD	index,
							DWORD	*pNumber);

/*************************************************
* T9SetScreenVirtualType
* ą¦ÄÜËµĂ÷Łş
	ÉčÖĂĐéÄâĆÁÄ»µÄŔŕĐÍ

* ˛ÎĘýËµĂ÷Łş 
.   [in]  number
	ĐčŇŞÉčÖĂµÄĆÁÄ»µÄĆÁÄ»şĹ   

.   [in]  virtualType					şĎŔíČˇÖµ·¶Î§0~3       0 vpa_RA_G__B_RB   1 vpa_RA_B__G_RB   2 vpa_G_RA__RB_B   3 vpa_B_RA__RB_G
	ĐéÄâĆÁÄ»µÄŔŕĐÍ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_InvalidScreen
	Î´Ôř´´˝¨¸Ă±ŕşĹµÄĆÁÄ»
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SetScreenVirtualType(DWORD number, int virtualType);


/*************************************************
* T9SetScreenConnectionStyle
* ą¦ÄÜËµĂ÷Łş
	ÉčÖĂĐéÄâĆÁÄ»µÄŔŕĐÍ

* ˛ÎĘýËµĂ÷Łş 
.   [in]  number
	ĐčŇŞÉčÖĂµÄĆÁÄ»µÄĆÁÄ»şĹ

.   [in]  connectionStyle				şĎŔíČˇÖµ·¶Î§0~3      0 cs_right_to_left    1 cs_left_to_right    2 cs_top_to_bottom  3 cs_bottom_to_top
	˝ÓĘŐż¨µÄĽ¶ÁŞ·˝Ďň

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_InvalidScreen
	Î´Ôř´´˝¨¸Ă±ŕşĹµÄĆÁÄ»
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SetScreenConnectionStyle(DWORD number, int connectionStyle);

/*************************************************
* T9SenderStart
* ą¦ÄÜËµĂ÷Łş
   łőĘĽ»ŻÍřż¨Ł¬żŞĘĽ·˘ËÍÍĽĎńĘýľÝ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	comunicationType
    Í¨Ń¶ĐÎĘ˝Ł¬ÍřżÚ»ňŐß´®żÚŁ¬ĎęĽűComunication Type define
.	[in]	pParam
	 ¸ůľÝ·˘ËÍĘýľÝµÄ·˝Ę˝˛»Í¬¶ř˛»Í¬Ł¬µ±ŇÔÍřżÚ·˘ËÍĘ±Ł¬pParamÎŞµ±Ç°ĘąÓĂµÄÍřż¨ĂűłĆ Ł»ŇÔ´®żÚ·˘ËÍĘ± pParamÎŞ´®żÚşĹ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_NoScreen
	»ąĂ»ÓĐ´´˝¨ĆÁŁ¬ĐčŇŞĎČ´´˝¨ËůÓĐµÄĆÁ˛ĹÄÜżŞĘĽ·˘ËÍ
.	T9E_OpenNICFailed
	´ňżŞÍřż¨Ę§°Ü
.	T9E_UNKNOWN
	Î´ÖŞ´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SenderStart(DWORD comunicationType, LPVOID pParam);


/*************************************************
* T9SenderStop
* ą¦ÄÜËµĂ÷Łş
    ÍŁÖą·˘ËÍÍĽĎńĘýľÝ

* ˛ÎĘýËµĂ÷Łş 

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SenderStop();

/*************************************************
* T9IsSenderRunning
* ą¦ÄÜËµĂ÷Łş
	ĘÇ·ńÍřÂçĘŐ·˘ÄŁżéŐýÔÚÔËĐĐ

* ˛ÎĘýËµĂ÷Łş 
	comunicationType Í¨Ń¶ĐÎĘ˝Ł¬ÍřżÚ»ňŐß´®żÚŁ¬ĎęĽűComunication Type define

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	ŐýÔÚÔËĐĐ
.	T9_E_FAILE
	Ă»ÓĐÔËĐĐ
*************************************************/
T9API	T9IsSenderRunning(DWORD comunicationType);

/*************************************************
* T9SetBirghtness
* ą¦ÄÜËµĂ÷Łş
      ÉčÖĂÁÁ¶Č

* ˛ÎĘýËµĂ÷Łş 
.	[in]	bright
	ÁÁ¶ČÖµ
.	[in]	bright_r
	şěÉ«µÄÁÁ¶ČÖµ
.	[in]	bright_g
	ÂĚÉ«µÄÁÁ¶ČÖµ
.	[in]	bright_b
	Ŕ¶É«µÄÁÁ¶ČÖµ
.	[in]	bright_vr
	Í¸Ă÷¶Č
.	[in]	colorTempture
	É«ÎÂ
* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SetBirghtness(BYTE bright, BYTE bright_r, BYTE bright_g, BYTE bright_b, BYTE bright_vr, WORD colorTempture);

/*************************************************
* T9SetIsUseRcvParam
* ą¦ÄÜËµĂ÷Łş
      ÉčÖĂĘÇ·ńĘąÓĂ˝ÓĘŐż¨˛ÎĘý

* ˛ÎĘýËµĂ÷Łş
.	[in]	bUseRcvParam
	TRUEŁ¬ĘąÓĂ˝ÓĘŐż¨˛ÎĘýŁ¬FALSEŁ¬ĘąÓĂĽĆËă»úĘµĘ±˛ÎĘý

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
łÉą¦
*************************************************/
T9API	T9SetIsUseRcvParam(BOOL bUseRcvParam);

/*************************************************
* T9SetPcRealTimeParam
* ą¦ÄÜËµĂ÷Łş
	¸Ä±äĽĆËă»úĘµĘ±˛ÎĘý

* ˛ÎĘýËµĂ÷Łş 
.	[in]	lpBuf
	˛ÎĘýżéÄÚ´ćµŘÖ· 
.	[in]	bufLen
	˛ÎĘýżéÄÚ´ćł¤¶Č

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SetPcRealTimeParam(LPBYTE lpBuf, DWORD bufLen);

/*************************************************
* SetSendParamScreenNumber
* ą¦ÄÜËµĂ÷Łş
	ÉčÖĂÔÚÄÄ¸öĆÁ·˘ËÍÍĽĎńĘýľÝÖ®Ç°Ł¬ĐčŇŞĎČ·˘ËÍµÄĆÁ˛ÎĘýľÝ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	number
	ĆÁÄ»±ŕşĹ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_InvalidScreen
	Î´Ôř´´˝¨¸Ă±ŕşĹµÄĆÁÄ»
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SetSendParamScreenNumber(DWORD number);


/*************************************************
* T9SendControlPack
* ą¦ÄÜËµĂ÷Łş
    ·˘ËÍżŘÖĆ˛ÎĘý°ü

* ˛ÎĘýËµĂ÷Łş 
.	[in]	pBuf
	˛ÎĘý°üÄÚ´ćµŘÖ·
.	[in]	bufLen
	˛ÎĘý°üÄÚ´ćł¤¶Č
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_SendThreadNotStart
	·˘ËÍĎßłĚĂ»ÓĐĆô¶Ż
.	T9E_ParamError
	˛ÎĘý´íÎóŁ¬Ö¸µÄ´«¸ř¸ĂşŻĘýµÄ˛ÎĘý´íÎóŁ¬¶ř˛»ĘÇpBufÄÚĘýľÝ´íÎó
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄżŘÖĆĂüÁî±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SendControlPack(LPBYTE pBuf, DWORD bufLen, BYTE portIndex);

/*************************************************
* T9SendRTCSetting
* ą¦ÄÜËµĂ÷Łş
    ·˘ËÍRTCÓĐąŘÉčÖĂ˛ÎĘý

* ˛ÎĘýËµĂ÷Łş 
.	[in]	pBuf
	˛ÎĘý°üÄÚ´ćµŘÖ·
.	[in]	bufLen
	˛ÎĘý°üÄÚ´ćł¤¶Č
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	E_FAIL
	Ę§°Ü
.	T9_NOERROR
	ŐýČ·

*************************************************/
T9API	T9SendRTCSetting(IN LPBYTE pBuf, IN DWORD bufLen, IN BYTE portIndex);

/*************************************************
* T9Ctrl_DetectRcvCardEx
* ą¦ÄÜËµĂ÷Łş
	Ě˝˛â˝ÓĘŐż¨

* ˛ÎĘýËµĂ÷Łş
.	[out] pRcvCount
	·µ»ŘĚ˝˛âµ˝µÄ˝ÓĘÜż¨ĘýÁż
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
	T9E_ParamError
	˛ÎĘý´í
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9Ctrl_DetectRcvCardEx(DWORD *pRcvCount, BYTE portIndex);

/*************************************************
* T9Ctrl_DetectRcvCardEx2
* ą¦ÄÜËµĂ÷Łş
	Ě˝˛â˝ÓĘŐż¨

* ˛ÎĘýËµĂ÷Łş
	[in] rcvIndex
	˝ÓĘŐż¨ĐňşĹŁ¬ČˇÖµ·¶Î§[0~65536]
	[in] pBuf
	ÄÚ´ćµŘÖ·Ł¬ÓĂŔ´´ć´˘˝ÓĘŐµ˝µÄż¨ĐĹĎ˘
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
* ·µ»ŘÖµËµĂ÷Łş
.	E_FAIL
	Ę§°Ü
.	T9E_ParamError
	˛ÎĘý´í
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	NOERROR
	ŐýČ·
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9Ctrl_DetectRcvCardEx2(WORD rcvIndex, LPBYTE pBuf, BYTE portIndex);

/*************************************************
* T9Ctrl_DetectRcvCardEx3
* ą¦ÄÜËµĂ÷Łş
	Ě˝˛â˝ÓĘŐż¨

* ˛ÎĘýËµĂ÷Łş
.	[out] pRcvCount
	·µ»ŘĚ˝˛âµ˝µÄ˝ÓĘÜż¨ĘýÁż
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
	[in] startIndex
	ĆđĘĽż¨şĹ
	[in] count
	Ě˝˛âż¨µÄĘýÁż

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
	T9E_ParamError
	˛ÎĘý´í
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9Ctrl_DetectRcvCardEx3(DWORD *pRcvCount, BYTE portIndex, WORD startIndex, WORD count);

// T9API	T9Ctrl_DetectRcvCardPin(BYTE portIndex, BYTE rcvIndex, SDetectRcvPinAnswer *pRcvPinAnswer);


/*************************************************
* T9Ctrl_DetectRcvCard
* ą¦ÄÜËµĂ÷Łş
	Ě˝˛â˝ÓĘŐż¨

* ˛ÎĘýËµĂ÷Łş
	rcvIndex
	˝ÓĘŐż¨ĐňşĹŁ¬ČˇÖµ·¶Î§[0~65536]
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
	* ·µ»ŘÖµËµĂ÷Łş
	T9E_ParamError
	˛ÎĘý´í
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_SendThreadNotStart
	·˘ËÍĎßłĚĂ»ÓĐĆô¶Ż
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄżŘÖĆĂüÁî±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9Ctrl_DetectRcvCard(WORD rcvIndex, BYTE portIndex);

/*************************************************
* T9ClearMackPacketCount
* ą¦ÄÜËµĂ÷Łş
	Çĺłý˝ÓĘŐż¨ÍřÂç°üĽĆĘý

* ˛ÎĘýËµĂ÷Łş
	rcvIndex
	˝ÓĘŐż¨ĐňşĹŁ¬ČˇÖµ·¶Î§[0~65536]
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	9E_ParamError
	˛ÎĘý´íÎó
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_SendThreadNotStart
	·˘ËÍĎßłĚĂ»ÓĐĆô¶Ż
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄżŘÖĆĂüÁî±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9ClearMackPacketCount(WORD rcvIndex, BYTE portIndex);


/*************************************************
* T9Ctrl_DetectFunCard
* ą¦ÄÜËµĂ÷Łş
	Ě˝˛âą¦ÄÜż¨

* ˛ÎĘýËµĂ÷Łş
.	funCardIndex
	ą¦ÄÜż¨ĐňşĹŁ¬ČˇÖµ·¶Î§[0~254]
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_SendThreadNotStart
	·˘ËÍĎßłĚĂ»ÓĐĆô¶Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄżŘÖĆĂüÁî±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9Ctrl_DetectFunCard(int funCardIndex, BYTE portIndex);


/*************************************************
* T9Ctrl_DetectFunCardEx
* ą¦ÄÜËµĂ÷Łş
	Ě˝˛âą¦ÄÜż¨

* ˛ÎĘýËµĂ÷Łş
.	funCardIndex
	ą¦ÄÜż¨ĐňşĹŁ¬ČˇÖµ·¶Î§[0~254]
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
	[in] pBuf
	ÄÚ´ćµŘÖ·Ł¬ÓĂŔ´´ć´˘˝ÓĘŐµ˝µÄż¨ĐĹĎ˘

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	Ęý´íÎó
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_SendThreadNotStart
	·˘ËÍĎßłĚĂ»ÓĐĆô¶Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄżŘÖĆĂüÁî±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9Ctrl_DetectFunCardEx(int funCardIndex, BYTE portIndex, LPBYTE pBuf);


/*************************************************
* T9Ctrl_5036PixelCheck
* ą¦ÄÜËµĂ÷Łş
	5036ÖđµăĽě˛â

* ˛ÎĘýËµĂ÷Łş
	[in] lpByte
	żŘÖĆĘýľÝÄÚ´ćµŘÖ·Ö¸Őë
	[in] bufLen
	żŘÖĆĘýľÝł¤¶Č
	[in] cardIndex
	ż¨ĐňşĹ         ČˇÖµ·¶Î§[0~2048]

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_SendThreadNotStart
	·˘ËÍĎßłĚĂ»ÓĐĆô¶Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄżŘÖĆĂüÁî±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9Ctrl_5036PixelCheck(LPBYTE lpByte, int bufLen, WORD cardIndex);

/*************************************************
* T9Ctrl_ExecFunPower
* ą¦ÄÜËµĂ÷Łş
	ą¦ÄÜż¨µçÔ´żŘÖĆ

* ˛ÎĘýËµĂ÷Łş
	[in] lpByte
	żŘÖĆĘýľÝÄÚ´ćµŘÖ·Ö¸Őë
	[in] bufLen
	żŘÖĆĘýľÝł¤¶Č

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_SendThreadNotStart
	·˘ËÍĎßłĚĂ»ÓĐĆô¶Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄżŘÖĆĂüÁî±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9Ctrl_ExecFunPower(LPBYTE lpByte, int bufLen);

/*************************************************
* T9Ctrl_ReqParamAndVer
* ą¦ÄÜËµĂ÷Łş
	Ě˝˛âą¦ÄÜż¨

* ˛ÎĘýËµĂ÷Łş
	rcvIndex
	˝ÓĘŐż¨ĐňşĹŁ¬ČˇÖµ·¶Î§[0~65536]

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_SendThreadNotStart
	·˘ËÍĎßłĚĂ»ÓĐĆô¶Ż
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄżŘÖĆĂüÁî±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9Ctrl_ReqParamAndVer(WORD rcvIndex);

/*************************************************
* T9Ctrl_FunCard_FlashOperation
* ą¦ÄÜËµĂ÷ŁşŁ¬
	ą¦ÄÜż¨Flash˛Ů×÷

* ˛ÎĘýËµĂ÷Łş
.	[in]	opType	
	˛Ů×÷ŔŕĐÍ: ČˇÖµÎŞFlashOperateTypeµÄÖµ
.	[in]	highAddress
	˛Ů×÷ÄżµÄ¸ßÎ»µŘÖ·
.	[in]	lowAddress
	˛Ů×÷ÄżµÄµÍÎ»µŘÖ·
.	[in]	lpBuf
	opTypeÎŞflash_op_writeŁ¬Đ´ČëµÄÄÚČÝŁ¬ÎŞ256¸ö×Ö˝Úł¤

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_SendThreadNotStart
	·˘ËÍĎßłĚĂ»ÓĐĆô¶Ż
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄżŘÖĆĂüÁî±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9Ctrl_FunCard_FlashOperation(BYTE	opType, 
									  BYTE	highAddress,
									  BYTE	lowAddress,
									  LPBYTE lpBuf);


/*************************************************
* T9SendScreenPicture
* ą¦ÄÜËµĂ÷Łş
    ·˘ËÍŇ»¸±ÍĽĎńĘýľÝŁ¨Ö±˝Ó·˘ËÍ·˝Ę˝Ł©

* ˛ÎĘýËµĂ÷Łş 
.	[in]	number
	ĆÁÄ»±ŕşĹ
.	[in]	pBuf
	ÍĽĎńĘýľÝÄÚ´ćµŘÖ·

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_InvalidScreen
	Î´Ôř´´˝¨¸Ă±ŕşĹµÄĆÁÄ»
.	T9E_NoScreenBufPool
	Ă»ÓĐ´´˝¨¸ÄĆÁÄ»µÄÍĽĎńÄÚ´ć»şłĺłŘ
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄÍĽĎńĘýľÝ±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SendScreenPicture(DWORD	number,
							LPBYTE	pBuf);

/*************************************************
* T9SendScreenCorrectCoef
* ą¦ÄÜËµĂ÷Łş
   ·˘ËÍÖđµăĐŁŐýĎµĘý

* ˛ÎĘýËµĂ÷Łş 
.	[in]	number
	ĆÁÄ»±ŕşĹ
.	[in]	pBuf
	ÖđµăĐŁŐýĎµĘýµŘÖ·

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_InvalidScreen
	Î´Ôř´´˝¨¸Ă±ŕşĹµÄĆÁÄ»
.	T9E_NoScreenBufPool
	Ă»ÓĐ´´˝¨¸ÄĆÁÄ»µÄÍĽĎńÄÚ´ć»şłĺłŘ
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄÍĽĎńĘýľÝ±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SendScreenCorrectCoef(DWORD	number,
								LPBYTE	pBuf);
/*************************************************
* T9SendScreenBlackPicture
* ą¦ÄÜËµĂ÷Łş
    ·˘ËÍŇ»·ůşÚÉ«µÄÍĽĎń¸řĆÁÄ»

* ˛ÎĘýËµĂ÷Łş 
.	[in]	number
	ĆÁÄ»±ŕşĹ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_InvalidScreen
	Î´Ôř´´˝¨¸Ă±ŕşĹµÄĆÁÄ»
.	T9E_NoScreenBufPool
	Ă»ÓĐ´´˝¨¸ÄĆÁÄ»µÄÍĽĎńÄÚ´ć»şłĺłŘ
.	T9_S_FALSE
	ÓÉÓÚÄÚ˛ż×čČűŁ¬·˘ËÍµÄÍĽĎńĘýľÝ±»Ĺ×Ćú
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SendScreenBlackPicture(DWORD	number);

/*************************************************
* T9SetCallBackWnd
* ą¦ÄÜËµĂ÷Łş
    ÉčÖĂ˝ÓĘÜµ×˛ăÄŁżéÍ¨ÖŞµÄ´°żÚľä±úŁ¬·˘ËÍÄŁżéÍ¨ąý´°żÚĎűĎ˘µÄ·˝Ę˝Í¨ÖŞÉĎ˛ăËüµÄ¸÷ÖÖĐĹĎ˘

* ˛ÎĘýËµĂ÷Łş 
.	[in]	hWnd
	´°żÚľä±ú
.	[in]	msgId
	·˘ËÍÄŁżé¸ř´°żÚ·˘ËÍ´°żÚĎűĎ˘µÄIDşĹ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
// T9API	T9SetCallBackWnd(HWND hWnd, DWORD msgId);


/*************************************************
* T9SetCallBackFun
* ą¦ÄÜËµĂ÷Łş
    ÉčÖĂ»Řµ÷şŻĘýŁ¬·˘ËÍÄŁżéµ÷ÓĂ»Řµ÷şŻĘýÍ¨ÖŞÉĎ˛ă¸÷ÖÖĐĹĎ˘

* ˛ÎĘýËµĂ÷Łş 
.	[in]	pCallbackFun
	»Řµ÷şŻĘýÖ¸ŐëŁ¬ČçąűpCallbackFunµČÓÚNULLŁ¬Çĺłý»Řµ÷şŻĘýÉč¶¨

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SetCallBackFun(T9SenderCallback pCallbackFun);

/*************************************************
* T9WriteHWParamToRcvCard
* ą¦ÄÜËµĂ÷Łş
    ·˘ËÍ˛ÎĘýµ˝˝ÓĘŐż¨

* ˛ÎĘýËµĂ÷Łş 
.	[in]	pBuf
	Ó˛Ľţ˛ÎĘýÄÚ´ćµŘÖ·
.	[in]	bufLen
	Ó˛Ľţ˛ÎĘýÄÚ´ćł¤¶Č
.	[in]	scrnIndex
	ĆÁÄ»Ë÷ŇýşĹ
.	[in]	cardCountŁ¬ČˇÖµ·¶Î§[1~2047]
	ż¨ĘýÁż
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	markedOnly
	ŁżŁżŁż

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9WriteHWParamToRcvCard(LPBYTE	pBuf, 
								DWORD	bufLen, 
								int		scrnIndex,
								int		skipCount, 
								int		cardCount, 
								BYTE	portIndex,
								BOOL	markedOnly);


/*************************************************
* T9WriteControlAreaToRcvCard
* ą¦ÄÜËµĂ÷Łş
    ·˘ËÍ˛ÎĘýµ˝˝ÓĘŐż¨

* ˛ÎĘýËµĂ÷Łş 
.	[in]	pBuf
	Ó˛Ľţ˛ÎĘýÄÚ´ćµŘÖ·
.	[in]	bufLen
	Ó˛Ľţ˛ÎĘýÄÚ´ćł¤¶Č
.	[in]	scrnIndex
	ĆÁÄ»Ë÷ŇýşĹ
.	[in]	skipCount
	ĚřąýÇ°ĂćµÄż¨˛»ÉčÖĂŁ¬skipCountÎŞĚřąýµÄż¨ĘýÁż
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	markedOnly
	ŁżŁżŁż

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9WriteControlAreaToRcvCard(LPBYTE	pBuf, 
									DWORD	bufLen, 
									int		scrnIndex,
									int		skipCount, 
									int		cardCount, 
									BYTE	portIndex,
									BOOL	markedOnly);


/*************************************************
* T9WriteHWParamToModule
* ą¦ÄÜËµĂ÷Łş
    ·˘ËÍ˛ÎĘýµ˝ÄŁ×é

* ˛ÎĘýËµĂ÷Łş 
.	[in]	pBuf
	Ó˛Ľţ˛ÎĘýÄÚ´ćµŘÖ·
.	[in]	bufLen
	Ó˛Ľţ˛ÎĘýÄÚ´ćł¤¶Č
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9WriteHWParamToModule(	LPBYTE	pBuf, 
								DWORD	bufLen, 
								BYTE	portIndex);

/*************************************************
* T9WriteHWParamToFunCard
* ą¦ÄÜËµĂ÷Łş
	·˘ËÍ˛ÎĘýµ˝ą¦ÄÜż¨

* ˛ÎĘýËµĂ÷Łş 
.	[in]	pBuf
	Ó˛Ľţ˛ÎĘýÄÚ´ćµŘÖ·
.	[in]	bufLen
	Ó˛Ľţ˛ÎĘýÄÚ´ćł¤¶Č
.	[in]	scrnIndex
	ĆÁÄ»Ë÷ŇýşĹ
.	[in]	skipCount
	ĚřąýÇ°ĂćµÄż¨˛»ÉčÖĂŁ¬skipCountÎŞĚřąýµÄż¨ĘýÁż
.	[in]	funIndexŁ¬ČˇÖµ·¶Î§[0~254]
	ż¨ĘýÁż

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9WriteHWParamToFunCard(LPBYTE pBuf, DWORD bufLen, int funIndex);

/*************************************************
* T9ReadHWParamToFile
* ą¦ÄÜËµĂ÷Łş
    ´Ó˝ÓĘÜż¨ÖĐ¶Á»Ř˛ÎĘý´ćČëÖ¸¶¨ÎÄĽţÖĐ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	pFileName
	±Ł´ć˛ÎĘýÎÄĽţµÄÂ·ľ¶Ăű
.	[in]	cardIndexŁ¬ČˇÖµ·¶Î§[0~254]
	˝ÓĘŐż¨şĹ
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_NicReadTimeOut
	¶ÁĘýľÝł¬Ę±
.	T9E_NoNicDataReceived
	ÓĐĘýľÝÎ´¶Áłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9ReadHWParamToFile(char *pFileName, WORD cardIndex, BYTE portIndex);

/*************************************************
* T9ReadHWParamToBuffer
* ą¦ÄÜËµĂ÷Łş
    ´Ó˝ÓĘÜż¨ÖĐ¶Á»Ř˛ÎĘý´ćČëÖ¸¶¨ÎÄĽţÖĐ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	cardIndexŁ¬ČˇÖµ·¶Î§[0~254]
	˝ÓĘŐż¨şĹ
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	lpBuffer
	´ć´˘˛ÎĘýÎÄĽţµÄÄÚ´ćÖ¸Őë
.	[in,out]	*pBufLen
	ÄÚ´ćł¤¶ČŁ¬µ÷ÓĂşó·µ»ŘĘµĽĘ¶ÁČˇµÄł¤¶Č

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_NicReadTimeOut
	¶ÁĘýľÝł¬Ę±
.	T9E_NoNicDataReceived
	ÓĐĘýľÝÎ´¶Áłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9ReadHWParamToBuffer(WORD cardIndex, BYTE portIndex, LPBYTE lpBuffer, UINT *pBufLen);

/*************************************************
* T9ReadFlashToBuffer
* ą¦ÄÜËµĂ÷Łş
    ¶ÁČˇflashµ˝ÄÚ´ćÖĐ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	cardIndexŁ¬ČˇÖµ·¶Î§[0~254]
	˝ÓĘŐż¨şĹ
.	[in]	hAddress
	¸ßµŘÖ·
.	[in]	lAddress
	µÍµŘÖ·
.	[in]	dataLen
	ĘýľÝł¤¶Č
.	[in]	pBuffer
	˝ÓĘŐÄÚ´ćµŘÖ·
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	bCallBack
	ĘÇ·ńĐčŇŞ»Řµ÷

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_NicReadTimeOut
	¶ÁĘýľÝł¬Ę±
.	T9E_NoNicDataReceived
	ÓĐĘýľÝÎ´¶Áłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9ReadFlashToBuffer(WORD	rcvIndex, 
							BYTE	hAddress,
							BYTE	lAddress,
							DWORD	dataLen,
							LPBYTE	pBuffer,
							BYTE	portIndex,
							BOOL	bCallBack);

/*************************************************
* T9UpgradeRcvCardFPGA2
* ą¦ÄÜËµĂ÷Łş
	ÉýĽ¶T9˝ÓĘŐż¨FPGAłĚĐň

* ˛ÎĘýËµĂ÷Łş 
.	[in]	lpBuffer
	FPGAłĚĐň
.	[in]	bufLen
	FPGAłĚĐň×Ö˝ÚĘý
.	[in]	cardIndex
	ÉýĽ¶µÄż¨şĹŁ¬[0~2047]Ę±ÎŞÖĆ¶¨ż¨Ł¬ÎŞ0xffffĘ±Ł¬ÉýĽ¶ËůÓĐż¨
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	flags
	ŁżŁżŁż
.	[in]	bQ9
	ĘÇ·ńĘÇQ9ż¨
* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9UpgradeRcvCardFPGA2(LPBYTE lpBuffer, DWORD bufLen, WORD cardIndex, BYTE portIndex, BYTE flags, BOOL bQ9);

//ÉýĽ¶˝ÓĘŐż¨µÄ˛»Í¬łĚĐň°ć±ľ
T9API	T9UpgradeRcvCardFPGA2Ex(LPBYTE lpBuffer, DWORD bufLen, WORD cardIndex, BYTE portIndex, BYTE flags, BOOL bQ9, BYTE programVersion);

/*************************************************
* T9ReadRcvCardFPGA2
* ą¦ÄÜËµĂ÷Łş
    ´Ó˝ÓĘÜż¨ÖĐ¶Á»ŘFPGA´ćČëÖ¸¶¨ÎÄĽţÖĐ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	lpBuffer
	±Ł´ćFPGAłĚĐňµÄÄÚ´ćµŘÖ·
.	[in]	bufLen
	±Ł´ćFPGAłĚĐňµÄÄÚ´ćł¤¶Č
.	[in]	cardIndex
	˝ÓĘŐż¨Ë÷ŇýşĹŁ¬ČˇÖµ·¶Î§[0~254]
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	bQ9
	ĘÇ·ńĘÇQ9ż¨
* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_VerifyFailed
	ĐŁŃéĘ§°Ü
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_NicReadTimeOut
	¶ÁĘýľÝł¬Ę±
.	T9E_NoNicDataReceived
	ÓĐĘýľÝÎ´¶Áłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_TooLittleBuffer
	·ÖĹäµÄÄÚ´ćĚ«Đˇ
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9ReadRcvCardFPGA2(LPBYTE lpBuffer, DWORD *pBufLen, WORD cardIndex, BYTE portIndex, BOOL bQ9);

//¶ÁČˇ˝ÓĘŐż¨µÄ˛»Í¬łĚĐň°ć±ľ
T9API	T9ReadRcvCardFPGA2Ex(LPBYTE lpBuffer, DWORD *pBufLen, WORD cardIndex, BYTE portIndex, BOOL bQ9, BYTE programVersion);

/*************************************************
* T9ClearCCCoefData
* ą¦ÄÜËµĂ÷Łş
    ÇĺłýÖđµăÁÁ¶ČĐŁŐýĎµĘý

* ˛ÎĘýËµĂ÷Łş 
	ÎŢ˛ÎĘý

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9ClearCCCoefData();

/*************************************************
* T9AddCCCoef
* ą¦ÄÜËµĂ÷Łş
    ĚíĽÓŇ»ŐĹ˝ÓĘŐż¨µÄÁÁ¶ČÖđµăĐŁŐýĘýľÝ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	rcvIndex
	˝ÓĘŐż¨ĐňşĹŁ¬´Ó0żŞĘĽĽĆĘý
.	[in]	lpCCCoefBuf
	ÖđµăĐŁŐýĘýľÝ	
.	[in]	bufLen
	ĐŁŐýĘýľÝÄÚ´ćł¤¶Č

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9AddCCCoef(WORD rcvIndex, LPBYTE lpCCCoefBuf, int bufLen);

T9API	T9AddCCCoef16(WORD rcvIndex, BYTE dataGroup, int moduleIndex, LPBYTE lpCCCoefBuf, int bufLen);

/*************************************************
* T9WriteCCCoefToRcvCard
* ą¦ÄÜËµĂ÷Łş
    ˝«ÖđµăÁÁ¶ČĐŁŐýĘýľÝ´ćČë˝ÓĘŐż¨ÖĐ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/

T9API	T9WriteCCCoefToRcvCard(BYTE portIndex);

T9API	T9WriteCCCoefToRcvCardMoudle(BYTE portIndex);

/*************************************************
* T9LoadRcvCCCoef
* ą¦ÄÜËµĂ÷Łş
    ˝«ż¨ÖĐÔ¤´ćµÄÖđµăÁÁ¶ČĐŁŐýĎµĘýÔŘČësd ram

* ˛ÎĘýËµĂ÷Łş 
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9LoadRcvCCCoef(BYTE portIndex);
T9API	T9LoadRcvCurrent(BYTE portIndex);


/*************************************************
* T9ClearChromaCCoefData
* ą¦ÄÜËµĂ÷Łş
    ÇĺłýÖđµăÉ«¶ČĐŁŐýĎµĘý

* ˛ÎĘýËµĂ÷Łş 
	ÎŢ˛ÎĘý

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9ClearChromaCCoefData();

/*************************************************
* T9AddChromaCCoef16
* ą¦ÄÜËµĂ÷Łş
    ĚíĽÓŇ»ŐĹ˝ÓĘŐż¨µÄÖđµăÉ«¶ČĐŁŐýĘýľÝ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	rcvIndex
	˝ÓĘŐż¨ĐňşĹŁ¬´Ó0żŞĘĽĽĆĘý
.	[in]	lpCCCoefBuf
	ÖđµăĐŁŐýĘýľÝ	
.	[in]	bufLen
	ĐŁŐýĘýľÝÄÚ´ćł¤¶Č

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9AddChromaCCoef16(WORD rcvIndex, LPBYTE lpCCCoefBuf, int bufLen);
T9API	T9AddChromaCCoef8(WORD rcvIndex, LPBYTE lpCCCoefBuf, int bufLen, BOOL b5A);

//T9API	T9AddMoudleChromaCCoef16(WORD rcvIndex, BYTE dataGroup, int moduleIndex, CRect moudleRc, LPBYTE lpCCCoefBuf, int bufLen);

/*************************************************
* T9WriteChromaCCoefToRcvCard
* ą¦ÄÜËµĂ÷Łş
    ˝«ÖđµăÉ«¶ČĐŁŐýĘýľÝ´ćČë˝ÓĘŐż¨ÖĐ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9WriteChromaCCoefToRcvCard(BYTE portIndex);

T9API	T9WriteChromaCCoefToRcvCardMoudle(BYTE portIndex);

/*************************************************
* T9LoadRcvChromaCCoef
* ą¦ÄÜËµĂ÷Łş
    ˝«ż¨ÖĐÔ¤´ćµÄÖđµăÉ«¶ČĐŁŐýĎµĘýÔŘČësd ram

* ˛ÎĘýËµĂ÷Łş 
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9LoadRcvChromaCCoef(BYTE portIndex);

/*************************************************
* T9ReadRcvCardFPGA
* ą¦ÄÜËµĂ÷Łş
    ´Ó˝ÓĘÜż¨ÖĐ¶Á»ŘFPGAłĚĐň´ćČëÖ¸¶¨ÎÄĽţÖĐ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	pFileName
	±Ł´ć˛ÎĘýÎÄĽţµÄÂ·ľ¶Ăű
.	[in]	cardIndex
	˝ÓĘŐż¨Ë÷ŇýşĹŁ¬ČˇÖµ·¶Î§[0~254]
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_VerifyFailed
	ĐŁŃéĘ§°Ü
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_NicReadTimeOut
	¶ÁĘýľÝł¬Ę±
.	T9E_NoNicDataReceived
	ÓĐĘýľÝÎ´¶Áłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9E_OpenFileError
	´ňżŞÎÄĽţłö´í
.	T9E_InvalidFile
	·Ç·¨ÎÄĽţ
.	T9E_ReadFileError
	¶ÁÎÄĽţłö´í
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9ReadRcvCardFPGA(char *pFileName, WORD cardIndex, BYTE portIndex);


/*************************************************
* T9AsynWriteFunCard
* ą¦ÄÜËµĂ÷Łş
    Ňě˛˝Đ´ą¦ÄÜż¨ĘýľÝ

* ˛ÎĘýËµĂ÷Łş 
.	[in]	funCardIndex
	ą¦ÄÜż¨ĐňşĹ
.	[in]	pBuffer
	Đ´ČëµÄĘýľÝÄÚ´ćµŘÖ·
.	[in]	bufLen
	Đ´ČëµÄĘýľÝł¤¶Č
.	[in]	highAddress
	¸ßÎ»µŘÖ·
.	[in]	lowAddress
	µŘÎ»µŘÖ·
.	[in]	timeOut
	×îł¤ÔĘĐí·˘ËÍµÄĘ±ĽäŁ¬µĄÎ»ŁşşÁĂë

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9AsynWriteFunCard(	int		funCardIndex,
							LPBYTE	pBuffer,
							DWORD	bufLen,
							int		highAddress,
							int		lowAddress,
							DWORD	timeOut);

/*************************************************
* T9StroageErase
* ą¦ÄÜËµĂ÷Łş
    ˛Áłý˝ÓĘŐż¨ÉĎFlash´ć´˘żŐĽäŔďµÄĘýľÝ

* ˛ÎĘýËµĂ÷Łş
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	rcvIndex
	˝ÓĘŐż¨ĐňşĹ
.	[in]	startBlock
	żŞĘĽ˛ÁłýµÄÉČÇřË÷ŇýşĹ
.	[in]	blockCount
	˛ÁłýµÄÉČÇřĘýÁż

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9StroageErase(BYTE		portIndex,
					   WORD		rcvIndex,
					   WORD		startBlock,
					   WORD		blockCount);

/*************************************************
* T9StroageWritePage
* ą¦ÄÜËµĂ÷Łş
    Ďň˝ÓĘŐż¨ÉĎFlash´ć´˘żŐĽäŔďĐ´ČëŇ»¸öpageµÄĘýľÝ

* ˛ÎĘýËµĂ÷Łş
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	rcvIndex
	˝ÓĘŐż¨ĐňşĹ
.	[in]	pageIndex
	ŇłĂćĐňşĹ
.	[in]	pBuffer
	Ö¸Ďň˝«Đ´ČëÉČÇřµÄĘýľÝµÄĆđĘĽµŘÖ·
.	[in]	pageSize
	page´óĐˇ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9StroageWritePage(	BYTE	portIndex,
							WORD	rcvIndex,
							DWORD	pageIndex,
							LPBYTE	pBuffer,
							int		pageSize);

/*************************************************
* T9StroageWriteSeciton
* ą¦ÄÜËµĂ÷Łş
    Ďň˝ÓĘŐż¨ÉĎFlash´ć´˘żŐĽäŔďĐ´ČëŇ»¸öÉČÇřµÄĘýľÝ

* ˛ÎĘýËµĂ÷Łş
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	rcvIndex
	˝ÓĘŐż¨ĐňşĹ
.	[in]	sectionIndex
	ÉČÇřż¨ĐňşĹ
.	[in]	sectionSize
	ÉČÇř´óĐˇ
.	[in]	pBuffer
	Ö¸Ďň˝«Đ´ČëÉČÇřµÄĘýľÝµÄĆđĘĽµŘÖ·
.	[in]	timeout
	ł¬ąýtimeoutĘ±ĽäĂ»ÓĐĐ´ÍęËăł¬Ę±

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9StroageWriteSeciton(	BYTE	portIndex,
								WORD	rcvIndex,
								DWORD	sectionIndex,
								DWORD	sectionSize,
								LPBYTE	pBuffer,
								DWORD	timeout);


/*************************************************
* T9SendDelayPacket
* ą¦ÄÜËµĂ÷Łş
    ˛ĺČëŇ»¸öŃÓĘ±ĘýľÝ°ü

* ˛ÎĘýËµĂ÷Łş 
.	[in]	packetSize
	˝ÓĘŐż¨ĐňşĹ
.	[in]	bAsyn
	ĘÇ·ńĽÓČëµ˝¶ÓÁĐÖĐ

* ·µ»ŘÖµËµĂ÷Łş
.	T9E_NICNotInitialized
	Ířż¨Ă»ÓĐłőĘĽ»Ż
.	T9E_OutOfMemeroy
	ÄÚ´ćŇçłö
.	T9E_ParamError
	˛ÎĘý´íÎó
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SendDelayPacket(DWORD packetSize, BOOL bAsyn);

/*************************************************
* T9SetLineSpace
* ą¦ÄÜËµĂ÷Łş
	ÉčÖĂ·˘ËÍĘýľÝĘ±µÄĐĐĽäĎ¶
* ˛ÎĘýËµĂ÷Łş 
.	[in]	lineSpace
	ĐĐĽäĎ¶µăĘý

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
// T9API	T9SetLineSpace(int lineSpace);


// /*************************************************
// * T9SetBigModuleMode
// * ą¦ÄÜËµĂ÷Łş
// 	ÉčÖĂÎŞ´óÄŁ×éÄŁĘ˝
// * ˛ÎĘýËµĂ÷Łş 
// .	[in]	bigModuleMode
// 	´óÄŁ×éÄŁĘ˝ŁşĎęĽűBigMoudule_Mode¶¨Ňĺ
// 
// * ·µ»ŘÖµËµĂ÷Łş
// .	T9_S_OK
// 	łÉą¦
// *************************************************/
// T9API	T9SetBigModuleMode(int bigModuleMode);


/*************************************************
* T9ClearSendCache
* ą¦ÄÜËµĂ÷Łş
	Çĺłý·˘ËÍ»ş´ć
* ˛ÎĘýËµĂ÷Łş 

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9ClearSendCache();


/*************************************************
* T9SetScreenHollow
* ą¦ÄÜËµĂ÷Łş
	Çĺłý·˘ËÍ»ş´ć
* ˛ÎĘýËµĂ÷Łş 
.	[in]	scrnNumber
	ĆÁÄ»ĐňşĹ
.	[in]	bNeedHollow
	ĘÇ·ńżŮĆÁ
.	[in]	senderRegion
	żŮĆÁÇřÓň

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SetScreenHollow(int		scrnNumber,
						  BOOL		bNeedHollow,
						  DWORD		senderRegion);

/*************************************************
* T9SendParam
* ą¦ÄÜËµĂ÷Łş
	·˘ËÍĘµĘ±˛ÎĘýµ˝˝ÓĘÜż¨ÉĎ
* ˛ÎĘýËµĂ÷Łş 

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9SendParam();

/*************************************************
* T9ReadModuleInfo
* ą¦ÄÜËµĂ÷Łş
	»ńČˇÄŁ×éĐĹĎ˘
* ˛ÎĘýËµĂ÷Łş 
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	rcvIndex
	˝ÓĘŐż¨ĐňşĹ
.	[in]	moduleIndex
	ÄŁ×éĐňşĹ
.	[in]	pBasicInfoBuf
	Ö¸Ďň´ć´˘ÄŁ×é»ů±ľĐĹĎ˘µÄÄÚ´ć
.	[in,out]	pBasicLen
	Ö¸¶¨pBasicInfoBufÖ¸ĎňµÄÄÚ´ćł¤¶ČŁ¬˛˘·µ»ŘĘµĽĘĘąÓĂµÄł¤¶Č
.	[in]		pMaintainInfoBuf
	Ö¸Ďň´ć´˘Î¬ĐŢĽÇÂĽµÄÄÚ´ć
.	[in,out]	maintainBufLen
	Ö¸¶¨pMaintainInfoBufÖ¸ĎňµÄÄÚ´ćł¤¶ČŁ¬˛˘·µ»ŘĘµĽĘĘąÓĂµÄł¤¶Č
.	[in]	pWatcher
	ŁżŁżŁż

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9ReadModuleInfo(BYTE	portIndex,
						 int	rcvIndex,
						 int	moduleIndex,
						 LPBYTE pBasicInfoBuf,
						 int	*pBasicLen,
						 LPBYTE pMaintainInfoBuf,
						 int	*pMaintainBufLen,
						 LPVOID pWatcher);


/*************************************************
* T9WriteModuleInfo
* ą¦ÄÜËµĂ÷Łş
	»ńČˇÄŁ×éĐĹĎ˘
* ˛ÎĘýËµĂ÷Łş 
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
	·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	rcvIndex
	˝ÓĘŐż¨ĐňşĹ
.	[in]	moduleIndex
	ÄŁ×éĐňşĹ
.	[in]	pBasicInfoBuf
	Ö¸Ďň´ć´˘ÄŁ×é»ů±ľĐĹĎ˘µÄÄÚ´ć
.	[in]	basicLen
	Ö¸¶¨pBasicInfoBufÖ¸ĎňµÄÄÚ´ćł¤¶Č
.	[in]	pMaintainInfoBuf
	Ö¸Ďň´ć´˘Î¬ĐŢĽÇÂĽµÄÄÚ´ć
.	[in]	maintainBufLen
	Ö¸¶¨pMaintainInfoBufÖ¸ĎňµÄÄÚ´ćł¤¶Č
.	[in]	pWatcher
	ŁżŁżŁż
* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
	łÉą¦
*************************************************/
T9API	T9WriteModuleInfo(BYTE	portIndex,
						 int	rcvIndex,
						 int	moduleIndex,
						 LPBYTE pBasicInfoBuf,
						 int	basicLen,
						 LPBYTE pMaintainInfoBuf,
						 int	maintainBufLen,
						 LPVOID pWatcher);


T9API	T9SetBlockFlags(DWORD blockFlags);
T9API	T9GetBlockFlags(DWORD &blockFlags);
T9API	T9ClearFlashSector(WORD portIndex, WORD rcvIndex, BYTE hAddress);
T9API	T9ClearFlashSector4KB(WORD portIndex, WORD rcvIndex, BYTE hAddress, BYTE lAddress);
T9API	T9Write256BytesToFlash(WORD portIndex, WORD rcvIndex, BYTE hAddress, BYTE lAddress, LPBYTE lpBuf);

T9API	T9MDetectMoudleState(WORD portIndex,
							 WORD rcvIndex,
							 BYTE *pMoudelState,
							 WORD stateLen);

T9API	T9MClearFlashSector(	WORD portIndex,
								WORD rcvIndex,
								BYTE dataGroup,
								BYTE moduleIndex,
								BYTE hAddress);
T9API	T9MClearFlashSector4KB(WORD portIndex,
								WORD rcvIndex,
								BYTE dataGroup,
								BYTE moduleIndex,
								BYTE hAddress,
								BYTE lAddress);
T9API	T9MWrite256BytesToFlash(WORD portIndex,
								 WORD rcvIndex,
								 BYTE dataGroup,
								 BYTE moduleIndex,
								 BYTE hAddress,
								 BYTE lAddress,
								 LPBYTE lpBuf);
T9API	T9MReadFlashToBuffer(BYTE	portIndex,
							 WORD	rcvIndex,
							 BYTE	dataGroup,
							 BYTE	moduleIndex,
							 BYTE	hAddress,
							 BYTE	lAddress,
							 DWORD	dataLen,
							 LPBYTE	pBuffer,
							 BOOL	bCallBack);


T9API	T9ReadPixelDetectInfo(	WORD portIndex,
								WORD rcvIndex,
								BYTE dataIndex,
								BYTE scanIndex,
								BYTE openOrBroken,
								BYTE colorInfo,
								WORD pixelCount,
								LPBYTE lpBuf);


T9API	T9GetAllPixelDetectInfo(WORD portIndex,
								BYTE openOrBroken,
								int  *pWidth,
								int  *pHeight,
								WORD *pInvalidRcv,
								LPBYTE *ppBuf,
								LPVOID pWatcher);


/*************************************************
* T9GetAllPixelDetectInfoEx
* ą¦ÄÜËµĂ÷Łş
ÉčÖĂ·˘ËÍż¨µÄ°ć±ľşĹ
* ˛ÎĘýËµĂ÷Łş 
.	[in]	portIndexŁ¬ČˇÖµ·¶Î§[0~3]
    ·˘ËÍż¨ÍřżÚşĹŁ¬ČçąűĘÇÍřżÚ·˘ËÍÄŁĘ˝Ł¬¸ř0xff
.	[in]	openOrBroken
	żŞÂ·»ňŐß¶ĚÂ·
.	[in]	pWidth
	»ńČˇĆÁµÄżí¶Č
.	[in]	pHeight
	»ńČˇĆÁµÄ¸ß¶Č
.	[in]	pInvalidRcv
	Ě˝˛âłö´íµÄ˝ÓĘŐż¨
.	[in]	lpBuf
	´ć´˘ÖđµăĐŁŐýĘýľÝµÄÄÚ´ćżé
.	[in]	bufLen
	´ć´˘ÖđµăĐŁŐýĘýľÝµÄÄÚ´ćżéł¤¶Č

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
    łÉą¦
.	T9E_OutOfMemeroy
    pBufł¤¶Č˛»ą»Ł¬ĐčŇŞµÄ×Ö˝ÚĘý=żí¶Čx¸ß¶Č
*************************************************/
T9API	T9GetAllPixelDetectInfoEx(	WORD	portIndex,
									BYTE	openOrBroken,
									int		*pWidth,
									int		*pHeight,
									WORD	*pInvalidRcv,
									LPBYTE	pBuf,
									int		bufLen,
									LPVOID pWatcher);

T9API	T9ReleaseBuffer(LPBYTE lpBuf);

/*************************************************
* T9SetSendingCardTypeVersion
* ą¦ÄÜËµĂ÷Łş
	ÉčÖĂ·˘ËÍż¨µÄ°ć±ľşĹ
* ˛ÎĘýËµĂ÷Łş 

* ·µ»ŘÖµËµĂ÷Łş
.	T9_S_OK
łÉą¦
*************************************************/
T9API	T9SetSendingCardTypeVersion(int type, int majorVersion, int minorVersion);

T9API	T9EnableCorrect(BOOL bEnable, WORD rcvIndex, BYTE portIndex);
T9API	T9EnableChromaCorrect(BOOL bEnable, WORD rcvIndex, BYTE portIndex);

//¶ÁĐ´·˘ËÍż¨Flash
T9API	T9ReadSendingFlash(DWORD startAddress, LPBYTE pBuffer, DWORD bufLen, int scIndex, BOOL bSecondFlash);
T9API	T9WriteSendingFlash(DWORD startAddress, LPBYTE pBuffer, DWORD bufLen, int scIndex, BOOL bSecondFlash);
T9API	T9WriteSendingFlash1(LPVOID	pWatcher, DWORD startAddress, LPBYTE pBuffer, DWORD bufLen, int scIndex, BOOL bSecondFlash);
T9API	T9WriteSendingFlash4kBStyle(LPVOID	pWatcher, //IProgressWatcher *pProgressWatcher
									DWORD	startAddress, 
									LPBYTE	pBuffer, 
									DWORD	bufLen,
									int		scIndex, 
									BOOL	bSecondFlash);
T9API	T9ClearSendingFlash4kB(DWORD startAddress, int scIndex, BOOL bSecondFlash);
T9API	T9ClearSendingFlash64kB(DWORD startAddress, int scIndex, BOOL bSecondFlash);
T9API	T9AtomicWriteSending256Bytes(DWORD startAddress, LPBYTE pBuffer, int scIndex, BOOL bSecondFlash);
T9API	T9SendSendingCmd(LPBYTE pCmdBuf, DWORD bufLen);
T9API	T9SendSendingCmdEx(LPBYTE pCmdBuf, DWORD bufLen, LPBYTE pBufOut, DWORD *pOutLen, int firstReadTime, int timeout);
T9API	T9SendSending0x99Cmd(BYTE	scIndex, 
							 BYTE	packFlag,
							 BYTE	packSubFlag,
							 BYTE	netPort,
							 LPBYTE	pCmdBuf,
							 DWORD	bufLen);

T9API	T9SetRectSignal(int x, int y, int width, int height);
T9API	T9SetSendingCardGama(BYTE scIndex, BOOL bSave, float gama);
T9API	T9SetSendingCardTestingMode(BYTE scIndex, BYTE testingMode);
T9API	T9SetSendingCardDateTime(BYTE scIndex, LPVOID pDateTime);
T9API	T9SetSendingCardEndDate(BYTE scIndex, LPVOID pEndDate);
T9API	SetSendingEnableBackupMode(BYTE scIndex, BYTE bEnable);

T9API	T9DetectSendingCard(int *pComIndex, BYTE cardIndex, LPBYTE lpBuffer, int bufLen);
T9API	T9SetSendingCardParam(LPVOID pWatcher, int cardIndex, LPBYTE lpBuffer, int bufLen);

/*************************************************
* T9RcvParam_Load 
* ą¦ÄÜËµĂ÷Łş(Description)
¶ÁČˇ˝ÓĘÜż¨˛ÎĘý(Load Receiving Card parameters)
* ˛ÎĘýËµĂ÷Łş(parameters)
[in]	pFile
˝ÓĘŐż¨˛ÎĘýÎÄĽţ(Receiving Card parameters)
[out]	phRcvParam
·µ»ŘĚ˝˛âµ˝˝ÓĘŐż¨˛ÎĘýľä±ú(Return Receiving Card Parameters Handle)
* ·µ»ŘÖµËµĂ÷Łş(return)
great than or equal 0, success, if less than 0, error
*************************************************/
T9API T9RcvParam_Load(wchar_t *pFile, HRCVPARAM *phRcvParam);


/*************************************************
* T9RcvParam_Release 
* ą¦ÄÜËµĂ÷Łş(Description)
ĘÍ·Ĺ˝ÓĘŐż¨˛ÎĘý(Release Receiving Card parameters)
* ˛ÎĘýËµĂ÷Łş(parameters)
[in]	hRcvParam
˝ÓĘŐż¨˛ÎĘýÎÄĽţľä±ú(Handle of Receiving Card parameters)
* ·µ»ŘÖµËµĂ÷Łş(return)
great than or equal 0, success, if less than 0, error
*************************************************/
T9API T9RcvParam_Release(HRCVPARAM hRcvParam);

/*************************************************
* T9RcvParam_Clone 
* ą¦ÄÜËµĂ÷Łş(Description)
żËÂˇ˝ÓĘŐż¨˛ÎĘý(Clone Receiving Card parameters)
* ˛ÎĘýËµĂ÷Łş(parameters)
[in]	hRcvParam
˝ÓĘŐż¨˛ÎĘýÎÄĽţľä±ú(Handle of Receiving Card parameters)
[in,out]	phRcvParam
Ö¸Ďň˝ÓĘŐż¨˛ÎĘýÎÄĽţľä±úŁ¬·µ»Ř±»żËÂˇ»ŘŔ´˝ÓĘŐż¨˛ÎĘýľä±ú(Point to the handle of Receiving Card parameters, which use to receive the new created parameters)
* ·µ»ŘÖµËµĂ÷Łş(return)
great than or equal 0, success, if less than 0, error
*************************************************/
T9API T9RcvParam_Clone(HRCVPARAM hRcvParam, HRCVPARAM *phRcvParam);
T9API T9RcvParam_SaveToBuffer(HRCVPARAM hRcvParam, LPBYTE lpBuf, int bufLen);
T9API T9RcvParam_LoadFromBuffer(HRCVPARAM hRcvParam, LPBYTE lpBuf, int bufLen);
T9API T9RcvParam_Create(HRCVPARAM *phRcvParam);
T9API T9RcvParam_GetSaveNeedBufLen(HRCVPARAM hRcvParam, int *pParamLen);
T9API T9RcvParam_GetCardCount(HRCVPARAM hRcvParam, int *pCount);

T9API T9SCParam_Create(HSCPARAM *phSCParam);
T9API T9SCParam_Release(HSCPARAM hSCParam);
T9API T9SCParam_Clone(HSCPARAM hSCParam, HSCPARAM *phSCParam);
T9API T9SCParam_SaveToBuffer(HSCPARAM hSCParam, LPBYTE lpBuf, int bufLen);
T9API T9SCParam_LoadFromBuffer(HSCPARAM hSCParam, LPBYTE lpBuf, int bufLen);
T9API T9SCParam_GetSaveNeedBufLen(HSCPARAM hSCParam, int *pParamLen);
}
