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
#define COMUNICATION_TYPE_COM		0x02	//串口发送
#define COMUNICATION_TYPE_ANY		0xFF	//任一形式


//Block define
#define BLOCK_NONE				0
#define BLOCK_ALL				0xffffffff
#define BLOCK_SEND_PIC			0x01
#define BLOCK_SEND_CTRL			0x02
#define BLOCK_ASYN_WRITEFUN		0x04
#define BLOCK_RCV_ANSWER		0x08
#define BLOCK_COEF_PIC			0x10	//逐点校正图片

#define UP_OLD_PROGRAM			0x01
#define UP_NEW_PROGRAM			0x02
#define UP_ALL_PROGRAM			0xff


// 各种回调消息定义
#define CBMSG_DROPFRAM				1		//网卡发送速度跟不上，出现丢帧，此时lParam无意义

#define CBMSG_RECEIVED_NIC_DATA		2		//从接受卡收到了网络数据，lParam，为指向SReceivedNicData的指针
struct SReceivedNicData
{
	LPBYTE dataAddress;
	int	   dataLen;
};

#define CBMSG_SAVE_HWParamToRcv		3		//保存参数到接受卡进度报告信息，
											//lParam高16为状态信息，0正在进行，1成功，2失败，低8位为进度百分比。

#define CBMSG_Read_HWParamFromRcv	4		//从接收卡读取参数进度报告信息，
											//lParam高8为状态信息，0正在进行，1成功，2失败。
											//lParam次高8为卡型号
											//lParam低16位的高八位为高版本号，低8位为低版本号。

#define CBMSG_Read_FPGAFromRcv		5		//从接收卡读取FPGA程序进度报告信息，
											//lParam高8位状态信息，0正在进行，1成功，2失败
											//lParam次高8位为卡型号
											//lParam低16位的高八位为高版本号，低8位为低版本号。

#define CBMSG_Upgrade_FPGAFromRcv	6		//发送FPGA程序到接收卡的进度报告信息，
											//lParam高8位为状态信息，0正在进行，1成功，2失败
											//lParam次高8位为进度信息，1开始，2擦除，3写入，4校验，0和前一次相同
											//lParam最低16位为卡号

#define CBMSG_Detected_RCV_Card		7		//探测到了接受卡，
											//lParam高16为状态信息，0开始探测，1成功，2失败，3探测到，次低八位为版本信息，低8位为参数卡序号（成功时为卡数量）

#define CBMSG_AsynWriteFunCard_Rslt	8		//异步写功能卡结果
											//lParam高为状态信息，0失败，1成功，2超时

#define CBMSG_SendCorrectCoef2Rcv   9       //发送校正数据到接收卡的进度信息，
                                            //lParam高16位为状态信息，0正在进行，1成功，2失败
                                            //lParam低16位为进度信息，0~100

#define CBMSG_Sending_FPGA_OP		10		//发送卡操作进度报告信息，
											//lParam高8位为状态信息，0正在进行，1成功，2失败
											//lParam次高8位为进度信息，1开始，2擦除，3写入，4读取，0和前一次相同
											//lParam最低16位无意义

#define CBMSG_Open_WinpcapError		11		//Open WinPcapError, lParam指向出错提示的字符串(char*)

#define CBMSG_Save_HWParamToSend  12		//发送参数到发送卡的进度报告消息
											//lParam高16为状态信息，0正在进行，1成功，2失败，低8位为进度百分比。


/*************************************************
* T9SenderCallback
* 功能说明：
    回调函数，参见各种回调消息定义

* 参数说明：
.   [in]	wParam
	消息类型
.	[in]	lParam
	消息值
.	[in]	data
	其他

* 返回值说明：

*************************************************/
typedef HRESULT (__stdcall *T9SenderCallback)(WPARAM wParam, LPARAM lParam, DWORD data);


extern "C"{

/*************************************************
* T9GetNetAdapterCount
* 功能说明：
    获取机器上已经安装的网卡数量

* 参数说明：
.   [out]	pCount
	返回网卡数量

* 返回值说明：
.   T9_S_OK
	成功 
*************************************************/
T9API	T9GetNetAdapterCount(DWORD *pCount);


/*************************************************
* T9GetNetAdapterInfo
* 功能说明：
    获取网卡信息

* 参数说明： 
.	[in]	index
	网卡索引号
.	[out]	pAdapterId
	返回该网卡的id，pAdapterId指向的内存大小不得小于260个字节
.	[in]	adapterIdBufLen
    pAdapterId实际指向的内存大小
.   [out]	pAdapterDesp
	返回该网卡的描述信息，pAdapterDesp指向的内存大小不得小于260个字节
.	[in]	adapterDespBufLen
	adapterDespBufLen指向的内存大小

* 返回值说明：
.	T9E_Adapter_Not_Exist
	无法获取该网卡的信息
.	T9E_Buffer_Overflow
	pAdapterId或者pAdapterDesp指向的内存太小
.	T9_S_OK
	成功
*************************************************/
T9API	T9GetNetAdapterInfo(DWORD	index,
							char	*pAdapterId,
							DWORD	adapterIdBufLen,	//需大于或者等于260
							char	*pAdapterDesp,
							DWORD	adapterDespBufLen);	//需大于或者等于260


/*************************************************
* T9CreateScreen
* 功能说明：
    创建一个屏幕

* 参数说明： 
.   [in]	number
	新创建屏幕的编号
.	[in]	width
	屏幕宽度					合理取值范围 1 - 1024*1024
.	[in]	height
	屏幕高度					合理取值范围 1 - 1024*1024
.	[in]	bVirtual
    是否是虚拟屏
.	[in]	virtualType
	虚拟屏类型				合理取值范围0~3       0 vpa_RA_G__B_RB   1 vpa_RA_B__G_RB   2 vpa_G_RA__RB_B   3 vpa_B_RA__RB_G
.	[in]	connectionType
	接收卡的级联方向	合理取值范围0~3      0 cs_right_to_left    1 cs_left_to_right    2 cs_top_to_bottom  3 cs_bottom_to_top
* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_InvalidNumber
	错误的屏编号，合理编号范围为1到MAX_SCREEN_NUMBER
.	T9E_ScreenAlreadyExist
	该编号的屏已经创建
.	T9E_NoImplement
	功能未实现
.	T9_S_OK
	成功
*************************************************/
T9API	T9CreateScreen(DWORD	number,
					   DWORD	width,
					   DWORD	height,
					   BOOL		bVirtual, 
					   int		virtualType, 
					   int		connectionType);


/*************************************************
* T9DestroyScreen
* 功能说明：
	删除一个屏幕

* 参数说明： 
.   [in]	number
	需要删除的屏幕的编号

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_InvalidScreen
	未曾创建该编号的屏幕
.	T9_S_OK
	成功
*************************************************/
T9API	T9DestroyScreen(DWORD	number);

/*************************************************
* T9DestroyAllScreen
* 功能说明：
	删除所有屏幕

* 参数说明： 


* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9DestroyAllScreen();

/*************************************************
* T9GetScreenInfo
* 功能说明：
	根据屏幕id号获取屏幕信息

* 参数说明： 
.   [in]	number
	需要获取信息的屏幕编号
.   [out]	pWidth 
	返回屏幕宽度
.   [out]	pHeight
	返回屏幕高度
.   [out]	pbVirtual
	返回屏幕是否是虚拟屏

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_InvalidScreen
	未曾创建该编号的屏幕
.	T9_S_OK
	成功
*************************************************/
T9API	T9GetScreenInfo(DWORD	number,
						DWORD	*pWidth,
						DWORD	*pHeight,
						BOOL	*pbVirtual);


/*************************************************
* T9GetScreenCount
* 功能说明：
	获取已经创建的屏幕数量

* 参数说明： 
.   [out]	pCount
	已经创建的屏幕数量

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9GetScreenCount(DWORD *pCount);


/*************************************************
* T9GetScreenNumber
* 功能说明：
	根据屏幕索引号获取屏幕id

* 参数说明： 
.   [in]	index
	屏幕索引号，索引号区间为[0, 屏幕数量减1]
.   [out]	pNumber
	返回屏幕编号

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_InvalidIndex
	无效屏幕索引号，参见index参数说明
.	T9_S_OK
	成功
*************************************************/
T9API	T9GetScreenNumber(	DWORD	index,
							DWORD	*pNumber);

/*************************************************
* T9SetScreenVirtualType
* 功能说明：
	设置虚拟屏幕的类型

* 参数说明： 
.   [in]  number
	需要设置的屏幕的屏幕号   

.   [in]  virtualType					合理取值范围0~3       0 vpa_RA_G__B_RB   1 vpa_RA_B__G_RB   2 vpa_G_RA__RB_B   3 vpa_B_RA__RB_G
	虚拟屏幕的类型

* 返回值说明：
.	T9E_InvalidScreen
	未曾创建该编号的屏幕
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9SetScreenVirtualType(DWORD number, int virtualType);


/*************************************************
* T9SetScreenConnectionStyle
* 功能说明：
	设置虚拟屏幕的类型

* 参数说明： 
.   [in]  number
	需要设置的屏幕的屏幕号

.   [in]  connectionStyle				合理取值范围0~3      0 cs_right_to_left    1 cs_left_to_right    2 cs_top_to_bottom  3 cs_bottom_to_top
	接收卡的级联方向

* 返回值说明：
.	T9E_InvalidScreen
	未曾创建该编号的屏幕
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9SetScreenConnectionStyle(DWORD number, int connectionStyle);

/*************************************************
* T9SenderStart
* 功能说明：
   初始化网卡，开始发送图像数据

* 参数说明： 
.	[in]	comunicationType
    通讯形式，网口或者串口，详见Comunication Type define
.	[in]	pParam
	 根据发送数据的方式不同而不同，当以网口发送时，pParam为当前使用的网卡名称 ；以串口发送时 pParam为串口号

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_NoScreen
	还没有创建屏，需要先创建所有的屏才能开始发送
.	T9E_OpenNICFailed
	打开网卡失败
.	T9E_UNKNOWN
	未知错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9SenderStart(DWORD comunicationType, LPVOID pParam);


/*************************************************
* T9SenderStop
* 功能说明：
    停止发送图像数据

* 参数说明： 

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9SenderStop();

/*************************************************
* T9IsSenderRunning
* 功能说明：
	是否网络收发模块正在运行

* 参数说明： 
	comunicationType 通讯形式，网口或者串口，详见Comunication Type define

* 返回值说明：
.	T9_S_OK
	正在运行
.	T9_E_FAILE
	没有运行
*************************************************/
T9API	T9IsSenderRunning(DWORD comunicationType);

/*************************************************
* T9SetBirghtness
* 功能说明：
      设置亮度

* 参数说明： 
.	[in]	bright
	亮度值
.	[in]	bright_r
	红色的亮度值
.	[in]	bright_g
	绿色的亮度值
.	[in]	bright_b
	蓝色的亮度值
.	[in]	bright_vr
	透明度
.	[in]	colorTempture
	色温
* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9SetBirghtness(BYTE bright, BYTE bright_r, BYTE bright_g, BYTE bright_b, BYTE bright_vr, WORD colorTempture);

/*************************************************
* T9SetIsUseRcvParam
* 功能说明：
      设置是否使用接收卡参数

* 参数说明：
.	[in]	bUseRcvParam
	TRUE，使用接收卡参数，FALSE，使用计算机实时参数

* 返回值说明：
.	T9_S_OK
成功
*************************************************/
T9API	T9SetIsUseRcvParam(BOOL bUseRcvParam);

/*************************************************
* T9SetPcRealTimeParam
* 功能说明：
	改变计算机实时参数

* 参数说明： 
.	[in]	lpBuf
	参数块内存地址 
.	[in]	bufLen
	参数块内存长度

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9SetPcRealTimeParam(LPBYTE lpBuf, DWORD bufLen);

/*************************************************
* SetSendParamScreenNumber
* 功能说明：
	设置在哪个屏发送图像数据之前，需要先发送的屏参数据

* 参数说明： 
.	[in]	number
	屏幕编号

* 返回值说明：
.	T9E_InvalidScreen
	未曾创建该编号的屏幕
.	T9_S_OK
	成功
*************************************************/
T9API	T9SetSendParamScreenNumber(DWORD number);


/*************************************************
* T9SendControlPack
* 功能说明：
    发送控制参数包

* 参数说明： 
.	[in]	pBuf
	参数包内存地址
.	[in]	bufLen
	参数包内存长度
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_SendThreadNotStart
	发送线程没有启动
.	T9E_ParamError
	参数错误，指的传给该函数的参数错误，而不是pBuf内数据错误
.	T9_S_FALSE
	由于内部阻塞，发送的控制命令被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9SendControlPack(LPBYTE pBuf, DWORD bufLen, BYTE portIndex);

/*************************************************
* T9SendRTCSetting
* 功能说明：
    发送RTC有关设置参数

* 参数说明： 
.	[in]	pBuf
	参数包内存地址
.	[in]	bufLen
	参数包内存长度
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	E_FAIL
	失败
.	T9_NOERROR
	正确

*************************************************/
T9API	T9SendRTCSetting(IN LPBYTE pBuf, IN DWORD bufLen, IN BYTE portIndex);

/*************************************************
* T9Ctrl_DetectRcvCardEx
* 功能说明：
	探测接收卡

* 参数说明：
.	[out] pRcvCount
	返回探测到的接受卡数量
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
	T9E_ParamError
	参数错
.	T9_S_OK
	成功
*************************************************/
T9API	T9Ctrl_DetectRcvCardEx(DWORD *pRcvCount, BYTE portIndex);

/*************************************************
* T9Ctrl_DetectRcvCardEx2
* 功能说明：
	探测接收卡

* 参数说明：
	[in] rcvIndex
	接收卡序号，取值范围[0~65536]
	[in] pBuf
	内存地址，用来存储接收到的卡信息
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
* 返回值说明：
.	E_FAIL
	失败
.	T9E_ParamError
	参数错
.	T9E_OutOfMemeroy
	内存溢出
.	NOERROR
	正确
.	T9_S_OK
	成功
*************************************************/
T9API	T9Ctrl_DetectRcvCardEx2(WORD rcvIndex, LPBYTE pBuf, BYTE portIndex);

/*************************************************
* T9Ctrl_DetectRcvCardEx3
* 功能说明：
	探测接收卡

* 参数说明：
.	[out] pRcvCount
	返回探测到的接受卡数量
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
	[in] startIndex
	起始卡号
	[in] count
	探测卡的数量

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
	T9E_ParamError
	参数错
.	T9_S_OK
	成功
*************************************************/
T9API	T9Ctrl_DetectRcvCardEx3(DWORD *pRcvCount, BYTE portIndex, WORD startIndex, WORD count);

// T9API	T9Ctrl_DetectRcvCardPin(BYTE portIndex, BYTE rcvIndex, SDetectRcvPinAnswer *pRcvPinAnswer);


/*************************************************
* T9Ctrl_DetectRcvCard
* 功能说明：
	探测接收卡

* 参数说明：
	rcvIndex
	接收卡序号，取值范围[0~65536]
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
	* 返回值说明：
	T9E_ParamError
	参数错
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_SendThreadNotStart
	发送线程没有启动
.	T9_S_FALSE
	由于内部阻塞，发送的控制命令被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9Ctrl_DetectRcvCard(WORD rcvIndex, BYTE portIndex);

/*************************************************
* T9ClearMackPacketCount
* 功能说明：
	清除接收卡网络包计数

* 参数说明：
	rcvIndex
	接收卡序号，取值范围[0~65536]
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	9E_ParamError
	参数错误
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_SendThreadNotStart
	发送线程没有启动
.	T9_S_FALSE
	由于内部阻塞，发送的控制命令被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9ClearMackPacketCount(WORD rcvIndex, BYTE portIndex);


/*************************************************
* T9Ctrl_DetectFunCard
* 功能说明：
	探测功能卡

* 参数说明：
.	funCardIndex
	功能卡序号，取值范围[0~254]
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_SendThreadNotStart
	发送线程没有启动
.	T9E_OutOfMemeroy
	内存溢出
.	T9_S_FALSE
	由于内部阻塞，发送的控制命令被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9Ctrl_DetectFunCard(int funCardIndex, BYTE portIndex);


/*************************************************
* T9Ctrl_DetectFunCardEx
* 功能说明：
	探测功能卡

* 参数说明：
.	funCardIndex
	功能卡序号，取值范围[0~254]
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
	[in] pBuf
	内存地址，用来存储接收到的卡信息

* 返回值说明：
.	T9E_ParamError
	数错误
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_SendThreadNotStart
	发送线程没有启动
.	T9E_OutOfMemeroy
	内存溢出
.	T9_S_FALSE
	由于内部阻塞，发送的控制命令被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9Ctrl_DetectFunCardEx(int funCardIndex, BYTE portIndex, LPBYTE pBuf);


/*************************************************
* T9Ctrl_5036PixelCheck
* 功能说明：
	5036逐点检测

* 参数说明：
	[in] lpByte
	控制数据内存地址指针
	[in] bufLen
	控制数据长度
	[in] cardIndex
	卡序号         取值范围[0~2048]

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_SendThreadNotStart
	发送线程没有启动
.	T9E_OutOfMemeroy
	内存溢出
.	T9_S_FALSE
	由于内部阻塞，发送的控制命令被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9Ctrl_5036PixelCheck(LPBYTE lpByte, int bufLen, WORD cardIndex);

/*************************************************
* T9Ctrl_ExecFunPower
* 功能说明：
	功能卡电源控制

* 参数说明：
	[in] lpByte
	控制数据内存地址指针
	[in] bufLen
	控制数据长度

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_SendThreadNotStart
	发送线程没有启动
.	T9E_OutOfMemeroy
	内存溢出
.	T9_S_FALSE
	由于内部阻塞，发送的控制命令被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9Ctrl_ExecFunPower(LPBYTE lpByte, int bufLen);

/*************************************************
* T9Ctrl_ReqParamAndVer
* 功能说明：
	探测功能卡

* 参数说明：
	rcvIndex
	接收卡序号，取值范围[0~65536]

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_SendThreadNotStart
	发送线程没有启动
.	T9_S_FALSE
	由于内部阻塞，发送的控制命令被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9Ctrl_ReqParamAndVer(WORD rcvIndex);

/*************************************************
* T9Ctrl_FunCard_FlashOperation
* 功能说明：，
	功能卡Flash操作

* 参数说明：
.	[in]	opType	
	操作类型: 取值为FlashOperateType的值
.	[in]	highAddress
	操作目的高位地址
.	[in]	lowAddress
	操作目的低位地址
.	[in]	lpBuf
	opType为flash_op_write，写入的内容，为256个字节长

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_SendThreadNotStart
	发送线程没有启动
.	T9_S_FALSE
	由于内部阻塞，发送的控制命令被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9Ctrl_FunCard_FlashOperation(BYTE	opType, 
									  BYTE	highAddress,
									  BYTE	lowAddress,
									  LPBYTE lpBuf);


/*************************************************
* T9SendScreenPicture
* 功能说明：
    发送一副图像数据（直接发送方式）

* 参数说明： 
.	[in]	number
	屏幕编号
.	[in]	pBuf
	图像数据内存地址

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_InvalidScreen
	未曾创建该编号的屏幕
.	T9E_NoScreenBufPool
	没有创建改屏幕的图像内存缓冲池
.	T9_S_FALSE
	由于内部阻塞，发送的图像数据被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9SendScreenPicture(DWORD	number,
							LPBYTE	pBuf);

/*************************************************
* T9SendScreenCorrectCoef
* 功能说明：
   发送逐点校正系数

* 参数说明： 
.	[in]	number
	屏幕编号
.	[in]	pBuf
	逐点校正系数地址

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_InvalidScreen
	未曾创建该编号的屏幕
.	T9E_NoScreenBufPool
	没有创建改屏幕的图像内存缓冲池
.	T9_S_FALSE
	由于内部阻塞，发送的图像数据被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9SendScreenCorrectCoef(DWORD	number,
								LPBYTE	pBuf);
/*************************************************
* T9SendScreenBlackPicture
* 功能说明：
    发送一幅黑色的图像给屏幕

* 参数说明： 
.	[in]	number
	屏幕编号

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9E_InvalidScreen
	未曾创建该编号的屏幕
.	T9E_NoScreenBufPool
	没有创建改屏幕的图像内存缓冲池
.	T9_S_FALSE
	由于内部阻塞，发送的图像数据被抛弃
.	T9_S_OK
	成功
*************************************************/
T9API	T9SendScreenBlackPicture(DWORD	number);

/*************************************************
* T9SetCallBackWnd
* 功能说明：
    设置接受底层模块通知的窗口句柄，发送模块通过窗口消息的方式通知上层它的各种信息

* 参数说明： 
.	[in]	hWnd
	窗口句柄
.	[in]	msgId
	发送模块给窗口发送窗口消息的ID号

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
// T9API	T9SetCallBackWnd(HWND hWnd, DWORD msgId);


/*************************************************
* T9SetCallBackFun
* 功能说明：
    设置回调函数，发送模块调用回调函数通知上层各种信息

* 参数说明： 
.	[in]	pCallbackFun
	回调函数指针，如果pCallbackFun等于NULL，清除回调函数设定

* 返回值说明：
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9SetCallBackFun(T9SenderCallback pCallbackFun);

/*************************************************
* T9WriteHWParamToRcvCard
* 功能说明：
    发送参数到接收卡

* 参数说明： 
.	[in]	pBuf
	硬件参数内存地址
.	[in]	bufLen
	硬件参数内存长度
.	[in]	scrnIndex
	屏幕索引号
.	[in]	cardCount，取值范围[1~2047]
	卡数量
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	markedOnly
	？？？

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
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
* 功能说明：
    发送参数到接收卡

* 参数说明： 
.	[in]	pBuf
	硬件参数内存地址
.	[in]	bufLen
	硬件参数内存长度
.	[in]	scrnIndex
	屏幕索引号
.	[in]	skipCount
	跳过前面的卡不设置，skipCount为跳过的卡数量
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	markedOnly
	？？？

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
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
* 功能说明：
    发送参数到模组

* 参数说明： 
.	[in]	pBuf
	硬件参数内存地址
.	[in]	bufLen
	硬件参数内存长度
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9WriteHWParamToModule(	LPBYTE	pBuf, 
								DWORD	bufLen, 
								BYTE	portIndex);

/*************************************************
* T9WriteHWParamToFunCard
* 功能说明：
	发送参数到功能卡

* 参数说明： 
.	[in]	pBuf
	硬件参数内存地址
.	[in]	bufLen
	硬件参数内存长度
.	[in]	scrnIndex
	屏幕索引号
.	[in]	skipCount
	跳过前面的卡不设置，skipCount为跳过的卡数量
.	[in]	funIndex，取值范围[0~254]
	卡数量

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9WriteHWParamToFunCard(LPBYTE pBuf, DWORD bufLen, int funIndex);

/*************************************************
* T9ReadHWParamToFile
* 功能说明：
    从接受卡中读回参数存入指定文件中

* 参数说明： 
.	[in]	pFileName
	保存参数文件的路径名
.	[in]	cardIndex，取值范围[0~254]
	接收卡号
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_NicReadTimeOut
	读数据超时
.	T9E_NoNicDataReceived
	有数据未读出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9ReadHWParamToFile(char *pFileName, WORD cardIndex, BYTE portIndex);

/*************************************************
* T9ReadHWParamToBuffer
* 功能说明：
    从接受卡中读回参数存入指定文件中

* 参数说明： 
.	[in]	cardIndex，取值范围[0~254]
	接收卡号
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	lpBuffer
	存储参数文件的内存指针
.	[in,out]	*pBufLen
	内存长度，调用后返回实际读取的长度

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_NicReadTimeOut
	读数据超时
.	T9E_NoNicDataReceived
	有数据未读出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9ReadHWParamToBuffer(WORD cardIndex, BYTE portIndex, LPBYTE lpBuffer, UINT *pBufLen);

/*************************************************
* T9ReadFlashToBuffer
* 功能说明：
    读取flash到内存中

* 参数说明： 
.	[in]	cardIndex，取值范围[0~254]
	接收卡号
.	[in]	hAddress
	高地址
.	[in]	lAddress
	低地址
.	[in]	dataLen
	数据长度
.	[in]	pBuffer
	接收内存地址
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	bCallBack
	是否需要回调

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_NicReadTimeOut
	读数据超时
.	T9E_NoNicDataReceived
	有数据未读出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
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
* 功能说明：
	升级T9接收卡FPGA程序

* 参数说明： 
.	[in]	lpBuffer
	FPGA程序
.	[in]	bufLen
	FPGA程序字节数
.	[in]	cardIndex
	升级的卡号，[0~2047]时为制定卡，为0xffff时，升级所有卡
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	flags
	？？？
.	[in]	bQ9
	是否是Q9卡
* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9UpgradeRcvCardFPGA2(LPBYTE lpBuffer, DWORD bufLen, WORD cardIndex, BYTE portIndex, BYTE flags, BOOL bQ9);

//升级接收卡的不同程序版本
T9API	T9UpgradeRcvCardFPGA2Ex(LPBYTE lpBuffer, DWORD bufLen, WORD cardIndex, BYTE portIndex, BYTE flags, BOOL bQ9, BYTE programVersion);

/*************************************************
* T9ReadRcvCardFPGA2
* 功能说明：
    从接受卡中读回FPGA存入指定文件中

* 参数说明： 
.	[in]	lpBuffer
	保存FPGA程序的内存地址
.	[in]	bufLen
	保存FPGA程序的内存长度
.	[in]	cardIndex
	接收卡索引号，取值范围[0~254]
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	bQ9
	是否是Q9卡
* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_VerifyFailed
	校验失败
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_NicReadTimeOut
	读数据超时
.	T9E_NoNicDataReceived
	有数据未读出
.	T9E_ParamError
	参数错误
.	T9E_TooLittleBuffer
	分配的内存太小
.	T9_S_OK
	成功
*************************************************/
T9API	T9ReadRcvCardFPGA2(LPBYTE lpBuffer, DWORD *pBufLen, WORD cardIndex, BYTE portIndex, BOOL bQ9);

//读取接收卡的不同程序版本
T9API	T9ReadRcvCardFPGA2Ex(LPBYTE lpBuffer, DWORD *pBufLen, WORD cardIndex, BYTE portIndex, BOOL bQ9, BYTE programVersion);

/*************************************************
* T9ClearCCCoefData
* 功能说明：
    清除逐点亮度校正系数

* 参数说明： 
	无参数

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9ClearCCCoefData();

/*************************************************
* T9AddCCCoef
* 功能说明：
    添加一张接收卡的亮度逐点校正数据

* 参数说明： 
.	[in]	rcvIndex
	接收卡序号，从0开始计数
.	[in]	lpCCCoefBuf
	逐点校正数据	
.	[in]	bufLen
	校正数据内存长度

* 返回值说明：
.	T9E_OutOfMemeroy
	内存溢出
.	T9_S_OK
	成功
*************************************************/
T9API	T9AddCCCoef(WORD rcvIndex, LPBYTE lpCCCoefBuf, int bufLen);

T9API	T9AddCCCoef16(WORD rcvIndex, BYTE dataGroup, int moduleIndex, LPBYTE lpCCCoefBuf, int bufLen);

/*************************************************
* T9WriteCCCoefToRcvCard
* 功能说明：
    将逐点亮度校正数据存入接收卡中

* 参数说明： 
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/

T9API	T9WriteCCCoefToRcvCard(BYTE portIndex);

T9API	T9WriteCCCoefToRcvCardMoudle(BYTE portIndex);

/*************************************************
* T9LoadRcvCCCoef
* 功能说明：
    将卡中预存的逐点亮度校正系数载入sd ram

* 参数说明： 
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9LoadRcvCCCoef(BYTE portIndex);
T9API	T9LoadRcvCurrent(BYTE portIndex);


/*************************************************
* T9ClearChromaCCoefData
* 功能说明：
    清除逐点色度校正系数

* 参数说明： 
	无参数

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9ClearChromaCCoefData();

/*************************************************
* T9AddChromaCCoef16
* 功能说明：
    添加一张接收卡的逐点色度校正数据

* 参数说明： 
.	[in]	rcvIndex
	接收卡序号，从0开始计数
.	[in]	lpCCCoefBuf
	逐点校正数据	
.	[in]	bufLen
	校正数据内存长度

* 返回值说明：
.	T9E_OutOfMemeroy
	内存溢出
.	T9_S_OK
	成功
*************************************************/
T9API	T9AddChromaCCoef16(WORD rcvIndex, LPBYTE lpCCCoefBuf, int bufLen);
T9API	T9AddChromaCCoef8(WORD rcvIndex, LPBYTE lpCCCoefBuf, int bufLen, BOOL b5A);

T9API	T9AddMoudleChromaCCoef16(WORD rcvIndex, BYTE dataGroup, int moduleIndex, CRect moudleRc, LPBYTE lpCCCoefBuf, int bufLen);

/*************************************************
* T9WriteChromaCCoefToRcvCard
* 功能说明：
    将逐点色度校正数据存入接收卡中

* 参数说明： 
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9WriteChromaCCoefToRcvCard(BYTE portIndex);

T9API	T9WriteChromaCCoefToRcvCardMoudle(BYTE portIndex);

/*************************************************
* T9LoadRcvChromaCCoef
* 功能说明：
    将卡中预存的逐点色度校正系数载入sd ram

* 参数说明： 
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9LoadRcvChromaCCoef(BYTE portIndex);

/*************************************************
* T9ReadRcvCardFPGA
* 功能说明：
    从接受卡中读回FPGA程序存入指定文件中

* 参数说明： 
.	[in]	pFileName
	保存参数文件的路径名
.	[in]	cardIndex
	接收卡索引号，取值范围[0~254]
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_VerifyFailed
	校验失败
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_NicReadTimeOut
	读数据超时
.	T9E_NoNicDataReceived
	有数据未读出
.	T9E_ParamError
	参数错误
.	T9E_OpenFileError
	打开文件出错
.	T9E_InvalidFile
	非法文件
.	T9E_ReadFileError
	读文件出错
.	T9_S_OK
	成功
*************************************************/
T9API	T9ReadRcvCardFPGA(char *pFileName, WORD cardIndex, BYTE portIndex);


/*************************************************
* T9AsynWriteFunCard
* 功能说明：
    异步写功能卡数据

* 参数说明： 
.	[in]	funCardIndex
	功能卡序号
.	[in]	pBuffer
	写入的数据内存地址
.	[in]	bufLen
	写入的数据长度
.	[in]	highAddress
	高位地址
.	[in]	lowAddress
	地位地址
.	[in]	timeOut
	最长允许发送的时间，单位：毫秒

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9AsynWriteFunCard(	int		funCardIndex,
							LPBYTE	pBuffer,
							DWORD	bufLen,
							int		highAddress,
							int		lowAddress,
							DWORD	timeOut);

/*************************************************
* T9StroageErase
* 功能说明：
    擦除接收卡上Flash存储空间里的数据

* 参数说明：
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	rcvIndex
	接收卡序号
.	[in]	startBlock
	开始擦除的扇区索引号
.	[in]	blockCount
	擦除的扇区数量

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9StroageErase(BYTE		portIndex,
					   WORD		rcvIndex,
					   WORD		startBlock,
					   WORD		blockCount);

/*************************************************
* T9StroageWritePage
* 功能说明：
    向接收卡上Flash存储空间里写入一个page的数据

* 参数说明：
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	rcvIndex
	接收卡序号
.	[in]	pageIndex
	页面序号
.	[in]	pBuffer
	指向将写入扇区的数据的起始地址
.	[in]	pageSize
	page大小

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9StroageWritePage(	BYTE	portIndex,
							WORD	rcvIndex,
							DWORD	pageIndex,
							LPBYTE	pBuffer,
							int		pageSize);

/*************************************************
* T9StroageWriteSeciton
* 功能说明：
    向接收卡上Flash存储空间里写入一个扇区的数据

* 参数说明：
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	rcvIndex
	接收卡序号
.	[in]	sectionIndex
	扇区卡序号
.	[in]	sectionSize
	扇区大小
.	[in]	pBuffer
	指向将写入扇区的数据的起始地址
.	[in]	timeout
	超过timeout时间没有写完算超时

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9StroageWriteSeciton(	BYTE	portIndex,
								WORD	rcvIndex,
								DWORD	sectionIndex,
								DWORD	sectionSize,
								LPBYTE	pBuffer,
								DWORD	timeout);


/*************************************************
* T9SendDelayPacket
* 功能说明：
    插入一个延时数据包

* 参数说明： 
.	[in]	packetSize
	接收卡序号
.	[in]	bAsyn
	是否加入到队列中

* 返回值说明：
.	T9E_NICNotInitialized
	网卡没有初始化
.	T9E_OutOfMemeroy
	内存溢出
.	T9E_ParamError
	参数错误
.	T9_S_OK
	成功
*************************************************/
T9API	T9SendDelayPacket(DWORD packetSize, BOOL bAsyn);

/*************************************************
* T9SetLineSpace
* 功能说明：
	设置发送数据时的行间隙
* 参数说明： 
.	[in]	lineSpace
	行间隙点数

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
// T9API	T9SetLineSpace(int lineSpace);


// /*************************************************
// * T9SetBigModuleMode
// * 功能说明：
// 	设置为大模组模式
// * 参数说明： 
// .	[in]	bigModuleMode
// 	大模组模式：详见BigMoudule_Mode定义
// 
// * 返回值说明：
// .	T9_S_OK
// 	成功
// *************************************************/
// T9API	T9SetBigModuleMode(int bigModuleMode);


/*************************************************
* T9ClearSendCache
* 功能说明：
	清除发送缓存
* 参数说明： 

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9ClearSendCache();


/*************************************************
* T9SetScreenHollow
* 功能说明：
	清除发送缓存
* 参数说明： 
.	[in]	scrnNumber
	屏幕序号
.	[in]	bNeedHollow
	是否抠屏
.	[in]	senderRegion
	抠屏区域

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9SetScreenHollow(int		scrnNumber,
						  BOOL		bNeedHollow,
						  DWORD		senderRegion);

/*************************************************
* T9SendParam
* 功能说明：
	发送实时参数到接受卡上
* 参数说明： 

* 返回值说明：
.	T9_S_OK
	成功
*************************************************/
T9API	T9SendParam();

/*************************************************
* T9ReadModuleInfo
* 功能说明：
	获取模组信息
* 参数说明： 
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	rcvIndex
	接收卡序号
.	[in]	moduleIndex
	模组序号
.	[in]	pBasicInfoBuf
	指向存储模组基本信息的内存
.	[in,out]	pBasicLen
	指定pBasicInfoBuf指向的内存长度，并返回实际使用的长度
.	[in]		pMaintainInfoBuf
	指向存储维修记录的内存
.	[in,out]	maintainBufLen
	指定pMaintainInfoBuf指向的内存长度，并返回实际使用的长度
.	[in]	pWatcher
	？？？

* 返回值说明：
.	T9_S_OK
	成功
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
* 功能说明：
	获取模组信息
* 参数说明： 
.	[in]	portIndex，取值范围[0~3]
	发送卡网口号，如果是网口发送模式，给0xff
.	[in]	rcvIndex
	接收卡序号
.	[in]	moduleIndex
	模组序号
.	[in]	pBasicInfoBuf
	指向存储模组基本信息的内存
.	[in]	basicLen
	指定pBasicInfoBuf指向的内存长度
.	[in]	pMaintainInfoBuf
	指向存储维修记录的内存
.	[in]	maintainBufLen
	指定pMaintainInfoBuf指向的内存长度
.	[in]	pWatcher
	？？？
* 返回值说明：
.	T9_S_OK
	成功
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
* 功能说明：
设置发送卡的版本号
* 参数说明： 
.	[in]	portIndex，取值范围[0~3]
    发送卡网口号，如果是网口发送模式，给0xff
.	[in]	openOrBroken
	开路或者短路
.	[in]	pWidth
	获取屏的宽度
.	[in]	pHeight
	获取屏的高度
.	[in]	pInvalidRcv
	探测出错的接收卡
.	[in]	lpBuf
	存储逐点校正数据的内存块
.	[in]	bufLen
	存储逐点校正数据的内存块长度

* 返回值说明：
.	T9_S_OK
    成功
.	T9E_OutOfMemeroy
    pBuf长度不够，需要的字节数=宽度x高度
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
* 功能说明：
	设置发送卡的版本号
* 参数说明： 

* 返回值说明：
.	T9_S_OK
成功
*************************************************/
T9API	T9SetSendingCardTypeVersion(int type, int majorVersion, int minorVersion);

T9API	T9EnableCorrect(BOOL bEnable, WORD rcvIndex, BYTE portIndex);
T9API	T9EnableChromaCorrect(BOOL bEnable, WORD rcvIndex, BYTE portIndex);

//读写发送卡Flash
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
* 功能说明：(Description)
读取接受卡参数(Load Receiving Card parameters)
* 参数说明：(parameters)
[in]	pFile
接收卡参数文件(Receiving Card parameters)
[out]	phRcvParam
返回探测到接收卡参数句柄(Return Receiving Card Parameters Handle)
* 返回值说明：(return)
great than or equal 0, success, if less than 0, error
*************************************************/
T9API T9RcvParam_Load(wchar_t *pFile, HRCVPARAM *phRcvParam);


/*************************************************
* T9RcvParam_Release 
* 功能说明：(Description)
释放接收卡参数(Release Receiving Card parameters)
* 参数说明：(parameters)
[in]	hRcvParam
接收卡参数文件句柄(Handle of Receiving Card parameters)
* 返回值说明：(return)
great than or equal 0, success, if less than 0, error
*************************************************/
T9API T9RcvParam_Release(HRCVPARAM hRcvParam);

/*************************************************
* T9RcvParam_Clone 
* 功能说明：(Description)
克隆接收卡参数(Clone Receiving Card parameters)
* 参数说明：(parameters)
[in]	hRcvParam
接收卡参数文件句柄(Handle of Receiving Card parameters)
[in,out]	phRcvParam
指向接收卡参数文件句柄，返回被克隆回来接收卡参数句柄(Point to the handle of Receiving Card parameters, which use to receive the new created parameters)
* 返回值说明：(return)
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