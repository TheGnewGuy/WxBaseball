/////////////////////////////////////////////////////////////////////////////
// Name:          FileRoutines.cpp                                         //
// Purpose:       Baseball statistics tracking tool for Strat-O-Matic      //
// Author:        John Gnew                                                //
// Created:       09/22/24                                                 //
// Copyright:     (c) John Gnew                                            //
// Modifications:                                                          //
//   Date       Description                                                //
// 01/09/24    In BuildPlayerStats the RBI section was displaying hits     //
// 03/13/25    In BuildPlayerStats corrected formatting, tested for float  //
//             values of Nan (not a number) and corrected handeling for    //
//             ERA and WHIP.                                               //
// 03/18/25    Changed calculation for OBP                                 //
// 04/01/25    Firstname and Lastname were backwards in export title       //
// 11/06/25    Add HittingCard to ExportTeam                               //
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

void FileRoutines::ExportTeam( int passedTeamID )
{
	wxFile exportBatter;
	wxFile exportPitcher;
	wxString exportFileNameBatter;
	wxString exportFileNamePitcher;
	char exportData[200];
	wxString strexportData;
	char Bunting[5] = {'A','B','C','D','E'};
	char HitRun[4] = {'A','B','C','D'};
	wxString Stealing[7] = { "AAA", "AA", "A", "B", "C", "D", "E" };
	char BatterHits[3] = { 'L', 'R', 'S' };
	char Power[2] = {'N','W'};
	int Running[15] = {5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
	int TRate[21] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
	int Pass[21] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
	int OutArm[12] = {-6,-5,-4,-3,-2,-1,0,1,2,3,4,5};
	int CatchArm[10] = {-4,-3,-2,-1,0,1,2,3,4,5};
	int PHold[16] = {-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9};

	wxString LeagueNameSave;
	wxString LeagueDir;
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
    strexportData.Printf( _T("Team Name,FirstName,LastName,AB,Runs,Hits,RBI,2B,3B,HR,W,K,RE,") );
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

	for ( i=0; i<(int)wxGetApp().pDBRoutines->m_arrayBatterStatsIDs.GetCount(); i++)
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

    strexportData.Printf( _T("Team Name,FirstName,LastName,IP,ER,Hits,Walks,Strikeouts,") );
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
    strexportData.Printf( _T("Wild Pitch,Balk,Fielding,ErrorRating,Bunting,HittingCard\n") );
	exportPitcher.Write( strexportData,strexportData.Length() );

	// Retrieve Team Batters into the arrays m_arrayPitcherStatsIDs
	wxGetApp().pDBRoutines->DBGetPitcherStatsID( passedTeamID );

	for ( i=0; i<(int)wxGetApp().pDBRoutines->m_arrayPitcherStatsIDs.GetCount(); i++)
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

		strexportData.Printf( _T("%1.2f,%1.3f,"),
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

		strexportData.Printf( _T("%i,%i,%i,%i,%c,%i\n"),
			wxGetApp().pDBRoutines->structPitcherData.WP,
			wxGetApp().pDBRoutines->structPitcherData.Balk,
			wxGetApp().pDBRoutines->structPitcherData.Pitcher,
			wxGetApp().pDBRoutines->structPitcherData.ER1,
			Bunting[wxGetApp().pDBRoutines->structPitcherData.Bunting],
			wxGetApp().pDBRoutines->structPitcherData.HittingCard);
		exportPitcher.Write( strexportData,strexportData.Length() );
	}

	exportPitcher.Close();
}

void FileRoutines::BuildPlayerStats(int leagueID, int conferenceID, int divisionID)
{
//	CFile HTMLFile;
//	char HTMLData[200];
	wxArrayString strHTMLData;
	wxArrayString strArrayHTMLData1;
	wxArrayString strArrayHTMLData2;
	wxArrayString strArrayHTMLData3;
//	CString strHTMLData;
//	CStringArray strArrayHTMLData1;
//	CStringArray strArrayHTMLData2;
//	CStringArray strArrayHTMLData3;
//	char datebuf[9], timebuf[9];
//	CString strIndex;
//	CString strBackgroundPicture;
//	CString strLinkColor;
//	CString strVLinkColor;
//	CString strBGColor;
//	CString strTextColor;
//
//	m_LeagueRecord leagueRecord;
//	m_ConferenceRecord conferenceRecord;
//	m_DivisionRecord divisionRecord;
//	m_TeamRecord teamRecord;
	int rcSqlStepTeam = 0;
	int rc = 0;
//	CHAR buffer[100];
	wxString sqlTeam;
	wxString sqlSelect;
	sqlite3_stmt *localStmtTeam;
	sqlite3_stmt *localStmtSelect;
	int teamID = 0;
	int sumAB = 0;
	int sumRuns = 0;
	int sumHits = 0;
	int sumPHits = 0;
	int sumRBI = 0;
	int sumDouble = 0;
	int sumTriple = 0;
	int sumHomeRun = 0;
	int sumPHomeRun = 0;
	int sumWalk = 0;
	int sumROE = 0;
	int sumSacrifice = 0;
	int sumStollen = 0;
	int sumHBP = 0;
	int sumCS = 0;
	int sumWins = 0;
	int sumLoss = 0;
	float sumInningsPitched = 0.0;
	int sumER = 0;
	int sumWalks = 0;
	int sumStrikeouts = 0;
	int totalWins = 0;
	int totalLoss = 0;
	float totalInningsPitched = 0.0;
	int totalER = 0;
	int totalWalks = 0;
	int totalStrikeouts = 0;

	int totalAB = 0;
	int totalRuns = 0;
	int totalHits = 0;
	int totalPHits = 0;
	int totalRBI = 0;
	int totalDouble = 0;
	int totalTriple = 0;
	int totalHomeRun = 0;
	int totalPHomeRun = 0;
	int totalWalk = 0;
	int totalROE = 0;
	int totalSacrifice = 0;
	int totalStollen = 0;
	int totalHBP = 0;
	int totalCS = 0;
	wxString strAVG;
	float fBA;
	float fSLG;
	float fOBP;
	float fERA;
	float fWHIP;
	wxString teamNameShort;
	wxString firstName;
	wxString lastName;
	wxString playerName;
	int homeRuns = 0;
	int atBats = 0;
	int runs = 0;
	int hits = 0;
	int rbi = 0;
	float avg;
	float slg;
	float obp;
	int doubles;
	int triples;
	int stolenBase = 0;
	int cs = 0;
	int strikeouts = 0;
	int walks = 0;
	int er = 0;
	float era;
	float ip;
	float whip;
	int wins = 0;
	int loss = 0;
	int starts = 0;
	int saves = 0;

	wxFile TXTFile;
	wxString TXTFileNameStats;
	wxString strexportData;
//	wxString exportFileNamePitcher;

	wxString LeagueNameSave;
	wxString LeagueDir;
//	int i;
	wxString MsgBuffer;
//	int LeagueID;
//
//	LeagueID = wxGetApp().pDBRoutines->DBGetLeague();
//	// This will populate the structLeagueData
//	wxGetApp().pDBRoutines->DBGetLeague( LeagueID );
//	// Now we need to get arrays with conference and division names
//	wxGetApp().pDBRoutines->DBGetLeagueConferenceDivision( LeagueID );

	wxGetApp().pDBRoutines->DBGetConference( conferenceID );
	wxGetApp().pDBRoutines->DBGetDivision( divisionID );

	// Check to see if the league directory exists
	LeagueNameSave = wxGetApp().pDBRoutines->structLeagueData.LeagueName;
	LeagueNameSave.Replace(" ", "_", true);
	LeagueDir = wxGetApp().g_DataDirectory + _T("/") + LeagueNameSave;

	if ( !wxDirExists( LeagueDir ) )
	{
		// If it does not exist, then create it with permissions 755
		wxMkdir( LeagueDir, wxS_IRUSR|wxS_IWUSR|wxS_IXUSR|wxS_IRGRP|wxS_IXGRP|wxS_IROTH|wxS_IXOTH );
	}

	// Create file name for the league stats file
	if ( divisionID == 1)
	{
		TXTFileNameStats = wxGetApp().g_DataDirectory + _T("/") + LeagueNameSave +
			_T("/") + wxGetApp().pDBRoutines->structLeagueData.LeagueName +
			_T("_") + wxGetApp().pDBRoutines->structConferenceData.ConferenceName +
			_T(".txt");
	}
	else
	{
		TXTFileNameStats = wxGetApp().g_DataDirectory + _T("/") + LeagueNameSave +
			_T("/") + wxGetApp().pDBRoutines->structLeagueData.LeagueName +
			_T("_") + wxGetApp().pDBRoutines->structConferenceData.ConferenceName +
			_T("_") + wxGetApp().pDBRoutines->structDivisionData.DivisionName + _T(".txt");
	}
	TXTFileNameStats.Replace(" ", "_", true);

	// Process the league stats file
	TXTFile.Create( TXTFileNameStats, true, wxS_DEFAULT);
	TXTFile.Open(TXTFileNameStats, wxFile::read_write);

    strexportData.Printf( _T("League: %s\n"), wxGetApp().pDBRoutines->structLeagueData.LeagueName );
	TXTFile.Write( strexportData,strexportData.Length() );

	if ( conferenceID != 1)
	{
		strexportData.Printf( _T("Conference: %s\n"), wxGetApp().pDBRoutines->structConferenceData.ConferenceName );
		TXTFile.Write( strexportData,strexportData.Length() );
	}
	if ( divisionID != 1)
	{
		strexportData.Printf( _T("Division: %s\n"), wxGetApp().pDBRoutines->structDivisionData.DivisionName );
		TXTFile.Write( strexportData,strexportData.Length() );
	}

    strexportData.Printf( _T("\n\nBATTING\n") );
	TXTFile.Write( strexportData,strexportData.Length() );
    strexportData.Printf( _T("    TEAM              AVG   SLG   OBP    AB    R     H  RBI   2B   3B  HR   SB   CS\n"));
	TXTFile.Write( strexportData,strexportData.Length() );


	/* Create SQL statement */
	sqlTeam = "SELECT "  \
		"T.TeamID, " \
		"sum(B.AB) AS sumAB, " \
		"sum(B.Runs), " \
		"sum(B.Hits) as sumHits, " \
		"sum(B.RBI), " \
		"sum(B.Doubles)," \
		"sum(B.Triples), " \
		"sum(B.HomeRuns), " \
		"sum(B.Walk), " \
		"sum(B.ReachedOnError), " \
		"sum(B.Sacrifice), " \
		"sum(B.StolenBase), " \
		"sum(B.HBP), " \
		"sum(B.CS) " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as B " \
		"ON T.TeamID = B.TeamID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"GROUP BY T.TeamID " \
		"ORDER BY (CAST(sumAB AS FLOAT) / sumHits) ASC";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlTeam, strlen(sqlTeam), &localStmtTeam, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for BATTERSTATS Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtTeam, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtTeam, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtTeam, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	rcSqlStepTeam = sqlite3_step(localStmtTeam);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		// Get ID of team
		teamID = sqlite3_column_int(localStmtTeam, 0);
		sumAB = sqlite3_column_int(localStmtTeam, 1);
		sumRuns = sqlite3_column_int(localStmtTeam, 2);
		sumHits = sqlite3_column_int(localStmtTeam, 3);
		sumRBI = sqlite3_column_int(localStmtTeam, 4);
		sumDouble = sqlite3_column_int(localStmtTeam, 5);
		sumTriple = sqlite3_column_int(localStmtTeam, 6);
		sumHomeRun = sqlite3_column_int(localStmtTeam, 7);
		sumWalk = sqlite3_column_int(localStmtTeam, 8);
		sumROE = sqlite3_column_int(localStmtTeam, 9);
		sumSacrifice = sqlite3_column_int(localStmtTeam, 10);
		sumStollen = sqlite3_column_int(localStmtTeam, 11);
		sumHBP = sqlite3_column_int(localStmtTeam, 12);
		sumCS = sqlite3_column_int(localStmtTeam, 13);
		totalAB += sumAB;
		totalRuns += sumRuns;
		totalHits += sumHits;
		totalRBI += sumRBI;
		totalDouble += sumDouble;
		totalTriple += sumTriple;
		totalHomeRun += sumHomeRun;
		totalWalk += sumWalk;
		totalROE += sumROE;
		totalSacrifice += sumSacrifice;
		totalStollen += sumStollen;
		totalHBP += sumHBP;
		totalCS += sumCS;

		fBA = (float)sumHits / sumAB;
		fSLG = (float)((sumHits - (sumDouble + sumTriple + sumHomeRun)) + (2 * sumDouble) + (3 * sumTriple) + (4 * sumHomeRun)) / (sumAB);
//		fOBP = (float)(sumHits + sumWalk + sumROE + sumSacrifice + sumStollen) / (sumAB + sumWalk + sumROE + sumSacrifice + sumStollen);
		fOBP = (float)(sumHits + sumWalk + sumHBP) / (sumAB + sumWalk + sumHBP + sumSacrifice);
		if ( isnan(fBA) )
			fBA = 0;
		if ( isnan(fSLG) )
			fSLG = 0;
		if ( isnan(fOBP) )
			fOBP = 0;

		wxGetApp().pDBRoutines->DBGetTeam(teamID);

		strexportData.Printf( _T("%s %-15.15s %1.3f %1.3f %1.3f %5i %4i %5i %4i %4i %4i %3i %4i %4i\n"),
			wxGetApp().pDBRoutines->structTeamData.TeamNameShort, wxGetApp().pDBRoutines->structTeamData.TeamName,
			fBA, fSLG, fOBP, sumAB, sumRuns, sumHits, sumRBI,
			sumDouble, sumTriple, sumHomeRun, sumStollen, sumCS);
		TXTFile.Write( strexportData,strexportData.Length() );

		// Get next team
		rcSqlStepTeam = sqlite3_step(localStmtTeam);
	}
	sqlite3_finalize(localStmtTeam);

	strexportData.Printf( _T("--- --------------- ----- ----- ----- ----- ---- ----- ---- ---- ---- --- ---- ----\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	fBA = (float)totalHits / totalAB;
// The float value "-nan(0x400000)" represents a "Not a Number"
	if ( isnan(fBA) )
		fBA = 0;
	fSLG = (float)((totalHits - (totalDouble + totalTriple + totalHomeRun)) + (2 * totalDouble) + (3 * totalTriple) + (4 * totalHomeRun)) / (totalAB);
	if ( isnan(fSLG) )
		fSLG = 0;
	fOBP = (float)(totalHits + totalWalk + totalHBP) / (totalAB + totalWalk + totalHBP + totalSacrifice);
	if ( isnan(fOBP) )
		fOBP = 0;
	strexportData.Printf( _T("LEAGUE TOTALS       %1.3f %1.3f %1.3f %5i %4i %5i %4i %4i %4i %3i %4i %4i\n"),
		fBA, fSLG, fOBP, totalAB, totalRuns, totalHits, totalRBI,
		totalDouble, totalTriple, totalHomeRun, totalStollen, totalCS);
	TXTFile.Write( strexportData,strexportData.Length() );

// Start of Pitching section
	strexportData.Printf( _T("\nPITCHING\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	strexportData.Printf( _T("    TEAM              ERA   WHIP  Wins  Loss      IP  Hits   ER  HR Walks    K\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	/* Create SQL statement */
	sqlTeam = "SELECT "  \
		"T.TeamID, " \
		"sum(P.Wins) AS wins, " \
		"sum(P.Loss) AS loss, " \
		"sum(CAST(P.InningsPitched AS FLOAT)) AS sumIP, " \
		"sum(P.ER) AS sumER, " \
		"sum(P.Hits), " \
		"sum(P.HomeRuns), " \
		"sum(P.Walks), " \
		"sum(P.Strikeouts) " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as P " \
		"ON T.TeamID = P.TeamID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"GROUP BY T.TeamID " \
		"ORDER BY wins DESC, loss ASC";
//	"ORDER BY ((CAST(sumER AS FLOAT) * 9) / CAST(sumIP AS FLOAT)) ASC";


	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlTeam, strlen(sqlTeam), &localStmtTeam, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for PitcherStats Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtTeam, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtTeam, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtTeam, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	rcSqlStepTeam = sqlite3_step(localStmtTeam);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		// Get ID of team
		teamID = sqlite3_column_int(localStmtTeam, 0);
		sumWins = sqlite3_column_int(localStmtTeam, 1);
		sumLoss = sqlite3_column_int(localStmtTeam, 2);
		sumInningsPitched = (float)sqlite3_column_double(localStmtTeam, 3);
		sumER = sqlite3_column_int(localStmtTeam, 4);
		sumPHits = sqlite3_column_int(localStmtTeam, 5);
		sumPHomeRun = sqlite3_column_int(localStmtTeam, 6);
		sumWalks = sqlite3_column_int(localStmtTeam, 7);
		sumStrikeouts = sqlite3_column_int(localStmtTeam, 8);
		totalWins += sumWins;
		totalLoss += sumLoss;
		totalInningsPitched += sumInningsPitched;
		totalER += sumER;
		totalPHits += sumPHits;
		totalPHomeRun += sumPHomeRun;
		totalWalks += sumWalks;
		totalStrikeouts += sumStrikeouts;

		fERA = ((float)sumER * 9) / sumInningsPitched;
// The float value "-nan(0x400000)" represents a "Not a Number"
		if ( isnan(fERA) )
			fERA = 0;
		fWHIP = (float)(sumPHits + sumWalks) / sumInningsPitched;
		if ( isnan(fWHIP) )
			fWHIP = 0;

		wxGetApp().pDBRoutines->DBGetTeam(teamID);

		strexportData.Printf( _T("%s %-15.15s %5.2f  %5.3f   %3i   %3i  %6.1f %5i %4i %3i  %4i %4i\n"),
			wxGetApp().pDBRoutines->structTeamData.TeamNameShort, wxGetApp().pDBRoutines->structTeamData.TeamName,
			fERA, fWHIP, sumWins, sumLoss, sumInningsPitched, sumPHits,
			sumER, sumPHomeRun, sumWalks, sumStrikeouts);
		TXTFile.Write( strexportData,strexportData.Length() );

		// Get next team
		rcSqlStepTeam = sqlite3_step(localStmtTeam);
	}
	sqlite3_finalize(localStmtTeam);

	strexportData.Printf( _T("--- --------------- -----  -----  ----  ---- -------  ---- ---- ---   ---  ---\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	fERA = ((float)totalER * 9) / totalInningsPitched;
	fWHIP = (float)(totalHits + totalWalks) / totalInningsPitched;
	if ( isnan(fERA) )
		fERA = 0;
	if ( isnan(fWHIP) )
		fWHIP = 0;
	strexportData.Printf( _T("LEAGUE TOTALS       %5.2f  %5.3f   %3i   %3i  %6.1f %5i %4i %3i  %4i %4i\n\n"),
		fERA, fWHIP, totalWins, totalLoss, totalInningsPitched, totalPHits, totalER, totalPHomeRun, totalWalks, totalStrikeouts);
	TXTFile.Write( strexportData,strexportData.Length() );

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.AB " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.AB DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData1.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		atBats = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i  "),
			teamNameShort, playerName, atBats);
		strArrayHTMLData1.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.Runs " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.Runs DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData2.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		runs = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s  %4i  "),
			teamNameShort, playerName, runs);
		strArrayHTMLData2.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.Hits " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.Hits DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData3.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		hits = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i\n"),
			teamNameShort, playerName, hits);
		strArrayHTMLData3.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	strexportData.Printf( _T("At Bats                    Runs                        Hits\n"));
	TXTFile.Write( strexportData,strexportData.Length() );


	for (int i = 0; i < (int)strArrayHTMLData3.Count(); i++)
	{
		strexportData = strArrayHTMLData1[i] + strArrayHTMLData2[i] + strArrayHTMLData3[i];
		TXTFile.Write( strexportData,strexportData.Length() );
	}

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.RBI " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.RBI DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData1.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		rbi = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i  "),
			teamNameShort, playerName, rbi);
		strArrayHTMLData1.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.AVG " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.AVG DESC, BS.AB DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData2.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		avg = (float)sqlite3_column_double(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %1.3f  "),
			teamNameShort, playerName, avg);
		strArrayHTMLData2.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.SLG " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.SLG DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData3.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		slg = (float)sqlite3_column_double(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %1.3f\n"),
			teamNameShort, playerName, slg);
		strArrayHTMLData3.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	strexportData.Printf( _T("\nRBI's                      Batting Averages            Slugging Percentages\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	for (int i = 0; i < (int)strArrayHTMLData3.Count(); i++)
	{
		strexportData = strArrayHTMLData1[i] + strArrayHTMLData2[i] + strArrayHTMLData3[i];
		TXTFile.Write( strexportData,strexportData.Length() );
	}

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.OBP " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.OBP DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData1.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		obp = (float)sqlite3_column_double(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %1.3f "),
			teamNameShort, playerName, obp);
		strArrayHTMLData1.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.Doubles " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.Doubles DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData2.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		doubles = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s  %4i  "),
			teamNameShort, playerName, doubles);
		strArrayHTMLData2.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.Triples " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.Triples DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Select OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData3.clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		triples = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i\n"),
			teamNameShort, playerName, triples);
		strArrayHTMLData3.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	strexportData.Printf( _T("\nOn Base Percentage         Doubles                     Triples\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	for (int i = 0; i < (int)strArrayHTMLData3.Count(); i++)
	{
		strexportData = strArrayHTMLData1[i] + strArrayHTMLData2[i] + strArrayHTMLData3[i];
		TXTFile.Write( strexportData,strexportData.Length() );
	}

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.HomeRuns " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.HomeRuns DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT HomeRuns OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData1.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		// Get ID of team
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		homeRuns = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i  "),
			teamNameShort, playerName, homeRuns);
		strArrayHTMLData1.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.StolenBase " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.StolenBase DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Stollenbase OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData2.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		stolenBase = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s  %4i  "),
			teamNameShort, playerName, stolenBase);
		strArrayHTMLData2.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.CS " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.CS DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT CS OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData3.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		cs = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i\n"),
			teamNameShort, playerName, cs);
		strArrayHTMLData3.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	strexportData.Printf( _T("\nHome Runs                  Stolen Basses               Caught Stealing\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	for (int i = 0; i < (int)strArrayHTMLData3.Count(); i++)
	{
		strexportData = strArrayHTMLData1[i] + strArrayHTMLData2[i] + strArrayHTMLData3[i];
		TXTFile.Write( strexportData,strexportData.Length() );
	}

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.Stirkeout " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.Stirkeout DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Strikeouts OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData1.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		// Get ID of team
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		strikeouts = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i  "),
			teamNameShort, playerName, strikeouts);
		strArrayHTMLData1.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"B.FirstName, " \
		"B.LastName, " \
		"BS.Walk " \
		"FROM TEAM AS T " \
		"JOIN BATTERSTATS as BS " \
		"ON T.TeamID = BS.TeamID " \
		"JOIN BATTER as B " \
		"ON BS.BatterID = B.BatterID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY BS.Walk DESC, BS.AVG DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Walks OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData2.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		walks = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s  %4i\n"),
			teamNameShort, playerName, walks);
		strArrayHTMLData2.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	strexportData.Printf( _T("\nStrike Outs                Walks\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	for (int i = 0; i < (int)strArrayHTMLData3.Count(); i++)
	{
		strexportData = strArrayHTMLData1[i] + strArrayHTMLData2[i];
		TXTFile.Write( strexportData,strexportData.Length() );
	}

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.ER " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY PS.ER DESC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT er OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData1.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		er = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i  "),
			teamNameShort, playerName, er);
		strArrayHTMLData1.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	// Limit on InningsPitched set to greater that 4 innings
	// Need better limit. A pitcher can have ERA of 0 but there are
	// way too many with zero that have not pitched many innings.
	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.ERA " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 AND PS.InningsPitched > 4 " \
		"ORDER BY PS.ERA ASC, PS.InningsPitched DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT ERA OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData2.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		era = (float)sqlite3_column_double(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %5.2f "),
			teamNameShort, playerName, era);
		strArrayHTMLData2.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);
	if (strArrayHTMLData2.IsEmpty())
		strArrayHTMLData2.Add("All Pitchers have 0 ERA    ");

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.InningsPitched " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY PS.InningsPitched DESC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT IP OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData3.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		ip = (float)sqlite3_column_double(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %6.1f\n"),
			teamNameShort, playerName, ip);
		strArrayHTMLData3.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	strexportData.Printf( _T("\nEarned Runs                Earned Run Average         Innings Pitched\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

//	MsgBuffer.Printf( wxT("Array1: %i, Array2: %i, Array3: %i\n"), (int)strArrayHTMLData1.Count(), (int)strArrayHTMLData2.Count(), (int)strArrayHTMLData3.Count() );
//	wxMessageBox(MsgBuffer);

	for (int i = 0; i < (int)strArrayHTMLData1.Count(); i++)
	{
		if ( i < strArrayHTMLData2.Count() )
		{
			strexportData = strArrayHTMLData1[i] + strArrayHTMLData2[i] + strArrayHTMLData3[i];
			TXTFile.Write( strexportData,strexportData.Length() );
		}
		else
		{
//			strexportData = strArrayHTMLData1[i] + "    1234567890123456 123456" + strArrayHTMLData3[i];
			strexportData = strArrayHTMLData1[i] + "                           " + strArrayHTMLData3[i];
			TXTFile.Write( strexportData,strexportData.Length() );
		}
	}

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.Hits " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY PS.Hits DESC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Hits OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData1.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		hits = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i  "),
			teamNameShort, playerName, hits);
		strArrayHTMLData1.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.WHIP " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 AND PS.InningsPitched > 0 " \
		"ORDER BY PS.WHIP ASC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT WHIP OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData2.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		whip = (float)sqlite3_column_double(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %5.3f "),
			teamNameShort, playerName, whip);
		strArrayHTMLData2.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);
	if (strArrayHTMLData2.IsEmpty())
		strArrayHTMLData2.Add("All Pitchers have 0 WHIP   ");

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.Walks " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY PS.Walks DESC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Walk OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData3.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		walks = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i\n"),
			teamNameShort, playerName, walks);
		strArrayHTMLData3.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	strexportData.Printf( _T("\nHits Allowed               WHIP                       Walks Allowed\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	for (int i = 0; i < (int)strArrayHTMLData1.Count(); i++)
	{
		if ( i < strArrayHTMLData2.Count() )
		{
			strexportData = strArrayHTMLData1[i] + strArrayHTMLData2[i] + strArrayHTMLData3[i];
			TXTFile.Write( strexportData,strexportData.Length() );
		}
		else
		{
//			strexportData = strArrayHTMLData1[i] + "    1234567890123456 123456" + strArrayHTMLData3[i];
			strexportData = strArrayHTMLData1[i] + "                           " + strArrayHTMLData3[i];
			TXTFile.Write( strexportData,strexportData.Length() );
		}
	}

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.Strikeouts " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY PS.Strikeouts DESC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT strikeouts OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData1.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		strikeouts = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i  "),
			teamNameShort, playerName, strikeouts);
		strArrayHTMLData1.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.Wins " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY PS.Wins DESC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Wins OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData2.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		wins = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s  %4i "),
			teamNameShort, playerName, wins);
		strArrayHTMLData2.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.Loss " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY PS.Loss DESC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Loss OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData3.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		loss = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i\n"),
			teamNameShort, playerName, loss);
		strArrayHTMLData3.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	strexportData.Printf( _T("\nStrikeouts                 Wins                       Loss\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	for (int i = 0; i < (int)strArrayHTMLData1.Count(); i++)
	{
		strexportData = strArrayHTMLData1[i] + strArrayHTMLData2[i] + strArrayHTMLData3[i];
		TXTFile.Write( strexportData,strexportData.Length() );
	}

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.Starts " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY PS.Starts DESC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Starts OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData1.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		starts = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i  "),
			teamNameShort, playerName, starts);
		strArrayHTMLData1.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.Saves " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY PS.Saves DESC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Saves OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData2.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		saves = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s  %4i "),
			teamNameShort, playerName, saves);
		strArrayHTMLData2.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	/* Create SQL statement */
	sqlSelect = "SELECT "  \
		"T.TeamNameShort, " \
		"P.FirstName, " \
		"P.LastName, " \
		"PS.HomeRuns " \
		"FROM TEAM AS T " \
		"JOIN PITCHERSTATS as PS " \
		"ON T.TeamID = PS.TeamID " \
		"JOIN PITCHER as P " \
		"ON PS.PitcherID = P.PitcherID " \
		"WHERE T.LeagueID = ?1 AND T.ConferenceID = ?2 AND T.DivisionID = ?3 " \
		"ORDER BY PS.HomeRuns DESC, PS.ERA DESC LIMIT 10";

	rc = sqlite3_prepare_v2(wxGetApp().pDBRoutines->m_db, sqlSelect, strlen(sqlSelect), &localStmtSelect, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for SELECT Homeruns OK:\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
//		wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtSelect, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 2, conferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(localStmtSelect, 3, divisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind divisionID int: %s\n"), sqlite3_errmsg(wxGetApp().pDBRoutines->m_db));
		wxMessageBox(MsgBuffer);
	}

	strArrayHTMLData3.Clear();
	rcSqlStepTeam = sqlite3_step(localStmtSelect);
	while (rcSqlStepTeam == SQLITE_ROW)
	{
		teamNameShort = sqlite3_column_text(localStmtSelect, 0);
		firstName = sqlite3_column_text(localStmtSelect, 1);
		lastName = sqlite3_column_text(localStmtSelect, 2);
		homeRuns = sqlite3_column_int(localStmtSelect, 3);

		playerName = firstName + _T(" ") + lastName;
		strexportData.Printf( _T("%s %-16.16s %4i\n"),
			teamNameShort, playerName, homeRuns);
		strArrayHTMLData3.Add(strexportData);

		// Get next player
		rcSqlStepTeam = sqlite3_step(localStmtSelect);
	}
	sqlite3_finalize(localStmtSelect);

	strexportData.Printf( _T("\nStarts                     Saves                      HomeRuns Allowed\n"));
	TXTFile.Write( strexportData,strexportData.Length() );

	for (int i = 0; i < (int)strArrayHTMLData1.Count(); i++)
	{
		strexportData = strArrayHTMLData1[i] + strArrayHTMLData2[i] + strArrayHTMLData3[i];
		TXTFile.Write( strexportData,strexportData.Length() );
	}

	// Write the Last date and time Updated line in the file
	strexportData.Printf( _T("\nLast Updated on %s at %2.2i:%2.2i.\n"),
		wxDateTime::Today().FormatDate(), wxDateTime::GetTmNow()->tm_hour, wxDateTime::GetTmNow()->tm_min );
	TXTFile.Write( strexportData,strexportData.Length() );

	TXTFile.Close();
}
