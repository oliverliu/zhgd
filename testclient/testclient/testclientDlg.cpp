
// xxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "testclient.h"
#include "testclientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "zc\ZCDataStruct.h"

#include "platform.h"
#include "platformhead.h"
#include "cutility.h"
#include "platforminterface.h"

#define ZC5		0x20000005 
#define CC5    0x00000FF5        //暂定ZC发送给CC5的数据包的目的地ID

const PLAT_UINT32 g_sid = 0xc1000000; //11,a1,b1
const PLAT_UINT32 g_did = 0x21000000;
const PLAT_UINT32 g_msgType = 0xff000001;

void createPacakge();

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestClientDlg dialog




CTestClientDlg::CTestClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CTestClientDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTestClientDlg message handlers

BOOL CTestClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	FILE *fp = NULL;
	fp = fopen("test.txt", "w");
	fclose(fp);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





static void outputPackage(const PLAT_UBYTE * buf, FILE* fp)
{
	PLAT_UINT32 count = CUtility::getUnitCounts(buf);
	fprintf(fp, "package has %d unit\n", count);
	for (int i = 0; i < count; i++)
	{
		PLAT_UBYTE * unit = CUtility::getUnitHead(buf, i);
		fprintf(fp, "NO.%2d package: unitId = %d, unitSize = %d, data:\n", 
			i, CUtility::getLittlePackUID(unit), CUtility::getLittlePackDataSize(unit));
		PLAT_UINT32 size = CUtility::getLittlePackSize(unit);
		for(int j = 0;j < size; j++)
		{
			fprintf(fp, "%02x  ", unit[j]);
		}
		fprintf(fp, "\n");
	}
}


void createPacakge();

CAppInterface app;

void CTestClientDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	
	FILE *fp = NULL;
	fp = fopen("test.txt", "a+");
 
	static int i = 0;
	if ( i == 0)
	{
		//app.AppInit();
		InitPlatformInterface();
	}
	fprintf(fp, "\n\nTest NO.%d\n", i++);

	//app.AppRead();
	ReadFormPlatform();
	
	fprintf(fp, "READ: \n");
	outputPackage(APP_READ_ADDR,fp);

	//create a package
	createPacakge();

	fprintf(fp, "Write: \n");
	outputPackage(APP_WRITE_ADDR, fp);
	//app.AppWrite();
	WriteToPlatform();

    fclose(fp);
}

class CPackUtility
{
public:
	CPackUtility(PLAT_UBYTE* );
	~CPackUtility();
	void pushPack(int type);

	void pushCC2ZC_RoutineMsg();
	void pushZC2CC_RoutinMsg();
	void pushZC2CC_TrackMsg();
	void pushZC2CC_SpeedRstMsg(); //speed restrict
	void pushCC2ZC_2ATSTrasMsg();
	void pushZC2CC_fromATSTrasMsg();
	void pushZC2CreateConnect(bool bcreate);
	
	void finished();
private:
	void pushPackZc2Atp();
	void pushPackZc2Ato();


	void updateIdxSt();
private:
	PLAT_UBYTE * m_phead;
	//PLAT_UBYTE * m_pcurrent;
	//T_DATA_INDEX    m_index;
	//int  m_seqnum;
};

void CPackUtility::updateIdxSt()
{
	//m_seqnum++;
	//if ( m_seqnum > 255)
	//{
	//	printf("!!!!!!Waring package sum large than 256, now is %d", m_seqnum + 1); 
	//	return;
	//}

	//m_index.unitAddrOffset[m_seqnum] = m_pcurrent - m_phead;
	//m_index.regionUnitNum =  m_seqnum + 1;
}

void CPackUtility::pushCC2ZC_RoutineMsg()
{
	//updateIdxSt();

	//
	//T_UNIT_HEAD     unithead;
	//T_MESSAGE_HEAD  msghead;

	//unithead.unitId = 0x01;
	//unithead.unitSize = 0;

	//msghead.sequenceNum = 2;
	//msghead.timeStamp = 2;
	//msghead.SID = g_sid;
	//msghead.DID = g_did;
	//msghead.msgType = g_msgType;
 //   msghead.msgLen = 0;
	//
	//typedef struct _msgInter
	//{
	//	PLAT_UINT32 info1[14];
	//	PLAT_UINT8  info2[9];
	//	PLAT_UINT32 info3;
	//	PLAT_UINT8  info4[2];
	//	PLAT_UINT32 info5[2];
	//} stMsgInter;

	//stMsgInter inter;
	//inter.info1[0] = msghead.DID;
	//inter.info1[1] = msghead.SID;
	//inter.info1[2] = msghead.msgType;
	//inter.info1[3] = sizeof(stMsgInter ) - 4 * sizeof(PLAT_UINT32);

	////info1
	//for( int i = 4; i < 14; i++ )
	//	inter.info1[i] = 0x01;
	//
	////change special value
	//inter.info1[5] = 0x02;
	//inter.info1[6] = 0x10;

	////info2
	//for(int i = 0; i < 9; i++ )
	//	inter.info2[i] = 0xa0;
	//
	//inter.info2[0] = 0xAA;
	//inter.info2[4] = 0x8;

	////info3
	//inter.info3 = 0x11;

	////info4
	//for(int i = 0; i < 2; i++ )
	//	inter.info4[i] = 0xb0;

	////info5
	//for( int i = 0; i < 2; i++ )
	//	inter.info5[i] = 0xc0;
	//
	//msghead.msgLen = sizeof(stMsgInter);
	//unithead.unitSize = msghead.msgLen + sizeof(T_MESSAGE_HEAD);

	//memcpy(m_pcurrent, &unithead, sizeof(T_UNIT_HEAD));
	//m_pcurrent += sizeof(T_UNIT_HEAD);
	//
	//memcpy(m_pcurrent, &msghead, sizeof(T_MESSAGE_HEAD));
	//m_pcurrent += sizeof(T_MESSAGE_HEAD);

	//memcpy(m_pcurrent, &inter, sizeof(stMsgInter));
	//m_pcurrent += sizeof(stMsgInter);

	//printf( "msghead len = %d, unithead size = %d\n", msghead.msgLen, unithead.unitSize);
}

#define toBig32(val) CUtility::LetoBe32(val)
//#define toBig64(val) CUtility::LetoBe64(val)
//#define toBig64(val) CUtility::BetoLe64(val)

#define toBig16(val) CUtility::LetoBe16(val)
//#define toLittle64(val) CUtility::BetoLe64(val)

void CPackUtility::pushZC2CreateConnect(bool bcreate)
{
	//updateIdxSt();
	PLAT_UBYTE pArray[1024*8];
	memset(pArray, 0, 1024*8);
	PLAT_UBYTE *ptmpH = pArray;

	T_UNIT_HEAD     unithead;
	T_MESSAGE_HEAD  msghead;

	//01 001011, 0xaa, 0x00,0x00
	unithead.unitId = toBig32(0x4baa0000);
	unithead.unitSize = toBig32(0);

	msghead.sequenceNum = toBig32(2);
	msghead.timeStamp = CUtility::LetoBe64((long long) 2) ;//toBig64(2);
	msghead.SID = toBig32(g_sid);
	msghead.DID = toBig32(g_sid);
	msghead.msgType = toBig32(g_msgType); //0xff000001
    msghead.msgLen = toBig32(0);
	
	typedef struct _msgInter
	{
		PLAT_UINT8  info1;
		PLAT_UINT8  info2;
		PLAT_UINT32  info3;
		PLAT_UINT32  info4;
		PLAT_UINT16  info5;
		PLAT_UINT16  info6;
	} stMsgInter;

	stMsgInter inter;
	inter.info1 = bcreate ? 0x10 : 0x11;
	inter.info2 = 0;
	inter.info3 =  toBig32(g_sid);
	inter.info4 =  toBig32(g_sid);
	inter.info5 =  toBig16(0);
	inter.info6 =  toBig16(0);
	
	msghead.msgLen =  sizeof(stMsgInter);
	unithead.unitSize = msghead.msgLen + sizeof(T_MESSAGE_HEAD);

	printf( "msghead len = %d, unithead size = %d\n",
		msghead.msgLen, unithead.unitSize);

	msghead.msgLen = toBig32(msghead.msgLen);
	unithead.unitSize  = toBig32(unithead.unitSize );

	printf( "msghead len = %0x, unithead size = %0x, in memory\n",
		msghead.msgLen, unithead.unitSize );


	memcpy(ptmpH, &unithead, sizeof(T_UNIT_HEAD));
	ptmpH += sizeof(T_UNIT_HEAD);
	
	memcpy(ptmpH, &msghead, sizeof(T_MESSAGE_HEAD));
	ptmpH += sizeof(T_MESSAGE_HEAD);

	memcpy(ptmpH, &inter, sizeof(stMsgInter));
	ptmpH += sizeof(stMsgInter);

	CUtility::pushBackPack(m_phead,pArray); 
}


void CPackUtility::pushZC2CC_RoutinMsg()
{
	//updateIdxSt();	
	//
	//T_UNIT_HEAD     unithead;
	//T_MESSAGE_HEAD  msghead;
	//
	////01 001011, 0xaa, 0x00,0x00
	//unithead.unitId = toBig32(0x01);
	//unithead.unitSize = toBig32(0);

	//msghead.sequenceNum = toBig32(2);
	//msghead.timeStamp = toBig64(2);
	//msghead.SID = toBig32(g_sid);
	//msghead.DID = toBig32(g_sid);
	//msghead.msgType = toBig32(g_msgType); //0xff000001
 //   msghead.msgLen = toBig32(0);
	//
	//typedef struct _msgInter
	//{
	//	PLAT_UINT32 info1[14];
	//	PLAT_UINT8  info2[9];
	//	PLAT_UINT32 info3;
	//	PLAT_UINT8  info4[2];
	//	PLAT_UINT32 info5[2];
	//} stMsgInter;
	//
	//stMsgInter inter;
	//inter.info1[0] = toBig32(msghead.DID);
	//inter.info1[1] = toBig32(msghead.SID);
	//inter.info1[2] = toBig32(msghead.msgType);
	//inter.info1[3] = toBig32(sizeof(stMsgInter ) - 4 * sizeof(PLAT_UINT32));
	//
	//int i = 0;
	////info1
	//for( i = 4; i < 14; i++ )
	//	inter.info1[i] = toBig32(0x01);
	//
	////change special value
	//inter.info1[5] = toBig32(0x02);
	//inter.info1[6] = toBig32(0x10);
	//
	////info2
	//for( i = 0; i < 9; i++ )
	//	inter.info2[i] = 0xa0;
	//
	//inter.info2[0] = 0xAA;
	//inter.info2[4] = 0x8;
	//
	////info3
	//inter.info3 = toBig32(0x11);
	//
	////info4
	//for(i = 0; i < 2; i++ )
	//	inter.info4[i] = 0xb0;
	//
	////info5
	//for( i = 0; i < 2; i++ )
	//	inter.info5[i] = toBig32(0xc0);
	//
	//msghead.msgLen = sizeof(stMsgInter);
	//unithead.unitSize = msghead.msgLen + sizeof(T_MESSAGE_HEAD);
	//
	//memcpy(m_pcurrent, &unithead, sizeof(T_UNIT_HEAD));
	//m_pcurrent += sizeof(T_UNIT_HEAD);
	//
	//memcpy(m_pcurrent, &msghead, sizeof(T_MESSAGE_HEAD));
	//m_pcurrent += sizeof(T_MESSAGE_HEAD);
	//
	//memcpy(m_pcurrent, &inter, sizeof(stMsgInter));
	//m_pcurrent += sizeof(stMsgInter);
	//
	//printf( "msghead len = %d, unithead size = %d\n", msghead.msgLen, unithead.unitSize);

	//msghead.msgLen = toBig32(msghead.msgLen);
	//unithead.unitSize  = toBig32(unithead.unitSize );

	//printf( "msghead len = %0x, unithead size = %0x, in memory\n",
	//	msghead.msgLen, unithead.unitSize );

}

void CPackUtility::pushZC2CC_TrackMsg()
{
	//updateIdxSt();

}

void CPackUtility::pushZC2CC_SpeedRstMsg() //speed restrict
{
	//updateIdxSt();

}

void CPackUtility::pushCC2ZC_2ATSTrasMsg()
{
	//updateIdxSt();
}

void CPackUtility::pushZC2CC_fromATSTrasMsg()
{
	//updateIdxSt();
}

CPackUtility::CPackUtility(PLAT_UBYTE* p)
{
	m_phead = p;


	CUtility::initBigPackIdx(p);
}

CPackUtility::~CPackUtility()
{
  // finished();
}

void CPackUtility::finished()
{
	//memcpy(m_phead, &m_index, sizeof(T_DATA_INDEX));	
}

enum packType
{
	zc2Cir = 0,
	zc2Broder,
	zc2Atp,
	zc2Ato,
	zc2Ci
};

void CPackUtility::pushPack(int type)
{
	//updateIdxSt();

	//switch(type)
	//{
	//case zc2Cir:
	//	break;
	//case zc2Broder:
	//	break;
	//case zc2Atp:
	//	break;
	//case zc2Ato:
	//	pushPackZc2Ato();
	//	break;
	//case zc2Ci:
	//	break;
	//default:
	//	break;
	//}
}

void CPackUtility::pushPackZc2Atp()
{
}

void CPackUtility::pushPackZc2Ato()
{
	//return;
	//T_UNIT_HEAD     unithead;
	//T_MESSAGE_HEAD  messagehead;
	//int i = 0;
	//unithead.unitId = ZC_ID_MESSAGEOUT;
	//unithead.unitSize = 2 + sizeof(messagehead);
	//
	//messagehead.sequenceNum =1;			//临时
	//messagehead.timeStamp =0;			//临时
	//messagehead.SID =ZC5;
	//messagehead.DID =ZC5;
	//messagehead.msgType =MT_ZC_TO_CC;
	//messagehead.msgLen = 2;

	//memcpy(m_pcurrent, &unithead, sizeof(unithead));
	//m_pcurrent +=sizeof(unithead);
	//memcpy(m_pcurrent, &messagehead, sizeof(messagehead));
	//m_pcurrent +=sizeof(messagehead);

	//char m[2] = {'a','b'};
	//memcpy(m_pcurrent, &m[0], sizeof(PLAT_UBYTE));
	//++m_pcurrent;
	//memcpy(m_pcurrent, &m[1], sizeof(PLAT_UBYTE));
	////memcpy(m_pcurrent, unithead + sizeof(PLAT_UINT32) * 2, 2);
}

void createPacakge()
{
	unsigned char* dstBuf = APP_WRITE_ADDR;

	CPackUtility	packUtil(dstBuf);
//	packUtil.pushCC2ZC_RoutineMsg();
// 	packUtil.pushZC2CC_RoutinMsg();

// 	packUtil.pushZC2CC_TrackMsg();
// 	packUtil.pushZC2CC_SpeedRstMsg(); //speed restrict
// 	packUtil.pushCC2ZC_2ATSTrasMsg();
// 	packUtil.pushZC2CC_fromATSTrasMsg();

	packUtil.pushZC2CreateConnect(true);
	packUtil.pushZC2CreateConnect(false);
	//packUtil.ZC2CreateConnect(false);
	
	//packUtil.finished();
}
