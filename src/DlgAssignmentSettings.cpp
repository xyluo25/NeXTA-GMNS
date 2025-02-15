// DlgAssignmentSettings.cpp : implementation file
//

#include "stdafx.h"
#include "TLite.h"
#include "DlgAssignmentSettings.h"
#include "MainFrm.h"

// CDlgAssignmentSettings dialog

IMPLEMENT_DYNAMIC(CDlgAssignmentSettings, CDialog)

CDlgAssignmentSettings::CDlgAssignmentSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAssignmentSettings::IDD, pParent)
	, m_NumberOfIterations(10)
	, m_DemandGlobalMultiplier(1.0f)
	, m_SimulationHorizon(0)
	, m_DemandLoadingMultipler(0)
	, m_NumberReportingDays(1)
	, m_SimulatorName(_T(""))
{
	m_bModifiedFlag  = false;

	m_SimulatorName = "DTALite.exe";


}

CDlgAssignmentSettings::~CDlgAssignmentSettings()
{

}

void CDlgAssignmentSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Number_Iterations, m_NumberOfIterations);
	DDV_MinMaxInt(pDX, m_NumberOfIterations, 1, 10000);
	DDX_Control(pDX, IDC_LIST_DEMAND_LOADING_MODE, m_DemandLoadingModeList);
	DDX_Control(pDX, IDC_LIST_SIMULATION_METHOD, m_SimulationMethodControl);

	DDX_Control(pDX, IDC_LIST_DEMAND_LOADING_MODE2, m_NetworkDataList);
	DDX_Control(pDX, IDC_LIST_SCENARIO, m_ScenarioList);

	DDX_Control(pDX, IDC_LIST_SIMULATION_METHOD2, m_SignalMethodList);
}


BEGIN_MESSAGE_MAP(CDlgAssignmentSettings, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_SIMULATION_METHOD, &CDlgAssignmentSettings::OnLbnSelchangeListSimulationMethod)
	ON_BN_CLICKED(IDOK, &CDlgAssignmentSettings::OnBnClickedOk)
	ON_LBN_SELCHANGE(IDC_LIST_DEMAND_LOADING_MODE, &CDlgAssignmentSettings::OnLbnSelchangeListDemandLoadingMode)
	ON_LBN_SELCHANGE(IDC_LIST_Routing_METHOD, &CDlgAssignmentSettings::OnLbnSelchangeListRoutingMethod)
	ON_BN_CLICKED(IDOK2, &CDlgAssignmentSettings::OnBnClickedOk2)
	ON_LBN_SELCHANGE(IDC_LIST_DEMAND_LOADING_MODE2, &CDlgAssignmentSettings::OnLbnSelchangeListDemandLoadingMode2)
	ON_BN_CLICKED(IDC_CHECK_EMISSION_DATA, &CDlgAssignmentSettings::OnBnClickedCheckEmissionData)
	ON_LBN_SELCHANGE(IDC_LIST_Signal_Control_Representation, &CDlgAssignmentSettings::OnLbnSelchangeListSignalControlRepresentation)
	ON_LBN_SELCHANGE(IDC_LIST_SCENARIO, &CDlgAssignmentSettings::OnLbnSelchangeListScenario)
	ON_BN_CLICKED(IDC_CONFIG, &CDlgAssignmentSettings::OnBnClickedConfig)
	ON_BN_CLICKED(IDC_BUTTON_View_Edit_Scenario_Setting, &CDlgAssignmentSettings::OnBnClickedButtonViewEditScenarioSetting)
END_MESSAGE_MAP()


// CDlgAssignmentSettings message handlers


BOOL CDlgAssignmentSettings::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
	str.Format("%d nodes", m_pDoc->m_NodeSet.size());
	m_NetworkDataList.AddString(str);

	str.Format("%d links", m_pDoc->m_LinkSet.size());
	m_NetworkDataList.AddString(str);

	str.Format("%d zones", m_pDoc->m_ZoneMap.size());
	m_NetworkDataList.AddString(str);

	//update count of activity location if new zones have been added. 
	m_pDoc->m_ActivityLocationCount = 0;

	std::map<int, DTAZone>	:: iterator itr;

	str.Format("%d link types", m_pDoc->m_LinkTypeMap.size());
	m_NetworkDataList.AddString(str);

	m_SimulationMethodControl.AddString ("1. Point Queue Model");
	m_SimulationMethodControl.AddString ("2. Newell's Kinematic Wave Model");


	m_SimulatorName = theApp.m_SimulatorString_64;


	UpdateData(0);

	m_SimulationMethodControl.SetCurSel(m_pDoc->m_traffic_flow_model-1);


	UpdateData(0);
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgAssignmentSettings::OnLbnSelchangeListSimulationMethod()
{


}
void CDlgAssignmentSettings::OnLbnSelchangeListDemandLoadingMode()
{

}

void CDlgAssignmentSettings::OnLbnSelchangeListRoutingMethod()
{

}

void CDlgAssignmentSettings::UpdateScenarioFile()
{
	UpdateData(true);


	//if(m_pDoc->m_traffic_analysis_method != 0)
	//{
	//	if(m_pDoc->CheckControlData () == false)
	//		return;
	//}
	if(m_pDoc->m_traffic_flow_model-1 != m_SimulationMethodControl.GetCurSel())
	{
		m_bModifiedFlag = true;
		m_pDoc->m_traffic_flow_model = m_SimulationMethodControl.GetCurSel()+1;
	}
	


	if(m_NumberOfIterations != m_pDoc->m_number_of_iterations)
	{
		m_bModifiedFlag = true;
		m_pDoc->m_number_of_iterations = m_NumberOfIterations;
	}

	if(m_NumberReportingDays != m_pDoc->m_number_of_reporting_days)
	{
		m_bModifiedFlag = true;
		m_pDoc->m_number_of_reporting_days = m_NumberReportingDays;
	}
	if (m_pDoc->m_signal_reresentation_model  != m_SignalMethodList.GetCurSel())
	{
		m_bModifiedFlag = true;
		m_pDoc->m_signal_reresentation_model = m_SignalMethodList.GetCurSel();
	}



}

void CDlgAssignmentSettings::OnBnClickedOk()
{

	UpdateScenarioFile();

	OnOK();
}




void CDlgAssignmentSettings::OnBnClickedOk2()
{
	OnCancel( );
}

void CDlgAssignmentSettings::OnLbnSelchangeListDemandLoadingMode2()
{
	
}

void CDlgAssignmentSettings::OnBnClickedCheckEmissionData()
{
	UpdateData(1);
	m_bModifiedFlag = true;

}


void CDlgAssignmentSettings::OnLbnSelchangeListSignalControlRepresentation()
{
	// TODO: Add your control notification handler code here
}

void CDlgAssignmentSettings::OnEnChangeEditNumberIterations3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CDlgAssignmentSettings::OnLbnSelchangeListScenario()
{
	// TODO: Add your control notification handler code here
}


void CDlgAssignmentSettings::OnBnClickedConfig()
{

	AfxMessageBox("Please make sure the selected program is placed under the NeXTA Software_release folder.", MB_ICONINFORMATION);

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_LONGNAMES|OFN_ENABLESIZING,
		_T("User Defined Traffic Simulator (*.exe;*.bat)|*.exe|*.bat|"),NULL,0,true);

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	CString NetworkFile = pMainFrame->m_CurrentDirectory;
	dlg.m_ofn.lpstrInitialDir = NetworkFile;

	if(dlg.DoModal() == IDOK)
	{
		CString m_SimulatorString = m_pDoc->GetLocalFileName (dlg.GetPathName());
	
		CString NEXTASettingsPath;
		CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
		NEXTASettingsPath.Format ("%s\\NEXTA_Settings.ini", pMainFrame->m_CurrentDirectory);


		WritePrivateProfileString("initialization", "UserDefinedSimulator_64",m_SimulatorString,NEXTASettingsPath);
		m_SimulatorName = m_pDoc->GetWorkspaceTitleName (m_SimulatorString) ;
		
		UpdateData(0);
	} 
}



void CDlgAssignmentSettings::OnBnClickedButtonViewEditScenarioSetting()
{

	UpdateScenarioFile();
	m_pDoc->OpenCSVFileInExcel (m_pDoc->m_ProjectDirectory +"input_scenario_settings.csv");
}
