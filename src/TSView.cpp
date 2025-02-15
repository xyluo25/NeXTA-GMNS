// TSView.cpp : implementation of the CTimeSpaceView class
//

#include "stdafx.h"
#pragma warning(disable: 4786) // kill exceedingly long STL symbol warning
#include <set>
#include "TLite.h"
#include "TLiteDoc.h"
#include "Network.h"

#include "TSView.h"
#include "Dlg_Find_Vehicle.h"
#include <vector>
#include <sstream>
#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern void g_SetCursor(_cursor_type cursor_type);
extern float g_Simulation_Time_Stamp;
extern CPen s_PenSimulationClock;
extern void g_SelectColorCode(CDC* pDC, int ColorCount);
extern void g_SelectSuperThickPenColor(CDC* pDC, int ColorCount);
CPen g_TempLinkPen2(PS_DASH,0,RGB(0,255,255));

CPen ThickPen(PS_SOLID,3,RGB(0,255,0));
CPen DoublePen(PS_SOLID,2,RGB(0,255,0));

CPen NormalPen(PS_SOLID,1,RGB(0,255,0));
CPen Normal2Pen(PS_SOLID,2,RGB(0,0,255));

CPen v_BluePen(PS_SOLID,1,RGB(0,0,255));
CPen v_BlackPen(PS_SOLID,1,RGB(0,0,0));
CPen v_RedPen(PS_SOLID,1,RGB(255,0,0));



CPen Pen(PS_SOLID,1,RGB(0,0,0));

CPen DashPen(PS_DASH,0,RGB(0,255,0));
CPen DoubleDashPen(PS_DASH,2,RGB(0,255,0));
CPen SelectPen(PS_DASH,2,RGB(255,0,0));

/////////////////////////////////////////////////////////////////////////////
// CTimeSpaceView

IMPLEMENT_DYNCREATE(CTimeSpaceView, CView)

BEGIN_MESSAGE_MAP(CTimeSpaceView, CView)
	//{{AFX_MSG_MAP(CTimeSpaceView)
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
////	ON_COMMAND(ID_TOOLS_LOADVEHICLETRAJACTORYFILE, &CTimeSpaceView::OnToolsLoadvehicletrajactoryfile)
//	ON_COMMAND(ID_NGSIMDATA_LANE1, &CTimeSpaceView::OnNgsimdataLane1)
//	ON_COMMAND(ID_NGSIMDATA_LANE2, &CTimeSpaceView::OnNgsimdataLane2)
//	ON_COMMAND(ID_NGSIMDATA_LANE3, &CTimeSpaceView::OnNgsimdataLane3)
//	ON_COMMAND(ID_NGSIMDATA_LANE4, &CTimeSpaceView::OnNgsimdataLane4)
//	ON_COMMAND(ID_NGSIMDATA_LANE5, &CTimeSpaceView::OnNgsimdataLane5)
//	ON_COMMAND(ID_NGSIMDATA_LANE6, &CTimeSpaceView::OnNgsimdataLane6)
//	ON_COMMAND(ID_NGSIMDATA_LANE7, &CTimeSpaceView::OnNgsimdataLane7)
//	ON_COMMAND(ID_NGSIMDATA_CARFOLLOWINGDATAEXTRACTION, &CTimeSpaceView::OnNgsimdataCarfollowingdataextraction)
//	//	ON_COMMAND(ID_NGSIMDATA_SHOWVEHICLEID, &CTimeSpaceView::OnNgsimdataShowvehicleid)
////	ON_UPDATE_COMMAND_UI(ID_NGSIMDATA_SHOWVEHICLEID, &CTimeSpaceView::OnUpdateNgsimdataShowvehicleid)
//	//	ON_COMMAND(ID_NGSIMDATA_SEARCH_BY_VEHICLE_ID, &CTimeSpaceView::OnNgsimdataSearchByVehicleId)
//	ON_COMMAND(ID_NGSIMMENU_SHOWCALCULATEDCUMULATIVEFLOWCOUNTANDDENSITY, &CTimeSpaceView::OnNgsimmenuShowcalculatedcumulativeflowcountanddensity)
//	ON_UPDATE_COMMAND_UI(ID_NGSIMMENU_SHOWCALCULATEDCUMULATIVEFLOWCOUNTANDDENSITY, &CTimeSpaceView::OnUpdateNgsimmenuShowcalculatedcumulativeflowcountanddensity)
//	ON_COMMAND(ID_NGSIMMENU_SHOWSPACETIME_CONTOUR, &CTimeSpaceView::OnNgsimmenuShowspacetimeContour)
//	ON_UPDATE_COMMAND_UI(ID_NGSIMMENU_SHOWSPACETIME_CONTOUR, &CTimeSpaceView::OnUpdateNgsimmenuShowspacetimeContour)
//	ON_COMMAND(ID_NGSIMMENU_SHOWCDFPLOT, &CTimeSpaceView::OnNgsimmenuShowcdfplot)
//	ON_UPDATE_COMMAND_UI(ID_NGSIMMENU_SHOWCDFPLOT, &CTimeSpaceView::OnUpdateNgsimmenuShowcdfplot)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeSpaceView construction/destruction
CTimeSpaceView::CTimeSpaceView()
{

	m_MaxBinValue = 200;  // 20 seconds
	m_BinSize = 50;
	m_MaxFrequency = 50;

	m_bShowCumulativeFlowCount = false;
	m_bShowDensityContour = false;
	m_bShowPDFPlot = false;

	m_DataCollectionTimeInternval_in_sec = 30;
	m_SelectedVehicleCount = 0;
	m_SelectedStartTime = m_SelectedEndTime = 0;
	m_SelectedStartLocalY = m_SelectedEndLocalY = 0.0f;

	m_bMouseDownFlag = false;
	m_bLoadedDataFlag = false;
	m_NumberOfVehicles_lane = 0;
	m_KJam = 220; // 220 veh/mile
	m_WaveSpeed = 12; // in mph 
	m_FreeflowSpeed = 60.0f;

	m_bShowVehicleIDFlag = false;
	m_bShowLeaderPositionFlag = false;

	m_bShowSimulationDataFlag = false;
	m_bShowBothResultsFlag = false;
	m_bColorBySpeedFlag = false;
	m_SelectLaneID = 0;
	m_SelectedVehicleID = -1;
	m_NumberOfVehicles = 0;
	m_NumberOfTimeSteps = 0;

	m_bShowTimetable = true;

	m_bShowSlackTime = true;

	Cur_MOE_type1 = NGSIM_trajectory; 
	Cur_MOE_type2 = NGSIM_simulated_trajectory;

	m_NumberOfTimeSteps = 1440;
	m_TimeLeft = 0;
	m_TimeRight = 1440;
	m_LocalYLowerBound = 0;

	m_bShowResourcePrice = false;
	m_bShowNodeCostLabel = false;

	bRangeInitialized = false;

	for(int LaneID = 0; LaneID < 7; LaneID++)
		VehicleSizeLane[LaneID] = 0;
}

CTimeSpaceView::~CTimeSpaceView()
{
	if(m_NumberOfVehicles>0)
	{
		m_VehicleDataList.clear ();
	}

	if(m_NumberOfVehicles_lane)
	{
		Deallocate3DDynamicArray<float>(m_VehicleSnapshotAry_Lane,_MAX_LANE_SIZE,m_NumberOfVehicles_lane);

		DeallocateDynamicArray<int>(m_VehicleStartTimeAry_Lane , _MAX_LANE_SIZE,m_NumberOfVehicles_lane);
		DeallocateDynamicArray<int>(m_VehicleEndTimeAry_Lane , _MAX_LANE_SIZE,m_NumberOfVehicles_lane);

	}


}

BOOL CTimeSpaceView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CView::PreCreateWindow(cs);
}

CTLiteDoc* CTimeSpaceView::GetTLDocument() 
{ return reinterpret_cast<CTLiteDoc*>(m_pDocument); }

/////////////////////////////////////////////////////////////////////////////
// CTimeSpaceView drawing

void CTimeSpaceView::OnDraw(CDC* pDC)
{
	if(m_bLoadedDataFlag == false)
	{
		m_bLoadedDataFlag = true;	
		OnToolsLoadvehicletrajactoryfile();

	}
	CRect rectClient(0,0,0,0);
	GetClientRect(&rectClient);

	CRect PlotRect;
	GetClientRect(PlotRect);

	CRect PlotRectOrg = PlotRect;

	PlotRect.top += 60;
	PlotRect.bottom -= 60;
	PlotRect.left += 60;
	PlotRect.right -= 60;

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);

	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rectClient.Width(),
		rectClient.Height());
	memDC.SelectObject(&bmp);

	// Custom draw on top of memDC
	CBrush brush;
	if (!brush.CreateSolidBrush(RGB(255,255,255)))
		return;

	brush.UnrealizeObject();
	memDC.FillRect(rectClient, &brush);

	m_PlotRectNGSIM = PlotRect;

	if(!m_bShowBothResultsFlag) 	 // only one window	
		DrawNGSIMVehicleTrajectory(&memDC,Cur_MOE_type1,PlotRect);
	else
	{
		m_bShowSimulationDataFlag = false;
		PlotRect.bottom = PlotRectOrg.bottom /2;

		m_PlotRectNGSIM = PlotRect;

		DrawNGSIMVehicleTrajectory(&memDC,Cur_MOE_type1,PlotRect);  // observed data


		m_bShowSimulationDataFlag = true;
		PlotRect.top = PlotRectOrg.bottom /2+45;
		PlotRect.bottom = PlotRectOrg.bottom -20;

		m_PlotRectSimulated =  PlotRect;

		DrawNGSIMVehicleTrajectory(&memDC,Cur_MOE_type2,PlotRect);  // simulated data 
	}

	pDC->BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &memDC, 0,
		0, SRCCOPY);

	ReleaseDC(pDC);

	// TODO: add draw code for native data here

}

/////////////////////////////////////////////////////////////////////////////
// CTimeSpaceView diagnostics

#ifdef _DEBUG
void CTimeSpaceView::AssertValid() const
{
	CView::AssertValid();
}

void CTimeSpaceView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTimeSpaceView message handlers

void CTimeSpaceView::DrawFramework(CDC* pDC,int MOEType,CRect PlotRect)
{
	if(!bRangeInitialized)
	{
		InitializeTimeRange();
		bRangeInitialized = true;
	}

	if(m_TimeLeft<0)
		m_TimeLeft = 0;

	if(m_TimeRight > m_NumberOfTimeSteps)
		m_TimeRight = m_NumberOfTimeSteps;

	CPen DataPen(PS_SOLID,1,RGB(0,0,0));

	CPen TimePen(PS_DOT,1,RGB(0,255,0));



	int i;
	// step 2: calculate m_UnitDistance;
	// data unit
	m_UnitDistance = 1;
	if((m_LocalYUpperBound - m_LocalYLowerBound)>0)
		m_UnitDistance = (float)(PlotRect.bottom - PlotRect.top)/(m_LocalYUpperBound - m_LocalYLowerBound);


	// step 3: time interval
	int TimeXPosition;

	int TimeInterval = FindClosestTimeResolution(m_TimeRight - m_TimeLeft);

	// time unit
	m_UnitTime = 1;
	if((m_TimeRight - m_TimeLeft)>0)
		m_UnitTime = (float)(PlotRect.right - PlotRect.left)/(m_TimeRight - m_TimeLeft);

	// step 4: draw time axis

	pDC->SelectObject(&TimePen);

	char buff[20];
	for(i=m_TimeLeft;i<=m_TimeRight;i+=TimeInterval)
	{
		TimeXPosition=(int)(PlotRect.left+(i-m_TimeLeft)*m_UnitTime);

		if(i>= m_TimeLeft)
		{
			pDC->MoveTo(TimeXPosition,PlotRect.bottom+2);
			pDC->LineTo(TimeXPosition,PlotRect.top);

			if(i/2 <10)
				TimeXPosition-=5;
			else
				TimeXPosition-=3;

			int min, sec;
			min = i/600;
			sec =  (i- min*600)/10;
			wsprintf(buff,"%2d:%02d",min, sec);
			pDC->TextOut(TimeXPosition,PlotRect.bottom+3,buff);
		}
	}

	// 	step 4: draw time axis

	pDC->SelectObject(&NormalPen);

	pDC->MoveTo(PlotRect.left, PlotRect.top);
	pDC->LineTo(PlotRect.left,PlotRect.bottom);

	pDC->MoveTo(PlotRect.right, PlotRect.top);
	pDC->LineTo(PlotRect.right,PlotRect.bottom);


}
void CTimeSpaceView::DrawNGSIMVehicleTrajectory(CDC* pDC,eNGSIMDisplayMode MOEType,CRect PlotRect)
{

	DrawFramework(pDC,MOEType,PlotRect);



	pDC->SetBkMode(TRANSPARENT);

	CString str_MOE, str_selected_link;

	if(m_bShowSimulationDataFlag && MOEType != NGSIM_trajectory)
		str_MOE.Format ("Time-Space Graph: Lane %d (simulated), Kj = %5.1f vml, W = %5.1f, Vf= %5.1f", m_SelectLaneID,m_KJam,m_WaveSpeed,m_FreeflowSpeed);
	else
	{  // NGSIM_trajectory
		str_MOE.Format ("Time-Space Graph: Lane %d (observed)", m_SelectLaneID);

		float distance = fabs(m_SelectedEndLocalY - m_SelectedStartLocalY)/5280.0f;
		float time = abs(m_SelectedEndTime - m_SelectedStartTime)/10.0f;  // in sec
		float speed_in_mph = distance/max(0.000001,time/3600); // in mph

		int count_of_vehicles = m_SelectedVehicleCount;
		float density = count_of_vehicles/max(0.0001, distance);  // per mile
		float flow = count_of_vehicles/max(0.000001, time*3600); // per hour

		str_selected_link.Format("from time %d to %d, [%.1f sec], from distance %8.3f to %8.3f [%8.4f miles], speed = %5.2f mph, # of veh = %d, k = %5.1f, q = %5.1f", 
			m_SelectedStartTime, m_SelectedEndTime,time ,
			m_SelectedStartLocalY, m_SelectedEndLocalY,distance,
			speed_in_mph, count_of_vehicles, density, flow
			);

	}




	if(m_TempLinkStartPoint!= m_TempLinkEndPoint)
	{
		pDC->SelectObject(&Normal2Pen);
		pDC->MoveTo(m_TempLinkStartPoint);
		pDC->LineTo(m_TempLinkEndPoint);
	}

	pDC->SelectObject(&NormalPen);

	pDC->TextOut(PlotRect.right/2-100,PlotRect.top-30,str_MOE);

	if(str_selected_link.GetLength () > 0)
		pDC->TextOut(PlotRect.right/2-300,PlotRect.top-10,str_selected_link);

	// draw vehicles

	int display_resolution = 1;
	display_resolution = max(1,(m_TimeRight - m_TimeLeft)/2000.0f);

	if(m_bShowSimulationDataFlag == false)
	{ // show original data

		for(int v=0; v<m_NumberOfVehicles; v+=1)
		{
			float StartTime = m_VehicleDataList[v].StartTime;

			if(StartTime < m_TimeLeft)
				StartTime = m_TimeLeft;

			float EndTime = m_VehicleDataList[v].EndTime;

			if(EndTime > m_TimeRight)
				EndTime = m_TimeRight;

			bool bIniFlag  = false;
			if(StartTime< EndTime)
			{
				for(int t= StartTime; t<=EndTime; t+=display_resolution)  
				{

					if(m_VehicleSnapshotAry[v][t].LaneID ==m_SelectLaneID)  // on the currently selected lane
					{

						if(!m_bColorBySpeedFlag)
						{
							switch(m_VehicleDataList[v].VehicleType)
							{

							case 1: pDC->SelectObject(&v_BlackPen); break;  // passenger 
							case 2: pDC->SelectObject(&v_BlackPen); break;  // truck
							case 3: pDC->SelectObject(&v_BluePen); break;   // motocycle
							default: 
								pDC->SelectObject(&v_BlackPen);
							}

							if(m_SelectedVehicleID == m_VehicleDataList[v].VehicleID)
							{
								pDC->SelectObject(&v_RedPen);
							}

						}else
						{
							CPen penmoe;

							float power;

							float speed = max(1,m_VehicleSnapshotAry[v][t].Speed_mph);
							power= min(1.0f, 55/speed);

							float n= power*100;
							int R=(int)((255*n)/100);
							int G=(int)(255*(100-n)/100); 
							int B=0;

							penmoe.CreatePen (PS_SOLID, 1, RGB(R,G,B));
							pDC->SelectObject(&penmoe);

						}


						int x=(int)(PlotRect.left+(t - m_TimeLeft)*m_UnitTime);  // timestamp
						int y = PlotRect.bottom - (int)(((m_VehicleSnapshotAry[v][t].LocalY-m_LocalYLowerBound)*m_UnitDistance)+0.50);  // distance


						if(!bIniFlag)
						{
							pDC->MoveTo(x,y);

							if(m_bShowVehicleIDFlag)
							{
								CString str_vid;
								str_vid.Format ("%d",m_VehicleDataList[v].VehicleID);


								if(m_SelectedVehicleID == m_VehicleDataList[v].VehicleID)
								{
									pDC->SetTextColor(RGB(255,0,0));
								}else
								{
									pDC->SetTextColor(RGB(0,0,0));
								}


								pDC->TextOut(x,y,str_vid);
							}

							bIniFlag = true;

						}else
						{
							//select color according to vehicle type
							switch(m_VehicleDataList[v].VehicleType)
							{

							case 1: pDC->SelectObject(&v_BluePen); break;
							case 2: pDC->SelectObject(&v_BlackPen); break;
							case 3: pDC->SelectObject(&v_RedPen); break;
							default:
								pDC->SelectObject(&v_BlackPen);
							}

							int follower_x = x;
							int follower_y = y;
							pDC->LineTo(x,y);

							if(m_bShowLeaderPositionFlag && (t+v)%50==0) // every 5 seconds
							{
								int leader_vid = m_VehicleSnapshotAry[v][t].PrecedingVehicleID;
								if(leader_vid>0) // preceeding vehicle has been identified in the orignal NGSim data
								{

									int leader_no = m_VehicleIDtoNOMap[leader_vid];  // convert vid to no in our array

									if( leader_no > 0 && leader_no < m_NumberOfVehicles)
									{
										pDC->SelectObject(&DashPen);

										int leader_y = PlotRect.bottom - (int)(((m_VehicleSnapshotAry[leader_no][t].LocalY-m_LocalYLowerBound)*m_UnitDistance)+0.50);
										pDC->LineTo(x,leader_y);

										if(m_VehicleSnapshotAry[leader_no][t].LaneID != m_VehicleSnapshotAry[v][t].LaneID)  // not on the same lane!
										{
											CString str_lane;
											str_lane.Format ("L%d",m_VehicleSnapshotAry[leader_no][t].LaneID );
											pDC->TextOut(x,leader_y,str_lane);
										}
									}

								}

								pDC->MoveTo(follower_x,follower_y);


							}


						}

					}

				}
			}
		}

	}else
	{
		// show simulation data for m_SelectLaneID

		int SelectedLaneNo = m_SelectLaneID-1; //SelectedLaneNo starts from 0
		for(int v=0; v<VehicleSizeLane[SelectedLaneNo]; v+=1)
		{
			bool bIniFlag  = false;
			for(int t= m_VehicleStartTimeAry_Lane[SelectedLaneNo][v]; t<=m_VehicleEndTimeAry_Lane[SelectedLaneNo][v]; t+=display_resolution)  
			{
				pDC->SelectObject(&v_BlackPen);

				int x=(int)(PlotRect.left+(t - m_TimeLeft)*m_UnitTime);
				int y = PlotRect.bottom - (int)(((m_VehicleSnapshotAry_Lane[SelectedLaneNo][v][t]-m_LocalYLowerBound)*m_UnitDistance)+0.50);


				if(!bIniFlag)
				{
					pDC->MoveTo(x,y);
					bIniFlag = true;

				}else
				{
					pDC->LineTo(x,y);

				}

			}

		}
	}




}

void CTimeSpaceView::RefreshWindow()
{
	OnUpdate(0, 0, 0);

}


BOOL CTimeSpaceView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int CurrentTime = int((pt.x - 60.0f)/m_UnitTime + m_TimeLeft);

	int LeftShiftSize = max(5,int(0.2*(CurrentTime-m_TimeLeft))/10*10);
	int RightShiftSize = max(5,int(0.2*(m_TimeRight-CurrentTime))/10*10);

	if(zDelta <	 0)
	{
		m_TimeLeft-=LeftShiftSize;
		m_TimeRight+=RightShiftSize;
	}
	else
	{
		m_TimeLeft+=LeftShiftSize;
		m_TimeRight-=RightShiftSize;
	}

	if(m_TimeLeft<0)  m_TimeLeft = 0;
	if(m_TimeRight>m_NumberOfTimeSteps) m_TimeRight = m_NumberOfTimeSteps;

	Invalidate();
	return true;

}

void CTimeSpaceView::InitializeTimeRange()
{
	int min_timestamp = m_NumberOfTimeSteps;
	int max_timestamp = 0;

	CTLiteDoc* pDoc = GetTLDocument();

	//if(pDoc->m_TrainVector.size() ==0)
	//{
	//	m_TimeLeft = 0;
	//	m_TimeRight = m_NumberOfTimeSteps;

	//	return;
	//}
	//// narrow the range for display

	//for(unsigned int v = 0; v<pDoc->m_TrainVector.size(); v++)
	//{

	//	DTA_Train* pTrain = pDoc->m_TrainVector[v];

	//	for(int n = 0; n< pTrain->m_NodeSize; n++)
	//	{

	//		if(min_timestamp > pTrain->m_aryTN[n].NodeTimestamp)
	//			min_timestamp = pTrain->m_aryTN[n].NodeTimestamp;

	//		if(max_timestamp < pTrain->m_aryTN[n].NodeTimestamp)
	//			max_timestamp = pTrain->m_aryTN[n].NodeTimestamp;

	//	}
	//}

	//m_TimeLeft = min_timestamp;
	//m_TimeRight = max_timestamp;

}

void CTimeSpaceView::OnRButtonDown(UINT nFlags, CPoint point)
{

	InitializeTimeRange();
	m_TempLinkStartPoint  = m_TempLinkEndPoint ;


	Invalidate();
	CView::OnRButtonDown(nFlags, point);
}

void CTimeSpaceView::OnTimetableResourceprice()
{
	m_bShowResourcePrice = !m_bShowResourcePrice;
	Invalidate();

}

void CTimeSpaceView::OnUpdateTimetableResourceprice(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowResourcePrice);
}

void CTimeSpaceView::OnTimetableNodecostlabel()
{
	m_bShowNodeCostLabel = ! m_bShowNodeCostLabel;
	Invalidate();


}

void CTimeSpaceView::OnUpdateTimetableNodecostlabel(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowNodeCostLabel);
}

void CTimeSpaceView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_last_cpoint = point;

	//	Invalidate();  // reflesh
		g_SetCursor(_cursor_movement_network);

	m_TempLinkStartPoint = point;
	m_TempLinkEndPoint = point;
	m_bMouseDownFlag = true;

	CView::OnLButtonDown(nFlags, point);
}

void CTimeSpaceView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bMouseDownFlag)
	{
		// if it is the first moving operation, erase the previous temporal link
		if(m_TempLinkStartPoint!=m_TempLinkEndPoint)
			DrawTemporalLink(m_TempLinkStartPoint,m_TempLinkEndPoint);
		CView::OnMouseMove(nFlags, point);
		// update m_TempLinkEndPoint from the current mouse point
		m_TempLinkEndPoint = point;

		// draw a new temporal link
		DrawTemporalLink(m_TempLinkStartPoint,m_TempLinkEndPoint);
	}

}

void CTimeSpaceView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// obtain local y and timestamps for selected link
	m_SelectedStartTime = GetTimestampFromPointX(m_TempLinkStartPoint.x, m_PlotRectNGSIM);
	m_SelectedEndTime = GetTimestampFromPointX(m_TempLinkEndPoint.x, m_PlotRectNGSIM);

	m_SelectedStartLocalY = GetLocalYFromPointY(m_TempLinkStartPoint.y, m_PlotRectNGSIM);
	m_SelectedEndLocalY = GetLocalYFromPointY(m_TempLinkEndPoint.y, m_PlotRectNGSIM);

	CountVehicles(m_SelectedStartTime,m_SelectedEndTime, m_SelectedStartLocalY,m_SelectedEndLocalY);

	m_bMouseDownFlag = false;
	Invalidate();

	CView::OnLButtonUp(nFlags, point);
}

void CTimeSpaceView::OnClose()
{
	CView::OnClose();
}



void CTimeSpaceView::OnToolsLoadvehicletrajactoryfile()
{

	static char BASED_CODE szFilter[] = "NGSIM Vehicle Trajectory File (*.txt)|*.txt||";

	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

	std::vector<VehicleSnapshotData> OriginalDataVector;

	CTime ExeStartTime = CTime::GetCurrentTime();

	if(dlg.DoModal() == IDOK)
	{
		LoadVehicleTrajectoryFile(dlg.GetPathName());
	}

	CTime ExeEndTime = CTime::GetCurrentTime();

	CTimeSpan ts = ExeEndTime  - ExeStartTime;

	CString str_running_time;

	str_running_time.Format ("Load %d vehicles from the vehicle trajectory file.\nLoading Time: %d hour %d min %d sec. \n",
		m_NumberOfVehicles, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());

	AfxMessageBox(str_running_time, MB_ICONINFORMATION);
}

void CTimeSpaceView::LoadVehicleTrajectoryFile(CString file_name)
{

	int Vehicle_ID; //1 
	int Frame_ID;//2
	int Total_Frames; //3
	int Global_Time ; //4
	float Local_X ; //5
	float Local_Y  ; //6
	float Global_X  ; //7
	float Global_Y  ; //8
	float Vehicle_Length ; //9
	float Vehicle_Width ; //10
	int	Vehicle_Class ; //11
	float	Vehicle_Velocity ; //12
	float	Vehicle_Acceleration  ; //13
	int 	Lane_Identification ; //14
	int	Preceding_Vehicle ; //15
	int	Following_Vehicle  ; //16
	float	Spacing ; //17
	float	Headway  ; //18
	float CalculatedSpeed;
	std::vector<VehicleSnapshotData> OriginalDataVector;
	CTime ExeStartTime = CTime::GetCurrentTime();

	int line = 0;



	CWaitCursor wc;

	char fname[_MAX_PATH];

	wsprintf(fname,"%s", file_name);

	FILE* st;



	m_LocalYUpperBound = 0;

	m_LocalYLowerBound = 9999999;



	fopen_s(&st,fname,"r");

	float prev_local_y = -1;
	if(st!=NULL)
	{

		m_NumberOfVehicles = 0;
		m_NumberOfTimeSteps = 0;
		int Prev_Vehicle_ID = -1;

		while(!feof(st))
		{	

			VehicleCFData vcd;



			Vehicle_ID = g_read_integer(st);  //1

			if(Vehicle_ID < 0)

				break;



			Frame_ID = g_read_integer(st); //2



			if(Frame_ID > m_NumberOfTimeSteps)

				m_NumberOfTimeSteps = Frame_ID;



			Total_Frames = g_read_integer(st); //3

			Global_Time = g_read_integer(st); //4



			Local_X = g_read_float(st); //5

			Local_Y = g_read_float(st); //6



			if(Local_Y > m_LocalYUpperBound)

				m_LocalYUpperBound = Local_Y;



			if(Local_Y < m_LocalYLowerBound)

				m_LocalYLowerBound = Local_Y;


			CalculatedSpeed = 0;

			if(Vehicle_ID == Prev_Vehicle_ID)
			{
				CalculatedSpeed = (Local_Y - prev_local_y)*10.0f * 0.681818182;  // convert from feet per second to mph
			}

			prev_local_y = Local_Y;


			Global_X = g_read_float(st); //7

			Global_Y = g_read_float(st); //8

			Vehicle_Length = g_read_float(st); //9

			Vehicle_Width = g_read_float(st); //10



			Vehicle_Class = g_read_integer(st); //11



			Vehicle_Velocity = g_read_float(st); //12

			Vehicle_Acceleration  = g_read_float(st); //13



			Lane_Identification = g_read_integer(st); //14

			Preceding_Vehicle = g_read_integer(st); //15

			Following_Vehicle = g_read_integer(st); //16



			Spacing =  g_read_float(st); //17

			Headway =  g_read_float(st); //18



			if(Vehicle_ID != Prev_Vehicle_ID)

			{


				m_NumberOfVehicles++;



				m_VehicleIDtoNOMap[Vehicle_ID] = m_NumberOfVehicles;  // ID to sequential No.



				Prev_Vehicle_ID = Vehicle_ID;



				vcd.VehicleID = Vehicle_ID;



				vcd.StartTime = Frame_ID;

				vcd.EndTime = Frame_ID + Total_Frames;

				if(Frame_ID + Total_Frames > m_NumberOfTimeSteps)
				{
					m_NumberOfTimeSteps = Frame_ID + Total_Frames;
				}

				vcd.VehicleType = Vehicle_Class;



				m_VehicleDataList.push_back (vcd);



			}



			VehicleSnapshotData element;

			element.VehicleID = m_NumberOfVehicles-1;

			element.Frame_ID  = Frame_ID;

			element.LocalY  =  Local_Y;  //m_NumberOfVehicles-1 make sure the index starting from 0

			element.LaneID  =  Lane_Identification; 

			element.Speed_mph  = Vehicle_Velocity * 0.681818182;  // convert from feet per second to mph
			element.CalculatedSpeed_mph = CalculatedSpeed;

			element.PrecedingVehicleID = Preceding_Vehicle;

			element.FollowingVehicleID = Following_Vehicle;



			m_SelectLaneID = Lane_Identification;  // set Lane ID as a given number



			OriginalDataVector.push_back(element);



			/*	

			//		ofstream  NGSIM_LogFile;

			//		NGSIM_LogFile.open ("C:\\temp\\NGSIM_Trajectry_Sample.txt", ios::out);

			//		NGSIM_LogFile.setf(ios::fixed);



			if (NGSIM_LogFile.is_open() && Lane_Identification == 4 && Frame_ID < 3000)

			{

			NGSIM_LogFile << Vehicle_ID << "," << Frame_ID << "," << Total_Frames << "," <<  Global_Time << ",";

			NGSIM_LogFile << Local_X <<  "," << Local_Y <<  "," << Global_X <<  "," << Global_Y <<  "," << Vehicle_Length <<  "," << Vehicle_Width << ",";

			NGSIM_LogFile	<< Vehicle_Class <<  "," << Vehicle_Velocity <<  "," << Vehicle_Acceleration <<  "," << Lane_Identification <<  "," <<	Preceding_Vehicle <<  "," <<  Following_Vehicle <<  "," << Spacing <<  "," << Headway << endl;



			}

			NGSIM_LogFile.close();



			*/

			line++;

		}

		fclose(st);

	}


	m_VehicleSnapshotAry	= AllocateDynamicArray<VehicleSnapshotData>(m_NumberOfVehicles,m_NumberOfTimeSteps+1);



	for(unsigned i  = 0; i < OriginalDataVector.size(); i++)

	{

		VehicleSnapshotData element = OriginalDataVector[i];

		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].LaneID =  element.LaneID;

		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].LocalY =  element.LocalY;

		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].Speed_mph =  element.Speed_mph;
		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].CalculatedSpeed_mph  =  element.CalculatedSpeed_mph;

		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].PrecedingVehicleID =  element.PrecedingVehicleID;

		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].FollowingVehicleID  =  element.FollowingVehicleID;

	}

	m_TimeLeft = 0;
	m_TimeRight = m_NumberOfTimeSteps;
	ConstructCellBasedDensityProfile();

}

void CTimeSpaceView::OnToolsLoadvehicletrajactoryfile_backup()
{
	int Vehicle_ID; //1 
	int Frame_ID;//2
	int Total_Frames; //3
	long long Global_Time ; //4
	double Local_X ; //5
	double Local_Y  ; //6
	double Global_X  ; //7
	double Global_Y  ; //8
	float Vehicle_Length ; //9
	float Vehicle_Width ; //10
	int	Vehicle_Class ; //11

	float Vehicle_Velocity ; //12
	float Vehicle_Acceleration  ; //13

	int Lane_Identification ; //14
	int	Preceding_Vehicle ; //15
	int	Following_Vehicle  ; //16

	float Spacing ; //17
	float Headway  ; //18


	static char BASED_CODE szFilter[] = "NGSIM Vehicle Trajectory File (*.txt)|*.txt||";
	CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilter);

	std::vector<VehicleSnapshotData> OriginalDataVector;

	CTime ExeStartTime = CTime::GetCurrentTime();
	int line = 0;

	if(dlg.DoModal() == IDOK)
	{
		CWaitCursor wc;
		char fname[_MAX_PATH];
		wsprintf(fname,"%s", dlg.GetPathName());
		//FILE* st;

		m_LocalYUpperBound = 0;
		m_LocalYLowerBound = 9999999;

		int Prev_Vehicle_ID = -9999;
		int Prev_Vehicle_ID_In_TargetLanes = -9999;
		int Last_Observed_LaneID = -1;
		bool b_OutputAVI = false;
		int first_frame_id = -1;

		//fopen_s(&st,fname,"r");
		ifstream in(dlg.GetPathName());
		if(in.is_open())
		{
			m_NumberOfVehicles = 0;
			m_NumberOfTimeSteps = 0;

			string s;

			ofstream  NGSIM_LogFile;
			NGSIM_LogFile.open ("C:\\NGSIM_Trajectry_Sample.csv", ios::out);
			NGSIM_LogFile.setf(ios::fixed);
			NGSIM_LogFile << "Veh_ID,Veh_Class,Lane_ID,Sensor_Type,Frame_ID,X,Y,Speed,Acceleration,Error" << endl;

			while(/*!feof(st)*/getline(in,s))
			{	
				VehicleCFData vcd;

				istringstream is(s);

				is >> Vehicle_ID >> Frame_ID >> Total_Frames >> Global_Time
					>> Local_X >> Local_Y >> Global_X >> Global_Y 
					>> Vehicle_Length >> Vehicle_Width >> Vehicle_Class
					>> Vehicle_Velocity >> Vehicle_Acceleration >> Lane_Identification
					>> Preceding_Vehicle >> Following_Vehicle >> Spacing >> Headway;

				//Vehicle_ID = g_read_integer(st);  //1
				if(Vehicle_ID < 0)
					break;

				//Frame_ID = g_read_integer(st); //2

				if(Frame_ID > m_NumberOfTimeSteps)
					m_NumberOfTimeSteps = Frame_ID;

				//Total_Frames = g_read_integer(st); //3
				//Global_Time = g_read_integer(st); //4

				//Local_X = g_read_float(st); //5
				//Local_Y = g_read_float(st); //6

				if(Local_Y > m_LocalYUpperBound)
					m_LocalYUpperBound = Local_Y;

				if(Local_Y < m_LocalYLowerBound)
					m_LocalYLowerBound = Local_Y;

				//Global_X = g_read_float(st); //7
				//Global_Y = g_read_float(st); //8
				//Vehicle_Length = g_read_float(st); //9
				//Vehicle_Width = g_read_float(st); //10
				//Vehicle_Class = g_read_integer(st); //11
				//Vehicle_Velocity = g_read_float(st); //12
				//Vehicle_Acceleration  = g_read_float(st); //13
				//Lane_Identification = g_read_integer(st); //14
				//Preceding_Vehicle = g_read_integer(st); //15
				//Following_Vehicle = g_read_integer(st); //16
				//Spacing =  g_read_float(st); //17
				//Headway =  g_read_float(st); //18

				if(Prev_Vehicle_ID != Vehicle_ID)
				{

					b_OutputAVI = true;
					m_NumberOfVehicles++;

					m_VehicleIDtoNOMap[Vehicle_ID] = m_NumberOfVehicles;  // ID to sequential No.

					//Prev_Vehicle_ID = Vehicle_ID;


					first_frame_id = Frame_ID;

					vcd.VehicleID = Vehicle_ID;
					vcd.StartTime = Frame_ID;
					vcd.EndTime = Frame_ID + Total_Frames;
					vcd.VehicleType = Vehicle_Class;

					m_VehicleDataList.push_back (vcd);

				}

				VehicleSnapshotData element;
				element.VehicleID = m_NumberOfVehicles-1;
				element.Frame_ID  = Frame_ID;
				element.LocalX = Local_X;
				element.LocalY  =  Local_Y;  //m_NumberOfVehicles-1 make sure the index starting from 0
				element.LaneID  =  Lane_Identification; 
				element.Speed_mph  = Vehicle_Velocity * 0.681818182;  // convert from feet per second to mph
				element.Acceleration = Vehicle_Acceleration * (3600*3600 / 5280);
				element.VehicleClass = Vehicle_Class;
				element.PrecedingVehicleID = Preceding_Vehicle;
				element.FollowingVehicleID = Following_Vehicle;

				m_SelectLaneID = Lane_Identification;  // set Lane ID as a given number

				OriginalDataVector.push_back(element);


				if (NGSIM_LogFile.is_open() )
				{
					if ( (Lane_Identification == 4 || Lane_Identification == 6))
					{
						//Normal output 
						NGSIM_LogFile << Vehicle_ID << "," << Vehicle_Class << "," 
							<< Lane_Identification << "," 
							<< 0 << ","
							<< Frame_ID << "," 
							<< Local_X <<  "," << Local_Y <<  "," 
							<< Vehicle_Velocity <<  "," << Vehicle_Acceleration << ","
							<< 0 // for normal data, the error is 0
							<< endl;

						//Loop detector, at fixed location (in this case: local_y = 860
						//10% error is added into observed speed
						if (Local_Y >= 860 && b_OutputAVI) 
						{
							NGSIM_LogFile << Vehicle_ID << "," << Vehicle_Class << "," 
								<< Lane_Identification << "," 
								<< 1 << ","
								<< Frame_ID << "," 
								<< Local_X <<  "," << 860 <<  ",";
							// for loop detector, based on the random number generated, >= 0.5 --> 0.1 otherwise -0.1
							float random_error = g_GetRandomRatio()>= 0.5?0.1:-0.1;
							NGSIM_LogFile << Vehicle_Velocity + Vehicle_Velocity * random_error  <<  "," 
								<< Vehicle_Acceleration << ","
								<< random_error
								<< endl;

							b_OutputAVI = false;
						}


						//AVI Data -- 20% error in observation time in term of time (FrameID)
						//First observation of one specific vehicle
						if(Prev_Vehicle_ID_In_TargetLanes != Vehicle_ID)
						{
							float random_error = g_GetRandomRatio()>= 0.5?0.2:-0.2;

							NGSIM_LogFile << Vehicle_ID << "," << Vehicle_Class << "," 
								<< Lane_Identification << "," 
								<< 2 << ","
								<< Frame_ID + (int)( Frame_ID * random_error) << "," 
								<< Local_X <<  "," << Local_Y <<  "," 
								<< Vehicle_Velocity <<  "," << Vehicle_Acceleration << ","
								<< random_error // for normal data, the error is 0
								<< endl;

							Prev_Vehicle_ID_In_TargetLanes = Vehicle_ID;
							Last_Observed_LaneID = Lane_Identification;
						}


						//Last observation of one specific vehicle
						if ( Frame_ID  == Total_Frames + first_frame_id -1)
						{
							float random_error = g_GetRandomRatio()>= 0.5?0.2:-0.2;

							NGSIM_LogFile << Vehicle_ID << "," << Vehicle_Class << "," 
								<< Lane_Identification << "," 
								<< 2 << ","
								<< Frame_ID + (int)( Frame_ID * random_error) << "," 
								<< Local_X <<  "," << Local_Y <<  "," 
								<< Vehicle_Velocity <<  "," << Vehicle_Acceleration << ","
								<< random_error // for normal data, the error is 0
								<< endl;
						}

						//GPS trace
						//10% error is added in to time, local_y and speed
						if ((Frame_ID - first_frame_id) % 100 == 0)
						{
							float random_error = g_GetRandomRatio()>= 0.5?0.1:-0.1;
							NGSIM_LogFile << Vehicle_ID << "," << Vehicle_Class << "," 
								<< Lane_Identification << "," 
								<< 3 << ","
								<< Frame_ID + (int) (Frame_ID * random_error) << "," 
								<< Local_X <<  "," << Local_Y + (int) (Frame_ID * random_error) <<  "," 
								<< Vehicle_Velocity + Vehicle_Velocity * random_error <<  "," 
								<< Vehicle_Acceleration << ","
								<< random_error // for normal data, the error is 0
								<< endl;
						}

					}

					//for those vehilce change from lane 4 or 6 to another lane
					if (Prev_Vehicle_ID_In_TargetLanes == Vehicle_ID && Last_Observed_LaneID != Lane_Identification)
					{
						if ( !(Last_Observed_LaneID == 4 || Last_Observed_LaneID == 6) && !(Lane_Identification == 4 || Lane_Identification == 6))
						{
						}
						else
						{
							float random_error = g_GetRandomRatio()>= 0.5?0.2:-0.2;

							int lane_id;
							if ( Last_Observed_LaneID == 4 || Last_Observed_LaneID == 6)
							{
								lane_id = Last_Observed_LaneID;
							}
							else
							{
								lane_id = Lane_Identification;
							}
							NGSIM_LogFile << Vehicle_ID << "," << Vehicle_Class << "," 
								<< lane_id << "," 
								<< 2 << ","
								<< Frame_ID + (int)( Frame_ID * random_error) << "," 
								<< Local_X <<  "," << Local_Y <<  "," 
								<< Vehicle_Velocity <<  "," << Vehicle_Acceleration << ","
								<< random_error // for normal data, the error is 0
								<< endl;
						}

					}

				}

				if(Prev_Vehicle_ID != Vehicle_ID)
				{
					Prev_Vehicle_ID = Vehicle_ID;
				}

				Last_Observed_LaneID = Lane_Identification;

				line++;
			}

			NGSIM_LogFile.close();
			//fclose(st);
		}
	}

	m_VehicleSnapshotAry	= AllocateDynamicArray<VehicleSnapshotData>(m_NumberOfVehicles,m_NumberOfTimeSteps+1);

	for(unsigned i  = 0; i < OriginalDataVector.size(); i++)
	{
		VehicleSnapshotData element = OriginalDataVector[i];
		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].LaneID =  element.LaneID;
		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].LocalY =  element.LocalY;
		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].Speed_mph =  element.Speed_mph;
		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].PrecedingVehicleID =  element.PrecedingVehicleID;
		m_VehicleSnapshotAry [element.VehicleID ] [element.Frame_ID].FollowingVehicleID  =  element.FollowingVehicleID;
	}

	m_TimeLeft = 0;
	m_TimeRight = m_NumberOfTimeSteps;
	/// loading statistics
	CTime ExeEndTime = CTime::GetCurrentTime();

	CTimeSpan ts = ExeEndTime  - ExeStartTime;
	CString str_running_time;

	str_running_time.Format ("Load %d vehicles and %d lines from the vehicle trajectory file.\nLoading Time: %d hour %d min %d sec. \n",
		m_NumberOfVehicles, line, ts.GetHours(), ts.GetMinutes(), ts.GetSeconds());

	AfxMessageBox(str_running_time, MB_ICONINFORMATION);
}
void CTimeSpaceView::OnNgsimdataLane1()
{
	m_SelectLaneID  = 1;
	Invalidate();
}

void CTimeSpaceView::OnNgsimdataLane2()
{
	m_SelectLaneID  = 2;
	Invalidate();
}

void CTimeSpaceView::OnNgsimdataLane3()
{
	m_SelectLaneID  = 3;
	Invalidate();
}

void CTimeSpaceView::OnNgsimdataLane4()
{
	m_SelectLaneID  = 4;
	Invalidate();
}

void CTimeSpaceView::OnNgsimdataLane5()
{
	m_SelectLaneID  = 5;
	Invalidate();
}

void CTimeSpaceView::OnNgsimdataLane6()
{
	m_SelectLaneID  = 6;
	Invalidate();
}

void CTimeSpaceView::OnNgsimdataLane7()
{
	m_SelectLaneID  = 7;
	Invalidate();
}

void CTimeSpaceView::OnNgsimdataColorspeed()
{
	m_bColorBySpeedFlag = !m_bColorBySpeedFlag;
	Invalidate();
}

void CTimeSpaceView::OnUpdateNgsimdataColorspeed(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bColorBySpeedFlag);
}

bool compare_timestamp (int first, int second)
{
	if(first < second) return true;
	else 
		return false;
}

void CTimeSpaceView::OnNgsimdataCarfollowingsimulation()
{
	// for each lane
	// find out the first vehicle on this lane
	// record the trajectory as the lead vehicle as the boundary condition
	// record the upstream and downstream sensor location: +- 50 feet 
	// count vehicles passing through sensors, build the arrival and departure timestamps
	// create new vehicle car following data array
	// simulation
	// display

	m_NumberOfVehicles_lane  = m_NumberOfTimeSteps / 20;  // 2 second per vehicle at most


	int t;
	m_VehicleSnapshotAry_Lane = Allocate3DDynamicArray<float>(_MAX_LANE_SIZE,m_NumberOfVehicles_lane,m_NumberOfTimeSteps+1);

	m_VehicleStartTimeAry_Lane = AllocateDynamicArray<int>(_MAX_LANE_SIZE,m_NumberOfVehicles_lane);
	m_VehicleEndTimeAry_Lane = AllocateDynamicArray<int>(_MAX_LANE_SIZE,m_NumberOfVehicles_lane);


	for(int LaneID=0; LaneID<_MAX_LANE_SIZE; LaneID++)
	{
		// find out the first vehicle on this lane

		int v;
		float FirstTimeStamp  = 999999;
		int FirstVehicleID = 0;
		for( v=0; v<m_NumberOfVehicles; v+=1)
		{
			for(t= m_VehicleDataList[v].StartTime; t<=m_VehicleDataList[v].EndTime; t+=1)  
			{
				if(m_VehicleSnapshotAry[v][t].LaneID ==LaneID+1)  //LaneID starts from 0
				{
					if(t < FirstTimeStamp)
					{
						FirstTimeStamp = t;
						FirstVehicleID = v;

					}
				}

			}
		}

		// record vehicle trajectory

		// for leader vehicle, initialize
		StartTimeLane[LaneID] = -1; 
		EndTimeLane[LaneID] = -1;


		for(t= m_VehicleDataList[FirstVehicleID].StartTime; t<=m_VehicleDataList[FirstVehicleID].EndTime; t+=1)  
		{

			if(StartTimeLane[LaneID] != -1 && m_VehicleSnapshotAry[FirstVehicleID][t].LaneID !=LaneID+1)
			{
				break; // switch to another link!
			}

			if(m_VehicleSnapshotAry[FirstVehicleID][t].LaneID ==LaneID+1)  //LaneID starts from 0
			{

				if(StartTimeLane[LaneID] == -1) // has not been initialized
				{
					StartTimeLane[LaneID] = t;
					StartLocalYLane[LaneID] = m_VehicleSnapshotAry[FirstVehicleID][t].LocalY;
				}

				EndTimeLane[LaneID] = t;  // keep assinging time stamp value
				EndLocalYLane[LaneID] = m_VehicleSnapshotAry[FirstVehicleID][t].LocalY;

				// record trajactory for the first vehicle
				m_VehicleSnapshotAry_Lane[LaneID][0][t] = m_VehicleSnapshotAry[FirstVehicleID][t].LocalY ; 
				// simulated data array (lane by lane)     original data array

			}
		}

		// set up the position for upstream and downstream (single) loop detectors, can be extended to double loops
		StartLocalYLane[LaneID] += 50.0f; // move up by 50 feet
		EndLocalYLane[LaneID] -= 50.0f; // move down by 50 feet


		// find passing time stamps of vehicles

		std::vector<int> StartPassingTimeStamp; 
		std::vector<int> EndPassingTimeStamp;

		for( v=0; v<m_NumberOfVehicles; v+=1)
		{
			for(t= m_VehicleDataList[v].StartTime; t<m_VehicleDataList[v].EndTime; t+=1)  
			{
				if(m_VehicleSnapshotAry[v][t].LaneID ==LaneID+1)  //LaneID starts from 0
				{

					if(m_VehicleSnapshotAry[v][t].LocalY <= StartLocalYLane[LaneID] && StartLocalYLane [LaneID]< m_VehicleSnapshotAry[v][t+1].LocalY )
					{
						StartPassingTimeStamp.push_back(t);
					}

					if(m_VehicleSnapshotAry[v][t].LocalY <= EndLocalYLane [LaneID] && EndLocalYLane [LaneID] < m_VehicleSnapshotAry[v][t+1].LocalY )
					{
						EndPassingTimeStamp.push_back(t);
					}

				}

			}
		}

		std::sort(StartPassingTimeStamp.begin(), StartPassingTimeStamp.end(), compare_timestamp);
		std::sort(EndPassingTimeStamp.begin(), EndPassingTimeStamp.end(), compare_timestamp);

		// here we assume the first leader vehicle is 0

		VehicleSizeLane[LaneID] = min(StartPassingTimeStamp.size (),EndPassingTimeStamp.size());
		for (v = 0; v < VehicleSizeLane[LaneID]; v++)  // 1 starts from the first follower.
		{
			m_VehicleStartTimeAry_Lane[LaneID][v] = StartPassingTimeStamp[v];
			m_VehicleEndTimeAry_Lane[LaneID][v] = EndPassingTimeStamp[v];
			TRACE("no.%d, %d, %d\n", v, StartPassingTimeStamp[v], EndPassingTimeStamp[v]); 
		}	

		// construct cumuluative count,
		// apply stochatic three detector theorem. 
		// reconstruct arrival and departure time sequences
		// run traffic simulation


		float m_FreeflowMoveDistance = m_FreeflowSpeed *5280 / 36000;  // 60 mph as free-flow speed, 5280 feet per mile, 3600 second with 0.1 seconds as time step

		for (v = 1; v < VehicleSizeLane[LaneID]; v++)  // start from vehicle 1 as the first follower
		{
			for(t= StartPassingTimeStamp[v]; t<= EndPassingTimeStamp[v]; t++) 
			{

				//calculate free-flow position
				//xiF(t) = xi(t-τ) + vf(τ)

				if(t==StartPassingTimeStamp[v] )
				{
					m_VehicleSnapshotAry_Lane[LaneID][v][t] =  StartLocalYLane[LaneID];  // assign starting position
					continue;
				}
				else
				{
					m_VehicleSnapshotAry_Lane[LaneID][v][t] = m_VehicleSnapshotAry_Lane[LaneID][v][t-1] +  m_FreeflowMoveDistance;
				}

				// if this vehicle exceeds the downstream sensor location, needs to stop (wait!)
				if( m_VehicleSnapshotAry_Lane[LaneID][v][t] > EndLocalYLane [LaneID] )
					m_VehicleSnapshotAry_Lane[LaneID][v][t] = EndLocalYLane [LaneID] ;

				// calculate congested position

				//τ = 1/(wkj)
				// δ = 1/kj
				float time_tau_in_time_step  = 1 /(m_WaveSpeed*m_KJam)*36000; // to 0.1 second  
				float spacing = 1.0f/m_KJam*5280;   // to feet
				//xiC(t) = xi-1(t-τ) - δ
				int time_t_minus_tau = t - int(time_tau_in_time_step); // need to convert time in second to time in simulation time interval

				if(time_t_minus_tau >=StartPassingTimeStamp[v-1])  // the leader has not reached destination yet
				{
					float CongestedDistance = max(StartLocalYLane[LaneID], m_VehicleSnapshotAry_Lane[LaneID][v-1][time_t_minus_tau]  - spacing); 
					// max (upsteram sensor location,) makes sure the congested distance is at least upsteram sensor location.
					// v-1 is the leader of the current vehicle, time_t_minus_tau is the lagged timestamp
					// xi(t) = min(xAF(t), xAC(t))
					if (m_VehicleSnapshotAry_Lane[LaneID][v][t]  > CongestedDistance && CongestedDistance >= m_VehicleSnapshotAry_Lane[LaneID][v][t-1])
						m_VehicleSnapshotAry_Lane[LaneID][v][t] = CongestedDistance;


				}

			}
		}
	}
	m_bShowSimulationDataFlag = true;

}

void  CTimeSpaceView::UpdateVehicleStartANDEndTimes(std::vector<int> &StartPassingTimeStamp, std::vector<int> &EndPassingTimeStamp)
{
	// construct cumuluative count, A, D. 
	// apply stochatic three detector theorem  to get A' and D'
	// apply deterministic detector theorem again to get the perfect A and D
	// reconstruct arrival and departure time sequences

}

void CTimeSpaceView::OnNgsimdataShowsimulatedresults()
{
	m_bShowSimulationDataFlag = ! m_bShowSimulationDataFlag;
	Invalidate();
}

void CTimeSpaceView::OnUpdateNgsimdataShowsimulatedresults(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowSimulationDataFlag);
}

void CTimeSpaceView::OnUpdateNgsimdataShowbothobservedandsimulationresults(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowBothResultsFlag);
}

void CTimeSpaceView::OnNgsimdataShowbothobservedandsimulationresults()
{
	m_bShowBothResultsFlag = !m_bShowBothResultsFlag;
	Invalidate();
}

void CTimeSpaceView::OnNgsimdataChangesettingsofcarfollowingmodel()
{

}

void CTimeSpaceView::OnNgsimdataCarfollowingdataextraction()
{
	CString str;
	CFileDialog dlg (FALSE, "*.csv", "*.csv",OFN_HIDEREADONLY | OFN_NOREADONLYRETURN | OFN_LONGNAMES,
		"(*.csv)|*.csv||", NULL);
	if(dlg.DoModal() == IDOK)
	{
		char fname[_MAX_PATH];
		wsprintf(fname,"%s", dlg.GetPathName());
		CWaitCursor wait;

		if(!ExportDataToFile(fname,1))
		{
			str.Format("The file %s could not be opened.\nPlease check if it is opened by Excel.", fname);
			AfxMessageBox(str);
		}

	}

}

bool CTimeSpaceView::ExportDataToFile(char csv_file[_MAX_PATH], int DataFlag)
{
	FILE* st;
	fopen_s(&st,csv_file,"w");

	m_VehicleNoVector_Lane.clear ();
	int v;
	for( v=0; v<m_NumberOfVehicles; v+=1)
	{
		float StartTime = m_VehicleDataList[v].StartTime;

		if(StartTime < m_TimeLeft)
			StartTime = m_TimeLeft;

		float EndTime = m_VehicleDataList[v].EndTime;

		if(EndTime > m_TimeRight)
			EndTime = m_TimeRight;

		if(StartTime< EndTime)
		{
			for(int t= StartTime; t<=EndTime; t+=1)  
			{

				if(m_VehicleSnapshotAry[v][t].LaneID ==m_SelectLaneID)
				{

					m_VehicleNoVector_Lane.push_back (v);   // add vehicle no v into the vector to be searched
					break;

				}
			}
		}

	}


	if(st!=NULL)
	{
		CWaitCursor wc;
		fprintf(st, "vehicle_id, vehicle_type,start_lane,time stamp (1/10 sec),local_y (feet),calculated speed, speed, acceleration, preceeding_vehicle_id,local_y,calculated speed,speed,acceleration,following_vehicle_id,local_y,calculated speed,speed,acceleration");

		int prev_leader_no = -1;
		int prev_follower_no = -1;

		for(v=0; v<m_NumberOfVehicles; v+=1)
		{
			bool bIniFlag  = false;
			float StartTime = m_VehicleDataList[v].StartTime;

			if(StartTime < m_TimeLeft)
				StartTime = m_TimeLeft;

			float EndTime = m_VehicleDataList[v].EndTime;

			if(EndTime > m_TimeRight)
				EndTime = m_TimeRight;

			if(StartTime< EndTime)
			{
				for(int t= StartTime; t<=EndTime; t+=1)  
				{

					if(m_VehicleSnapshotAry[v][t].LaneID ==m_SelectLaneID)
					{

						if( bIniFlag == false)
						{
							fprintf(st,"\n%d,%d,%d,",m_VehicleDataList[v].VehicleID,m_VehicleDataList[v].VehicleType,m_SelectLaneID );
							bIniFlag = true;

						}else
						{
							fprintf(st,",,,");

						}

						//time,local_y,calculated speed, speed
						fprintf(st,"%d,%9.3f,%9.3f,%9.3f,",t,m_VehicleSnapshotAry[v][t].LocalY, m_VehicleSnapshotAry[v][t].CalculatedSpeed_mph , m_VehicleSnapshotAry[v][t].Speed_mph); 

						//acceleration
						if(t>StartTime)
							fprintf(st,"%9.3f,",m_VehicleSnapshotAry[v][t].Speed_mph - m_VehicleSnapshotAry[v][t-1].Speed_mph); 
						else // t== StartTime
							fprintf(st,"%9.3f,",0.0f); 


						int leader_no = FindPreceedingVehicleNo(m_SelectLaneID,m_VehicleSnapshotAry[v][t].LocalY,t);
						//time,local_y,calculated speed, speed
						if(leader_no>=0)
						{
							fprintf(st,"%d,%9.3f,%9.3f,%9.3f,",m_VehicleDataList[leader_no].VehicleID ,
								m_VehicleSnapshotAry[leader_no][t].LocalY,
								m_VehicleSnapshotAry[leader_no][t].CalculatedSpeed_mph ,
								m_VehicleSnapshotAry[leader_no][t].Speed_mph); 
							if(prev_leader_no == leader_no)
								fprintf(st,"%9.3f,",m_VehicleSnapshotAry[leader_no][t].Speed_mph - m_VehicleSnapshotAry[leader_no][t-1].Speed_mph); 
							else
								fprintf(st,"0.0,");
						}else
						{
							fprintf(st,"-1,0,0,0,",0.0f); 

						}

						prev_leader_no = leader_no;

						int follower_no = FindFollowingVehicleNo(m_SelectLaneID,m_VehicleSnapshotAry[v][t].LocalY,t);
						//time,local_y,speed
						if(follower_no>=0)
						{
							fprintf(st,"%d,%9.3f,%9.3f,%9.3f,",m_VehicleDataList[follower_no].VehicleID ,
								m_VehicleSnapshotAry[follower_no][t].LocalY,
								m_VehicleSnapshotAry[follower_no][t].CalculatedSpeed_mph , 
								m_VehicleSnapshotAry[follower_no][t].Speed_mph); 
							if(prev_follower_no ==follower_no ) 
								fprintf(st,"%9.3f\n",m_VehicleSnapshotAry[follower_no][t].Speed_mph - m_VehicleSnapshotAry[follower_no][t-1].Speed_mph); 
							else
								fprintf(st,"0\n"); 

						}else
						{
							fprintf(st,"-1,0,0,0\n",0.0f); 

						}

						prev_follower_no = follower_no;



					}


				}

			}

		}

		fclose(st);
		return true;
	}
	return false;
}

void CTimeSpaceView::OnNgsimdataShowvehicleid()
{
	m_bShowVehicleIDFlag = !m_bShowVehicleIDFlag;
	Invalidate();
}

void CTimeSpaceView::OnUpdateNgsimdataShowvehicleid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowVehicleIDFlag);

}

void CTimeSpaceView::OnNgsimdataShowpreceedingvehicleposition()
{
	m_bShowLeaderPositionFlag = !m_bShowLeaderPositionFlag;
	Invalidate();
}

void CTimeSpaceView::OnUpdateNgsimdataShowpreceedingvehicleposition(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowLeaderPositionFlag);
}


int CTimeSpaceView::FindPreceedingVehicleNo(int LaneNo,float LocalY, int t)
{
	unsigned int vi;
	int leader_no = -1;
	float local_y_leader = 9999999;
	for( vi=0; vi<m_VehicleNoVector_Lane.size(); vi+=1)
	{
		int v = m_VehicleNoVector_Lane[vi];
		float StartTime = m_VehicleDataList[v].StartTime;

		if(StartTime < m_TimeLeft)
			StartTime = m_TimeLeft;

		float EndTime = m_VehicleDataList[v].EndTime;

		if(EndTime > m_TimeRight)
			EndTime = m_TimeRight;

		if(StartTime<= t && t <= EndTime && m_VehicleSnapshotAry[v][t].LaneID ==m_SelectLaneID && m_VehicleSnapshotAry[v][t].LocalY > LocalY + 0.1)
		{
			if(m_VehicleSnapshotAry[v][t].LocalY < local_y_leader )  // taking minimum
			{
				local_y_leader = m_VehicleSnapshotAry[v][t].LocalY ;
				leader_no = v;

			}

		}
	}

	return leader_no;
}

int CTimeSpaceView::FindFollowingVehicleNo(int LaneNo,float LocalY, int t)
{
	int vi;
	int follower_no = -1;
	float local_y_follower = 0;
	for( vi=0; vi<m_VehicleNoVector_Lane.size(); vi+=1)
	{
		int v = m_VehicleNoVector_Lane[vi];
		float StartTime = m_VehicleDataList[v].StartTime;

		if(StartTime < m_TimeLeft)
			StartTime = m_TimeLeft;

		float EndTime = m_VehicleDataList[v].EndTime;

		if(EndTime > m_TimeRight)
			EndTime = m_TimeRight;

		if(StartTime<= t && t <= EndTime && m_VehicleSnapshotAry[v][t].LaneID ==m_SelectLaneID && m_VehicleSnapshotAry[v][t].LocalY < LocalY - 0.1)
		{
			if(m_VehicleSnapshotAry[v][t].LocalY > local_y_follower )  // taking maximum
			{
				local_y_follower = m_VehicleSnapshotAry[v][t].LocalY ;
				follower_no = v;

			}

		}
	}

	return follower_no;
}

void CTimeSpaceView::OnNgsimdataSearchByVehicleId()
{
	//CDlg_Find_Vehicle dlg;
	//if(dlg.DoModal () == IDOK)
	//{
	//	m_SelectedVehicleID = dlg.m_VehicleID;
	//	Invalidate();	
	//}

}


void  CTimeSpaceView::DrawTemporalLink(CPoint start_point, CPoint end_point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);

	dc.SelectObject(&g_TempLinkPen2);
	// Select drawing mode

	int oldROP2 = dc.SetROP2(R2_XORPEN);

	// Draw the line
	dc.MoveTo(start_point);
	dc.LineTo(end_point);

	// Reset drawing mode
	dc.SetROP2(oldROP2);

}


int CTimeSpaceView::CountVehicles(int SelectedStartTime, int SelectedEndTime, float SelectedStartLocalY,float SelectedEndLocalY)
{
	m_SelectedVehicleCount = 0;

	TRACE("----");

	if(SelectedStartTime > SelectedEndTime)  //swap timestamps and locations
	{
		int temp = 	SelectedStartTime;
		SelectedStartTime = SelectedEndTime;
		SelectedEndTime = temp;

		float temp_y = SelectedStartLocalY;
		SelectedStartLocalY = SelectedEndLocalY;
		SelectedEndLocalY = temp_y;
	}


	for(int v=0; v<m_NumberOfVehicles; v+=1)
	{ 
		bool intersection_flag = false;  // default value: false

		float current_local_y ;
		float previous_local_y = -1;
		for(int t = SelectedStartTime; t<= SelectedEndTime; t++)  // first loop
		{
			current_local_y  = SelectedStartLocalY + (float)(t-SelectedStartTime)/max(1,	SelectedEndTime-SelectedStartTime)*(SelectedEndLocalY - SelectedStartLocalY); 

			if(previous_local_y < 0 )  // initialization
			{

				if(SelectedStartTime == SelectedEndTime)  // at the same time, special condition applied here
				{
					previous_local_y =  SelectedEndLocalY;
				}else
				{
				previous_local_y =  SelectedStartLocalY;
				}
			}

			float i_x = 0;
			float i_y = 0;

			if( m_VehicleDataList[v].StartTime < t && m_VehicleDataList[v].EndTime >t 
				&& m_VehicleSnapshotAry[v][t].LaneID == m_SelectLaneID )
			{
				int intersection_test = g_get_line_intersection(t-1,previous_local_y,t , current_local_y,
					t-1, m_VehicleSnapshotAry[v][t-1].LocalY, t, m_VehicleSnapshotAry[v][t].LocalY, &i_x, &i_y) ;

				if(intersection_test >0)
				{
					intersection_flag = true;
					break;

				}  //
			}
		}
		if(intersection_flag)
		{
			m_SelectedVehicleCount ++;
		}

	}  // for each vehicle

	return m_SelectedVehicleCount;
}

void CTimeSpaceView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

	CountVehicles(m_SelectedStartTime,m_SelectedEndTime, m_SelectedStartLocalY,m_SelectedEndLocalY);
}
void CTimeSpaceView::ConstructCellBasedDensityProfile()
{
	float SensorSpacing = 300.0f;  // 0.1 mile
	// step 1: point sensor data

	float boundary_offset = 50;
	float DetectorLocalY;
	for(DetectorLocalY = m_LocalYLowerBound + boundary_offset; DetectorLocalY <= m_LocalYUpperBound - boundary_offset; DetectorLocalY+= SensorSpacing) // - 50 and +50 at boundary.
	{
		// find passing time stamps of vehicles

		std::vector<int> StartPassingTimeStamp; 
		std::vector<int> EndPassingTimeStamp;

		for(int v=0; v< m_NumberOfVehicles; v+=1)  // for all vehicles
		{
			for(int t= m_VehicleDataList[v].StartTime; t<m_VehicleDataList[v].EndTime; t+=1)  // for all valid timestamp
			{
				if(m_VehicleSnapshotAry[v][t].LaneID ==m_SelectLaneID)   // for passing lane
				{
					if(m_VehicleSnapshotAry[v][t].LocalY <= DetectorLocalY && DetectorLocalY < m_VehicleSnapshotAry[v][t+1].LocalY )
					{
						StartPassingTimeStamp.push_back(t);
					}
				}
			}
		}
		std::sort(StartPassingTimeStamp.begin(), StartPassingTimeStamp.end(), compare_timestamp);

		PointSensorData point_sensor;

		point_sensor.DetectorLocalY = DetectorLocalY;
		point_sensor.PassingTimeStampVector = StartPassingTimeStamp;

		int data_collection_time_interval_in_sec = 30;
		int number_of_time_intervals = m_NumberOfTimeSteps / data_collection_time_interval_in_sec/10;
		point_sensor.ConstructCumulativeFlowVector(data_collection_time_interval_in_sec,number_of_time_intervals);

		m_CorridorSensorData.DataCollectionTimeInterval_in_sec = data_collection_time_interval_in_sec;
		m_CorridorSensorData.PointSensorDataVector.push_back(point_sensor);

	}

	// step 2: construct link sensor data

	 m_CorridorSensorData.CreateLinkSensorDataFromPointSensorVector();

	//step 3: scan image to count vehicles
	for (int iLink = 0; iLink < m_CorridorSensorData.LinkSensorDataVector.size(); iLink++)
	{

		LinkSensorData element = m_CorridorSensorData.LinkSensorDataVector[iLink];
		for(int t = 0; t < element.SpaceScanCountVector .size(); t++)
		{
			int timestamp  = t* element.DataCollectionTimeInterval_in_sec*10;
			int count = CountVehicles(timestamp,timestamp, element.DetectorLocalY_Downstream,element.DetectorLocalY_Upstream);
			m_CorridorSensorData.LinkSensorDataVector[iLink].SpaceScanCountVector[t] = count;
			if(count)
				TRACE("");

			m_CorridorSensorData.LinkSensorDataVector[iLink].DensityVector[t] = count*1.0/element.DetectorSpacing_;

		}

	}

	// construct downstream cumuative flow counts
	m_CorridorSensorData.LocalY_Upstream = m_LocalYLowerBound + boundary_offset;
	m_CorridorSensorData.LocalY_Downstream = m_LocalYUpperBound - boundary_offset;

	std::vector<int> PassingTimeStamp;

	DetectorLocalY = m_CorridorSensorData.LocalY_Downstream;
		for(int v=0; v< m_NumberOfVehicles; v+=1)  // for all vehicles
		{
			for(int t= m_VehicleDataList[v].StartTime; t<m_VehicleDataList[v].EndTime; t+=1)  // for all valid timestamp
			{
				if(m_VehicleSnapshotAry[v][t].LaneID ==m_SelectLaneID)   // for passing lane
				{
					if(m_VehicleSnapshotAry[v][t].LocalY <= DetectorLocalY && DetectorLocalY < m_VehicleSnapshotAry[v][t+1].LocalY )
					{
						PassingTimeStamp.push_back(t);
					}
				}
			}
		}
		std::sort(PassingTimeStamp.begin(), PassingTimeStamp.end(), compare_timestamp);
 
		m_CorridorSensorData.ConstructCumulativeFlowCount(PassingTimeStamp,m_NumberOfTimeSteps/10);
  
		m_CorridorSensorData.EstimateSpaceScanCountVectorForAllLinks();


}
void CTimeSpaceView::OnNgsimmenuExportcumulativeflowcountdata()
{
	ConstructCellBasedDensityProfile();
}

void CTimeSpaceView::OnNgsimmenuShowcalculatedcumulativeflowcountanddensity()
{
	m_bShowCumulativeFlowCount = !m_bShowCumulativeFlowCount;
	Invalidate();
}

void CTimeSpaceView::OnUpdateNgsimmenuShowcalculatedcumulativeflowcountanddensity(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowCumulativeFlowCount);
}

void CTimeSpaceView::OnNgsimmenuShowspacetimeContour()
{
	m_bShowDensityContour = !m_bShowDensityContour;
	Invalidate();
}

void CTimeSpaceView::OnUpdateNgsimmenuShowspacetimeContour(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowDensityContour);
}


void CTimeSpaceView::DrawPDFPlot(CDC* pDC,CRect PlotRect, std::vector<int> ValueVector1, std::vector<int> ValueVector2, int MaxBinValue, int BinSize, int MaxFrequency)
{

	std::vector<int> FrequencyCount1;
	std::vector<int> FrequencyCount2;

	
	unsigned int i;
	// initialization
	for(i = 0; i<= BinSize; i++)
	{
		FrequencyCount1.push_back (0);
		FrequencyCount2.push_back (0);
	
	}

	unsigned int j;
	int bin_interval  = MaxBinValue / BinSize +1;

	for(j = 0; j < ValueVector1.size(); j++)
	{
	 int bin_no = ValueVector1[j] / bin_interval;
	 if(bin_no < FrequencyCount1.size())
	 {
	 FrequencyCount1[bin_no]++;  // count frequency
	 }
	}

	for(j = 0; j < ValueVector2.size(); j++)
	{
	 int bin_no = ValueVector2[j] / bin_interval;

	 if(bin_no < FrequencyCount2.size())
	 {
		 FrequencyCount2[bin_no]++;  // count frequency
	 }
	}


	// draw figures;

	pDC->FillSolidRect (PlotRect,RGB(192,192,192));  //grey brush
	

	float x_axis_unit =  PlotRect.Width ()*1.0f/ MaxBinValue;
	float y_axis_unit =  PlotRect.Height() *1.0f/ MaxFrequency;

	int k;

	CPen Ground_truth_Pen(PS_SOLID,1,RGB(255,0,0));
	CPen Estimated_Pen(PS_SOLID,1,RGB(0,0,255));

	pDC->SelectObject (&Ground_truth_Pen);
	for(k = 0; k <=BinSize; k++)
	{
	 int x =  PlotRect.left + k*1.0f*x_axis_unit;
	 int y =  PlotRect.bottom - FrequencyCount1[k]*y_axis_unit;

	 if(k==0)
	 {
		 pDC->MoveTo (x,y);
	 }else
	 {
		 pDC->LineTo (x,y);
	 }

	}

	pDC->SelectObject (&Estimated_Pen);

	for(k = 0; k <=BinSize; k++)
	{
	 int x =  PlotRect.left + k*1.0f*x_axis_unit;
	 int y =  PlotRect.bottom - FrequencyCount2[k]*y_axis_unit;

	 if(k==0)
	 {
		 pDC->MoveTo (x,y);
	 }else
	 {
		 pDC->LineTo (x,y);
	 }

	}


}

void CTimeSpaceView::SetupMaxFrequencyAndBinValue()
{
	// setup m_MaxBinValue
	m_MaxFrequency  = 0;
	m_MaxBinValue = 100;  // manual setup

	unsigned int iLink;
/*	
	for (iLink = 0; iLink < m_CorridorSensorData.LinkSensorDataVector.size(); iLink++)
	{
	std::vector<int> ValueVector1;
	
	ValueVector1 = m_CorridorSensorData.PointSensorDataVector [iLink].PassingHeadwayVector ;  // upstream sensor

	std::sort(ValueVector1.begin(), ValueVector1.end());
	m_MaxBinValue = max(m_MaxBinValue,ValueVector1[ValueVector1.size()-1]);

	}
*/
	// setup m_MaxFrequency
	for (iLink = 0; iLink < m_CorridorSensorData.LinkSensorDataVector.size(); iLink++)
	{
	unsigned int i;
	std::vector<int> FrequencyCount1;

	// initialization
	for(i = 0; i<= m_BinSize; i++)
	{
		FrequencyCount1.push_back (0);
	}

	int bin_interval  = m_MaxBinValue / m_BinSize+1;

	std::vector<int> ValueVector1;
	
	ValueVector1 = m_CorridorSensorData.PointSensorDataVector [iLink].PassingHeadwayVector ;  // upstream sensor

	unsigned int j;
	for(j = 0; j < ValueVector1.size(); j++)
	{

	 int bin_no = ValueVector1[j] / bin_interval;

		 if(bin_no < FrequencyCount1.size())
		 {
		 FrequencyCount1[bin_no]++;  // count frequency
		 }else
		 {
//		 FrequencyCount1[FrequencyCount1.size()-1]++;  // count frequency for out of bound value
		 }
	}


	std::sort(FrequencyCount1.begin(), FrequencyCount1.end());
	
	m_MaxFrequency = max(m_MaxFrequency, FrequencyCount1[FrequencyCount1.size()-1]);

	}
	
}
void CTimeSpaceView::OnNgsimmenuShowcdfplot()
{
	m_bShowPDFPlot = !m_bShowPDFPlot;
	Invalidate();
}

void CTimeSpaceView::OnUpdateNgsimmenuShowcdfplot(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bShowPDFPlot);
	
}


