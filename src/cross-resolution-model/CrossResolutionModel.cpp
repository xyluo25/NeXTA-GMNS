//  Portions Copyright 2010 Tao Xing (captainxingtao@gmail.com), Xuesong Zhou (xzhou99@gmail.com)
//

//   If you help write or modify the code, please also list your names here.
//   The reason of having Copyright info here is to ensure all the modified version, as a whole, under the GPL 
//   and further prevent a violation of the GPL.

// More about "How to use GNU licenses for your own software"
// http://www.gnu.org/licenses/gpl-howto.html


//    This file is part of NeXTA Version 3 (Open-source).

//    NEXTA is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    NEXTA is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with NEXTA.  If not, see <http://www.gnu.org/licenses/>.

#include "stdafx.h"
#include "..//Geometry.h"
#include "..//CSVParser.h"
#include "..//TLite.h"
#include "..//Network.h"
#include "..//TLiteDoc.h"
#ifndef _WIN64
#include "..//Data-Interface//include//ogrsf_frmts.h"
#endif 

#include "..//MainFrm.h"

#include "SignalNode.h"


extern CString g_GetExcelColumnFromNumber(int column);

static string g_DTA_movement_column_name[DTA_SIG_MOVEMENT_MAX_COLUMN] = {"node_id","scenario_no","start_day_no","end_day_no",
"start_time_in_min","end_time_in_min","notes",
"record_name",	"reference_id", 
"NBL2", "NBL","NBT","NBR","NBR2",
"SBL2","SBL","SBT","SBR","SBR2",
"EBL2","EBL","EBT","EBR","EBR2",
"WBL2","WBL","WBT","WBR","WBR2",
"NEL","NET","NER",
"NWL","NWT","NWR",
"SEL","SET","SER",
"SWL","SWT","SWR",
"PED","HOLD",
"geometry"};

static int g_DTA_movement_default_phase[DTA_SIG_MOVEMENT_MAX_COLUMN] = {0,0,0,0,
0,0,0,0,0,
7,7,4,4,4,
3,3,8,8,8,
1,1,6,6,6,
5,5,2,2,2,
7,4,4,
1,6,6,
3,8,8,
5,2,2};

static int g_DTA_phase_default_BRP[17] = {111,112,211,212,121,122,221,222,311,312,411,412,321,322,421,422};


static int g_DTA_default_split_in_seconds[9] = 
{0, 10,21,10,21,10,21,10,21};


static string g_DTA_movement_row_name[DTA_MOVEMENT_ATTRIBUTE_MAX_ROW] = {"Up Node","Dest Node","StreetName","TurnVolume","Lanes","Shared","Width","Storage","StLanes","Grade","Speed","FirstDetect","LastDetect",
"Phase1","PermPhase1","DetectPhase1","EffectiveGreen", "Capacity","V/C","Delay","LOS","IdealFlow","LostTime","SatFlowRatePerLaneGroup","SatFlowPerm","SatFlowRTOR","HeadwayFact","Volume","Peds","Bicycles","PHF","Growth",
"HeavyAgents","BusStops","Midblock","Distance","TravelTime","Street Name"};


////  phase data 
static string g_DTA_phasing_column_name[DTA_SIG_PHASE_MAX_COLUMN] = {"timing_plan_name", "node_id",
"key","value",
"D1","D2","D3","D4",
"D5","D6","D7","D8", "D9","D10",
"D11","D12","D13","D14",
"D15","D16","D17","D18",
"P1","P2","P3","P4","P5","P6","P7","P8","geometry"};


static string g_DTA_phasing_row_name[DTA_PHASE_ATTRIBUTE_MAX_ROW] = {
	"GreenDuration",
	"NextPhaseNo",
	"MovementVector"
};

void DTA_Movement_Data_Matrix::InitializeKeyValue(int node_id, string geometry_str)
{
	for(int ii = 0; ii < DTA_MOVEMENT_ATTRIBUTE_MAX_ROW; ii++)
	{
		DTA_SIG_MOVEMENT_ROW i = (DTA_SIG_MOVEMENT_ROW)ii;
		SetValue(DTA_SIG_MOVEMENT_NODE_ID,  i, node_id);

		SetValue(DTA_SIG_MOVEMENT_REFERENCE_ID,  i, node_id);

		SetValue(DTA_SIG_MOVEMENT_SCENARIO_NO,  i, 1);
		SetValue(DTA_SIG_MOVEMENT_START_DAY_NO,  i, 1);
		SetValue(DTA_SIG_MOVEMENT_END_DAY_NO,  i, 100);

		SetValue(DTA_SIG_MOVEMENT_START_MIN_IN_SECOND,  i, 0);
		SetValue(DTA_SIG_MOVEMENT_END_MIN_IN_SECOND,  i, 1440);

		SetString(DTA_SIG_MOVEMENT_RECORD_NAME, i, g_DTA_movement_row_name[i]);
		SetString(DTA_SIG_MOVEMENT_GEOMETRY, i, geometry_str);


	}
}
void DTA_Phasing_Data_Matrix::InitializeKeyValue(int node_id, string geometry_str)
{
	for(int ii = 0; ii < DTA_PHASE_ATTRIBUTE_MAX_ROW; ii++)
	{
		DTA_SIG_PHASE_ROW i = (DTA_SIG_PHASE_ROW)ii;
		SetValue(DTA_SIG_PHASE_NODE_ID,  i, node_id);

		SetString(DTA_SIG_Geometry, i, geometry_str);


	}
}

DTA_Direction CTLiteDoc::Find_Closest_Angle_to_Approach(int angle)
{
	if(angle < 23)
	{
		return DTA_East;
	}else if(angle < 45 && m_ApproachMap.find(DTA_East)== m_ApproachMap.end())  // East has not been used previously
	{
		return DTA_East;
	}
	else if(angle < 68 ) 
	{
		if(m_ApproachMap.find(DTA_North)== m_ApproachMap.end())  //North not used
			return DTA_North;
		else
			return DTA_NorthEast;
	}
	else if(angle < 113) 
	{
		return DTA_North;
	}
	else if(angle < 135) 
	{
		if(m_ApproachMap.find(DTA_North)== m_ApproachMap.end())  //North not used
			return DTA_North;
		else
			return DTA_NorthWest;
	}
	else if(angle < 158) 
	{
		if(m_ApproachMap.find(DTA_West)== m_ApproachMap.end())  //West not used
			return DTA_West;
		else
			return DTA_NorthWest;
	}
	else if(angle < 203) 
	{
		return DTA_West;
	}
	else if(angle < 225 && m_ApproachMap.find(DTA_West)== m_ApproachMap.end())  //West not used
	{
		return DTA_West;
	}
	else if(angle < 248) 
	{
		if(m_ApproachMap.find(DTA_South)== m_ApproachMap.end())  //South not used
			return DTA_South;
		else
			return DTA_SouthWest;
	}
	else if(angle < 293) 
	{
		return DTA_South;
	}
	else if(angle < 315) 
	{
		if(m_ApproachMap.find(DTA_South)== m_ApproachMap.end())  //South not used
			return DTA_South;
		else
			return DTA_SouthEast;
	}
	else if(angle < 338) 
	{
		if(m_ApproachMap.find(DTA_East)== m_ApproachMap.end())  //East not used
			return DTA_East;
		else
			return DTA_SouthEast;
	}
	else
		return DTA_East;
}
DTA_Direction CTLiteDoc::Find_Angle_to_Approach_8_direction(int angle)
{
	if(angle < 23)
	{
		return DTA_West;
	}
	else if(angle < 68) 
	{
		return DTA_SouthWest;
	}
	else if(angle < 113) 
	{
		return DTA_South; 
	}
	else if(angle < 158) 
	{
		return DTA_SouthEast;
	}
	else if(angle < 203) 
	{
		return DTA_East; 
	}
	else if(angle < 248) 
	{
		return DTA_NorthEast; 
	}
	else if(angle < 293) 
	{
		return DTA_North; 
	}
	else if(angle < 338) 
	{
		return DTA_NorthWest; 
	}
	else
		return DTA_West; 
}

DTA_Direction CTLiteDoc::Find_Angle_to_Approach_4_directionWithoutGivenDirection(int angle, DTA_Direction not_use_DTA_Direction)
{

	if(not_use_DTA_Direction== DTA_East)
	{
		if(angle > 180) 
		{
			return DTA_South;
		}else
			return DTA_North;

	}

	if(not_use_DTA_Direction== DTA_West)
	{
		if(angle < 180) 
		{
			return DTA_South;
		}else
			return DTA_North;

	}

	if(not_use_DTA_Direction== DTA_North)
	{
		if(angle < 90 || angle > 270) 
		{
			return DTA_East;
		}else
			return DTA_West;

	}

	if(not_use_DTA_Direction== DTA_South)
	{
		if(angle < 90 || angle > 270) 
		{
			return DTA_East;
		}else
			return DTA_West;

	}

	return DTA_NotDefined;
}

DTA_Direction CTLiteDoc::Find_Angle_to_Approach_4_direction(int angle, int &relative_angel_difference_from_main_direction)
{
	relative_angel_difference_from_main_direction = 0;
	if(angle < 45)
	{
		relative_angel_difference_from_main_direction = abs(angle-0);

		return DTA_East;
	}
	else if(angle < 135) 
	{
		relative_angel_difference_from_main_direction = abs(angle-90);
		return DTA_North;
	}
	else if(angle < 225) 
	{
		relative_angel_difference_from_main_direction = abs(angle-180);
		return DTA_West;
	}
	else if(angle < 315) 
	{
		relative_angel_difference_from_main_direction = abs(angle-270);

		return DTA_South;
	}
	else 
	{
		relative_angel_difference_from_main_direction = abs(angle-360);
		return DTA_East;

	}
}




DTA_Turn CTLiteDoc::Find_RelativeAngle_to_Left_OR_Right_Turn(int relative_angle)
{
	int min_diff = 89;

	int ideal_left = 90;
	int ideal_right = -90;
	int ideal_through = 0;

	if(abs(relative_angle - ideal_left) <= min_diff)
		return DTA_LeftTurn;

	if(abs(relative_angle - ideal_right) <= min_diff)
		return DTA_RightTurn;

	min_diff = 179;
	if(abs(relative_angle - ideal_left) <= min_diff)
		return DTA_LeftTurn;

	if(abs(relative_angle - ideal_right) <= min_diff)
		return DTA_RightTurn;


	return DTA_OtherTurn;
}

DTA_Turn CTLiteDoc::Find_RelativeAngle_to_Turn(int relative_angle)
{
	int min_through_diff = 45;

	int ideal_left = 90;
	int ideal_right = -90;
	int ideal_through = 0;


	if(abs(relative_angle - ideal_through) <= min_through_diff)
		return DTA_Through;

	if(abs(relative_angle - ideal_left) <= min_through_diff)
		return DTA_LeftTurn;

	if(abs(relative_angle - ideal_right) <= min_through_diff)
		return DTA_RightTurn;

	min_through_diff = 89;

	if(abs(relative_angle - ideal_left) <= min_through_diff)
		return DTA_LeftTurn;

	if(abs(relative_angle - ideal_right) <= min_through_diff)
		return DTA_RightTurn;


	return DTA_OtherTurn;
}
double CTLiteDoc::Find_P2P_Distance(GDPoint p1, GDPoint p2)
{
	double delta_x  = p2.x - p1.x;
	double delta_y  = p2.y - p1.y;

	return sqrt(delta_x*delta_x + delta_y* delta_y);


}


int CTLiteDoc::Find_P2P_Angle(GDPoint p1, GDPoint p2)
{
	float delta_x  = p2.x - p1.x;
	float delta_y  = p2.y - p1.y;

	if(fabs(delta_x) < 0.00001)
		delta_x =0;

	if(fabs(delta_y) < 0.00001)
		delta_y =0;

	int angle = atan2(delta_y, delta_x) * 180 / PI + 0.5;
	// angle = 90 - angle;

	while(angle < 0) 
		angle += 360;

	while(angle > 360) 
		angle -= 360;

	return angle;

}
DTA_Turn CTLiteDoc::Find_PPP_to_Turn(GDPoint p1, GDPoint p2, GDPoint p3)
{

	int relative_angle = Find_PPP_RelativeAngle(p1,p2,p3);
	return Find_RelativeAngle_to_Turn(relative_angle);

}

DTA_SIG_MOVEMENT GetMovementIndexFromDirectionAndTurn(DTA_Direction direction , DTA_Turn movement_turn)
{

	DTA_SIG_MOVEMENT  movement_approach_turn = DTA_NBL2;  // default value
	switch (direction)
	{
	case DTA_North:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_NBT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_NBL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_NBR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_NBL2; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_NBR2; break;
		}
		break;
	case DTA_East:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_EBT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_EBL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_EBR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_EBL2; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_EBR2; break;
		}
		break;
	case DTA_South:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_SBT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_SBL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_SBR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_SBL2; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_SBR2; break;
		}
		break;
	case DTA_West:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_WBT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_WBL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_WBR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_WBL2; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_WBR2; break;
		}
		break;
	case DTA_NorthEast:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_NET; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_NEL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_NER; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_NEL; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_NER; break;
		}
		break;
	case DTA_NorthWest:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_NWT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_NWL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_NWR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_NWL; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_NWR; break;
		}
		break;
	case DTA_SouthEast:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_SET; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_SEL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_SER; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_SEL; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_SER; break;
		}
		break;
	case DTA_SouthWest:
		switch (movement_turn)
		{
		case DTA_Through: movement_approach_turn = DTA_SWT; break;
		case DTA_LeftTurn: movement_approach_turn = DTA_SWL; break;
		case DTA_RightTurn: movement_approach_turn = DTA_SWR; break;
		case DTA_LeftTurn2: movement_approach_turn = DTA_SWL; break;
		case DTA_RightTurn2: movement_approach_turn = DTA_SWR; break;
		}
		break;
	}

	return movement_approach_turn;
}


DTA_Turn CTLiteDoc::Find_PPP_to_Turn_with_DTAApproach(GDPoint p1, GDPoint p2, GDPoint p3,DTA_Direction approach1, DTA_Direction approach2)
{
	if(approach2 == m_OpposingDirectionMap[approach1])
		return DTA_Through;
	else
	{
		int relative_angle = Find_PPP_RelativeAngle(p1,p2,p3);
		return Find_RelativeAngle_to_Left_OR_Right_Turn(relative_angle);
	}

}


int CTLiteDoc::Find_PPP_RelativeAngle(GDPoint p1, GDPoint p2, GDPoint p3)
{
	int relative_angle;

	int angle1 = Find_P2P_Angle(p1,p2);
	int angle2 = Find_P2P_Angle(p2,p3);
	relative_angle = angle2 - angle1;

	while(relative_angle > 180) 
		relative_angle -= 360;

	while(relative_angle < -180)
		relative_angle += 360;

	return relative_angle;
}

int CTLiteDoc::FindUniqueLinkID()
{
	int i;
	for( i= 1; i < m_LinkIDRecordVector.size(); i++) 
	{
		if( m_LinkIDRecordVector[i] == 0)
		{
			m_LinkIDRecordVector[i] = 1;
			return i;
		}
	}

	// all link ids have been used;
	m_LinkIDRecordVector.push_back(i);
	m_LinkIDRecordVector[i] = 1;
	return i;
}

void CTLiteDoc::ConstructMovementVector() 
// this function has 8 directions
{ 
	CString SettingsFile;
	SettingsFile = this->m_ProjectFile ;
	int prohibit_non_specified_movement_on_freeway_and_ramp = 1; 

	m_hide_non_specified_movement_on_freeway_and_ramp = 1; //(int)(g_GetPrivateProfileDouble("movement", "hide_non_specified_movement_on_freeway_and_ramp", 1, SettingsFile));


	// input: node, link and geometry
	// output: m_MovementVector for each node

	m_OpposingDirectionMap[DTA_North] = DTA_South;
	m_OpposingDirectionMap[DTA_South] = DTA_North;

	m_OpposingDirectionMap[DTA_East] = DTA_West;
	m_OpposingDirectionMap[DTA_West] =  DTA_East;

	m_OpposingDirectionMap[DTA_NorthEast] =  DTA_SouthWest;
	m_OpposingDirectionMap[DTA_SouthWest] =  DTA_NorthEast;

	m_OpposingDirectionMap[DTA_SouthEast] =  DTA_NorthWest;
	m_OpposingDirectionMap[DTA_NorthWest] =  DTA_SouthEast;

	m_ApproachMap.clear();

	int signal_count = 0;
	// generate all movements
	int i = 0;
	for (std::list<DTANode*>::iterator  iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++, i++)
	{  // for current node

		(*iNode)->m_MovementVector .clear ();

		signal_count ++;
		// step 1: mark movement turn first
		m_ApproachMap.clear();
		for(int inbound_i= 0; inbound_i< (*iNode)->m_IncomingLinkVector.size(); inbound_i++)
		{
			// for each incoming link
			int IncomingLinkNo = (*iNode)->m_IncomingLinkVector[inbound_i];

			if(m_LinkNoMap.find(IncomingLinkNo) != m_LinkNoMap.end())
			{
			DTALink* pIncomingLink = m_LinkNoMap[IncomingLinkNo];

			for(int outbound_i= 0; outbound_i< (*iNode)->m_OutgoingLinkVector.size(); outbound_i++)
			{
				//for each outging link
				//for each outging link
				int OutgoingLinkNo = (*iNode)->m_OutgoingLinkVector[outbound_i];

				if(m_LinkNoMap.find(OutgoingLinkNo) == m_LinkNoMap.end())
					continue;

				DTALink* pOutgoingLink = m_LinkNoMap[OutgoingLinkNo];

				if(pOutgoingLink==NULL)
					continue;



				if (m_LinkNoMap.find(OutgoingLinkNo)!= m_LinkNoMap.end() && pIncomingLink->m_FromNodeID  != pOutgoingLink->m_ToNodeID )
				{
					// do not consider u-turn

					DTANodeMovement element;

					element.in_link_to_node_id = pIncomingLink->m_ToNodeID ;					
					element.IncomingLinkNo = pIncomingLink->m_LinkNo ;
					element.OutgoingLinkNo = pOutgoingLink ->m_LinkNo ;
					element.in_link_from_node_id = pIncomingLink->m_FromNodeID ;		
					element.out_link_to_node_id = pOutgoingLink ->m_ToNodeID ;

					if (pIncomingLink->m_FromNodeNo == 270486 && pIncomingLink->m_ToNodeNo == 281 && pOutgoingLink->m_ToNodeNo == 285)
					{
						TRACE("");
					}								


					GDPoint p1, p2, p3;
					p1  = m_NodeIDMap[element.in_link_from_node_id]->pt;
					p2  = m_NodeIDMap[element.in_link_to_node_id]->pt;
					p3  = m_NodeIDMap[element.out_link_to_node_id]->pt;

					element.movement_direction = Find_Angle_to_Approach_8_direction(Find_P2P_Angle(p1,p2));

					m_ApproachMap[element.movement_direction] = 1;

				}  // not u turn

			} // outgoing link

			}
		}  //incoming link
		// step 2: scan each inbound link and outbound link


		for(int inbound_i= 0; inbound_i< (*iNode)->m_IncomingLinkVector.size(); inbound_i++)
		{
			// for each incoming link
			int IncomingLinkNo = (*iNode)->m_IncomingLinkVector[inbound_i];

			if(m_LinkNoMap.find(IncomingLinkNo) == m_LinkNoMap.end())
				continue;

			DTALink* pIncomingLink = m_LinkNoMap[IncomingLinkNo];

			// pre processing to determine the number of turning movements

			std::map<DTA_Turn,int> MovementCount;
			std::map<DTA_Turn,int> MaxAbsAngelForMovement;

			for(int outbound_i= 0; outbound_i< (*iNode)->m_OutgoingLinkVector.size(); outbound_i++)
			{
				//for each outging link
				//for each outging link
				int OutgoingLinkNo = (*iNode)->m_OutgoingLinkVector[outbound_i];


				if(m_LinkNoMap.find(OutgoingLinkNo) == m_LinkNoMap.end())
					continue;

				DTALink* pOutgoingLink = m_LinkNoMap[OutgoingLinkNo];

				if(pOutgoingLink==NULL)
					continue;

				if (pIncomingLink->m_FromNodeID  != pOutgoingLink->m_ToNodeID )
				{
					// do not consider u-turn

					DTANodeMovement element;

					element.in_link_to_node_id = pIncomingLink->m_ToNodeID ;					
					element.IncomingLinkNo = pIncomingLink->m_LinkNo;
					element.OutgoingLinkNo = pOutgoingLink ->m_LinkNo ;
					element.in_link_from_node_id = pIncomingLink->m_FromNodeID ;		
					element.out_link_to_node_id = pOutgoingLink ->m_ToNodeID ;

					int up_node = m_NodeIDMap[element.in_link_from_node_id]->m_NodeID;

					GDPoint p1, p2, p3;
					p1  = m_NodeIDMap[element.in_link_from_node_id]->pt;
					p2  = m_NodeIDMap[element.in_link_to_node_id]->pt;
					p3  = m_NodeIDMap[element.out_link_to_node_id]->pt;


					// method 1: identify movement approach from scratch 
					element.movement_direction = Find_Closest_Angle_to_Approach(Find_P2P_Angle(p1,p2));
					// method 2: read pre-generated layout file from synchro

					CString str_key;
					str_key.Format("%d,%d",element.in_link_to_node_id, element.in_link_from_node_id);


					element.movement_turn = Find_PPP_to_Turn(p1,p2,p3);

					if (m_PredefinedApproachMap.find(str_key) != m_PredefinedApproachMap.end())  // approach has been predefined in synchro_layout.csv file
					{
						element.movement_direction = m_PredefinedApproachMap[str_key];

						str_key.Format("%d,%d",element.in_link_to_node_id, element.out_link_to_node_id);

						DTA_Direction approach2= m_PredefinedApproachMap[str_key];

						element.movement_turn = Find_PPP_to_Turn_with_DTAApproach(p1,p2,p3,element.movement_direction,approach2);
						MovementCount[element.movement_turn]+=1;

						if(MaxAbsAngelForMovement.find(element.movement_turn) == MaxAbsAngelForMovement.end()) // initialize
						{

							MaxAbsAngelForMovement[element.movement_turn] = abs(Find_PPP_RelativeAngle(p1,p2,p3));
						}else  // with data before
						{
							int relative_angel = abs(Find_PPP_RelativeAngle(p1,p2,p3));

							if(relative_angel > MaxAbsAngelForMovement[element.movement_turn])
								MaxAbsAngelForMovement[element.movement_turn] = relative_angel;

						}

					}

				}
			}  // outgoing

			////////////////////////////////// end of processing /////////////////////////////////////////////// //

			// for each incoming link
			for(int outbound_i= 0; outbound_i< (*iNode)->m_OutgoingLinkVector.size(); outbound_i++)
			{
				int OutgoingLinkNo = (*iNode)->m_OutgoingLinkVector[outbound_i];

				if(m_LinkNoMap.find(OutgoingLinkNo) == m_LinkNoMap.end())
					continue;
				DTALink* pOutgoingLink = m_LinkNoMap[OutgoingLinkNo];

				if (pIncomingLink->m_FromNodeID  != pOutgoingLink->m_ToNodeID )
				{
					// do not consider u-turn

					DTANodeMovement element;


					element.in_link_to_node_id = pIncomingLink->m_ToNodeID ;					
					element.IncomingLinkNo = pIncomingLink->m_LinkNo ;
					element.OutgoingLinkNo = pOutgoingLink ->m_LinkNo ;
					element.in_link_from_node_id = pIncomingLink->m_FromNodeID ;		
					element.out_link_to_node_id = pOutgoingLink ->m_ToNodeID ;

					int up_node = m_NodeIDMap[element.in_link_from_node_id]->m_NodeID;

					if(element.in_link_from_node_id == 49 &&  element.in_link_to_node_id == 14 &&  element.out_link_to_node_id == 20)
					{
						TRACE("Up Node: %d\n",up_node);
					}

					GDPoint p1, p2, p3;
					p1  = m_NodeIDMap[element.in_link_from_node_id]->pt;
					p2  = m_NodeIDMap[element.in_link_to_node_id]->pt;
					p3  = m_NodeIDMap[element.out_link_to_node_id]->pt;


					// method 1: identify movement approach from scratch 
					element.movement_direction = Find_Closest_Angle_to_Approach(Find_P2P_Angle(p1,p2));
					// method 2: read pre-generated layout file from synchro

					CString str_key;
					str_key.Format("%d,%d",element.in_link_to_node_id, element.in_link_from_node_id);


					element.movement_turn = Find_PPP_to_Turn(p1,p2,p3);


					int node_number =  m_NodeIDMap[element.out_link_to_node_id]->m_NodeID ;

					switch (element.movement_turn)
					{
					case DTA_Through: 
						element.QEM_Lanes = pIncomingLink->m_NumberOfLanes;
						break;
					case DTA_LeftTurn: 
						element.QEM_Lanes = pIncomingLink->m_NumberOfLanes;
						break;
					case DTA_LeftTurn2:
						element.QEM_Lanes = pIncomingLink->m_NumberOfLeftTurnLanes ;
						break;

					case DTA_RightTurn:
						element.QEM_Lanes =  pIncomingLink->m_NumberOfLeftTurnLanes ;
						break;

					case DTA_RightTurn2: 
						element.QEM_Lanes = pIncomingLink->m_NumberOfRightTurnLanes ; 
						break;
					}

					element.QEM_Speed  = pIncomingLink->m_FreeSpeed ;



					if (m_PredefinedApproachMap.find(str_key) != m_PredefinedApproachMap.end())  // approach has been predefined in synchro_layout.csv file
					{
						element.movement_direction = m_PredefinedApproachMap[str_key];

						str_key.Format("%d,%d",element.in_link_to_node_id, element.out_link_to_node_id);

						DTA_Direction approach2= m_PredefinedApproachMap[str_key];

						element.movement_turn = Find_PPP_to_Turn_with_DTAApproach(p1,p2,p3,element.movement_direction,approach2);

						if(MovementCount[element.movement_turn] >=2)
						{
							// we have more than 1 movement, use angle to determine movement again

							int relative_angel = abs(Find_PPP_RelativeAngle(p1,p2,p3));

							if(relative_angel == MaxAbsAngelForMovement[element.movement_turn])
							{  // adjust movement

								if(element.movement_turn == DTA_LeftTurn)
									element.movement_turn = DTA_LeftTurn2;

								if(element.movement_turn == DTA_RightTurn)
									element.movement_turn = DTA_RightTurn2;

							}


						}

					}


					// determine  movement type /direction here
					element.movement_approach_turn = DTA_LANES_COLUME_init;
					switch (element.movement_direction)
					{
					case DTA_North:

						
						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_NBT; element.QEM_Phase1 = 2; break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_NBL; element.QEM_PermPhase1 = 2; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_NBR; element.QEM_PermPhase1 = 2; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_NBL2; element.QEM_PermPhase1 = 2; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_NBR2; element.QEM_PermPhase1 = 2; break;
						}
						break;
					case DTA_East:

						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_EBT; element.QEM_Phase1 = 4;  break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_EBL; element.QEM_PermPhase1 = 4; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_EBR; element.QEM_PermPhase1 = 4; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_EBL2; element.QEM_PermPhase1 = 4; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_EBR2; element.QEM_PermPhase1 = 4;  break;
						}
						break;
					case DTA_South:

						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_SBT; element.QEM_Phase1 = 8;  break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_SBL; element.QEM_PermPhase1 = 8;  break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_SBR; element.QEM_PermPhase1 = 8; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_SBL2; element.QEM_PermPhase1 = 8; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_SBR2; element.QEM_PermPhase1 = 8; break;
						}
						break;
					case DTA_West:
						
						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_WBT; element.QEM_Phase1 = 6; break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_WBL; element.QEM_PermPhase1 = 6; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_WBR; element.QEM_PermPhase1 = 6; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_WBL2; element.QEM_PermPhase1 = 6; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_WBR2; element.QEM_PermPhase1 = 6;  break;
						}
						break;
					case DTA_NorthEast:

						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_NET; element.QEM_Phase1 = 2; break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_NEL; element.QEM_PermPhase1 = 2; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_NER; element.QEM_PermPhase1 = 2; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_NEL; element.QEM_PermPhase1 = 2; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_NER; element.QEM_PermPhase1 = 2;  break;
						}
						break;
					case DTA_NorthWest:

						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_NWT; 						element.QEM_Phase1 = 4; break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_NWL; 						element.QEM_PermPhase1 = 4; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_NWR; 						element.QEM_PermPhase1 = 4; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_NWL; 						element.QEM_PermPhase1 = 4; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_NWR; 						element.QEM_PermPhase1 = 4; break;
						}
						break;
					case DTA_SouthEast:

						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_SET; 						element.QEM_Phase1 = 8;  break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_SEL; 						element.QEM_PermPhase1 = 8; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_SER; 						element.QEM_PermPhase1 = 8; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_SEL; 						element.QEM_PermPhase1 = 8; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_SER; 						element.QEM_PermPhase1 = 8; break;
						}
						break;
					case DTA_SouthWest:
						switch (element.movement_turn)
						{
						case DTA_Through: element.movement_approach_turn = DTA_SWT; 						element.QEM_Phase1 = 6;break;
						case DTA_LeftTurn: element.movement_approach_turn = DTA_SWL; 						element.QEM_PermPhase1 = 6; break;
						case DTA_RightTurn: element.movement_approach_turn = DTA_SWR; 						element.QEM_PermPhase1 = 6; break;
						case DTA_LeftTurn2: element.movement_approach_turn = DTA_SWL;						element.QEM_PermPhase1 = 6; break;
						case DTA_RightTurn2: element.movement_approach_turn = DTA_SWR; 						element.QEM_PermPhase1 = 6; break;
						}
						break;
					}

					// rule 1: 

					if(element.movement_turn == DTA_OtherTurn || element.movement_turn == DTA_LeftTurn || element.movement_turn == DTA_RightTurn2 )

					{
						if( (m_LinkTypeMap[pIncomingLink->m_link_type].IsFreeway () &&  m_LinkTypeMap[pOutgoingLink->m_link_type].IsRamp  () ) 
							|| (m_LinkTypeMap[pIncomingLink->m_link_type].IsRamp () &&  m_LinkTypeMap[pOutgoingLink->m_link_type].IsFreeway  ()))
						{

							element.bNonspecifiedTurnDirectionOnFreewayAndRamps = true;



							// do not load the non-specified movement for ramp to freeway or freeway to ramp connection
							element.turning_prohibition_flag = true;

						}
					}

					//rule 2:
					if(m_LinkTypeMap[pIncomingLink->m_link_type].IsRamp () &&  m_LinkTypeMap[pOutgoingLink->m_link_type].IsRamp  ())
					{

						// test if the incoming ramp is connected with a freeway link
						bool bWithConnectedFreewayLink = false;
						DTANode* pNode = m_NodeNoMap[ pIncomingLink->m_ToNodeNo ]; 

						for(int outbound_ii= 0; outbound_ii<pNode->m_OutgoingLinkVector.size(); outbound_ii++)
						{
							int OutgoingLinkNo2 = pNode->m_OutgoingLinkVector[outbound_ii];
							DTALink* pOutgoingLink2 = m_LinkNoMap[OutgoingLinkNo2];

							if(m_LinkTypeMap[pOutgoingLink2->m_link_type].IsFreeway ())
							{
								bWithConnectedFreewayLink = true;
								break;
							}

						}
						// 

						if(bWithConnectedFreewayLink == true)
						{
							element.bNonspecifiedTurnDirectionOnFreewayAndRamps = true;

							// do not load the non-specified movement for ramp to freeway or freeway to ramp connection
							element.turning_prohibition_flag = true;
						}
					}

					//rule 3:

					if( element.movement_turn == DTA_OtherTurn  && m_LinkTypeMap[pIncomingLink->m_link_type].IsFreeway () &&  m_LinkTypeMap[pOutgoingLink->m_link_type].IsFreeway  () ) 
					{

						// test if the outgoing freeway is connected with an incoming ramp
						bool bWithConnectedRampLink = false;
						DTANode* pNode = m_NodeNoMap[ pIncomingLink->m_ToNodeNo ]; 

						for(int inbound_ii= 0; inbound_ii<pNode->m_IncomingLinkVector.size(); inbound_ii++)
						{
							int IncomingLinkNo2 = pNode->m_IncomingLinkVector[inbound_ii];
							DTALink* pIncomingLink2 = m_LinkNoMap[IncomingLinkNo2];

							if(m_LinkTypeMap[pIncomingLink2->m_link_type].IsRamp ())
							{
								bWithConnectedRampLink = true;
								break;
							}

						}
						// 

						if(bWithConnectedRampLink == true)
						{
							element.bNonspecifiedTurnDirectionOnFreewayAndRamps = true;

							// do not load the non-specified movement for ramp to freeway or freeway to ramp connection
							element.turning_prohibition_flag = true;
						}					
					}


					if(element.QEM_EffectiveGreen == 0 && ((*iNode)->m_ControlType == m_ControlType_PretimedSignal ||  (*iNode)->m_ControlType == m_ControlType_ActuatedSignal) && element.turning_prohibition_flag==0) 
						element.QEM_EffectiveGreen = 6;

				
						// create movement vector for all timing plans

					(*iNode)->SetupNodeMovementVector(element);


				}  // for each feasible movement (without U-turn)

			} // for each outbound link

		} // for each inbound link

	}// for each node

	m_ApproachMap.clear();

	  
	m_bMovementAvailableFlag = 1;
}



void CTLiteDoc::Constructandexportsignaldata()
{


	m_Synchro_ProjectDirectory  = m_ProjectDirectory + "Exporting_Synchro_UTDF\\";
	if ( GetFileAttributes(m_Synchro_ProjectDirectory) == INVALID_FILE_ATTRIBUTES) 
	{
		CreateDirectory(m_Synchro_ProjectDirectory,NULL);
	}



	if (m_bMovementAvailableFlag == 0)  // has not been initialized. 
	{
		ConstructMovementVector();
	}
	ExportSynchroVersion6Files();

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetMainWnd();
	ShellExecute( NULL,  "explore", m_Synchro_ProjectDirectory, NULL,  NULL, SW_SHOWNORMAL );




}


void CTLiteDoc::ExportSynchroVersion6Files(std::string TimingPlanName)
{

}

void CTLiteDoc::ExportSingleSynchroFile(CString SynchroProjectFile)
{ 

}


bool CTLiteDoc::ReadSynchroPreGeneratedLayoutFile(LPCTSTR lpszFileName)
{
	const int approach_size = 8;

	string approach_column_name_str[approach_size] = { "NID","SID",	"EID","WID","NEID","NWID","SEID","SWID"};

	// we need to use the opposite direction
	DTA_Direction approach_vector[approach_size] = { 
		DTA_South,
		DTA_North,
		DTA_West,
		DTA_East,
		DTA_SouthWest,
		DTA_SouthEast,
		DTA_NorthWest,
		DTA_NorthEast};

		int approach_node_id[approach_size];

		m_PredefinedApproachMap.clear();

		CCSVParser parser;
		parser.m_bSkipFirstLine  = true;  // skip the first line  
		if (parser.OpenCSVFile(lpszFileName))
		{
			int i=0;
			while(parser.ReadRecord())
			{
				int node_id;
				string name;
				DTANode* pNode = 0;


				if(parser.GetValueByFieldName("INTID",node_id) == false)
					break;

				for(int a = 0; a< approach_size; a++)
				{	
					approach_node_id[a]=0;  // initialization
					bool bField_Exist = parser.GetValueByFieldName(approach_column_name_str[a],approach_node_id[a]);

					if(bField_Exist && approach_node_id[a]>0)
					{
						TRACE("node = %d,%d,%s\n", node_id, approach_node_id[a], approach_column_name_str[a].c_str ());

						CString str_key;
						str_key.Format("%d,%d",node_id, approach_node_id[a]);

						m_PredefinedApproachMap[str_key] = approach_vector[a];

					}

				}


			}

			AfxMessageBox("Synchro_layout.csv file is used to specify the movement vector in QEM and Synchro exporting functions.", MB_ICONINFORMATION);
			return true;
		}
		return false;
}



typedef std::pair<CString, int> LinkProp_Pair;

struct IntCmp 
{

	bool operator()(const LinkProp_Pair &lhs, const LinkProp_Pair &rhs) {
		return lhs.second > rhs.second;
	}
};


void CTLiteDoc::OnDetectorExportlinkflowproportionmatrixtocsvfile()
{

	CWaitCursor wait;
	
	std::string timing_plan_name = "0";
	
	m_LinkFlowProportionMap.clear ();
	m_LinkFlowProportionODMap.clear ();

	if(m_AgentSet.size()>=50000)
	{
		AfxMessageBox("Generating link flow proportion matrix from Agent/agent files might take a while...", MB_ICONINFORMATION);

	}

	int sensor_count = 0;
	for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
	{
		DTALink* pLink = (*iLink);
		if(pLink->m_CountSensorID.size()>0)
		{
			sensor_count ++;
		}
	}

	//if(sensor_count == 0)
	//{
	//	AfxMessageBox("Please input sensor data or specify observed link peak hourly count first, before generating link flow proportion matrix. Currently, there is 0 link with observation data.", MB_ICONINFORMATION);
	//	return;
	//}


	int departure_time_interval = 1440;

	std::list<DTAAgent*>::iterator iAgent;

	for (iAgent = m_AgentSet.begin(); iAgent != m_AgentSet.end(); iAgent++)
	{
		DTAAgent* pAgent = (*iAgent);

		if(pAgent->m_AgentID == 34)
		{
			TRACE("");
		}

		if(pAgent->m_bComplete )
		{

			for(int i= 2; i<  pAgent->m_NodeSize; i++)
			{
				DTALink* pLink0 = m_LinkNoMap[pAgent->m_NodeAry[i-1].LinkNo]; // i=0, LinkNo = -1;
				DTALink* pLink1 = m_LinkNoMap[pAgent->m_NodeAry[i].LinkNo];

				CString movement_label;


			}

			CString OD_label;

			//movement
			OD_label.Format ("%03d,%03d,%02d,", pAgent->m_o_ZoneID ,pAgent->m_d_ZoneID , int(pAgent->m_DepartureTime / departure_time_interval)); 

			m_LinkFlowProportionODMap[OD_label]++;

		}
	}


	int NumberOfODPairs = m_LinkFlowProportionODMap.size();

	int NumberOfSensors =0;
	// export the output 

	// for each element in m_LinkFlowProportionODMap

	FILE* st;

	CString directory;
	directory = m_ProjectFile.Left(m_ProjectFile.ReverseFind('\\') + 1);

	CString AMS_File = directory+"AMS_link_flow_proportion_matrix.csv";

	FILE* st2;
	CString AMS_FileVector = directory + "AMS_link_flow_proportion_vector.csv";
	fopen_s(&st2, AMS_FileVector, "w");
	fprintf(st2, "Ozone,Dzone,tau,from_node,to_node,ratio,Agent_count,OD_volume,\n");


	fopen_s(&st,AMS_File,"w");
	if(st!=NULL)
	{
		// first line 
		CString ColumnSizeName = g_GetExcelColumnFromNumber(NumberOfSensors+8);

		fprintf(st,"Target Cell/Objective Function,=SUM(D6:D%d)+SUM(G3:%s3),,,,Sensor Link=>,", NumberOfODPairs+5,ColumnSizeName);

		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);
			if(pLink->m_bSensorData)
			{

				fprintf(st,"%d->%d,",(*iLink)->m_FromNodeID , (*iLink)->m_ToNodeID );

			}

		}
		fprintf(st,",Movement=>,");

		std::list<DTANode*>::iterator iNode;
		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{
					fprintf(st,"%d->%d->%d (%s),", 
						m_NodeIDMap[movement.in_link_from_node_id ]->m_NodeID  ,
						m_NodeIDMap[movement.in_link_to_node_id]->m_NodeID  ,
						m_NodeIDMap[movement.out_link_to_node_id]->m_NodeID  ,
						GetTurnDirectionString(movement.movement_approach_turn));
				}

			}
		}
		fprintf(st,"\n");

		// second line: name
		fprintf(st,",,,,,Name=>,");
		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);
			if(pLink->m_bSensorData)
			{

				fprintf(st,"%s (%c),",(*iLink)->m_Name .c_str (), GetApproachChar((*iLink)->m_FromApproach));

			}

		}
		fprintf(st,",Movement=>,");

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{
					int FromNode = m_NodeIDMap[movement.in_link_from_node_id ]->m_NodeID;
					int CurrentNode = m_NodeIDMap[movement.in_link_to_node_id]->m_NodeID;
					int DestinationNode = m_NodeIDMap[movement.out_link_to_node_id]->m_NodeID;

					DTALink* pLink1 = FindLinkWithNodeIDs(FromNode,CurrentNode);
					DTALink* pLink2 = FindLinkWithNodeIDs(CurrentNode, DestinationNode);

					fprintf(st,"%s->%s (%s),", 

						pLink1->m_Name.c_str (), 
						pLink2->m_Name.c_str (), 
						GetTurnDirectionString(movement.movement_approach_turn));
				}

			}
		}


		fprintf(st,"\n,,,,,Deviation of Observed and Estimated Count=>,");
		int ColumnIndex = 7;

		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);
			if(pLink->m_bSensorData)
			{
				// with sensor data
				CString ColumnName = g_GetExcelColumnFromNumber(ColumnIndex);
				fprintf(st,"=(%s4-%s5)*(%s4-%s5),",ColumnName, ColumnName,ColumnName, ColumnName);

				ColumnIndex++;

			}
		}

		ColumnIndex++;
		ColumnIndex++;

		fprintf(st,",,");

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{
					CString ColumnName = g_GetExcelColumnFromNumber(ColumnIndex);
					fprintf(st,"=(%s4-%s5)*(%s4-%s5),",ColumnName, ColumnName,ColumnName, ColumnName);
					ColumnIndex++;

				}

			}
		}



		fprintf(st,"\n,,,,,Observed Count=>,");
		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);

			if(pLink->m_FromNodeID  == 1 && 
				pLink->m_ToNodeID == 14)
			{
				TRACE("");
			}

			
		}

		fprintf(st,",,");

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{
					fprintf(st,"%d,", movement.obs_turn_hourly_count);
				}

			}
		}


		fprintf(st,"\norigin zone id,destination zone id,departure time (hour),");

		fprintf(st,"OD volume deviation,Target OD Volume,||Estimated OD volume: Estimated Flow count=>,");

		ColumnIndex = 7;

		for (std::list<DTALink*>::iterator iLink  = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
		{
			DTALink* pLink = (*iLink);
			if(pLink->m_bSensorData)
			{
				CString ColumnName = g_GetExcelColumnFromNumber(ColumnIndex);
				fprintf(st,"=SUMPRODUCT(($F$6:$F$%d)*(%s6:%s%d)),",NumberOfODPairs+5,ColumnName, ColumnName,NumberOfODPairs+5);

				ColumnIndex++;

				// with sensor data

			}

		}

		fprintf(st,",,");

		ColumnIndex++;
		ColumnIndex++;

		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
		{

			for(unsigned int m = 0; m< (*iNode)->m_MovementVector .size(); m++)
			{
				DTANodeMovement movement = (*iNode)->m_MovementVector[m];
				if(movement.obs_turn_hourly_count >=1)
				{

					CString ColumnName = g_GetExcelColumnFromNumber(ColumnIndex);
					fprintf(st,"=SUMPRODUCT(($F$6:$F$%d)*(%s6:%s%d)),",NumberOfODPairs+5,ColumnName, ColumnName,NumberOfODPairs+5);
					ColumnIndex++;


				}

			}
		}



		fprintf(st,"\n");

		// sort link flow porportion matrix
		std::vector<LinkProp_Pair> myvec(m_LinkFlowProportionODMap.begin(), m_LinkFlowProportionODMap.end());
		std::sort(myvec.begin(), myvec.end(), IntCmp());

		// second block


		int row_index = 6;
		int movement_index = 1;
		//		for (itr = m_LinkFlowProportionODMap.begin(); itr != m_LinkFlowProportionODMap.end(); itr++)
		for (unsigned ii = 0; ii < myvec.size(); ii++)
		{

			fprintf(st, "%s=(E%d-F%d)*(E%d-F%d),%d,%d,", myvec[ii].first, row_index, row_index, row_index, row_index,
				myvec[ii].second, myvec[ii].second);

			row_index++;
			// OD pair

			for (std::list<DTALink*>::iterator iLink = m_LinkSet.begin(); iLink != m_LinkSet.end(); iLink++)
			{
				DTALink* pLink = (*iLink);
				if (pLink->m_bSensorData)
				{
					CString LinkString;

					LinkString.Format("%s%d->%d", myvec[ii].first, (*iLink)->m_FromNodeID, (*iLink)->m_ToNodeID);

					if (m_LinkFlowProportionMap.find(LinkString) != m_LinkFlowProportionMap.end())
					{
						float count = m_LinkFlowProportionMap[LinkString];
						float ratio = count / max(1, myvec[ii].second);

						fprintf(st, "%.4f,", ratio);

						std::map<CString, int >::iterator it_this;
						it_this = m_LinkFlowProportionMap.find(LinkString);
						fprintf(st2, "%s,%.4f,%.1f,%d\n", it_this->first, ratio, count, myvec[ii].second);

					}
					else
					{

						fprintf(st, "0,");

					}

				}

			}
		}
			fprintf(st,"\n");

		fclose(st);
		fclose(st2);

		OpenCSVFileInExcel(AMS_File);

	}else
	{
		AfxMessageBox("File AMS_link_flow_proportion_matrix.csv cannot be opened.");

	}

}





void CTLiteDoc::SetupPhaseData(int node_id,  int phase_numbr, DTA_SIG_PHASE_ROW attribute, float value)
{
	CString phasing_map_key = GetPhasingMapKey(node_id);

	std::string value_str;

	if(value - (int)value >=0.1)
		value_str = string_format( "%.0f", value);
	else
		value_str = string_format( "%.1f", value);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);
	m_DTAPhasingMap[phasing_map_key].SetString ( (DTA_SIG_PHASE) (phase_numbr+ DTA_SIG_PHASE_VALUE), attribute, value_str);

}

void CTLiteDoc::SetupPhaseData(int node_id,  int phase_numbr, DTA_SIG_PHASE_ROW attribute, int value)
{
	CString phasing_map_key = GetPhasingMapKey(node_id);


	std::string value_str;
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);

	if(value >=1 || value == -1)
	{ 
	value_str = string_format( "%d", value);

	m_DTAPhasingMap[phasing_map_key].SetString ( (DTA_SIG_PHASE) (phase_numbr+ DTA_SIG_PHASE_VALUE), attribute, value_str);
	}

}

void CTLiteDoc::SetupPhaseData(int node_id, int phase_numbr, DTA_SIG_PHASE_ROW attribute, std::string value_str)
{
	CString phasing_map_key = GetPhasingMapKey(node_id);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);
	m_DTAPhasingMap[phasing_map_key].SetString ( (DTA_SIG_PHASE) (phase_numbr+ DTA_SIG_PHASE_VALUE), attribute, value_str);

}

void CTLiteDoc::SetupPhaseData(int node_id, int phase_numbr, DTA_SIG_PHASE_ROW attribute, CString value_str)
{
	CString phasing_map_key = GetPhasingMapKey(node_id);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);
	m_DTAPhasingMap[phasing_map_key].SetString ( (DTA_SIG_PHASE) (phase_numbr+ DTA_SIG_PHASE_VALUE), attribute, CString2StdString(value_str));

}

void CTLiteDoc::SetupSignalValue(int node_id,  DTA_SIG_PHASE_ROW attribute, int value)
{
	CString phasing_map_key = GetPhasingMapKey(node_id);
	std::string value_str;
	if(value>=1)
	{ 
	value_str = string_format( "%d", value);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_VALUE, attribute, value_str);
	}
}
void CTLiteDoc::SetupSignalValue(int node_id, DTA_SIG_PHASE_ROW attribute, float value)
{
	CString phasing_map_key = GetPhasingMapKey(node_id);
	std::string value_str;


	if(value - (int)value >=0.1)
		value_str = string_format( "%.0f", value);
	else
		value_str = string_format( "%.1f", value);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_VALUE, attribute, value_str);

}

void CTLiteDoc::SetupSignalValue(int node_id,  DTA_SIG_PHASE_ROW attribute, CString value_str)
{
	CString phasing_map_key  = GetPhasingMapKey(node_id);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_NODE_ID, attribute, CString2StdString(phasing_map_key) );
	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_KEY, attribute, g_DTA_phasing_row_name[attribute]);

	m_DTAPhasingMap[phasing_map_key].SetString ( DTA_SIG_PHASE_VALUE, attribute, CString2StdString(value_str));

}

DTA_Phasing_Data_Matrix CTLiteDoc::GetPhaseData(int node_id)
{

	DTA_Phasing_Data_Matrix element;  // empty element

	CString key = GetPhasingMapKey(node_id);


	if(m_DTAPhasingMap.find(key) != m_DTAPhasingMap.end())
		return m_DTAPhasingMap[key];
	else 
	{  
		//CString str;
		//str.Format("Phasing Map Key (%d:%s) not defined", node_id.c_str () );
		//AfxMessageBox(str);

	}
	return element;

}

BOOL  CTLiteDoc::IfMovementIncludedInPhase(int node_id, int phase_no, long from_node_id, int destination_node_id)
{
	CString value;
	value.Format("%d;%d;%d", from_node_id, node_id, destination_node_id);

	DTANode* pNode = this->m_NodeIDMap[node_id];

	if (phase_no >= pNode->m_node_phase_vector.size())
		return false;

	for (int m = 0; m < pNode->m_node_phase_vector[phase_no].movement_vector.size(); m++)
	{
		if (pNode->m_node_phase_vector[phase_no].movement_vector[m].Compare(value) == 0)
			return true;
	}

	return false;

}

BOOL  CTLiteDoc::IfMovementDirIncludedInPhase(int node_id, int phase_no, int movement_index)
{
	DTA_Phasing_Data_Matrix element = GetPhaseData(node_id);

	CString MovementDirVector = element.GetString((DTA_SIG_PHASE)(DTA_SIG_PHASE_VALUE + phase_no), PHASE_MOVEMENT_DIR_VECTOR);

	DTANode* pNode  = this->m_NodeIDMap  [node_id];

	if (pNode == NULL)
		return false;

	CString value =GetTurnDirectionString(pNode->m_MovementVector[ movement_index].movement_approach_turn);

	if (MovementDirVector.Find(value) != -1) // find the value
		return true;
	else
		return false;


}


bool CTLiteDoc::ReadSynchroCombinedCSVFile(LPCTSTR lpszFileName)
{
	return false;
//	CWaitCursor wait;
//
//	CString file_name;
//	file_name.Format ("%s", lpszFileName);
//	CString Synchro_directory = file_name.Left(file_name.ReverseFind('\\') + 1);
//	// read users' prespecified control type
//
//	CopyDefaultFile(m_DefaultDataFolder,Synchro_directory,Synchro_directory,"input_node_control_type.csv");
//	CopyDefaultFile(m_DefaultDataFolder,Synchro_directory,Synchro_directory,"link_type.csv");
//
//	if(ReadLinkTypeCSVFile(Synchro_directory+"link_type.csv") == false)
//	{
//		CString msg;
//		msg.Format("Please first make sure file %s\\link_type.csv exists before importing synchro single csv file.", Synchro_directory);
//		AfxMessageBox(msg, MB_ICONINFORMATION);
//
//	}
//
//	string direction_vector[12] = {"NBL","NBT","NBR","SBL","SBT","SBR","EBL","EBT","EBR","WBL",	"WBT",	"WBR"};
//
//	string lane_att_name_str[28] = {"Lanes","Shared","Width","Storage","StLanes","Grade","Speed",
//					"FirstDetect","LastDetect","Phase1","PermPhase1","DetectPhase1","IdealFlow","LostTime",
//					"SatFlowRatePerLaneGroup","Volume","SatFlowRTOR","HeadwayFact","Volume","Peds","Bicycles","PHF","Growth","HeavyAgents","BusStops","Midblock","Distance","TravelTime"};
//
//string phasing_column_name[DTA_SIG_PHASE_MAX_COLUMN] = {"node_id", "D1","D2","D3","D4",
//"D5","D6","D7","D8", "D9","D10",
//"D11","D12","D13","D14",
//"D15","D16","D17","D18"};
//
//
//
//	std::map<int,int> UpNodeIDMap;
//	std::map<int,int> DestNodeIDMap;
//
//
//	std::map<long, long> m_INTIDMap;
//	std::map<long, long> m_NodeOrgNumber2INTIDMap;
//	std::map<long, long> m_NodeOrgNumber2NodeNoMap;
//
//
//	std::string timing_plan_name = "0"; 
//	
//	CCSVParser parser;
//	parser.m_bSynchroSingleCSVFile   = true;  // single file
//	if (parser.OpenCSVFile(lpszFileName, false /*do not read first line as the header*/))
//	{
//		int i=0;
//		int j= 1;  // actual node id for synchro
//
//		std::map<int,int> from_node_id_map;
//
//		while(parser.ReadRecord())
//		{
//
//			if(parser.m_SynchroSectionName.find ("Nodes")!=  string::npos) 
//			{
//
//				int intid;
//				string name;
//				DTANode* pNode = 0;
//
//				int node_type;
//				double X;
//				double Y;
//				if(parser.GetValueByFieldName("INTID",intid) == false)
//					continue;
//
//				if(!parser.GetValueByFieldName("INTNAME",name))
//					name = "";
//
//				if(!parser.GetValueByFieldName("TYPE",node_type))
//					node_type = 0;
//
//				// use the X and Y as default values first
//				bool bFieldX_Exist = parser.GetValueByFieldName("X",X);
//				parser.GetValueByFieldName("Y",Y);
//
//				pNode = new DTANode;
//
//				pNode->m_Name = name;
//
//				pNode->m_ControlType = m_ControlType_NoControl;  // default value
//
//
//				pNode->pt.x = X/5280.0f;  // feet to mile
//				pNode->pt.y = Y/5280.0f;  // feet to mile
//
//
//				int node_number_estimator  = j;
//				if(intid < 10000 && intid > j)
//				{
//					node_number_estimator = intid;  // encounter skipped nodes, move 
//					j = node_number_estimator;
//
//				}
//				pNode->m_NodeID = node_number_estimator; // intid is the name used , start from 1
//				m_INTIDMap [intid] = i;
//				m_NodeOrgNumber2INTIDMap[intid] = node_number_estimator;
//				pNode->m_NodeID = i;
//				pNode->m_ZoneID = 0;
//				pNode->m_CycleLengthInSecond = 0;
//				m_NodeSet.push_back(pNode);
//				m_NodeNoMap[i] = pNode;
//				m_NodeNotoIDMap[i] = node_number_estimator; // start from 1
//				m_NodeIDtoNodeNoMap[node_number_estimator] = i;
//				m_NodeOrgNumber2NodeNoMap[intid] = i;
//				i++;
//				j++;
//
//			} // node block
//			////////////////////////////////////////////
//
//			m_DefaultLinkType = m_LinkTypeArterial;
//
//			if(parser.m_SynchroSectionName.find ("Links")!=  string::npos) 
//			{
//
//
//				string direction_vector[8] = {"NB","SB","EB","WB","NE","NW","SE","SW"};
//
//				int intid;
//				string name;
//				DTANode* pNode = 0;
//
//				int node_type;
//				double X;
//				double Y;
//				if(parser.GetValueByFieldName("INTID",intid) == false)
//					continue;
//
//				parser.GetValueByFieldName("RECORDNAME",name);
//				int INTID;
//				int m;
//				//			switch (name)
//				if (name == "Up ID" || name == "UpNodeNo")
//				{
//
//
//					for(int direction = 0; direction < 8; direction++)
//					{
//						int  incoming_node_number;
//						if(parser.GetValueByFieldName(direction_vector[direction],incoming_node_number))// value exits
//						{
//							// add a new link
//							long to_node_id = m_INTIDMap[intid];
//
//
//							if(m_NodeIDtoNodeNoMap.find(incoming_node_number) != m_NodeIDtoNodeNoMap.end() )
//							{
//								long from_node_id = m_NodeIDtoNodeNoMap[incoming_node_number];
//								from_node_id_map[direction] = from_node_id;
//								//if(m_NodeIDMap[to_node_id]->m_ControlType == m_ControlType_ExternalNode) XUESONG
//								// add new link if the outbound node is an external node
//								AddNewLink(from_node_id, to_node_id,true, false);
//								AddNewLink(to_node_id,from_node_id,true, false);
//
//							} else
//							{
//
//								if(m_NodeOrgNumber2NodeNoMap.find(incoming_node_number) != m_NodeOrgNumber2NodeNoMap.end())
//								{
//
//									long from_node_id = m_NodeOrgNumber2NodeNoMap[incoming_node_number];
//									from_node_id_map[direction] = from_node_id;
//									//if(m_NodeIDMap[to_node_id]->m_ControlType == m_ControlType_ExternalNode) XUESONG
//									// add new link if the outbound node is an external node
//									AddNewLink(from_node_id, to_node_id,true);
//									AddNewLink(to_node_id,from_node_id,true);
//								}
//
//							}
//
//
//						}
//
//
//					}
//
//				}
//
//				///
//
//				
//				if (name == "Lanes")
//				{
//
//					for(int direction = 0; direction < 8; direction++)
//					{
//						int  value;
//						if(parser.GetValueByFieldName(direction_vector[direction],value))// value exits
//						{
//							// add a new link
//							long to_node_id = m_INTIDMap[intid];
//							long from_node_id = from_node_id_map[direction];
//							DTALink* pLink = FindLinkWithNodeNo(from_node_id, to_node_id);
//
//							if(pLink!=NULL)
//						 {
//							 pLink->m_NumberOfLanes = value;
//						 }
//
//						}
//
//
//					}	
//
//				} // end of Lanes attributes;
//
//				if (name == "Distance")
//				{
//
//					for(int direction = 0; direction < 8; direction++)
//					{
//						float  value;
//						if(parser.GetValueByFieldName(direction_vector[direction],value))// value exits
//						{
//							// add a new link
//							long to_node_id = m_INTIDMap[intid];
//							long from_node_id = from_node_id_map[direction];
//							DTALink* pLink = FindLinkWithNodeNo(from_node_id, to_node_id);
//
//							if(pLink!=NULL)
//						 {
//							 pLink->m_Length  = value/5280;  // feet to mile
//						 }
//
//						}
//
//
//					}	
//
//				} // end of Lanes attributes;
//
//				if (name == "Speed")
//				{
//
//					for(int direction = 0; direction < 8; direction++)
//					{
//						float  value;
//						if(parser.GetValueByFieldName(direction_vector[direction],value))// value exits
//						{
//							// add a new link
//							long to_node_id = m_INTIDMap[intid];
//							long from_node_id = from_node_id_map[direction];
//							DTALink* pLink = FindLinkWithNodeNo(from_node_id, to_node_id);
//
//							if(pLink!=NULL)
//						 {
//							 pLink->m_FreeSpeed   = value;  // Free Speed
//						 }
//
//						}
//
//
//					}	
//
//				} // end of Lanes attributes;
//
//				if (name == "Name")
//				{
//
//					for(int direction = 0; direction < 8; direction++)
//					{
//						string  name;
//						if(parser.GetValueByFieldName(direction_vector[direction],name))// value exits
//						{
//							// add a new link
//							long to_node_id = m_INTIDMap[intid];
//							long from_node_id = from_node_id_map[direction];
//							DTALink* pLink = FindLinkWithNodeNo(from_node_id, to_node_id);
//
//							if(pLink!=NULL)
//						 {
//							 pLink->m_Name = name;
//						 }
//
//						}
//
//
//					}	
//
//				} // end of name attributes;
//
//			} // link block
//
//			if(parser.m_SynchroSectionName.find ("Lanes")!=  string::npos) 
//			{
//
//				// finish reading node and link blocks, now we construct movement data
//
//				if(m_bMovementAvailableFlag==0)  // has not been initialized. 
//				{
//					ConstructMovementVector();
//
//
//					m_MovementPointerMap.clear();
//
//					std::list<DTANode*>::iterator iNode;
//					for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
//					{
//						if((*iNode)->m_NodeID  == 7)
//						{
//							TRACE("");
//
//						}
//
//						for(unsigned int m = 0; m< (*iNode)->m_MovementVector .size(); m++)
//						{
//
//							DTANodeMovement movement = (*iNode)-> m_MovementVector[m];
//
//							CString label;
//							int up_node_id = m_NodeIDMap[movement.in_link_from_node_id]->m_NodeID     ;
//							int dest_node_id = m_NodeIDMap[movement.out_link_to_node_id ]->m_NodeID ;
//							label.Format("%d;%d;%d", up_node_id,(*iNode)->m_NodeID ,dest_node_id);
//
//							m_MovementPointerMap[label] = &((*iNode)-> m_MovementVector[m]); // store pointer
//
//
//						}
//
//					}
//
//				}
//
//				//
//
//
//				int intid;
//				string name;
//				DTANode* pNode = 0;
//
//				int node_type;
//				double X;
//				double Y;
//				if(parser.GetValueByFieldName("INTID",intid) == false)
//					continue;
//
//				parser.GetValueByFieldName("RECORDNAME",name);
//
//				int m;
//				//			switch (name)
//				if (name == "Up Node" || name == "UpNodeNo")
//				{
//					UpNodeIDMap.clear();
//					DestNodeIDMap.clear();
//
//					for(int direction = 0; direction < 12; direction++)
//					{
//						int  incoming_node_number;
//						if(parser.GetValueByFieldName(direction_vector[direction],incoming_node_number))// value exits
//						{
//							UpNodeIDMap[direction] = incoming_node_number;
//
//
//						}
//
//
//					} //direction
//
//				}  // Up Node
//
//				if (name == "Dest Node" || name == "DestNodeNo")
//				{
//
//					for(int direction = 0; direction < 12; direction++)
//					{
//						int  outgoing_node_number;
//						if(parser.GetValueByFieldName(direction_vector[direction],outgoing_node_number))// value exits
//						{
//
//							DestNodeIDMap[direction] = outgoing_node_number;
//
//						}
//
//					} //direction
//
//				}  // Dest Node
//
//				// Lanes
//
//				int attribute_index = -1;
//
//				if(name == "Volume")
//				{
//					TRACE("");
//				}
//
//				for(int n = 0;  n < 28; n ++)
//				{
//
//					if (name == lane_att_name_str[n])
//					{
//						attribute_index = n;
//						break;
//					}
//
//
//				}
//
//
//				if (attribute_index>=0)  // attrite has been defined
//				{
//
//					long to_node_id = m_INTIDMap[intid];
//					for(int direction = 0; direction < 12; direction++)
//					{
//
//
//
//						if(UpNodeIDMap.find(direction) != UpNodeIDMap.end() )
//						{
//
//							int UpNodeNo = -1;
//
//							if(m_NodeIDtoNodeNoMap.find(UpNodeIDMap[direction]) != m_NodeIDtoNodeNoMap.end())
//							{
//								UpNodeNo = m_NodeIDtoNodeNoMap[UpNodeIDMap[direction]];
//
//							}else
//							{
//								if(m_NodeOrgNumber2NodeNoMap.find(UpNodeIDMap[direction]) != m_NodeOrgNumber2NodeNoMap.end())
//								{
//									UpNodeNo = m_NodeOrgNumber2NodeNoMap[UpNodeIDMap[direction]];
//								}			
//
//							}
//
//
//
//							int DestNodeNo = -1;
//
//							if(m_NodeIDtoNodeNoMap.find(DestNodeIDMap[direction]) != m_NodeIDtoNodeNoMap.end())
//							{
//								DestNodeNo = m_NodeIDtoNodeNoMap[DestNodeIDMap[direction]];
//
//							}else
//							{
//								if(m_NodeOrgNumber2NodeNoMap.find(DestNodeIDMap[direction]) != m_NodeOrgNumber2NodeNoMap.end())
//								{
//									DestNodeNo = m_NodeOrgNumber2NodeNoMap[DestNodeIDMap[direction]];
//								}	
//
//
//							}
//
//
//							int CurrentNodeNo = -1;
//							if(m_NodeIDtoNodeNoMap.find(intid) != m_NodeIDtoNodeNoMap.end())
//							{
//								CurrentNodeNo = m_NodeIDtoNodeNoMap[intid];
//
//							}else
//							{
//								if(m_NodeOrgNumber2NodeNoMap.find(intid) != m_NodeOrgNumber2NodeNoMap.end())
//								{
//									CurrentNodeNo = m_NodeOrgNumber2NodeNoMap[intid];
//								}	
//
//
//							}
//
//
//							if(UpNodeNo<0)
//								TRACE("\nNode ID %d cannot be found.",UpNodeIDMap[direction]);
//
//							if(DestNodeNo<0)
//								TRACE("\nNode ID %d cannot be found.",DestNodeIDMap[direction]);
//
//							if(CurrentNodeNo<0)
//								TRACE("\nNode ID %d cannot be found.",intid);
//
//
//							CString label;
//							label.Format("%d;%d;%d", UpNodeNo,CurrentNodeNo,DestNodeNo);
//							if(m_MovementPointerMap.find(label) != m_MovementPointerMap.end())
//							{
//								DTANodeMovement* pMovement = m_MovementPointerMap[label];
//
//								int  value = 0;
//								if(parser.GetValueByFieldName(direction_vector[direction],value))// value exits
//								{
//
//									pMovement->QEM_dir_string = direction_vector[direction];
//
//
//									/* {"Lanes","Shared","Width","Storage","StLanes","Grade","Speed",
//									"FirstDetect","LastDetect","Phase1","PermPhase1","DetectPhase1","IdealFlow","LostTime","SatFlowRatePerLaneGroup",
//									14 "SatFlowPerm","SatFlowRTOR","HeadwayFact","Volume","Peds","Bicycles","PHF","Growth","HeavyAgents","BusStops","Midblock","Distance","TravelTime"};
//									*/
//									switch(attribute_index)
//									{
//									case  0:// "Lanes"
//										pMovement->QEM_Lanes = value; break;
//									case  1:// "Shared"
//										pMovement->QEM_Shared = value; break;
//									case  2:// "Width"
//										pMovement->QEM_Width = value; break;
//									case  3:// "Storage"
//										pMovement->QEM_Storage = value; break;
//									case  4:// "StLanes"
//										pMovement->QEM_StLanes = value; break;
//									case  5:// "Grade"
//										pMovement->QEM_Grade = value; break;
//									case  6:// "Speed"
//										pMovement->QEM_Speed = value; break;
//									case  9:// "Phase1"
//										pMovement->QEM_Phase1 = value; break;
//									case  10:// "PermPhase1"
//										pMovement->QEM_PermPhase1 = value; break;
//									case  11:// "DetectPhase1"
//										pMovement->QEM_DetectPhase1 = value; break;
//									case  12:// "IdealFlow"
//										pMovement->QEM_IdealFlow = value; break;
//									case  13:// "LostTime"
//										pMovement->QEM_LostTime = value; break;
//									case  14:// "SatFlowRatePerLaneGroup"
//										pMovement->QEM_SatFlow = value; break;
//									case  15:// "Volume"
//										pMovement->QEM_TurnVolume  = value; break;
//
//									}
//								}
//
//							}else
//							{
//
//								TRACE("Cannot find movement %s\n", label);
//
//							}
//
//						}
//
//					}  //direction
//				} // lanes
//
//				/////////////
//
//
//			}  // Lane attribute
//
//
//
//			if(parser.m_SynchroSectionName.find ("Timeplans")!=  string::npos) 
//			{
//
//
//				string name;
//				parser.GetValueByFieldName("RECORDNAME",name);
//				int intid;
//				if(parser.GetValueByFieldName("INTID",intid) == false)
//					continue;
//
//				if(name == "Cycle Length")
//				{
//					int intid;
//					if(parser.GetValueByFieldName("INTID",intid) == false)
//						continue;
//
//					int node_id = m_INTIDMap[intid];
//
//					int data = 0;
//					if(parser.GetValueByFieldName("DATA",data) == false)
//						continue;
//
//					m_NodeIDMap[node_id]->m_CycleLengthInSecond = data;
//
//					if(data > 10)  // set 
//						m_NodeIDMap[node_id]->m_ControlType = m_ControlType_PretimedSignal;
//
//	
//					SetupSignalValue(intid ,TIMING_CycleLength,data);
//
//
//
//				}
//
//				if(name == "Offset")
//				{
//					int intid;
//					if(parser.GetValueByFieldName("INTID",intid) == false)
//						continue;
//
//					int node_id = m_INTIDMap[intid];
//
//					int data = 0;
//					if(parser.GetValueByFieldName("DATA",data) == false)
//						continue;
//
//					m_NodeIDMap[node_id]->m_SignalOffsetInSecond  = data;
//
//					SetupSignalValue(intid ,TIMING_Offset,data);
//				}
//
//
//			}
//
//			if(parser.m_SynchroSectionName.find ("Phases")!=  string::npos) 
//			{
//
//				string name;
//				parser.GetValueByFieldName("RECORDNAME",name);
//
//
//				int intid;
//				if(parser.GetValueByFieldName("INTID",intid) == false)
//					continue;
//
//
//				int node_id = m_INTIDMap[intid];
//
//				CString phasing_map_key  =  GetPhasingMapKey(intid,"0");
//
//
//				if(name == "Start")
//				{
//
//					for(int d = 1; d <=16; d++)
//					{
//						parser.GetValueByFieldName(phasing_column_name[d],m_NodeIDMap[node_id]->m_SignalPhaseStartTime[d]);
//					}
//
//
//				}
//
//				if(name == "MaxGreen")
//				{
//
//					for(int d = 0; d <=16; d++)
//					{
//						parser.GetValueByFieldName(phasing_column_name[d],m_NodeIDMap[node_id]->m_SignalPhaseGreenTime[d]);
//					}
//				}
//
//			int row_no = 0;
//
//			for(row_no = 0; row_no < DTA_PHASE_ATTRIBUTE_MAX_ROW; row_no++)
//			{
//				if( name.compare (g_DTA_phasing_row_name[row_no]) == 0)
//				{
//
//					// row_no is the selecte no. 
//					break;
//
//				}
//
//			}
//
//
//			if(row_no < DTA_PHASE_ATTRIBUTE_MAX_ROW)
//			{
//				for(int j = DTA_SIG_PHASE_D1; j< DTA_SIG_PHASE_D16; j++)
//				{
//
//					string value;
//					parser.GetValueByFieldName(g_DTA_phasing_column_name[j],value);
//					m_DTAPhasingMap[phasing_map_key ].m_AMSPhasingData [row_no] [j] = value;
//				}
//
//			}
//			
//			}
//		}//while
//
//		parser.CloseCSVFile ();
//
//		// update QEM turn percentage
//
//		// turning percentage
//		std::list<DTANode*>::iterator iNode;
//
//		for (iNode = m_NodeSet.begin(); iNode != m_NodeSet.end(); iNode++)
//		{
//			for(unsigned int m = 0; m< (*iNode)->m_MovementVector .size(); m++)
//			{
//				DTANodeMovement* pMovement = &((*iNode)->m_MovementVector[m]);
//
//
//				if(pMovement->QEM_Phase1 >0 && pMovement->QEM_Phase1 <=16)
//				{
//
//				pMovement->QEM_StartTime = (*iNode)->m_SignalPhaseStartTime [pMovement->QEM_Phase1];
//				pMovement->QEM_EndTime = pMovement->QEM_StartTime + (*iNode)->m_SignalPhaseGreenTime [pMovement->QEM_Phase1];
//
//						std::string movement_vector; 
//
//						DTA_Phasing_Data_Matrix element  = GetPhaseData((*iNode)->m_NodeID );
//
//
//						DTA_SIG_PHASE column = (DTA_SIG_PHASE)( DTA_SIG_PHASE_VALUE + pMovement->QEM_Phase1) ;
//						//get existing movement data
//						movement_vector = element.GetSTDString( column, PHASE_MOVEMENT_VECTOR );
//
//						// add movement data
//						movement_vector += string_format("%d",m_NodeIDMap[pMovement->in_link_from_node_id]->m_NodeID);
//						movement_vector += ":";
//						movement_vector += string_format("%d",m_NodeIDMap[pMovement->out_link_to_node_id ]->m_NodeID); 
//						movement_vector += ";";
//
//
//						int QEM_end_time = (int)(pMovement->QEM_EndTime+0.5);
//
//				SetupPhaseData((*iNode)->m_NodeID, pMovement->QEM_Phase1, PHASE_End, QEM_end_time);
//
//
//
//						SetupPhaseData((*iNode)->m_NodeID, pMovement->QEM_Phase1, PHASE_MOVEMENT_VECTOR, movement_vector);
//
//				}
//				DTALink* pLink0 = m_LinkNoMap[pMovement->IncomingLinkNo  ];
//
//				int total_link_count = 0;
//				for(unsigned int j = 0; j< (*iNode)->m_MovementVector .size(); j++)
//				{
//
//					if((*iNode)->m_MovementVector[j].IncomingLinkNo == pMovement->IncomingLinkNo )
//					{
//						total_link_count+= (*iNode)->m_MovementVector[j].QEM_TurnVolume ;
//					}
//
//				}
//
//				if (pMovement->QEM_TurnVolume >=1)
//				{
//					pMovement->QEM_TurnPercentage = 
//						pMovement->QEM_TurnVolume * 100.0f / max(1,total_link_count);
//				}
//			}
//
//		}
//
//
//		UpdateAllMovementGreenStartAndEndTime("0");
//
//		m_NodeDataLoadingStatus.Format ("%d nodes are loaded from file %s.",m_NodeSet.size(),lpszFileName);
//		return true;
//	}else
//	{
//		AfxMessageBox("Error: File LAYOUT.csv cannot be opened.\nIt might be currently used and locked by EXCEL.");
//		return false;
//		//		g_ProgramStop();
//	}
//

}



