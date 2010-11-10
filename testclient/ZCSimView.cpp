// ZCSimView.cpp : implementation of the CZCSimView class
//

#include "stdafx.h"
#include "ZCSim.h"

#include "ZCSimDoc.h"
#include "ZCSimView.h"
#include "MainFrm.h"

#include "..\platformlib\platform.h"
#include "..\platformlib\cutility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern  PLAT_UBYTE APP_READ_ADDR[SIZE];
extern  PLAT_UBYTE APP_WRITE_ADDR[SIZE];

void createPacakge();

#define ZC5		0x20000005 
#define CC5    0x00000FF5        //暂定ZC发送给CC5的数据包的目的地ID
CAppInterface   cam; 

const PLAT_UINT32 g_sid = 0xc1000000; //11,a1,b1
const PLAT_UINT32 g_did = 0x21000000;
const PLAT_UINT32 g_msgType = 0xff000001;


/////////////////////////////////////////////////////////////////////////////
// CZCSimView
CZCSimView *g_simview;
extern CZCSimApp theApp;

IMPLEMENT_DYNCREATE(CZCSimView, CFormView)

BEGIN_MESSAGE_MAP(CZCSimView, CFormView)
	//{{AFX_MSG_MAP(CZCSimView)
	ON_COMMAND(IDM_READ_CONFIG, OnReadConfig)
	ON_COMMAND(IDM_BEAGIN_SEND, OnBeaginSend)
	ON_COMMAND(IDM_STOP_SEND, OnStopSend)
	ON_BN_CLICKED(IDC_BTN_EMG, OnBtnEmg)
	ON_BN_CLICKED(IDC_BTN_STOPREQ, OnBtnStopreq)
	ON_BN_CLICKED(IDC_RADIO_EASTDIR, OnRadioEastdir)
	ON_BN_CLICKED(IDC_RADIO_WESTDIR, OnRadioWestdir)
	ON_BN_CLICKED(IDC_SETOK, OnSetok)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZCSimView construction/destruction

CZCSimView::CZCSimView()
	: CFormView(CZCSimView::IDD)
{
	//{{AFX_DATA_INIT(CZCSimView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	g_simview =this;
	trackdataversion =0;
	speedlimitversion =0;
	SwitchCount =0;
	StationCount =0;
	m_sendflag =0;
	m_EbCmd =0x55;
	m_stopAssReq =0x55;
	m_stopSigId =0;
	m_MalDir =0xAA;
	m_MalSecId =-1;
	m_MalOffset =0;
	m_MalEndType =0;  
}

CZCSimView::~CZCSimView()
{
	int count,i;
	SwitchVar *switchvar;
	StationVar *stationvar;
	count =m_switchlist.GetCount();
	for(i =0; i<count; i++)
	{
		switchvar =m_switchlist.GetTail();
		delete switchvar;
	}
	
	count =m_stationlist.GetCount();
	for(i=0; i<count; i++)
	{
		stationvar =m_stationlist.GetTail();
		delete stationvar;
	}
}

void CZCSimView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CZCSimView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CZCSimView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CZCSimView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	FILE *fp = NULL;
	
	fp = fopen("test.txt", "w");
	fclose(fp);

}

/////////////////////////////////////////////////////////////////////////////
// CZCSimView printing

BOOL CZCSimView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CZCSimView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CZCSimView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CZCSimView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CZCSimView diagnostics

#ifdef _DEBUG
void CZCSimView::AssertValid() const
{
	CFormView::AssertValid();
}

void CZCSimView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CZCSimDoc* CZCSimView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CZCSimDoc)));
	return (CZCSimDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CZCSimView message handlers

void CZCSimView::OnReadConfig() 
{
	// TODO: Add your command handler code here
	CString str;
	int i;
	char *pchToken =NULL;
	char *switchIDbuf =NULL;	
	CString switchID;
	char *swstatusbuf =NULL;
	CString switchStatus;
	SwitchVar *switchdata;
	char *stationIDbuf =NULL;
	CString stationID;
	char *stationstatusbuf =NULL;
	CString stationStatus;
	StationVar *stationdata;
	CString strTemp;
	
	CFileDialog file(TRUE);
	file.DoModal();
	str =file.GetPathName();
	trackdataversion =GetPrivateProfileInt("TRACKCONFIG","TrackDataVersion", 0, str);
	speedlimitversion =GetPrivateProfileInt("TRACKCONFIG", "SpeedLimitVersion", 0, str);

	SwitchCount =GetPrivateProfileInt("SWITCH", "SwitchCount", 0, str);
	int SwitchIDArray[5];
	int SwitchStatusArray[5];

	GetPrivateProfileString("SWITCH", "SwitchID", " ",switchID.GetBuffer(25),25,str);
	switchIDbuf = switchID.GetBuffer(switchID.GetLength());
	switchID.ReleaseBuffer();
	pchToken = strtok(switchIDbuf, ",");
	for (i=0; i<SwitchCount; i++)
	{
		if (i == (SwitchCount -1))
		{
			strTemp.Format("%s",pchToken);
			SwitchIDArray[i] =atoi(strTemp.Left(strTemp.GetLength()-1));    //去掉分号";"
		}
		else
		{
			SwitchIDArray[i] =atoi(pchToken);  
		}				
		pchToken = strtok(NULL, ",");
	}

	GetPrivateProfileString("SWITCH", "SwitchStatus", " ",switchStatus.GetBuffer(25),25,str);
	swstatusbuf = switchStatus.GetBuffer(switchStatus.GetLength());
	switchStatus.ReleaseBuffer();
	pchToken = strtok(swstatusbuf, ",");
	for (i=0; i<SwitchCount; i++)
	{	
		if (i == (SwitchCount -1))
		{		
			strTemp.Format("%s",pchToken);
			SwitchStatusArray[i] =atoi(strTemp.Left(strTemp.GetLength()-1));
		}
		else
		{
			SwitchStatusArray[i] =atoi(pchToken);
		}			
		pchToken = strtok(NULL, ",");
	}

	for(i=0; i<SwitchCount; i++)
	{
		switchdata= new SwitchVar;
		switchdata->m_switchID =SwitchIDArray[i];
		switchdata->m_switchstatus =SwitchStatusArray[i];	
		m_switchlist.AddTail(switchdata);		
	}

	StationCount =GetPrivateProfileInt("STATION", "StationCount", 0, str);
	int StationIDArray[5];
	int StationStatusArray[5];

	GetPrivateProfileString("STATION", "StationID", " ",stationID.GetBuffer(25),25,str);
	stationIDbuf = stationID.GetBuffer(stationID.GetLength());
	stationID.ReleaseBuffer();
	pchToken = strtok(stationIDbuf, ",");

	for (i=0; i<StationCount; i++)
	{			
		if (i == (StationCount -1))
		{
			strTemp.Format("%s",pchToken);
			StationIDArray[i] =atoi(strTemp.Left(strTemp.GetLength()-1));
		}
		else
		{ 
			StationIDArray[i] =atoi(pchToken);
		}				
		pchToken = strtok(NULL, ",");
	}

	GetPrivateProfileString("STATION", "StationStatus", " ",stationStatus.GetBuffer(25),25,str);
	stationstatusbuf =stationStatus.GetBuffer(stationStatus.GetLength());
	stationStatus.ReleaseBuffer();
	pchToken = strtok(stationstatusbuf, ",");
	for (i=0; i<StationCount; i++)
	{	
		if (i == (StationCount -1))
		{
			strTemp.Format("%s",pchToken);
			StationStatusArray[i] =atoi(strTemp.Left(strTemp.GetLength()-1));	
		}
		else
		{
			StationStatusArray[i] =atoi(pchToken);	
		}
		pchToken = strtok(NULL, ",");
	}

	for(i=0; i<StationCount; i++)
	{
		stationdata= new StationVar;
		stationdata->m_stationID =StationIDArray[i];
		stationdata->m_stationStatus =StationStatusArray[i];	
		m_stationlist.AddTail(stationdata);		
	}
}

void CZCSimView::OnBeaginSend() 
{
	// TODO: Add your command handler code here
	m_sendflag =1;
	AfxBeginThread(DataCommThread,NULL);
	((CMainFrame*)(theApp.GetMainWnd()))->m_wndStatusBar.SetPaneText(0, "数据收发开始");
	
}

void CZCSimView::OnStopSend() 
{
	// TODO: Add your command handler code here
	m_sendflag =0;
    ((CMainFrame*)(theApp.GetMainWnd()))->m_wndStatusBar.SetPaneText(0, "数据收发结束");
}

void CZCSimView::OnBtnEmg() 
{
	// TODO: Add your control notification handler code here
	m_EbCmd =0xAA;			//紧急停车命令，EB(0xAA)，非EB(0x55)
}

void CZCSimView::OnBtnStopreq() 
{
	// TODO: Add your control notification handler code here
	m_stopAssReq =0xAA;		//停车保证请求，有请求（0xAA），无请求（0x55）
}

void CZCSimView::OnRadioEastdir() 
{
	// TODO: Add your control notification handler code here
	m_MalDir =0xAA;     //MAL方向（0xAA），西向（0x55）
}

void CZCSimView::OnRadioWestdir() 
{
	// TODO: Add your control notification handler code here
	m_MalDir =0x55;		//MAL方向（0xAA），西向（0x55）
}

void CZCSimView::OnSetok() 
{
	// TODO: Add your control notification handler code here
	m_stopSigId =GetDlgItemInt(IDC_MALSEGID);
	m_MalOffset =GetDlgItemInt(IDC_MALSEGOFFSET);
	m_MalEndType =GetDlgItemInt(IDC_MALENDTYPE);
}

void outputPackage(const PLAT_UBYTE * buf, FILE* fp)
{
	PLAT_UINT32 count = CUtility::getUnitCounts(buf);
	fprintf(fp, "package has %d unit\n", count);
	for (int i = 0; i < count; i++)
	{
		PLAT_UBYTE * unit = CUtility::getUnitHead(buf, i);
		fprintf(fp, "NO.%2d package: unitId = %d, unitSize = %d, data:\n", 
			i, CUtility::getLittlePackUID(unit), CUtility::getLittlePackUSize(unit));
		PLAT_UINT32 size = CUtility::getLittlePackSize(unit);
		for(int j = 0;j < size; j++)
		{
			fprintf(fp, "%02x  ", unit[j]);
		}
		fprintf(fp, "\n");
	}
}

UINT  DataCommThread(LPVOID)
{
	////书写数据交互程序

// 	PLAT_UINT8  recvinfo[NETSIZE];
// 	PLAT_UINT8  sendinfo[NETSIZE];
// 	T_DATA_INDEX index;
// 	T_UNIT_HEAD  head;
// 	PLAT_UINT8  unitbuf[SIZE];
// 	PLAT_UINT32 SID;
	FILE *fp = NULL;

	fp = fopen("test.txt", "a+");

// 	fprintf(fp, "\n\n");
// 	memset(&index, 0x00, sizeof(index));
// 	memset(&head, 0x00, sizeof(head));
// 	memset(unitbuf, 0x00, sizeof(PLAT_UINT8) * SIZE);
// 	memset(recvinfo, 0x00, sizeof(PLAT_UINT8) * NETSIZE);
// 	memset(sendinfo, 0x00, sizeof(PLAT_UINT8) * NETSIZE);
// 
// 
// 	SID = ZC5;
 
	//cam.AppInit(sendinfo, recvinfo, SID, "192.168.20.100");
	static int i = 0;
	if ( i == 0)
	{
		cam.AppInit();
	}
	fprintf(fp, "\n\nTest NO.%d\n", i++);


	//cam.AppInit();
	cam.AppRead();
	fprintf(fp, "READ: \n");
	outputPackage(APP_READ_ADDR,fp);

	//create a package
	createPacakge();
//	DataSend(sendinfo);

	fprintf(fp, "Write: \n");
	outputPackage(APP_WRITE_ADDR, fp);
	cam.AppWrite();

    fclose(fp);

	return 0;
}

/*-----------------------输入、输出数据包打印函数-------------------------*/
void outputUint(FILE* file, PLAT_UINT8* uout,int len)
{
	int i;
	for(i = 0;i < len;i ++)
	{
		if(uout[i] != 0)
			fprintf(file, "%u  ",uout[i]);
	}
	fprintf(file, "\n");
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


	void finished();
private:
	void pushPackZc2Atp();
	void pushPackZc2Ato();

	void updateIdxSt();
private:
	PLAT_UBYTE * m_phead;
	PLAT_UBYTE * m_pcurrent;
	T_DATA_INDEX    m_index;
	int  m_seqnum;
};

void CPackUtility::updateIdxSt()
{
	m_seqnum++;
	if ( m_seqnum > 255)
	{
		printf("!!!!!!Waring package sum large than 256, now is %d", m_seqnum + 1); 
		return;
	}

	m_index.unitAddrOffset[m_seqnum] = m_pcurrent - m_phead;
	m_index.regionUnitNum =  m_seqnum + 1;
}



void CPackUtility::pushCC2ZC_RoutineMsg()
{
	updateIdxSt();

	
	T_UNIT_HEAD     unithead;
	T_MESSAGE_HEAD  msghead;

	unithead.unitId = 0x01;
	unithead.unitSize = 0;

	msghead.sequenceNum = 2;
	msghead.timeStamp = 2;
	msghead.SID = g_sid;
	msghead.DID = g_did;
	msghead.msgType = g_msgType;
    msghead.msgLen = 0;
	
	typedef struct _msgInter
	{
		PLAT_UINT32 info1[14];
		PLAT_UINT8  info2[9];
		PLAT_UINT32 info3;
		PLAT_UINT8  info4[2];
		PLAT_UINT32 info5[2];
	} stMsgInter;

	stMsgInter inter;
	inter.info1[0] = msghead.DID;
	inter.info1[1] = msghead.SID;
	inter.info1[2] = msghead.msgType;
	inter.info1[3] = sizeof(stMsgInter ) - 4 * sizeof(PLAT_UINT32);

	//info1
	for( int i = 4; i < 14; i++ )
		inter.info1[i] = 0x01;
	
	//change special value
	inter.info1[5] = 0x02;
	inter.info1[6] = 0x10;

	//info2
	for( i = 0; i < 9; i++ )
		inter.info2[i] = 0xa0;
	
	inter.info2[0] = 0xAA;
	inter.info2[4] = 0x8;

	//info3
	inter.info3 = 0x11;

	//info4
	for(i = 0; i < 2; i++ )
		inter.info4[i] = 0xb0;

	//info5
	for( i = 0; i < 2; i++ )
		inter.info5[i] = 0xc0;
	
	msghead.msgLen = sizeof(stMsgInter);
	unithead.unitSize = msghead.msgLen + sizeof(T_MESSAGE_HEAD);

	memcpy(m_pcurrent, &unithead, sizeof(T_UNIT_HEAD));
	m_pcurrent += sizeof(T_UNIT_HEAD);
	
	memcpy(m_pcurrent, &msghead, sizeof(T_MESSAGE_HEAD));
	m_pcurrent += sizeof(T_MESSAGE_HEAD);

	memcpy(m_pcurrent, &inter, sizeof(stMsgInter));
	m_pcurrent += sizeof(stMsgInter);

	printf( "msghead len = %d, unithead size = %d\n", msghead.msgLen, unithead.unitSize);
}

void CPackUtility::pushZC2CC_RoutinMsg()
{
	updateIdxSt();	
	
	T_UNIT_HEAD     unithead;
	T_MESSAGE_HEAD  msghead;
	
	unithead.unitId = 0x01;
	unithead.unitSize = 0;
	
	msghead.sequenceNum = 2;
	msghead.timeStamp = 2;
	msghead.SID = g_sid;
	msghead.DID = g_sid;
	msghead.msgType = g_msgType;
    msghead.msgLen = 0;
	
	typedef struct _msgInter
	{
		PLAT_UINT32 info1[14];
		PLAT_UINT8  info2[9];
		PLAT_UINT32 info3;
		PLAT_UINT8  info4[2];
		PLAT_UINT32 info5[2];
	} stMsgInter;
	
	stMsgInter inter;
	inter.info1[0] = msghead.DID;
	inter.info1[1] = msghead.SID;
	inter.info1[2] = msghead.msgType;
	inter.info1[3] = sizeof(stMsgInter ) - 4 * sizeof(PLAT_UINT32);
	
	//info1
	for( int i = 4; i < 14; i++ )
		inter.info1[i] = 0x01;
	
	//change special value
	inter.info1[5] = 0x02;
	inter.info1[6] = 0x10;
	
	//info2
	for( i = 0; i < 9; i++ )
		inter.info2[i] = 0xa0;
	
	inter.info2[0] = 0xAA;
	inter.info2[4] = 0x8;
	
	//info3
	inter.info3 = 0x11;
	
	//info4
	for(i = 0; i < 2; i++ )
		inter.info4[i] = 0xb0;
	
	//info5
	for( i = 0; i < 2; i++ )
		inter.info5[i] = 0xc0;
	
	msghead.msgLen = sizeof(stMsgInter);
	unithead.unitSize = msghead.msgLen + sizeof(T_MESSAGE_HEAD);
	
	memcpy(m_pcurrent, &unithead, sizeof(T_UNIT_HEAD));
	m_pcurrent += sizeof(T_UNIT_HEAD);
	
	memcpy(m_pcurrent, &msghead, sizeof(T_MESSAGE_HEAD));
	m_pcurrent += sizeof(T_MESSAGE_HEAD);
	
	memcpy(m_pcurrent, &inter, sizeof(stMsgInter));
	m_pcurrent += sizeof(stMsgInter);
	
	printf( "msghead len = %d, unithead size = %d\n", msghead.msgLen, unithead.unitSize);

}

void CPackUtility::pushZC2CC_TrackMsg()
{
	updateIdxSt();

}

void CPackUtility::pushZC2CC_SpeedRstMsg() //speed restrict
{
	updateIdxSt();

}

void CPackUtility::pushCC2ZC_2ATSTrasMsg()
{
	updateIdxSt();
}

void CPackUtility::pushZC2CC_fromATSTrasMsg()
{
	updateIdxSt();
}

CPackUtility::CPackUtility(PLAT_UBYTE* p)
{
	m_phead = p;
	m_pcurrent = p + sizeof(T_DATA_INDEX);
	memset(&m_index, 0x00, sizeof(T_DATA_INDEX));
	m_index.platformHealth =1;
	m_index.platformStatus =1;
	m_index.regionUnitNum = 0;
	m_seqnum = -1;
}

CPackUtility::~CPackUtility()
{
   finished();
}

void CPackUtility::finished()
{
	memcpy(m_phead, &m_index, sizeof(T_DATA_INDEX));	
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
	updateIdxSt();

	switch(type)
	{
		case zc2Cir:
		{
			ZC_TO_CCCIR		zctocccir;
			T_UNIT_HEAD     unithead;
			T_MESSAGE_HEAD  messagehead;
			unithead.unitId =ZC_ID_MESSAGEOUT;
			
			messagehead.sequenceNum =1;       //临时
			messagehead.timeStamp =0;         //临时
			messagehead.SID =ZC5;                  //通信包的源为ZC5
			messagehead.DID =CC5;                  //通信包的目的地为CC5(即ATP1和ATO1)
			messagehead.msgType = MT_ZC_TO_CC;
			messagehead.msgLen =sizeof(zctocccir);
			
			zctocccir.trainId =11;
			zctocccir.stopSigId =g_simview->m_stopSigId;
			zctocccir.stopAssReq =g_simview->m_stopAssReq;
			zctocccir.seqNo =1;
			zctocccir.MalSecId =g_simview->m_MalSecId;
			zctocccir.MalOffset =g_simview->m_MalOffset;
			zctocccir.MalEndType =g_simview->m_MalEndType;
			zctocccir.MalDir =g_simview->m_MalDir;
			zctocccir.EbCmd =g_simview->m_EbCmd;
		
			unithead.unitSize =sizeof(T_MESSAGE_HEAD)+sizeof(zctocccir);
			memcpy(m_pcurrent, &unithead, sizeof(unithead));
			m_pcurrent +=sizeof(unithead);
			memcpy(m_pcurrent, &messagehead, sizeof(messagehead));
			m_pcurrent +=sizeof(messagehead);
			memcpy(m_pcurrent, &zctocccir, sizeof(zctocccir));
			m_pcurrent +=sizeof(zctocccir);

		}
		break;
		case zc2Broder:
		{
			ZC_TO_CCBRODE   zctoccbrode;
			T_UNIT_HEAD     unithead;
			T_MESSAGE_HEAD  messagehead;
			int i = 0;
			unithead.unitId =ZC_ID_MESSAGEOUT;
			
			messagehead.sequenceNum =1;			//临时
			messagehead.timeStamp =0;			//临时
			messagehead.SID =ZC5;
			messagehead.DID =CC5;
			messagehead.msgType =MT_ZC_TO_CC;
			messagehead.msgLen =sizeof(zctoccbrode);
			
			zctoccbrode.gdDbVer =g_simview->trackdataversion;
			zctoccbrode.tsrDbVer =g_simview->speedlimitversion;
			zctoccbrode.swNum =g_simview->SwitchCount;
			SwitchVar *sw;
			POSITION swpos =g_simview->m_switchlist.GetHeadPosition();
			for(i=0; i<g_simview->SwitchCount; i++)
			{
				sw =g_simview->m_switchlist.GetNext(swpos);
				zctoccbrode.swId[i] =sw->m_switchID;
				zctoccbrode.swState[i] =sw->m_switchstatus;	
			}

			zctoccbrode.stationNum =g_simview->StationCount;
			StationVar *station;
			POSITION stationpos =g_simview->m_stationlist.GetHeadPosition();
			for(i =0; i<g_simview->StationCount; i++)
			{
				station =g_simview->m_stationlist.GetNext(stationpos);
				zctoccbrode.stationId[i] =station->m_stationID;
				zctoccbrode.stationState[i] =station->m_stationStatus;
			}
			
			unithead.unitSize =sizeof(messagehead)+sizeof(zctoccbrode);
			memcpy(m_pcurrent, &unithead, sizeof(unithead));
			m_pcurrent +=sizeof(unithead);
			memcpy(m_pcurrent, &messagehead, sizeof(messagehead));
			m_pcurrent +=sizeof(messagehead);
			memcpy(m_pcurrent, &zctoccbrode, sizeof(zctoccbrode));

		}
		break;
		case zc2Atp:
			pushPackZc2Atp();
			break;
		case zc2Ato:
			pushPackZc2Ato();
			break;
		case zc2Ci:
			break;
		default:
			break;

	}
}

void CPackUtility::pushPackZc2Atp()
{
	ZC_TO_CCBRODE   zctoccbrode;
	T_UNIT_HEAD     unithead;
	T_MESSAGE_HEAD  messagehead;
	int i = 0;
	unithead.unitId =ZC_ID_MESSAGEOUT;
	
	messagehead.sequenceNum =1;			//临时
	messagehead.timeStamp =0;			//临时
	messagehead.SID =ZC5;
	messagehead.DID =ZC5;
	messagehead.msgType =MT_ZC_TO_CC;
	messagehead.msgLen =sizeof(zctoccbrode);
	
	zctoccbrode.gdDbVer =g_simview->trackdataversion;
	zctoccbrode.tsrDbVer =g_simview->speedlimitversion;
	zctoccbrode.swNum =g_simview->SwitchCount;
	SwitchVar *sw;
	POSITION swpos =g_simview->m_switchlist.GetHeadPosition();
	for(i=0; i<g_simview->SwitchCount; i++)
	{
		sw =g_simview->m_switchlist.GetNext(swpos);
		zctoccbrode.swId[i] =sw->m_switchID;
		zctoccbrode.swState[i] =sw->m_switchstatus;
		
	}
	zctoccbrode.stationNum =g_simview->StationCount;
	StationVar *station;
	POSITION stationpos =g_simview->m_stationlist.GetHeadPosition();
	for(i =0; i<g_simview->StationCount; i++)
	{
		station =g_simview->m_stationlist.GetNext(stationpos);
		zctoccbrode.stationId[i] =station->m_stationID;
		zctoccbrode.stationState[i] =station->m_stationStatus;
	}
	
	unithead.unitSize =sizeof(messagehead)+sizeof(zctoccbrode);
	memcpy(m_pcurrent, &unithead, sizeof(unithead));
	m_pcurrent +=sizeof(unithead);
	memcpy(m_pcurrent, &messagehead, sizeof(messagehead));
	m_pcurrent +=sizeof(messagehead);
	memcpy(m_pcurrent, &zctoccbrode, sizeof(zctoccbrode));
}

void CPackUtility::pushPackZc2Ato()
{
	T_UNIT_HEAD     unithead;
	T_MESSAGE_HEAD  messagehead;
	int i = 0;
	unithead.unitId = ZC_ID_MESSAGEOUT;
	unithead.unitSize = 2 + sizeof(messagehead);
	
	messagehead.sequenceNum =1;			//临时
	messagehead.timeStamp =0;			//临时
	messagehead.SID =ZC5;
	messagehead.DID =ZC5;
	messagehead.msgType =MT_ZC_TO_CC;
	messagehead.msgLen = 2;

	memcpy(m_pcurrent, &unithead, sizeof(unithead));
	m_pcurrent +=sizeof(unithead);
	memcpy(m_pcurrent, &messagehead, sizeof(messagehead));
	m_pcurrent +=sizeof(messagehead);

	char m[2] = {'a','b'};
	memcpy(m_pcurrent, &m[0], sizeof(PLAT_UBYTE));
	++m_pcurrent;
	memcpy(m_pcurrent, &m[1], sizeof(PLAT_UBYTE));
	//memcpy(m_pcurrent, unithead + sizeof(PLAT_UINT32) * 2, 2);
}

void  DataSend(PLAT_UINT8* sendinfo)
{
	CPackUtility	packUtil(sendinfo);
	packUtil.pushPack(zc2Cir); 
	packUtil.pushPack(zc2Broder); 
	packUtil.pushPack(zc2Atp);
	packUtil.pushPack(zc2Ato);

	packUtil.finished();

}

void createPacakge()
{
	unsigned char* dstBuf = APP_WRITE_ADDR;

	CPackUtility	packUtil(dstBuf);
	packUtil.pushCC2ZC_RoutineMsg();
 	packUtil.pushZC2CC_RoutinMsg();
// 	packUtil.pushZC2CC_TrackMsg();
// 	packUtil.pushZC2CC_SpeedRstMsg(); //speed restrict
// 	packUtil.pushCC2ZC_2ATSTrasMsg();
// 	packUtil.pushZC2CC_fromATSTrasMsg();


// 	packUtil.pushPack(zc2Cir); 
// 	packUtil.pushPack(zc2Broder); 
// 	packUtil.pushPack(zc2Atp);
// 	packUtil.pushPack(zc2Ato);
	
	packUtil.finished();
}
