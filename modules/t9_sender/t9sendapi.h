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
#define COMUNICATION_TYPE_COM		0x02	//���ڷ���
#define COMUNICATION_TYPE_ANY		0xFF	//��һ��ʽ


//Block define
#define BLOCK_NONE				0
#define BLOCK_ALL				0xffffffff
#define BLOCK_SEND_PIC			0x01
#define BLOCK_SEND_CTRL			0x02
#define BLOCK_ASYN_WRITEFUN		0x04
#define BLOCK_RCV_ANSWER		0x08
#define BLOCK_COEF_PIC			0x10	//���У��ͼƬ

#define UP_OLD_PROGRAM			0x01
#define UP_NEW_PROGRAM			0x02
#define UP_ALL_PROGRAM			0xff


// ���ֻص���Ϣ����
#define CBMSG_DROPFRAM				1		//���������ٶȸ����ϣ����ֶ�֡����ʱlParam������

#define CBMSG_RECEIVED_NIC_DATA		2		//�ӽ��ܿ��յ����������ݣ�lParam��Ϊָ��SReceivedNicData��ָ��
struct SReceivedNicData
{
	LPBYTE dataAddress;
	int	   dataLen;
};

#define CBMSG_SAVE_HWParamToRcv		3		//������������ܿ����ȱ�����Ϣ��
											//lParam��16Ϊ״̬��Ϣ��0���ڽ��У�1�ɹ���2ʧ�ܣ���8λΪ���Ȱٷֱȡ�

#define CBMSG_Read_HWParamFromRcv	4		//�ӽ��տ���ȡ�������ȱ�����Ϣ��
											//lParam��8Ϊ״̬��Ϣ��0���ڽ��У�1�ɹ���2ʧ�ܡ�
											//lParam�θ�8Ϊ���ͺ�
											//lParam��16λ�ĸ߰�λΪ�߰汾�ţ���8λΪ�Ͱ汾�š�

#define CBMSG_Read_FPGAFromRcv		5		//�ӽ��տ���ȡFPGA������ȱ�����Ϣ��
											//lParam��8λ״̬��Ϣ��0���ڽ��У�1�ɹ���2ʧ��
											//lParam�θ�8λΪ���ͺ�
											//lParam��16λ�ĸ߰�λΪ�߰汾�ţ���8λΪ�Ͱ汾�š�

#define CBMSG_Upgrade_FPGAFromRcv	6		//����FPGA���򵽽��տ��Ľ��ȱ�����Ϣ��
											//lParam��8λΪ״̬��Ϣ��0���ڽ��У�1�ɹ���2ʧ��
											//lParam�θ�8λΪ������Ϣ��1��ʼ��2������3д�룬4У�飬0��ǰһ����ͬ
											//lParam���16λΪ����

#define CBMSG_Detected_RCV_Card		7		//̽�⵽�˽��ܿ���
											//lParam��16Ϊ״̬��Ϣ��0��ʼ̽�⣬1�ɹ���2ʧ�ܣ�3̽�⵽���εͰ�λΪ�汾��Ϣ����8λΪ��������ţ��ɹ�ʱΪ��������

#define CBMSG_AsynWriteFunCard_Rslt	8		//�첽д���ܿ����
											//lParam��Ϊ״̬��Ϣ��0ʧ�ܣ�1�ɹ���2��ʱ

#define CBMSG_SendCorrectCoef2Rcv   9       //����У�����ݵ����տ��Ľ�����Ϣ��
                                            //lParam��16λΪ״̬��Ϣ��0���ڽ��У�1�ɹ���2ʧ��
                                            //lParam��16λΪ������Ϣ��0~100

#define CBMSG_Sending_FPGA_OP		10		//���Ϳ��������ȱ�����Ϣ��
											//lParam��8λΪ״̬��Ϣ��0���ڽ��У�1�ɹ���2ʧ��
											//lParam�θ�8λΪ������Ϣ��1��ʼ��2������3д�룬4��ȡ��0��ǰһ����ͬ
											//lParam���16λ������

#define CBMSG_Open_WinpcapError		11		//Open WinPcapError, lParamָ�������ʾ���ַ���(char*)

#define CBMSG_Save_HWParamToSend  12		//���Ͳ��������Ϳ��Ľ��ȱ�����Ϣ
											//lParam��16Ϊ״̬��Ϣ��0���ڽ��У�1�ɹ���2ʧ�ܣ���8λΪ���Ȱٷֱȡ�


/*************************************************
* T9SenderCallback
* ����˵����
    �ص��������μ����ֻص���Ϣ����

* ����˵����
.   [in]	wParam
	��Ϣ����
.	[in]	lParam
	��Ϣֵ
.	[in]	data
	����

* ����ֵ˵����

*************************************************/
typedef HRESULT (__stdcall *T9SenderCallback)(WPARAM wParam, LPARAM lParam, DWORD data);


extern "C"{

/*************************************************
* T9GetNetAdapterCount
* ����˵����
    ��ȡ�������Ѿ���װ����������

* ����˵����
.   [out]	pCount
	������������

* ����ֵ˵����
.   T9_S_OK
	�ɹ� 
*************************************************/
T9API	T9GetNetAdapterCount(DWORD *pCount);


/*************************************************
* T9GetNetAdapterInfo
* ����˵����
    ��ȡ������Ϣ

* ����˵���� 
.	[in]	index
	����������
.	[out]	pAdapterId
	���ظ�������id��pAdapterIdָ����ڴ��С����С��260���ֽ�
.	[in]	adapterIdBufLen
    pAdapterIdʵ��ָ����ڴ��С
.   [out]	pAdapterDesp
	���ظ�������������Ϣ��pAdapterDespָ����ڴ��С����С��260���ֽ�
.	[in]	adapterDespBufLen
	adapterDespBufLenָ����ڴ��С

* ����ֵ˵����
.	T9E_Adapter_Not_Exist
	�޷���ȡ����������Ϣ
.	T9E_Buffer_Overflow
	pAdapterId����pAdapterDespָ����ڴ�̫С
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9GetNetAdapterInfo(DWORD	index,
							char	*pAdapterId,
							DWORD	adapterIdBufLen,	//����ڻ��ߵ���260
							char	*pAdapterDesp,
							DWORD	adapterDespBufLen);	//����ڻ��ߵ���260


/*************************************************
* T9CreateScreen
* ����˵����
    ����һ����Ļ

* ����˵���� 
.   [in]	number
	�´�����Ļ�ı��
.	[in]	width
	��Ļ���					����ȡֵ��Χ 1 - 1024*1024
.	[in]	height
	��Ļ�߶�					����ȡֵ��Χ 1 - 1024*1024
.	[in]	bVirtual
    �Ƿ���������
.	[in]	virtualType
	����������				����ȡֵ��Χ0~3       0 vpa_RA_G__B_RB   1 vpa_RA_B__G_RB   2 vpa_G_RA__RB_B   3 vpa_B_RA__RB_G
.	[in]	connectionType
	���տ��ļ�������	����ȡֵ��Χ0~3      0 cs_right_to_left    1 cs_left_to_right    2 cs_top_to_bottom  3 cs_bottom_to_top
* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_InvalidNumber
	���������ţ������ŷ�ΧΪ1��MAX_SCREEN_NUMBER
.	T9E_ScreenAlreadyExist
	�ñ�ŵ����Ѿ�����
.	T9E_NoImplement
	����δʵ��
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9CreateScreen(DWORD	number,
					   DWORD	width,
					   DWORD	height,
					   BOOL		bVirtual, 
					   int		virtualType, 
					   int		connectionType);


/*************************************************
* T9DestroyScreen
* ����˵����
	ɾ��һ����Ļ

* ����˵���� 
.   [in]	number
	��Ҫɾ������Ļ�ı��

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_InvalidScreen
	δ�������ñ�ŵ���Ļ
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9DestroyScreen(DWORD	number);

/*************************************************
* T9DestroyAllScreen
* ����˵����
	ɾ��������Ļ

* ����˵���� 


* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9DestroyAllScreen();

/*************************************************
* T9GetScreenInfo
* ����˵����
	������Ļid�Ż�ȡ��Ļ��Ϣ

* ����˵���� 
.   [in]	number
	��Ҫ��ȡ��Ϣ����Ļ���
.   [out]	pWidth 
	������Ļ���
.   [out]	pHeight
	������Ļ�߶�
.   [out]	pbVirtual
	������Ļ�Ƿ���������

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_InvalidScreen
	δ�������ñ�ŵ���Ļ
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9GetScreenInfo(DWORD	number,
						DWORD	*pWidth,
						DWORD	*pHeight,
						BOOL	*pbVirtual);


/*************************************************
* T9GetScreenCount
* ����˵����
	��ȡ�Ѿ���������Ļ����

* ����˵���� 
.   [out]	pCount
	�Ѿ���������Ļ����

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9GetScreenCount(DWORD *pCount);


/*************************************************
* T9GetScreenNumber
* ����˵����
	������Ļ�����Ż�ȡ��Ļid

* ����˵���� 
.   [in]	index
	��Ļ�����ţ�����������Ϊ[0, ��Ļ������1]
.   [out]	pNumber
	������Ļ���

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_InvalidIndex
	��Ч��Ļ�����ţ��μ�index����˵��
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9GetScreenNumber(	DWORD	index,
							DWORD	*pNumber);

/*************************************************
* T9SetScreenVirtualType
* ����˵����
	����������Ļ������

* ����˵���� 
.   [in]  number
	��Ҫ���õ���Ļ����Ļ��   

.   [in]  virtualType					����ȡֵ��Χ0~3       0 vpa_RA_G__B_RB   1 vpa_RA_B__G_RB   2 vpa_G_RA__RB_B   3 vpa_B_RA__RB_G
	������Ļ������

* ����ֵ˵����
.	T9E_InvalidScreen
	δ�������ñ�ŵ���Ļ
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SetScreenVirtualType(DWORD number, int virtualType);


/*************************************************
* T9SetScreenConnectionStyle
* ����˵����
	����������Ļ������

* ����˵���� 
.   [in]  number
	��Ҫ���õ���Ļ����Ļ��

.   [in]  connectionStyle				����ȡֵ��Χ0~3      0 cs_right_to_left    1 cs_left_to_right    2 cs_top_to_bottom  3 cs_bottom_to_top
	���տ��ļ�������

* ����ֵ˵����
.	T9E_InvalidScreen
	δ�������ñ�ŵ���Ļ
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SetScreenConnectionStyle(DWORD number, int connectionStyle);

/*************************************************
* T9SenderStart
* ����˵����
   ��ʼ����������ʼ����ͼ������

* ����˵���� 
.	[in]	comunicationType
    ͨѶ��ʽ�����ڻ��ߴ��ڣ����Comunication Type define
.	[in]	pParam
	 ���ݷ������ݵķ�ʽ��ͬ����ͬ���������ڷ���ʱ��pParamΪ��ǰʹ�õ��������� ���Դ��ڷ���ʱ pParamΪ���ں�

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_NoScreen
	��û�д���������Ҫ�ȴ������е������ܿ�ʼ����
.	T9E_OpenNICFailed
	������ʧ��
.	T9E_UNKNOWN
	δ֪����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SenderStart(DWORD comunicationType, LPVOID pParam);


/*************************************************
* T9SenderStop
* ����˵����
    ֹͣ����ͼ������

* ����˵���� 

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SenderStop();

/*************************************************
* T9IsSenderRunning
* ����˵����
	�Ƿ������շ�ģ����������

* ����˵���� 
	comunicationType ͨѶ��ʽ�����ڻ��ߴ��ڣ����Comunication Type define

* ����ֵ˵����
.	T9_S_OK
	��������
.	T9_E_FAILE
	û������
*************************************************/
T9API	T9IsSenderRunning(DWORD comunicationType);

/*************************************************
* T9SetBirghtness
* ����˵����
      ��������

* ����˵���� 
.	[in]	bright
	����ֵ
.	[in]	bright_r
	��ɫ������ֵ
.	[in]	bright_g
	��ɫ������ֵ
.	[in]	bright_b
	��ɫ������ֵ
.	[in]	bright_vr
	͸����
.	[in]	colorTempture
	ɫ��
* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SetBirghtness(BYTE bright, BYTE bright_r, BYTE bright_g, BYTE bright_b, BYTE bright_vr, WORD colorTempture);

/*************************************************
* T9SetIsUseRcvParam
* ����˵����
      �����Ƿ�ʹ�ý��տ�����

* ����˵����
.	[in]	bUseRcvParam
	TRUE��ʹ�ý��տ�������FALSE��ʹ�ü����ʵʱ����

* ����ֵ˵����
.	T9_S_OK
�ɹ�
*************************************************/
T9API	T9SetIsUseRcvParam(BOOL bUseRcvParam);

/*************************************************
* T9SetPcRealTimeParam
* ����˵����
	�ı�����ʵʱ����

* ����˵���� 
.	[in]	lpBuf
	�������ڴ��ַ 
.	[in]	bufLen
	�������ڴ泤��

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SetPcRealTimeParam(LPBYTE lpBuf, DWORD bufLen);

/*************************************************
* SetSendParamScreenNumber
* ����˵����
	�������ĸ�������ͼ������֮ǰ����Ҫ�ȷ��͵���������

* ����˵���� 
.	[in]	number
	��Ļ���

* ����ֵ˵����
.	T9E_InvalidScreen
	δ�������ñ�ŵ���Ļ
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SetSendParamScreenNumber(DWORD number);


/*************************************************
* T9SendControlPack
* ����˵����
    ���Ϳ��Ʋ�����

* ����˵���� 
.	[in]	pBuf
	�������ڴ��ַ
.	[in]	bufLen
	�������ڴ泤��
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_SendThreadNotStart
	�����߳�û������
.	T9E_ParamError
	��������ָ�Ĵ����ú����Ĳ������󣬶�����pBuf�����ݴ���
.	T9_S_FALSE
	�����ڲ����������͵Ŀ����������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SendControlPack(LPBYTE pBuf, DWORD bufLen, BYTE portIndex);

/*************************************************
* T9SendRTCSetting
* ����˵����
    ����RTC�й����ò���

* ����˵���� 
.	[in]	pBuf
	�������ڴ��ַ
.	[in]	bufLen
	�������ڴ泤��
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	E_FAIL
	ʧ��
.	T9_NOERROR
	��ȷ

*************************************************/
T9API	T9SendRTCSetting(IN LPBYTE pBuf, IN DWORD bufLen, IN BYTE portIndex);

/*************************************************
* T9Ctrl_DetectRcvCardEx
* ����˵����
	̽����տ�

* ����˵����
.	[out] pRcvCount
	����̽�⵽�Ľ��ܿ�����
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
	T9E_ParamError
	������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9Ctrl_DetectRcvCardEx(DWORD *pRcvCount, BYTE portIndex);

/*************************************************
* T9Ctrl_DetectRcvCardEx2
* ����˵����
	̽����տ�

* ����˵����
	[in] rcvIndex
	���տ���ţ�ȡֵ��Χ[0~65536]
	[in] pBuf
	�ڴ��ַ�������洢���յ��Ŀ���Ϣ
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
* ����ֵ˵����
.	E_FAIL
	ʧ��
.	T9E_ParamError
	������
.	T9E_OutOfMemeroy
	�ڴ����
.	NOERROR
	��ȷ
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9Ctrl_DetectRcvCardEx2(WORD rcvIndex, LPBYTE pBuf, BYTE portIndex);

/*************************************************
* T9Ctrl_DetectRcvCardEx3
* ����˵����
	̽����տ�

* ����˵����
.	[out] pRcvCount
	����̽�⵽�Ľ��ܿ�����
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
	[in] startIndex
	��ʼ����
	[in] count
	̽�⿨������

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
	T9E_ParamError
	������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9Ctrl_DetectRcvCardEx3(DWORD *pRcvCount, BYTE portIndex, WORD startIndex, WORD count);

// T9API	T9Ctrl_DetectRcvCardPin(BYTE portIndex, BYTE rcvIndex, SDetectRcvPinAnswer *pRcvPinAnswer);


/*************************************************
* T9Ctrl_DetectRcvCard
* ����˵����
	̽����տ�

* ����˵����
	rcvIndex
	���տ���ţ�ȡֵ��Χ[0~65536]
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
	* ����ֵ˵����
	T9E_ParamError
	������
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_SendThreadNotStart
	�����߳�û������
.	T9_S_FALSE
	�����ڲ����������͵Ŀ����������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9Ctrl_DetectRcvCard(WORD rcvIndex, BYTE portIndex);

/*************************************************
* T9ClearMackPacketCount
* ����˵����
	������տ����������

* ����˵����
	rcvIndex
	���տ���ţ�ȡֵ��Χ[0~65536]
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	9E_ParamError
	��������
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_SendThreadNotStart
	�����߳�û������
.	T9_S_FALSE
	�����ڲ����������͵Ŀ����������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9ClearMackPacketCount(WORD rcvIndex, BYTE portIndex);


/*************************************************
* T9Ctrl_DetectFunCard
* ����˵����
	̽�⹦�ܿ�

* ����˵����
.	funCardIndex
	���ܿ���ţ�ȡֵ��Χ[0~254]
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_SendThreadNotStart
	�����߳�û������
.	T9E_OutOfMemeroy
	�ڴ����
.	T9_S_FALSE
	�����ڲ����������͵Ŀ����������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9Ctrl_DetectFunCard(int funCardIndex, BYTE portIndex);


/*************************************************
* T9Ctrl_DetectFunCardEx
* ����˵����
	̽�⹦�ܿ�

* ����˵����
.	funCardIndex
	���ܿ���ţ�ȡֵ��Χ[0~254]
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
	[in] pBuf
	�ڴ��ַ�������洢���յ��Ŀ���Ϣ

* ����ֵ˵����
.	T9E_ParamError
	������
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_SendThreadNotStart
	�����߳�û������
.	T9E_OutOfMemeroy
	�ڴ����
.	T9_S_FALSE
	�����ڲ����������͵Ŀ����������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9Ctrl_DetectFunCardEx(int funCardIndex, BYTE portIndex, LPBYTE pBuf);


/*************************************************
* T9Ctrl_5036PixelCheck
* ����˵����
	5036�����

* ����˵����
	[in] lpByte
	���������ڴ��ַָ��
	[in] bufLen
	�������ݳ���
	[in] cardIndex
	�����         ȡֵ��Χ[0~2048]

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_SendThreadNotStart
	�����߳�û������
.	T9E_OutOfMemeroy
	�ڴ����
.	T9_S_FALSE
	�����ڲ����������͵Ŀ����������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9Ctrl_5036PixelCheck(LPBYTE lpByte, int bufLen, WORD cardIndex);

/*************************************************
* T9Ctrl_ExecFunPower
* ����˵����
	���ܿ���Դ����

* ����˵����
	[in] lpByte
	���������ڴ��ַָ��
	[in] bufLen
	�������ݳ���

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_SendThreadNotStart
	�����߳�û������
.	T9E_OutOfMemeroy
	�ڴ����
.	T9_S_FALSE
	�����ڲ����������͵Ŀ����������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9Ctrl_ExecFunPower(LPBYTE lpByte, int bufLen);

/*************************************************
* T9Ctrl_ReqParamAndVer
* ����˵����
	̽�⹦�ܿ�

* ����˵����
	rcvIndex
	���տ���ţ�ȡֵ��Χ[0~65536]

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_SendThreadNotStart
	�����߳�û������
.	T9_S_FALSE
	�����ڲ����������͵Ŀ����������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9Ctrl_ReqParamAndVer(WORD rcvIndex);

/*************************************************
* T9Ctrl_FunCard_FlashOperation
* ����˵������
	���ܿ�Flash����

* ����˵����
.	[in]	opType	
	��������: ȡֵΪFlashOperateType��ֵ
.	[in]	highAddress
	����Ŀ�ĸ�λ��ַ
.	[in]	lowAddress
	����Ŀ�ĵ�λ��ַ
.	[in]	lpBuf
	opTypeΪflash_op_write��д������ݣ�Ϊ256���ֽڳ�

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_SendThreadNotStart
	�����߳�û������
.	T9_S_FALSE
	�����ڲ����������͵Ŀ����������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9Ctrl_FunCard_FlashOperation(BYTE	opType, 
									  BYTE	highAddress,
									  BYTE	lowAddress,
									  LPBYTE lpBuf);


/*************************************************
* T9SendScreenPicture
* ����˵����
    ����һ��ͼ�����ݣ�ֱ�ӷ��ͷ�ʽ��

* ����˵���� 
.	[in]	number
	��Ļ���
.	[in]	pBuf
	ͼ�������ڴ��ַ

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_InvalidScreen
	δ�������ñ�ŵ���Ļ
.	T9E_NoScreenBufPool
	û�д�������Ļ��ͼ���ڴ滺���
.	T9_S_FALSE
	�����ڲ����������͵�ͼ�����ݱ�����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SendScreenPicture(DWORD	number,
							LPBYTE	pBuf);

/*************************************************
* T9SendScreenCorrectCoef
* ����˵����
   �������У��ϵ��

* ����˵���� 
.	[in]	number
	��Ļ���
.	[in]	pBuf
	���У��ϵ����ַ

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_InvalidScreen
	δ�������ñ�ŵ���Ļ
.	T9E_NoScreenBufPool
	û�д�������Ļ��ͼ���ڴ滺���
.	T9_S_FALSE
	�����ڲ����������͵�ͼ�����ݱ�����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SendScreenCorrectCoef(DWORD	number,
								LPBYTE	pBuf);
/*************************************************
* T9SendScreenBlackPicture
* ����˵����
    ����һ����ɫ��ͼ�����Ļ

* ����˵���� 
.	[in]	number
	��Ļ���

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9E_InvalidScreen
	δ�������ñ�ŵ���Ļ
.	T9E_NoScreenBufPool
	û�д�������Ļ��ͼ���ڴ滺���
.	T9_S_FALSE
	�����ڲ����������͵�ͼ�����ݱ�����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SendScreenBlackPicture(DWORD	number);

/*************************************************
* T9SetCallBackWnd
* ����˵����
    ���ý��ܵײ�ģ��֪ͨ�Ĵ��ھ��������ģ��ͨ��������Ϣ�ķ�ʽ֪ͨ�ϲ����ĸ�����Ϣ

* ����˵���� 
.	[in]	hWnd
	���ھ��
.	[in]	msgId
	����ģ������ڷ��ʹ�����Ϣ��ID��

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
// T9API	T9SetCallBackWnd(HWND hWnd, DWORD msgId);


/*************************************************
* T9SetCallBackFun
* ����˵����
    ���ûص�����������ģ����ûص�����֪ͨ�ϲ������Ϣ

* ����˵���� 
.	[in]	pCallbackFun
	�ص�����ָ�룬���pCallbackFun����NULL������ص������趨

* ����ֵ˵����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SetCallBackFun(T9SenderCallback pCallbackFun);

/*************************************************
* T9WriteHWParamToRcvCard
* ����˵����
    ���Ͳ��������տ�

* ����˵���� 
.	[in]	pBuf
	Ӳ�������ڴ��ַ
.	[in]	bufLen
	Ӳ�������ڴ泤��
.	[in]	scrnIndex
	��Ļ������
.	[in]	cardCount��ȡֵ��Χ[1~2047]
	������
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	markedOnly
	������

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
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
* ����˵����
    ���Ͳ��������տ�

* ����˵���� 
.	[in]	pBuf
	Ӳ�������ڴ��ַ
.	[in]	bufLen
	Ӳ�������ڴ泤��
.	[in]	scrnIndex
	��Ļ������
.	[in]	skipCount
	����ǰ��Ŀ������ã�skipCountΪ�����Ŀ�����
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	markedOnly
	������

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
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
* ����˵����
    ���Ͳ�����ģ��

* ����˵���� 
.	[in]	pBuf
	Ӳ�������ڴ��ַ
.	[in]	bufLen
	Ӳ�������ڴ泤��
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9WriteHWParamToModule(	LPBYTE	pBuf, 
								DWORD	bufLen, 
								BYTE	portIndex);

/*************************************************
* T9WriteHWParamToFunCard
* ����˵����
	���Ͳ��������ܿ�

* ����˵���� 
.	[in]	pBuf
	Ӳ�������ڴ��ַ
.	[in]	bufLen
	Ӳ�������ڴ泤��
.	[in]	scrnIndex
	��Ļ������
.	[in]	skipCount
	����ǰ��Ŀ������ã�skipCountΪ�����Ŀ�����
.	[in]	funIndex��ȡֵ��Χ[0~254]
	������

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9WriteHWParamToFunCard(LPBYTE pBuf, DWORD bufLen, int funIndex);

/*************************************************
* T9ReadHWParamToFile
* ����˵����
    �ӽ��ܿ��ж��ز�������ָ���ļ���

* ����˵���� 
.	[in]	pFileName
	��������ļ���·����
.	[in]	cardIndex��ȡֵ��Χ[0~254]
	���տ���
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_NicReadTimeOut
	�����ݳ�ʱ
.	T9E_NoNicDataReceived
	������δ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9ReadHWParamToFile(char *pFileName, WORD cardIndex, BYTE portIndex);

/*************************************************
* T9ReadHWParamToBuffer
* ����˵����
    �ӽ��ܿ��ж��ز�������ָ���ļ���

* ����˵���� 
.	[in]	cardIndex��ȡֵ��Χ[0~254]
	���տ���
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	lpBuffer
	�洢�����ļ����ڴ�ָ��
.	[in,out]	*pBufLen
	�ڴ泤�ȣ����ú󷵻�ʵ�ʶ�ȡ�ĳ���

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_NicReadTimeOut
	�����ݳ�ʱ
.	T9E_NoNicDataReceived
	������δ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9ReadHWParamToBuffer(WORD cardIndex, BYTE portIndex, LPBYTE lpBuffer, UINT *pBufLen);

/*************************************************
* T9ReadFlashToBuffer
* ����˵����
    ��ȡflash���ڴ���

* ����˵���� 
.	[in]	cardIndex��ȡֵ��Χ[0~254]
	���տ���
.	[in]	hAddress
	�ߵ�ַ
.	[in]	lAddress
	�͵�ַ
.	[in]	dataLen
	���ݳ���
.	[in]	pBuffer
	�����ڴ��ַ
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	bCallBack
	�Ƿ���Ҫ�ص�

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_NicReadTimeOut
	�����ݳ�ʱ
.	T9E_NoNicDataReceived
	������δ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
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
* ����˵����
	����T9���տ�FPGA����

* ����˵���� 
.	[in]	lpBuffer
	FPGA����
.	[in]	bufLen
	FPGA�����ֽ���
.	[in]	cardIndex
	�����Ŀ��ţ�[0~2047]ʱΪ�ƶ�����Ϊ0xffffʱ���������п�
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	flags
	������
.	[in]	bQ9
	�Ƿ���Q9��
* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9UpgradeRcvCardFPGA2(LPBYTE lpBuffer, DWORD bufLen, WORD cardIndex, BYTE portIndex, BYTE flags, BOOL bQ9);

//�������տ��Ĳ�ͬ����汾
T9API	T9UpgradeRcvCardFPGA2Ex(LPBYTE lpBuffer, DWORD bufLen, WORD cardIndex, BYTE portIndex, BYTE flags, BOOL bQ9, BYTE programVersion);

/*************************************************
* T9ReadRcvCardFPGA2
* ����˵����
    �ӽ��ܿ��ж���FPGA����ָ���ļ���

* ����˵���� 
.	[in]	lpBuffer
	����FPGA������ڴ��ַ
.	[in]	bufLen
	����FPGA������ڴ泤��
.	[in]	cardIndex
	���տ������ţ�ȡֵ��Χ[0~254]
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	bQ9
	�Ƿ���Q9��
* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_VerifyFailed
	У��ʧ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_NicReadTimeOut
	�����ݳ�ʱ
.	T9E_NoNicDataReceived
	������δ����
.	T9E_ParamError
	��������
.	T9E_TooLittleBuffer
	������ڴ�̫С
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9ReadRcvCardFPGA2(LPBYTE lpBuffer, DWORD *pBufLen, WORD cardIndex, BYTE portIndex, BOOL bQ9);

//��ȡ���տ��Ĳ�ͬ����汾
T9API	T9ReadRcvCardFPGA2Ex(LPBYTE lpBuffer, DWORD *pBufLen, WORD cardIndex, BYTE portIndex, BOOL bQ9, BYTE programVersion);

/*************************************************
* T9ClearCCCoefData
* ����˵����
    ����������У��ϵ��

* ����˵���� 
	�޲���

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9ClearCCCoefData();

/*************************************************
* T9AddCCCoef
* ����˵����
    ���һ�Ž��տ����������У������

* ����˵���� 
.	[in]	rcvIndex
	���տ���ţ���0��ʼ����
.	[in]	lpCCCoefBuf
	���У������	
.	[in]	bufLen
	У�������ڴ泤��

* ����ֵ˵����
.	T9E_OutOfMemeroy
	�ڴ����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9AddCCCoef(WORD rcvIndex, LPBYTE lpCCCoefBuf, int bufLen);

T9API	T9AddCCCoef16(WORD rcvIndex, BYTE dataGroup, int moduleIndex, LPBYTE lpCCCoefBuf, int bufLen);

/*************************************************
* T9WriteCCCoefToRcvCard
* ����˵����
    ���������У�����ݴ�����տ���

* ����˵���� 
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/

T9API	T9WriteCCCoefToRcvCard(BYTE portIndex);

T9API	T9WriteCCCoefToRcvCardMoudle(BYTE portIndex);

/*************************************************
* T9LoadRcvCCCoef
* ����˵����
    ������Ԥ����������У��ϵ������sd ram

* ����˵���� 
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9LoadRcvCCCoef(BYTE portIndex);
T9API	T9LoadRcvCurrent(BYTE portIndex);


/*************************************************
* T9ClearChromaCCoefData
* ����˵����
    ������ɫ��У��ϵ��

* ����˵���� 
	�޲���

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9ClearChromaCCoefData();

/*************************************************
* T9AddChromaCCoef16
* ����˵����
    ���һ�Ž��տ������ɫ��У������

* ����˵���� 
.	[in]	rcvIndex
	���տ���ţ���0��ʼ����
.	[in]	lpCCCoefBuf
	���У������	
.	[in]	bufLen
	У�������ڴ泤��

* ����ֵ˵����
.	T9E_OutOfMemeroy
	�ڴ����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9AddChromaCCoef16(WORD rcvIndex, LPBYTE lpCCCoefBuf, int bufLen);
T9API	T9AddChromaCCoef8(WORD rcvIndex, LPBYTE lpCCCoefBuf, int bufLen, BOOL b5A);

T9API	T9AddMoudleChromaCCoef16(WORD rcvIndex, BYTE dataGroup, int moduleIndex, CRect moudleRc, LPBYTE lpCCCoefBuf, int bufLen);

/*************************************************
* T9WriteChromaCCoefToRcvCard
* ����˵����
    �����ɫ��У�����ݴ�����տ���

* ����˵���� 
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9WriteChromaCCoefToRcvCard(BYTE portIndex);

T9API	T9WriteChromaCCoefToRcvCardMoudle(BYTE portIndex);

/*************************************************
* T9LoadRcvChromaCCoef
* ����˵����
    ������Ԥ������ɫ��У��ϵ������sd ram

* ����˵���� 
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9LoadRcvChromaCCoef(BYTE portIndex);

/*************************************************
* T9ReadRcvCardFPGA
* ����˵����
    �ӽ��ܿ��ж���FPGA�������ָ���ļ���

* ����˵���� 
.	[in]	pFileName
	��������ļ���·����
.	[in]	cardIndex
	���տ������ţ�ȡֵ��Χ[0~254]
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_VerifyFailed
	У��ʧ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_NicReadTimeOut
	�����ݳ�ʱ
.	T9E_NoNicDataReceived
	������δ����
.	T9E_ParamError
	��������
.	T9E_OpenFileError
	���ļ�����
.	T9E_InvalidFile
	�Ƿ��ļ�
.	T9E_ReadFileError
	���ļ�����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9ReadRcvCardFPGA(char *pFileName, WORD cardIndex, BYTE portIndex);


/*************************************************
* T9AsynWriteFunCard
* ����˵����
    �첽д���ܿ�����

* ����˵���� 
.	[in]	funCardIndex
	���ܿ����
.	[in]	pBuffer
	д��������ڴ��ַ
.	[in]	bufLen
	д������ݳ���
.	[in]	highAddress
	��λ��ַ
.	[in]	lowAddress
	��λ��ַ
.	[in]	timeOut
	������͵�ʱ�䣬��λ������

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9AsynWriteFunCard(	int		funCardIndex,
							LPBYTE	pBuffer,
							DWORD	bufLen,
							int		highAddress,
							int		lowAddress,
							DWORD	timeOut);

/*************************************************
* T9StroageErase
* ����˵����
    �������տ���Flash�洢�ռ��������

* ����˵����
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	rcvIndex
	���տ����
.	[in]	startBlock
	��ʼ����������������
.	[in]	blockCount
	��������������

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9StroageErase(BYTE		portIndex,
					   WORD		rcvIndex,
					   WORD		startBlock,
					   WORD		blockCount);

/*************************************************
* T9StroageWritePage
* ����˵����
    ����տ���Flash�洢�ռ���д��һ��page������

* ����˵����
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	rcvIndex
	���տ����
.	[in]	pageIndex
	ҳ�����
.	[in]	pBuffer
	ָ��д�����������ݵ���ʼ��ַ
.	[in]	pageSize
	page��С

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9StroageWritePage(	BYTE	portIndex,
							WORD	rcvIndex,
							DWORD	pageIndex,
							LPBYTE	pBuffer,
							int		pageSize);

/*************************************************
* T9StroageWriteSeciton
* ����˵����
    ����տ���Flash�洢�ռ���д��һ������������

* ����˵����
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	rcvIndex
	���տ����
.	[in]	sectionIndex
	���������
.	[in]	sectionSize
	������С
.	[in]	pBuffer
	ָ��д�����������ݵ���ʼ��ַ
.	[in]	timeout
	����timeoutʱ��û��д���㳬ʱ

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9StroageWriteSeciton(	BYTE	portIndex,
								WORD	rcvIndex,
								DWORD	sectionIndex,
								DWORD	sectionSize,
								LPBYTE	pBuffer,
								DWORD	timeout);


/*************************************************
* T9SendDelayPacket
* ����˵����
    ����һ����ʱ���ݰ�

* ����˵���� 
.	[in]	packetSize
	���տ����
.	[in]	bAsyn
	�Ƿ���뵽������

* ����ֵ˵����
.	T9E_NICNotInitialized
	����û�г�ʼ��
.	T9E_OutOfMemeroy
	�ڴ����
.	T9E_ParamError
	��������
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SendDelayPacket(DWORD packetSize, BOOL bAsyn);

/*************************************************
* T9SetLineSpace
* ����˵����
	���÷�������ʱ���м�϶
* ����˵���� 
.	[in]	lineSpace
	�м�϶����

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
// T9API	T9SetLineSpace(int lineSpace);


// /*************************************************
// * T9SetBigModuleMode
// * ����˵����
// 	����Ϊ��ģ��ģʽ
// * ����˵���� 
// .	[in]	bigModuleMode
// 	��ģ��ģʽ�����BigMoudule_Mode����
// 
// * ����ֵ˵����
// .	T9_S_OK
// 	�ɹ�
// *************************************************/
// T9API	T9SetBigModuleMode(int bigModuleMode);


/*************************************************
* T9ClearSendCache
* ����˵����
	������ͻ���
* ����˵���� 

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9ClearSendCache();


/*************************************************
* T9SetScreenHollow
* ����˵����
	������ͻ���
* ����˵���� 
.	[in]	scrnNumber
	��Ļ���
.	[in]	bNeedHollow
	�Ƿ����
.	[in]	senderRegion
	��������

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SetScreenHollow(int		scrnNumber,
						  BOOL		bNeedHollow,
						  DWORD		senderRegion);

/*************************************************
* T9SendParam
* ����˵����
	����ʵʱ���������ܿ���
* ����˵���� 

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
*************************************************/
T9API	T9SendParam();

/*************************************************
* T9ReadModuleInfo
* ����˵����
	��ȡģ����Ϣ
* ����˵���� 
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	rcvIndex
	���տ����
.	[in]	moduleIndex
	ģ�����
.	[in]	pBasicInfoBuf
	ָ��洢ģ�������Ϣ���ڴ�
.	[in,out]	pBasicLen
	ָ��pBasicInfoBufָ����ڴ泤�ȣ�������ʵ��ʹ�õĳ���
.	[in]		pMaintainInfoBuf
	ָ��洢ά�޼�¼���ڴ�
.	[in,out]	maintainBufLen
	ָ��pMaintainInfoBufָ����ڴ泤�ȣ�������ʵ��ʹ�õĳ���
.	[in]	pWatcher
	������

* ����ֵ˵����
.	T9_S_OK
	�ɹ�
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
* ����˵����
	��ȡģ����Ϣ
* ����˵���� 
.	[in]	portIndex��ȡֵ��Χ[0~3]
	���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	rcvIndex
	���տ����
.	[in]	moduleIndex
	ģ�����
.	[in]	pBasicInfoBuf
	ָ��洢ģ�������Ϣ���ڴ�
.	[in]	basicLen
	ָ��pBasicInfoBufָ����ڴ泤��
.	[in]	pMaintainInfoBuf
	ָ��洢ά�޼�¼���ڴ�
.	[in]	maintainBufLen
	ָ��pMaintainInfoBufָ����ڴ泤��
.	[in]	pWatcher
	������
* ����ֵ˵����
.	T9_S_OK
	�ɹ�
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
* ����˵����
���÷��Ϳ��İ汾��
* ����˵���� 
.	[in]	portIndex��ȡֵ��Χ[0~3]
    ���Ϳ����ںţ���������ڷ���ģʽ����0xff
.	[in]	openOrBroken
	��·���߶�·
.	[in]	pWidth
	��ȡ���Ŀ��
.	[in]	pHeight
	��ȡ���ĸ߶�
.	[in]	pInvalidRcv
	̽�����Ľ��տ�
.	[in]	lpBuf
	�洢���У�����ݵ��ڴ��
.	[in]	bufLen
	�洢���У�����ݵ��ڴ�鳤��

* ����ֵ˵����
.	T9_S_OK
    �ɹ�
.	T9E_OutOfMemeroy
    pBuf���Ȳ�������Ҫ���ֽ���=���x�߶�
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
* ����˵����
	���÷��Ϳ��İ汾��
* ����˵���� 

* ����ֵ˵����
.	T9_S_OK
�ɹ�
*************************************************/
T9API	T9SetSendingCardTypeVersion(int type, int majorVersion, int minorVersion);

T9API	T9EnableCorrect(BOOL bEnable, WORD rcvIndex, BYTE portIndex);
T9API	T9EnableChromaCorrect(BOOL bEnable, WORD rcvIndex, BYTE portIndex);

//��д���Ϳ�Flash
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
* ����˵����(Description)
��ȡ���ܿ�����(Load Receiving Card parameters)
* ����˵����(parameters)
[in]	pFile
���տ������ļ�(Receiving Card parameters)
[out]	phRcvParam
����̽�⵽���տ��������(Return Receiving Card Parameters Handle)
* ����ֵ˵����(return)
great than or equal 0, success, if less than 0, error
*************************************************/
T9API T9RcvParam_Load(wchar_t *pFile, HRCVPARAM *phRcvParam);


/*************************************************
* T9RcvParam_Release 
* ����˵����(Description)
�ͷŽ��տ�����(Release Receiving Card parameters)
* ����˵����(parameters)
[in]	hRcvParam
���տ������ļ����(Handle of Receiving Card parameters)
* ����ֵ˵����(return)
great than or equal 0, success, if less than 0, error
*************************************************/
T9API T9RcvParam_Release(HRCVPARAM hRcvParam);

/*************************************************
* T9RcvParam_Clone 
* ����˵����(Description)
��¡���տ�����(Clone Receiving Card parameters)
* ����˵����(parameters)
[in]	hRcvParam
���տ������ļ����(Handle of Receiving Card parameters)
[in,out]	phRcvParam
ָ����տ������ļ���������ر���¡�������տ��������(Point to the handle of Receiving Card parameters, which use to receive the new created parameters)
* ����ֵ˵����(return)
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