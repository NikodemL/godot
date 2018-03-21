#pragma once

//正确返回值定义
#define T9_S_OK				S_OK		//正确
#define T9_NOERROR			NOERROR		//正确
#define T9_S_FALSE			1		//正确，但没有执行
#define T9_E_FAIL			E_FAIL	//失败

//出错代码定义
//格式说明，按winsows出错代码标准定义，不会与Windows自带出错代码重复
// T9_ERROR_XXXX	=	(0xE0830000 + x)	//类别定义		0XE0800000 11-1-0- 0000 1000 0011-0000 0000 0000 0000

//通用错误
#define T9E_UNKNOWN					0xE0830001			//未知错误
#define T9E_ParamError				0xE0830002			//参数错误
#define T9E_NullPoint				0xE0830003			//遇到空指针
#define T9E_NoImplement				0xE0830004			//未实现
#define T9E_Unsupport				0xE0830005			//不支持
#define T9E_OutOfMemeroy			0xE0830006			//内存不够
#define T9E_CreateFileError			0xE0830007			//创建文件出错
#define T9E_OpenFileError			0xE0830008			//打开文件出错
#define T9E_InvalidFile				0xE0830009			//无效文件
#define T9E_ReadFileError			0xE083000A			//读文件出错
#define T9E_WriteFileError			0xE083000B			//写文件出错
#define T9E_FileDamaged				0xE083000C			//文件被损坏
#define T9E_CreateWndFailed			0xE083000D			//创建窗口失败
#define T9E_NotInit					0xE083000E			//没有初始化
#define T9E_FileNotExist			0xE083000F			//文件不存在
#define T9E_CreateDCError			0xE0830010			//创建DC出错
#define T9E_CreateRichEditFailed	0xE0830011			//创建RICH EDIT窗口失败
#define T9E_CreateDialogFailed		0xE0830012			//创建对话框失败
#define T9E_CalcutateMD5Failed		0xE0830013			//计算MD5码出错
#define T9E_EnumFailed				0xE0830014			//枚举出错
#define T9E_EnumNoMore				0xE0830015			//枚举结束，没有更多的文件了
#define T9E_cmdLineError			0xE0830016			//命令行出错
#define T9E_cmdLineParamError		0xE0830017			//命令行参数出错
#define T9E_createWndFailed			0xE0830018			//创建窗口出错
#define T9E_getProcessWndFailed		0xE0830019			//没有找到进程窗口
#define T9E_TooLittleBuffer			0xE083001A			//太少的存储空间
#define T9E_OperationCaceled		0xE083001B			//操作被取消
#define T9E_NoMoreItem				0xE083001C			//
#define T9E_InvalidIndex			0xE083001D			//
#define T9E_InvalidTag				0xE083001E			//无效标示
#define T9E_DB_Not_Open				0xE083001F			//数据库没有被打开
#define T9E_DB_Query_Failed			0xE0830020			//查询出错
#define T9E_Canceled				0xE0830021			//任务取消了
#define T9E_UnsupporttedVersion		0xE0830022			//不支持的版本


//发送错误
#define T9E_InvalidNumber			0xE0830200			//无效的LED屏编号
#define T9E_InvalidScreen			0xE0830201			//未曾创建该编号的屏幕
#define T9E_ScreenAlreadyExist		0xE0830202			//该编号的屏已经创建
#define T9E_NoScreenBufPool			0xE0830203			//无效的LED屏ID
#define T9E_OpenNICFailed			0xE0830204			//打开网卡失败
#define T9E_NoScreen				0xE0830205			//屏数量为0，需要先创建屏
#define T9E_Adapter_Not_Exist		0xE0830206			//网卡不存在
#define T9E_Buffer_Overflow			0xE0830207			//内存溢出，分配的内存太小
#define T9E_NICNotInitialized		0xE0830209			//网卡没有初始化
#define T9E_SendThreadNotStart		0xE083020A			//发送线程没有启动
#define T9E_NoNicDataReceived		0xE083020B			//没有收到数据
#define T9E_NicDataNotFromRcv		0xE083020C			//收到的数据不是来自接收卡
#define T9E_NicReadTimeOut			0xE083020D			//抓取接收卡数据超时
#define T9E_VerifyFailed			0xE083020E			//校验出错
#define T9E_NoCPPData				0xE083020F			//没有逐点校正数据
#define T9E_OpenComFailed			0xE0830210			//打开串口出错
#define T9E_COMNotInitialized		0xE0830211			//串口没有初始化
#define T9E_SendToSendCardFailed	0xE0830212			//发送给发送卡失败
#define T9E_Swap_Not11Corresponding	0xE0830213			//数据口交换不是一一对应
#define T9E_Swap_RcvAreaInvalid		0xE0830214			//接收卡的控制区域不正确

//渲染错误
#define T9E_NoRenderTexture			0xE0830300			//没有用来渲染的纹理
#define T9E_tooShortDuration		0xE0830301			//播放时间太短，以至于不能满足做完全部效果所需的时间
#define T9E_createACapDevFailed		0xE0830302			//创建音频采集设备出错
#define T9E_createVCapDevFailed		0xE0830303			//创建视频采集设备出错
#define T9E_regionSizeZero			0xE0830304			//区域大小为0
#define T9E_OpenAccessFailed		0xE0830305			//打开access失败
#define T9E_OpenODBCFailed			0xE0830306			//打开ODBC失败
#define T9E_OpenSqlServerFailed		0xE0830307			//打开Sql Server失败
#define T9E_NoVideoStream			0xE0830308			//没有视频流

//编辑错误
#define T9E_CreateScrnWndFailed		0xE0830400			//创建视频采集设备出错
#define T9E_NoPPTInstalled			0xE0830401          //没有安装Microsoft Powerpoint软件
#define T9E_NoExceInstalled			0xE0830402			//没有安装Microsoft Excel软件
#define T9E_NoWordInstalled			0xE0830403			//没有安装Microsoft Word软件

//发送卡相关错误
#define	T9E_SC_EraseTimeOut			0xE0830500			//擦除超时
#define	T9E_SC_WriteTimeOut			0xE0830501			//写入超时
#define	T9E_SC_SendFailed			0xE0830502			//发送失败
#define	T9E_SC_OpenComFailed		0xE0830503			//打开串口失败

//逐点校正相关
#define	T9E_CC_FLAT_IMAGE			0xE0830520			//图片太平滑，无法识别
#define	T9E_CC_DARK_IMAGE			0xE0830520			//图片太黑，无法识别

//网络相关错误
#define	T9E_SC_NotConnected			0xE0830600			//没有连接上
#define	T9E_SC_NotLogon				0xE0830601			//没有登陆

//VPContrl　相关错误
#define T9E_NO_DEV_LAN				0xE0830700			//没有可用网卡
#define T9E_CREATEUDP_FAIL			0xE0830701			//创建UDP连接失败
#define T9E_CARDCOUNT_LESS			0xE0830702			//接收卡数必须大于0

//相机错误
#define  T9E_EDS_NOT_CONNECTTED		0xE0830800			//没有连接上相机
#define  T9E_EDS_EVF_NOT_STARTTED	0xE0830801			//相机取景没有打开
#define  T9E_EDS_TAKEPHOTO			0xE0830802			//拍照错误
#define  T9E_EDS_DIR_NOT_FOUND		0xE0830803			//路径错误
#define  T9E_EDS_DEVICE_BUSY		0xE0830804			//设备忙
#define  T9E_EDS_DEVICE_NOT_FOUND	0xE0830805			//设备未发现
#define  T9E_EDS_STREAM_IO_ERROR	0xE0830806			// 相机流错误
#define  T9E_EDS_ACTION				0xE0830807			// 相机操控命令错误（SendCommand）
#define  T9E_EDS_CONNECT			0xE0830808			// 相机连接错误（SendCommand）

//相机拍照产生的错误
#define  T9E_EDS_TAKE_PICTURE_AF_NG					0xE0830809			// 对焦错误
#define  T9E_EDS_TAKE_PICTURE_MIRROR_UP_NG        	0xE083080A			// 当前配置镜头向上Currently configuring mirror up
#define  T9E_EDS_TAKE_PICTURE_SENSOR_CLEANING_NG	0xE083080B			// 当前清洁传感器Currently cleaning sensor
#define  T9E_EDS_TAKE_PICTURE_SILENCE_NG            0xE083080C			// 当前执行沉默操作Currently performing silent operations 
#define  T9E_EDS_TAKE_PICTURE_NO_CARD_NG           	0xE083080D			// 相机卡卡没有安装Card not installed
#define  T9E_EDS_TAKE_PICTURE_CARD_NG               0xE083080E			// 写相机卡错误Error writing to card 
#define  T9E_EDS_TAKE_PICTURE_CARD_PROTECT_NG   	0xE083080F			// 相机卡写保护Card write protected 
#define  T9E_EDS_TAKE_PICTURE_MOVIE_CROP_NG       	0xE0830810			// 拍摄视频错误_NG（此错误文档中未提及）
#define  T9E_EDS_TAKE_PICTURE_STROBO_CHARGE_NG 		0xE0830811			//闪光转化错误 STROBO_CHARGE_NG（此错误文档中未提及）
#define  T9E_EDS_PARTIAL_ERROR				 		0xE0830812			//部分出错


//LED监控软件 LedMonitor的返回值
#define  LED_MONITOR_NO_HISTORYERROR				0xE0830A00			//当前查询的日期无错误日志
#define  LED_MONITOR_STATE_NOMAL					0xE0830A01			//当前状态信息正常
enum ConnectionStyle	//级联方向
{
	cs_right_to_left = 0,	//从右到左
	cs_left_to_right,		//从左到右
	cs_top_to_bottom,		//从上到下
	cs_bottom_to_top,		//从下到上
};

enum VirtualPixelArrange
{
	vpa_RA_G__B_RB = 0,	//红A 绿/蓝 红B
	vpa_RA_B__G_RB,		//红A 蓝/绿 红B
	vpa_G_RA__RB_B,		//绿 红A/红B 蓝
	vpa_B_RA__RB_G,		//蓝 红A/红B 绿
};

enum PlayMode
{
	PlayMode_ByD3D = 1,
	PlayMode_CaptureScrn,
	PlayMode_CaptureWithPlay,
};

enum Lan_ID
{
	Lan_Auto			= 0,
	Lan_Chinese			= 1,
	Lan_Chinese_BIG5	= 2,
	Lan_English			= 3,
	Lan_French			= 4,
	Lan_Japanese		= 5,
	Lan_Korean			= 6,
	Lan_Russian			= 7,
	Lan_Thai			= 8,
	Lan_Spanish			= 9,
	Lan_Polish			= 10,
	Lan_Swdish			= 11,
};

enum T9VideoStand
{	T9VideoStandard_None = 0x00000000,
	T9VideoStandard_PAL = 0x00000001,
	T9VideoStandard_NTSC = 0x00000002,
};

// #ifdef _DEBUG
// #define MON_PROGRAM_NAME    _T("VisionMonitorD")
// #define MON_PROGRAM_EXE		_T("VisionMonitorD.exe")
// #else
#define MON_PROGRAM_NAME    _T("VisionMonitor")
#define MON_PROGRAM_EXE		_T("VisionMonitor.exe")
// #endif

// {7C003C9D-6097-4c8b-9C00-D7DCD173DBA5}
static const GUID FTP_TRAN_CLIENT_ID = 
{ 0x7c003c9d, 0x6097, 0x4c8b, { 0x9c, 0x0, 0xd7, 0xdc, 0xd1, 0x73, 0xdb, 0xa5 } };

// {2DCABA20-0BED-44b9-A4C1-C9D01916812C}
static const GUID CHROMA_CORRECTION_PARAM_FILE_ID = 
{ 0x2dcaba20, 0xbed, 0x44b9, { 0xa4, 0xc1, 0xc9, 0xd0, 0x19, 0x16, 0x81, 0x2c } };

// {025788CC-46E6-49c1-94E1-8232D615FAA2}
static const GUID COMPRESSED_3BYTES_LUMI_CALIBRATION_COEF_FILE_ID = 
{ 0x25788cc, 0x46e6, 0x49c1, { 0x94, 0xe1, 0x82, 0x32, 0xd6, 0x15, 0xfa, 0xa2 } };

// {A2D45FF0-9ECF-4f93-8F8A-CDD11E52CA32}
static const GUID COMPRESSED_3FLOATS_LUMI_CALIBRATION_COEF_FILE_ID = 
{ 0xa2d45ff0, 0x9ecf, 0x4f93, { 0x8f, 0x8a, 0xcd, 0xd1, 0x1e, 0x52, 0xca, 0x32 } };

// {585C58E3-8B77-451c-8C89-93C947AB3DFF}
static const GUID CalibrationClient_ID = 
{ 0x585c58e3, 0x8b77, 0x451c, { 0x8c, 0x89, 0x93, 0xc9, 0x47, 0xab, 0x3d, 0xff } };
// {CEF300A7-C746-481e-B3D2-5EDD63314661}
static const GUID COMPRESSED_9FLOATS_LUMI_CALIBRATION_COEF_FILE_ID = 
{ 0xcef300a7, 0xc746, 0x481e, { 0xb3, 0xd2, 0x5e, 0xdd, 0x63, 0x31, 0x46, 0x61 } };



enum ChromaCoef
{
	chroma_r_from_r	= 0,
	chroma_g_from_r,
	chroma_b_from_r,
	chroma_r_from_g,
	chroma_g_from_g,
	chroma_b_from_g,
	chroma_r_from_b,
	chroma_g_from_b,
	chroma_b_from_b,
	chroma_Component_max,
};

#pragma pack(1)

struct SChroma
{
	float chromaCoef[9];
};

struct S3FloatCoef
{
	float colorCoef[3];
};

#pragma pack()