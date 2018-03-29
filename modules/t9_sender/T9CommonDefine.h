#pragma once

//��ȷ����ֵ����
#define T9_S_OK				S_OK		//��ȷ
#define T9_NOERROR			NOERROR		//��ȷ
#define T9_S_FALSE			1		//��ȷ����û��ִ��
#define T9_E_FAIL			E_FAIL	//ʧ��

//������붨��
//��ʽ˵������winsows��������׼���壬������Windows�Դ���������ظ�
// T9_ERROR_XXXX	=	(0xE0830000 + x)	//�����		0XE0800000 11-1-0- 0000 1000 0011-0000 0000 0000 0000

//ͨ�ô���
#define T9E_UNKNOWN					0xE0830001			//δ֪����
#define T9E_ParamError				0xE0830002			//��������
#define T9E_NullPoint				0xE0830003			//������ָ��
#define T9E_NoImplement				0xE0830004			//δʵ��
#define T9E_Unsupport				0xE0830005			//��֧��
#define T9E_OutOfMemeroy			0xE0830006			//�ڴ治��
#define T9E_CreateFileError			0xE0830007			//�����ļ�����
#define T9E_OpenFileError			0xE0830008			//���ļ�����
#define T9E_InvalidFile				0xE0830009			//��Ч�ļ�
#define T9E_ReadFileError			0xE083000A			//���ļ�����
#define T9E_WriteFileError			0xE083000B			//д�ļ�����
#define T9E_FileDamaged				0xE083000C			//�ļ�����
#define T9E_CreateWndFailed			0xE083000D			//��������ʧ��
#define T9E_NotInit					0xE083000E			//û�г�ʼ��
#define T9E_FileNotExist			0xE083000F			//�ļ�������
#define T9E_CreateDCError			0xE0830010			//����DC����
#define T9E_CreateRichEditFailed	0xE0830011			//����RICH EDIT����ʧ��
#define T9E_CreateDialogFailed		0xE0830012			//�����Ի���ʧ��
#define T9E_CalcutateMD5Failed		0xE0830013			//����MD5�����
#define T9E_EnumFailed				0xE0830014			//ö�ٳ���
#define T9E_EnumNoMore				0xE0830015			//ö�ٽ�����û�и�����ļ���
#define T9E_cmdLineError			0xE0830016			//�����г���
#define T9E_cmdLineParamError		0xE0830017			//�����в�������
#define T9E_createWndFailed			0xE0830018			//�������ڳ���
#define T9E_getProcessWndFailed		0xE0830019			//û���ҵ����̴���
#define T9E_TooLittleBuffer			0xE083001A			//̫�ٵĴ洢�ռ�
#define T9E_OperationCaceled		0xE083001B			//������ȡ��
#define T9E_NoMoreItem				0xE083001C			//
#define T9E_InvalidIndex			0xE083001D			//
#define T9E_InvalidTag				0xE083001E			//��Ч��ʾ
#define T9E_DB_Not_Open				0xE083001F			//���ݿ�û�б���
#define T9E_DB_Query_Failed			0xE0830020			//��ѯ����
#define T9E_Canceled				0xE0830021			//����ȡ����
#define T9E_UnsupporttedVersion		0xE0830022			//��֧�ֵİ汾


//���ʹ���
#define T9E_InvalidNumber			0xE0830200			//��Ч��LED�����
#define T9E_InvalidScreen			0xE0830201			//δ�������ñ�ŵ���Ļ
#define T9E_ScreenAlreadyExist		0xE0830202			//�ñ�ŵ����Ѿ�����
#define T9E_NoScreenBufPool			0xE0830203			//��Ч��LED��ID
#define T9E_OpenNICFailed			0xE0830204			//������ʧ��
#define T9E_NoScreen				0xE0830205			//������Ϊ0����Ҫ�ȴ�����
#define T9E_Adapter_Not_Exist		0xE0830206			//����������
#define T9E_Buffer_Overflow			0xE0830207			//�ڴ������������ڴ�̫С
#define T9E_NICNotInitialized		0xE0830209			//����û�г�ʼ��
#define T9E_SendThreadNotStart		0xE083020A			//�����߳�û������
#define T9E_NoNicDataReceived		0xE083020B			//û���յ�����
#define T9E_NicDataNotFromRcv		0xE083020C			//�յ������ݲ������Խ��տ�
#define T9E_NicReadTimeOut			0xE083020D			//ץȡ���տ����ݳ�ʱ
#define T9E_VerifyFailed			0xE083020E			//У�����
#define T9E_NoCPPData				0xE083020F			//û�����У������
#define T9E_OpenComFailed			0xE0830210			//�򿪴��ڳ���
#define T9E_COMNotInitialized		0xE0830211			//����û�г�ʼ��
#define T9E_SendToSendCardFailed	0xE0830212			//���͸����Ϳ�ʧ��
#define T9E_Swap_Not11Corresponding	0xE0830213			//���ݿڽ�������һһ��Ӧ
#define T9E_Swap_RcvAreaInvalid		0xE0830214			//���տ��Ŀ���������ȷ

//��Ⱦ����
#define T9E_NoRenderTexture			0xE0830300			//û��������Ⱦ������
#define T9E_tooShortDuration		0xE0830301			//����ʱ��̫�̣������ڲ�����������ȫ��Ч�������ʱ��
#define T9E_createACapDevFailed		0xE0830302			//������Ƶ�ɼ��豸����
#define T9E_createVCapDevFailed		0xE0830303			//������Ƶ�ɼ��豸����
#define T9E_regionSizeZero			0xE0830304			//�����СΪ0
#define T9E_OpenAccessFailed		0xE0830305			//��accessʧ��
#define T9E_OpenODBCFailed			0xE0830306			//��ODBCʧ��
#define T9E_OpenSqlServerFailed		0xE0830307			//��Sql Serverʧ��
#define T9E_NoVideoStream			0xE0830308			//û����Ƶ��

//�༭����
#define T9E_CreateScrnWndFailed		0xE0830400			//������Ƶ�ɼ��豸����
#define T9E_NoPPTInstalled			0xE0830401          //û�а�װMicrosoft Powerpoint���
#define T9E_NoExceInstalled			0xE0830402			//û�а�װMicrosoft Excel���
#define T9E_NoWordInstalled			0xE0830403			//û�а�װMicrosoft Word���

//���Ϳ���ش���
#define	T9E_SC_EraseTimeOut			0xE0830500			//������ʱ
#define	T9E_SC_WriteTimeOut			0xE0830501			//д�볬ʱ
#define	T9E_SC_SendFailed			0xE0830502			//����ʧ��
#define	T9E_SC_OpenComFailed		0xE0830503			//�򿪴���ʧ��

//���У�����
#define	T9E_CC_FLAT_IMAGE			0xE0830520			//ͼƬ̫ƽ�����޷�ʶ��
#define	T9E_CC_DARK_IMAGE			0xE0830520			//ͼƬ̫�ڣ��޷�ʶ��

//������ش���
#define	T9E_SC_NotConnected			0xE0830600			//û��������
#define	T9E_SC_NotLogon				0xE0830601			//û�е�½

//VPContrl����ش���
#define T9E_NO_DEV_LAN				0xE0830700			//û�п�������
#define T9E_CREATEUDP_FAIL			0xE0830701			//����UDP����ʧ��
#define T9E_CARDCOUNT_LESS			0xE0830702			//���տ����������0

//�������
#define  T9E_EDS_NOT_CONNECTTED		0xE0830800			//û�����������
#define  T9E_EDS_EVF_NOT_STARTTED	0xE0830801			//���ȡ��û�д�
#define  T9E_EDS_TAKEPHOTO			0xE0830802			//���մ���
#define  T9E_EDS_DIR_NOT_FOUND		0xE0830803			//·������
#define  T9E_EDS_DEVICE_BUSY		0xE0830804			//�豸æ
#define  T9E_EDS_DEVICE_NOT_FOUND	0xE0830805			//�豸δ����
#define  T9E_EDS_STREAM_IO_ERROR	0xE0830806			// ���������
#define  T9E_EDS_ACTION				0xE0830807			// ����ٿ��������SendCommand��
#define  T9E_EDS_CONNECT			0xE0830808			// ������Ӵ���SendCommand��

//������ղ����Ĵ���
#define  T9E_EDS_TAKE_PICTURE_AF_NG					0xE0830809			// �Խ�����
#define  T9E_EDS_TAKE_PICTURE_MIRROR_UP_NG        	0xE083080A			// ��ǰ���þ�ͷ����Currently configuring mirror up
#define  T9E_EDS_TAKE_PICTURE_SENSOR_CLEANING_NG	0xE083080B			// ��ǰ��ഫ����Currently cleaning sensor
#define  T9E_EDS_TAKE_PICTURE_SILENCE_NG            0xE083080C			// ��ǰִ�г�Ĭ����Currently performing silent operations 
#define  T9E_EDS_TAKE_PICTURE_NO_CARD_NG           	0xE083080D			// �������û�а�װCard not installed
#define  T9E_EDS_TAKE_PICTURE_CARD_NG               0xE083080E			// д���������Error writing to card 
#define  T9E_EDS_TAKE_PICTURE_CARD_PROTECT_NG   	0xE083080F			// �����д����Card write protected 
#define  T9E_EDS_TAKE_PICTURE_MOVIE_CROP_NG       	0xE0830810			// ������Ƶ����_NG���˴����ĵ���δ�ἰ��
#define  T9E_EDS_TAKE_PICTURE_STROBO_CHARGE_NG 		0xE0830811			//����ת������ STROBO_CHARGE_NG���˴����ĵ���δ�ἰ��
#define  T9E_EDS_PARTIAL_ERROR				 		0xE0830812			//���ֳ���


//LED������ LedMonitor�ķ���ֵ
#define  LED_MONITOR_NO_HISTORYERROR				0xE0830A00			//��ǰ��ѯ�������޴�����־
#define  LED_MONITOR_STATE_NOMAL					0xE0830A01			//��ǰ״̬��Ϣ����
enum ConnectionStyle	//��������
{
	cs_right_to_left = 0,	//���ҵ���
	cs_left_to_right,		//������
	cs_top_to_bottom,		//���ϵ���
	cs_bottom_to_top,		//���µ���
};

enum VirtualPixelArrange
{
	vpa_RA_G__B_RB = 0,	//��A ��/�� ��B
	vpa_RA_B__G_RB,		//��A ��/�� ��B
	vpa_G_RA__RB_B,		//�� ��A/��B ��
	vpa_B_RA__RB_G,		//�� ��A/��B ��
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