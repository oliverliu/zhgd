; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CZCSimView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ZCSim.h"
LastPage=0

ClassCount=5
Class1=CZCSimApp
Class2=CZCSimDoc
Class3=CZCSimView
Class4=CMainFrame

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class5=CAboutDlg
Resource3=IDD_ZCSIM_FORM

[CLS:CZCSimApp]
Type=0
HeaderFile=ZCSim.h
ImplementationFile=ZCSim.cpp
Filter=N

[CLS:CZCSimDoc]
Type=0
HeaderFile=ZCSimDoc.h
ImplementationFile=ZCSimDoc.cpp
Filter=N

[CLS:CZCSimView]
Type=0
HeaderFile=ZCSimView.h
ImplementationFile=ZCSimView.cpp
Filter=D
BaseClass=CFormView
VirtualFilter=VWC
LastObject=IDC_SETOK


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=IDM_BEAGIN_SEND




[CLS:CAboutDlg]
Type=0
HeaderFile=ZCSim.cpp
ImplementationFile=ZCSim.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
Command16=IDM_READ_CONFIG
Command17=ID_APP_ABOUT
Command18=IDM_BEAGIN_SEND
Command19=IDM_STOP_SEND
CommandCount=19

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_ZCSIM_FORM]
Type=1
Class=CZCSimView
ControlCount=15
Control1=IDC_BTN_EMG,button,1342242816
Control2=IDC_BTN_STOPREQ,button,1342242816
Control3=IDC_STOPSIGNAL_SEQ,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_RADIO_EASTDIR,button,1342177289
Control6=IDC_RADIO_WESTDIR,button,1342177289
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_MALSEGID,edit,1350631552
Control12=IDC_MALSEGOFFSET,edit,1350631552
Control13=IDC_MALENDTYPE,edit,1350631552
Control14=IDC_STATIC,static,1342181383
Control15=IDC_SETOK,button,1342242816

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

