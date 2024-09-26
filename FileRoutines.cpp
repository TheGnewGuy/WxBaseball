/////////////////////////////////////////////////////////////////////////////
// Name:          FileRoutines.cpp                                         //
// Purpose:       Baseball statistics tracking tool for Strat-O-Matic      //
// Author:        John Gnew                                                //
// Created:       09/22/24                                                 //
// Copyright:     (c) John Gnew                                            //
// Modifications:                                                          //
//   Date       Description                                                //
// ??/??/??    X                                                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "FileRoutines.h"
#include "WxBaseballMain.h"
#include "WxBaseballApp.h"
#include "DBRoutines.h"


FileRoutines::FileRoutines()
{
	//ctor
}

FileRoutines::~FileRoutines()
{
	//dtor
}

int FileRoutines::ExportTeam( int passedTeamID )
{
	wxFile exportBatter;
	wxFile exportPitcher;
	wxString exportFileNameBatter;
	wxString exportFileNamePitcher;
	char exportData[200];
	wxString strexportData;
	char Bunting[5] = {'A','B','C','D','E'};
	char HitRun[4] = {'A','B','C','D'};
//	char Stealing[7] = {'3','2','A','B','C','D','E'};
//	CStringA Stealing[7] = { "AAA", "AA", "A", "B", "C", "D", "E" };
	wxString Stealing[7] = { "AAA", "AA", "A", "B", "C", "D", "E" };
	char BatterHits[3] = { 'L', 'R', 'S' };
	char Power[2] = {'N','W'};
	int Running[15] = {5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
	int TRate[21] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
	int Pass[21] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
	int OutArm[12] = {-6,-5,-4,-3,-2,-1,0,1,2,3,4,5};
	int CatchArm[10] = {-4,-3,-2,-1,0,1,2,3,4,5};
	int PHold[16] = {9,8,7,6,5,4,3,2,1,0,-1,-2,-3,-4,-5,-6};
//	m_TeamRecord teamRecord;
//	m_LeagueRecord leagueRecord;
//	m_ConferenceRecord conferenceRecord;
//	m_DivisionRecord divisionRecord;
//	m_BatterStatsRecord batterStatsRecord;
//	m_BatterRecord batterRecord;
//	m_PitcherStatsRecord pitcherStatsRecord;
//	m_PitcherRecord pitcherRecord;

	sqlite3_stmt *localStmt;
	char *sqlBatterStats;
	char *sqlPitcherStats;
	int rc;
	int rcSqlStep = 0;
	char buffer[100];
	int batterStatsID = 0;
	int pitcherStatsID = 0;
	wxString LeagueNameSave;
	wxString LeagueDir;
	wxString MsgBuffer;
	int i;

	wxGetApp().pDBRoutines->DBGetTeam( passedTeamID );
	wxGetApp().pDBRoutines->DBGetLeague( wxGetApp().pDBRoutines->structTeamData.LeagueID );
	wxGetApp().pDBRoutines->DBGetConference( wxGetApp().pDBRoutines->structTeamData.ConferenceID );
	wxGetApp().pDBRoutines->DBGetDivision( wxGetApp().pDBRoutines->structTeamData.DivisionID );

	// Check to see if the league directory exists
	LeagueNameSave = wxGetApp().pDBRoutines->structLeagueData.LeagueName;
	LeagueNameSave.Replace(" ", "_", true);
	LeagueDir = wxGetApp().g_DataDirectory + _T("/") + LeagueNameSave;
	if ( !wxDirExists( LeagueDir ) )
	{
		// If it does not exist, then create it with permissions 755
		wxMkdir( LeagueDir, wxS_IRUSR|wxS_IWUSR|wxS_IXUSR|wxS_IRGRP|wxS_IXGRP|wxS_IROTH|wxS_IXOTH );
	}

	// Create file name for Batters
	exportFileNameBatter = wxGetApp().g_DataDirectory + _T("/") + LeagueNameSave +
		_T("/") + _T("Batter") +
		_T("_") + wxGetApp().pDBRoutines->structTeamData.TeamName + _T(".csv");
	exportFileNameBatter.Replace(" ", "_", true);

	// Create file name for Pitchers
	exportFileNamePitcher = wxGetApp().g_DataDirectory + _T("/") + LeagueNameSave +
		_T("/") + _T("Pitcher") +
		_T("_") + wxGetApp().pDBRoutines->structTeamData.TeamName + _T(".csv");
	exportFileNamePitcher.Replace(" ", "_", true);

	// Process Batter file
	exportBatter.Create( exportFileNameBatter, true, wxS_DEFAULT);
	exportBatter.Open(exportFileNameBatter, wxFile::read_write);
    strexportData.Printf( _T("Team Name,LastName,FirstName,AB,Runs,Hits,RBI,2B,3B,HR,W,K,RE,") );
	exportBatter.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("SF,SB,CS,Bunting,Stealing,Running,Hit Run,LRS,P,C,") );
	exportBatter.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("1B,2B,SS,3B,LF,CF,RF,ChB,ChL,ChR,") );
	exportBatter.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("AVG,SLG,OBP,Games,HBP,") );
	exportBatter.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("Ch1B,Ch2B,Ch3B,ChHR,ChW,ChDP,") );
	exportBatter.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("Ch1BL,Ch2BL,Ch3BL,ChHRL,ChWL,ChDPL,") );
	exportBatter.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("Ch1BR,Ch2BR,Ch3BR,ChHRR,ChWR,ChDPR,") );
	exportBatter.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("ER1,ER2,ER3,ER4,ER5,ER6,ER7,ER8,ER9,") );
	exportBatter.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("T Rating,Passball,Power Left,Power Right,Outfield Arm,Catcher Arm\n") );
	exportBatter.Write( strexportData,strexportData.Length() );

	// Retrieve Team Batters into the arrays m_arrayBatterStatsIDs, m_arrayBatterFirstNames, m_arrayBatterLastNames
	wxGetApp().pDBRoutines->DBGetBatterStatsID( passedTeamID );

	for ( i=0; i<wxGetApp().pDBRoutines->m_arrayBatterStatsIDs.GetCount(); i++)
	{
		wxGetApp().pDBRoutines->DBGetBatterData( wxGetApp().pDBRoutines->m_arrayBatterStatsIDs[i] );

		strexportData.Printf( _T("%s,%s,%s,"),
			wxGetApp().pDBRoutines->structTeamData.TeamName,
			wxGetApp().pDBRoutines->structBatterData.FirstName,
			wxGetApp().pDBRoutines->structBatterData.LastName);
		exportBatter.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,"),
			wxGetApp().pDBRoutines->structBatterStats.AB,
			wxGetApp().pDBRoutines->structBatterStats.Runs,
			wxGetApp().pDBRoutines->structBatterStats.Hits,
			wxGetApp().pDBRoutines->structBatterStats.RBI,
			wxGetApp().pDBRoutines->structBatterStats.Doubles,
			wxGetApp().pDBRoutines->structBatterStats.Triples,
			wxGetApp().pDBRoutines->structBatterStats.HomeRuns,
			wxGetApp().pDBRoutines->structBatterStats.Walk,
			wxGetApp().pDBRoutines->structBatterStats.Strikeout,
			wxGetApp().pDBRoutines->structBatterStats.ReachedOnError);
		exportBatter.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%i,%i,%i,%c,%s,%i,%c,%c,%i,%i,"),
			wxGetApp().pDBRoutines->structBatterStats.Sacrifice,
			wxGetApp().pDBRoutines->structBatterStats.StollenBase,
			wxGetApp().pDBRoutines->structBatterStats.CS,
			Bunting[wxGetApp().pDBRoutines->structBatterData.Bunting],
			Stealing[wxGetApp().pDBRoutines->structBatterData.Stealing],
			wxGetApp().pDBRoutines->structBatterData.Running,
			HitRun[wxGetApp().pDBRoutines->structBatterData.HitRun],
			BatterHits[wxGetApp().pDBRoutines->structBatterData.BatterHits],
			wxGetApp().pDBRoutines->structBatterData.Pitcher,
			wxGetApp().pDBRoutines->structBatterData.Catcher);
		exportBatter.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%i,%i,%i,%i,%i,%i,%i,%2.2f,%2.2f,%2.2f,"),
			wxGetApp().pDBRoutines->structBatterData.FirstBase,
			wxGetApp().pDBRoutines->structBatterData.SecondBase,
			wxGetApp().pDBRoutines->structBatterData.Shortstop,
			wxGetApp().pDBRoutines->structBatterData.ThirdBase,
			wxGetApp().pDBRoutines->structBatterData.LeftField,
			wxGetApp().pDBRoutines->structBatterData.CenterField,
			wxGetApp().pDBRoutines->structBatterData.RightField,
			wxGetApp().pDBRoutines->structBatterData.OBChanceBasic,
			wxGetApp().pDBRoutines->structBatterData.OBChanceLeft,
			wxGetApp().pDBRoutines->structBatterData.OBChanceRight);
		exportBatter.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%1.3f,%1.3f,%1.3f,"),
			wxGetApp().pDBRoutines->structBatterStats.AVG,
			wxGetApp().pDBRoutines->structBatterStats.SLG,
			wxGetApp().pDBRoutines->structBatterStats.OBP);
		exportBatter.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%i,%i,"),
			wxGetApp().pDBRoutines->structBatterStats.Games,
			wxGetApp().pDBRoutines->structBatterStats.HBP);
		exportBatter.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,"),
			wxGetApp().pDBRoutines->structBatterData.OBChanceSingle,
			wxGetApp().pDBRoutines->structBatterData.OBChanceDouble,
			wxGetApp().pDBRoutines->structBatterData.OBChanceTriple,
			wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRun,
			wxGetApp().pDBRoutines->structBatterData.OBChanceWalk,
			wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlay);
		exportBatter.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,"),
			wxGetApp().pDBRoutines->structBatterData.OBChanceSingleLeft,
			wxGetApp().pDBRoutines->structBatterData.OBChanceDoubleLeft,
			wxGetApp().pDBRoutines->structBatterData.OBChanceTripleLeft,
			wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRunLeft,
			wxGetApp().pDBRoutines->structBatterData.OBChanceWalkLeft,
			wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlayLeft);
		exportBatter.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,"),
			wxGetApp().pDBRoutines->structBatterData.OBChanceSingleRight,
			wxGetApp().pDBRoutines->structBatterData.OBChanceDoubleRight,
			wxGetApp().pDBRoutines->structBatterData.OBChanceTripleRight,
			wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRunRight,
			wxGetApp().pDBRoutines->structBatterData.OBChanceWalkRight,
			wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlayRight);
		exportBatter.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%i,%i,%i,%i,%i,%i,%i,%i,%i,"),
			wxGetApp().pDBRoutines->structBatterData.ER1,
			wxGetApp().pDBRoutines->structBatterData.ER2,
			wxGetApp().pDBRoutines->structBatterData.ER3,
			wxGetApp().pDBRoutines->structBatterData.ER4,
			wxGetApp().pDBRoutines->structBatterData.ER5,
			wxGetApp().pDBRoutines->structBatterData.ER6,
			wxGetApp().pDBRoutines->structBatterData.ER7,
			wxGetApp().pDBRoutines->structBatterData.ER8,
			wxGetApp().pDBRoutines->structBatterData.ER9);
		exportBatter.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%i,%i,%c,%c,%i,%i\n"),
			wxGetApp().pDBRoutines->structBatterData.TRate,
			wxGetApp().pDBRoutines->structBatterData.Pass,
			Power[wxGetApp().pDBRoutines->structBatterData.PowerLeft],
			Power[wxGetApp().pDBRoutines->structBatterData.PowerRight],
			wxGetApp().pDBRoutines->structBatterData.OutArm,
			wxGetApp().pDBRoutines->structBatterData.CatchArm);
		exportBatter.Write( strexportData,strexportData.Length() );
	}

	exportBatter.Close();

	// Process Pitcher file
	exportPitcher.Create( exportFileNamePitcher, true, wxS_DEFAULT);
	exportPitcher.Open(exportFileNamePitcher, wxFile::read_write);

    strexportData.Printf( _T("Team Name,LastName,FirstName,IP,ER,Hits,Walks,Strikeouts,") );
	exportPitcher.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("Homeruns,Hold,Wins,Loss,Saves,Starter,Relief,Throws,") );
	exportPitcher.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("ChB,ChL,ChR,") );
	exportPitcher.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("ERA,WHIP,") );
	exportPitcher.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("Starts,") );
	exportPitcher.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("Games,Completed Games,") );
	exportPitcher.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("Ch1B,Ch2B,Ch3B,ChHR,ChW,ChDP,") );
	exportPitcher.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("Ch1BL,Ch2BL,Ch3BL,ChHRL,ChWL,ChDPL,") );
	exportPitcher.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("Ch1BR,Ch2BR,Ch3BR,ChHRR,ChWR,ChDPR,") );
	exportPitcher.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("Wild Pitch,Balk,Fielding,ErrorRating,Bunting\n") );
	exportPitcher.Write( strexportData,strexportData.Length() );

	// Retrieve Team Batters into the arrays m_arrayPitcherStatsIDs
	wxGetApp().pDBRoutines->DBGetPitcherStatsID( passedTeamID );

	for ( i=0; i<wxGetApp().pDBRoutines->m_arrayPitcherStatsIDs.GetCount(); i++)
	{
		wxGetApp().pDBRoutines->DBGetPitcherData( wxGetApp().pDBRoutines->m_arrayPitcherStatsIDs[i] );

		strexportData.Printf( _T("%s,%s,%s,"),
			wxGetApp().pDBRoutines->structTeamData.TeamName,
			wxGetApp().pDBRoutines->structPitcherData.FirstName,
			wxGetApp().pDBRoutines->structPitcherData.LastName);
		exportPitcher.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%3.2f,%i,%i,%i,%i,%i,%i,%i,%i,"),
			wxGetApp().pDBRoutines->structPitcherStats.InningsPitched,
			wxGetApp().pDBRoutines->structPitcherStats.ER,
			wxGetApp().pDBRoutines->structPitcherStats.Hits,
			wxGetApp().pDBRoutines->structPitcherStats.Walks,
			wxGetApp().pDBRoutines->structPitcherStats.Strikeouts,
			wxGetApp().pDBRoutines->structPitcherStats.HomeRuns,
//			// The value for Hold in the DB is actually the cursor position from the dialog
//			// Not the actual value
//			PHold[pitcherRecord.Hold],
			wxGetApp().pDBRoutines->structPitcherData.Hold,
			wxGetApp().pDBRoutines->structPitcherStats.Wins,
			wxGetApp().pDBRoutines->structPitcherStats.Loss);
		exportPitcher.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%i,%i,%i,%c,%2.2f,%2.2f,%2.2f,"),
			wxGetApp().pDBRoutines->structPitcherStats.Saves,
			wxGetApp().pDBRoutines->structPitcherData.Starter,
			wxGetApp().pDBRoutines->structPitcherData.Relief,
			BatterHits[wxGetApp().pDBRoutines->structPitcherData.Throws],
			wxGetApp().pDBRoutines->structPitcherData.OBChanceBasic,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceLeft,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceRight);
		exportPitcher.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%1.2f,%1.2f,"),
			wxGetApp().pDBRoutines->structPitcherStats.ERA,
			wxGetApp().pDBRoutines->structPitcherStats.WHIP);
		exportPitcher.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%i,"),
			wxGetApp().pDBRoutines->structPitcherStats.Starts);
		exportPitcher.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%i,%i,"),
			wxGetApp().pDBRoutines->structPitcherStats.Games,
			wxGetApp().pDBRoutines->structPitcherStats.CompleteGames);
		exportPitcher.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,"),
			wxGetApp().pDBRoutines->structPitcherData.OBChanceSingle,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceDouble,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceTriple,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRun,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceWalk,
			wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlay);
		exportPitcher.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,"),
			wxGetApp().pDBRoutines->structPitcherData.OBChanceSingleLeft,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceDoubleLeft,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceTripleLeft,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRunLeft,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceWalkLeft,
			wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlayLeft);
		exportPitcher.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,%2.2f,"),
			wxGetApp().pDBRoutines->structPitcherData.OBChanceSingleRight,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceDoubleRight,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceTripleRight,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRunRight,
			wxGetApp().pDBRoutines->structPitcherData.OBChanceWalkRight,
			wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlayRight);
		exportPitcher.Write( strexportData,strexportData.Length() );

		strexportData.Printf( _T("%i,%i,%i,%i,%c\n"),
			wxGetApp().pDBRoutines->structPitcherData.WP,
			wxGetApp().pDBRoutines->structPitcherData.Balk,
			wxGetApp().pDBRoutines->structPitcherData.Pitcher,
			wxGetApp().pDBRoutines->structPitcherData.ER1,
			Bunting[wxGetApp().pDBRoutines->structPitcherData.Bunting]);
		exportPitcher.Write( strexportData,strexportData.Length() );
	}

	exportPitcher.Close();
}
