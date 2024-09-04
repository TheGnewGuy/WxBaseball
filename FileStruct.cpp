/////////////////////////////////////////////////////////////////////////////
// Name:          FileStruct.cpp                                           //
// Purpose:       Baseball statistics tracking tool for Strat-O-Matic      //
// Author:        John Gnew                                                //
// Created:       08/01/05                                                 //
// Copyright:     (c) John Gnew                                            //
// Modifications:                                                          //
//   Date       Description                                                //
// 08/26/09     Changed the strcpy to strncpy in the line                  //
//              strncpy(temp,"All Base Teams                ",30);         //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
//       Set up TeamStruct                                                 //
//       Set up LeagueStruct                                               //
//       Set up Options file                                               //
//       make wxGetApp().g_DataDir access global from one location         //
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/file.h>
    #include <wx/filename.h>
    #include <wx/filefn.h>
#endif
#include <wx/filename.h>
#include <wx/stdpaths.h>
// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------
#include "main.h"
#include "FileStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LENGTH_TEAM_SECTION 74
#define LENGTH_COUNT_SECTION 1

/////////////////////////////////////////////////////////////////////////////
// BatterStruct

BatterStruct::BatterStruct()
{
	m_RecordSize = 30+         // Player name
		sizeof(m_AB)+
		sizeof(m_Runs)+
		sizeof(m_Hits)+
		sizeof(m_RBI)+
		sizeof(m_2B)+
		sizeof(m_3B)+
		sizeof(m_HomeRuns)+
		sizeof(m_Walk)+
		sizeof(m_StrikeOut)+
		sizeof(m_ReachedOnError)+
		sizeof(m_Sacrifice)+
		sizeof(m_StolenBase)+
		sizeof(m_CS)+
		sizeof(m_Filler1)+
		sizeof(m_Filler2)+
		sizeof(m_Games)+
		sizeof(m_HBP)+
		sizeof(m_Pitcher)+
		sizeof(m_Catcher)+
		sizeof(m_FirstBase)+
		sizeof(m_SecondBase)+
		sizeof(m_ShortStop)+
		sizeof(m_ThirdBase)+
		sizeof(m_LeftField)+
		sizeof(m_CenterField)+
		sizeof(m_RightField)+
		sizeof(m_bER1)+
		sizeof(m_bER2)+
		sizeof(m_bER3)+
		sizeof(m_bER4)+
		sizeof(m_bER5)+
		sizeof(m_bER6)+
		sizeof(m_bER7)+
		sizeof(m_bER8)+
		sizeof(m_bER9)+
		sizeof(m_bBatterHits)+
		6+				// On Base Chance Basic
		6+				// On Base Chance Left
		6+				// On Base Chance Right
		6+				// On Base Chance Walk
		6+				// On Base Chance Single
		6+				// On Base Chance Double
		6+				// On Base Chance Triple
		6+				// On Base Chance HomeRun
		6+				// Chance Double Play
		6+				// On Base Chance Walk Right
		6+				// On Base Chance Single Right
		6+				// On Base Chance Double Right
		6+				// On Base Chance Triple Right
		6+				// On Base Chance HomeRun Right
		6+				// Chance Double Play Right
		6+				// On Base Chance Walk Left
		6+				// On Base Chance Single Left
		6+				// On Base Chance Double Left
		6+				// On Base Chance Triple Left
		6+				// On Base Chance HomeRun Left
		6+				// Chance Double Play Left
		6+				// Filler
		6+				// Filler
		6+				// Filler
		sizeof(m_bBunting)+
		sizeof(m_bHitRun)+
		sizeof(m_bRunning)+
		sizeof(m_bStealing)+
		sizeof(m_bTRate)+
		sizeof(m_bPass)+
		sizeof(m_bPowerL)+
		sizeof(m_bPowerR)+
		sizeof(m_bOutArm)+
		sizeof(m_bCatchArm)+
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte);	// Filler
	m_Filler1 = 0;
	m_Filler2 = 0;
	m_bFiller1 = 0;
	m_CS = 0;
	m_OBChanceBasic.Empty();
	m_OBChanceLeft.Empty();
	m_OBChanceRight.Empty();
}

BatterStruct::~BatterStruct()
{
}

int BatterStruct::UpdateBatter(wxString BatterFileName, int number)
{
	long SeekPosition;
	wxFile wxFileTeam;

    // Calcualte the correct position in the file. "number" is
    // the number of the player in the file
    SeekPosition = LENGTH_TEAM_SECTION + ( number * m_RecordSize );

	wxFileTeam.Open( BatterFileName, wxFile::read_write );
	wxFileTeam.Seek( SeekPosition, wxFromStart );
	BatterWrite( &wxFileTeam );
	// Close file
	wxFileTeam.Close();
	return 1;
}

int BatterStruct::AddBatter(wxString BatterFileName)
{
	wxByte count;
	wxFile wxFileTeam;

	count = 0;
	// Open file and write new player
	wxFileTeam.Open( BatterFileName, wxFile::read_write );
	wxFileTeam.Read( &count, sizeof(count) );
	count++;
	wxFileTeam.Seek( 0, wxFromStart );
	wxFileTeam.Write( &count, sizeof(count) );
	wxFileTeam.SeekEnd(0);
	BatterWrite( &wxFileTeam );
	// Close file
	wxFileTeam.Close();
	return 1;
}

int BatterStruct::GetBatter(wxString BatterFileName, int number)
{
	long SeekPosition;
	wxFile wxFileTeam;

    // Calcualte the correct position in the file. "number" is
    // the number of the player in the file
    SeekPosition = LENGTH_TEAM_SECTION + ( number * m_RecordSize );

	wxFileTeam.Open( BatterFileName, wxFile::read );
	wxFileTeam.Seek( SeekPosition, wxFromStart );
	BatterRead( &wxFileTeam );
	// Close file
	wxFileTeam.Close();
	return 1;
}

wxArrayString BatterStruct::ReturnAllBatters(wxString BatterFileName)
{
	wxArrayString my_BatterArray;
	int count;
	int i;

	count = GetCountBatter( BatterFileName );

	for (i=0; i<count; i++)
	{
    	GetBatter( BatterFileName, i );
    	my_BatterArray.Add( m_PlayerName.Mid(0,30) );
    }

	return my_BatterArray;
}

int BatterStruct::BatterWrite(wxFile* myFile)
{
	myFile->Write(m_PlayerName,30);
	myFile->Write(&m_AB,sizeof(m_AB));
	myFile->Write(&m_Runs,sizeof(m_Runs));
	myFile->Write(&m_Hits,sizeof(m_Hits));
	myFile->Write(&m_RBI,sizeof(m_RBI));
	myFile->Write(&m_2B,sizeof(m_2B));
	myFile->Write(&m_3B,sizeof(m_3B));
	myFile->Write(&m_HomeRuns,sizeof(m_HomeRuns));
	myFile->Write(&m_Walk,sizeof(m_Walk));
	myFile->Write(&m_StrikeOut,sizeof(m_StrikeOut));
	myFile->Write(&m_ReachedOnError,sizeof(m_ReachedOnError));
	myFile->Write(&m_Sacrifice,sizeof(m_Sacrifice));
	myFile->Write(&m_StolenBase,sizeof(m_StolenBase));
	myFile->Write(&m_CS,sizeof(m_CS));
	myFile->Write(&m_Filler1,sizeof(m_Filler1));
	myFile->Write(&m_Filler2,sizeof(m_Filler2));
	myFile->Write(&m_Games,sizeof(m_Games));
	myFile->Write(&m_HBP,sizeof(m_HBP));
	myFile->Write(&m_Pitcher,sizeof(m_Pitcher));
	myFile->Write(&m_Catcher,sizeof(m_Catcher));
	myFile->Write(&m_FirstBase,sizeof(m_FirstBase));
	myFile->Write(&m_SecondBase,sizeof(m_SecondBase));
	myFile->Write(&m_ShortStop,sizeof(m_ShortStop));
	myFile->Write(&m_ThirdBase,sizeof(m_ThirdBase));
	myFile->Write(&m_LeftField,sizeof(m_LeftField));
	myFile->Write(&m_CenterField,sizeof(m_CenterField));
	myFile->Write(&m_RightField,sizeof(m_RightField));
	myFile->Write(&m_bER1,sizeof(m_bER1));
	myFile->Write(&m_bER2,sizeof(m_bER2));
	myFile->Write(&m_bER3,sizeof(m_bER3));
	myFile->Write(&m_bER4,sizeof(m_bER4));
	myFile->Write(&m_bER5,sizeof(m_bER5));
	myFile->Write(&m_bER6,sizeof(m_bER6));
	myFile->Write(&m_bER7,sizeof(m_bER7));
	myFile->Write(&m_bER8,sizeof(m_bER8));
	myFile->Write(&m_bER9,sizeof(m_bER9));
	myFile->Write(&m_bBatterHits,sizeof(m_bBatterHits));
	myFile->Write(m_OBChanceBasic,6);
	myFile->Write(m_OBChanceLeft,6);
	myFile->Write(m_OBChanceRight,6);
	myFile->Write(m_OBChanceWalk,6);
	myFile->Write(m_OBChanceSingle,6);
	myFile->Write(m_OBChanceDouble,6);
	myFile->Write(m_OBChanceTriple,6);
	myFile->Write(m_OBChanceHomeRun,6);
	myFile->Write(m_ChanceDoublePlay,6);
	myFile->Write(m_OBChanceWalkRight,6);		// Filler
	myFile->Write(m_OBChanceSingleRight,6);
	myFile->Write(m_OBChanceDoubleRight,6);
	myFile->Write(m_OBChanceTripleRight,6);
	myFile->Write(m_OBChanceHomeRunRight,6);
	myFile->Write(m_ChanceDoublePlayRight,6);
	myFile->Write(m_OBChanceWalkLeft,6);
	myFile->Write(m_OBChanceSingleLeft,6);
	myFile->Write(m_OBChanceDoubleLeft,6);
	myFile->Write(m_OBChanceTripleLeft,6);
	myFile->Write(m_OBChanceHomeRunLeft,6);
	myFile->Write(m_ChanceDoublePlayLeft,6);
	myFile->Write("0     ",6);		// Filler
	myFile->Write("0     ",6);		// Filler
	myFile->Write("0     ",6);		// Filler
	myFile->Write(&m_bBunting,sizeof(m_bBunting));
	myFile->Write(&m_bHitRun,sizeof(m_bHitRun));
	myFile->Write(&m_bRunning,sizeof(m_bRunning));
	myFile->Write(&m_bStealing,sizeof(m_bStealing));
	myFile->Write(&m_bTRate,sizeof(m_bTRate));	// Place holder for T Rating
	myFile->Write(&m_bPass,sizeof(m_bPass));	// Place holder for Passball
	myFile->Write(&m_bPowerL,sizeof(m_bPowerL));	// Place holder for Power Left
	myFile->Write(&m_bPowerR,sizeof(m_bPowerR));	// Place holder for Power Right
	myFile->Write(&m_bOutArm,sizeof(m_bOutArm));	// Place holder for Outfielder Arm
	myFile->Write(&m_bCatchArm,sizeof(m_bCatchArm));	// Place holder for Catcher Arm
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	return 1;
}

int BatterStruct::BatterRead(wxFile* myFile)
{
	wxChar cTemp[30];
	wxChar cTemp6[6];
	wxByte bTemp;

	myFile->Read(cTemp,30);
	m_PlayerName.Printf("%30s",cTemp);
	myFile->Read(&m_AB,sizeof(m_AB));
	myFile->Read(&m_Runs,sizeof(m_Runs));
	myFile->Read(&m_Hits,sizeof(m_Hits));
	myFile->Read(&m_RBI,sizeof(m_RBI));
	myFile->Read(&m_2B,sizeof(m_2B));
	myFile->Read(&m_3B,sizeof(m_3B));
	myFile->Read(&m_HomeRuns,sizeof(m_HomeRuns));
	myFile->Read(&m_Walk,sizeof(m_Walk));
	myFile->Read(&m_StrikeOut,sizeof(m_StrikeOut));
	myFile->Read(&m_ReachedOnError,sizeof(m_ReachedOnError));
	myFile->Read(&m_Sacrifice,sizeof(m_Sacrifice));
	myFile->Read(&m_StolenBase,sizeof(m_StolenBase));
	myFile->Read(&m_CS,sizeof(m_CS));
	myFile->Read(&m_Filler1,sizeof(m_Filler1));
	myFile->Read(&m_Filler2,sizeof(m_Filler2));
	myFile->Read(&m_Games,sizeof(m_Games));
	myFile->Read(&m_HBP,sizeof(m_HBP));
	myFile->Read(&m_Pitcher,sizeof(m_Pitcher));
	myFile->Read(&m_Catcher,sizeof(m_Catcher));
	myFile->Read(&m_FirstBase,sizeof(m_FirstBase));
	myFile->Read(&m_SecondBase,sizeof(m_SecondBase));
	myFile->Read(&m_ShortStop,sizeof(m_ShortStop));
	myFile->Read(&m_ThirdBase,sizeof(m_ThirdBase));
	myFile->Read(&m_LeftField,sizeof(m_LeftField));
	myFile->Read(&m_CenterField,sizeof(m_CenterField));
	myFile->Read(&m_RightField,sizeof(m_RightField));
	myFile->Read(&m_bER1,sizeof(m_bER1));
	myFile->Read(&m_bER2,sizeof(m_bER2));
	myFile->Read(&m_bER3,sizeof(m_bER3));
	myFile->Read(&m_bER4,sizeof(m_bER4));
	myFile->Read(&m_bER5,sizeof(m_bER5));
	myFile->Read(&m_bER6,sizeof(m_bER6));
	myFile->Read(&m_bER7,sizeof(m_bER7));
	myFile->Read(&m_bER8,sizeof(m_bER8));
	myFile->Read(&m_bER9,sizeof(m_bER9));
	myFile->Read(&m_bBatterHits,sizeof(m_bBatterHits));
	myFile->Read(cTemp6,6);
	m_OBChanceBasic.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceWalk.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceSingle.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceDouble.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceTriple.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceHomeRun.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_ChanceDoublePlay.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceWalkRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceSingleRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceDoubleRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceTripleRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceHomeRunRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_ChanceDoublePlayRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceWalkLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceSingleLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceDoubleLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceTripleLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceHomeRunLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_ChanceDoublePlayLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	myFile->Read(cTemp6,6);
	myFile->Read(cTemp6,6);
	myFile->Read(&m_bBunting,sizeof(m_bBunting));
	myFile->Read(&m_bHitRun,sizeof(m_bHitRun));
	myFile->Read(&m_bRunning,sizeof(m_bRunning));
	myFile->Read(&m_bStealing,sizeof(m_bStealing));
	myFile->Read(&m_bTRate,sizeof(m_bTRate));	// Place holder for T Rating
	myFile->Read(&m_bPass,sizeof(m_bPass));	// Place holder for Passball
	myFile->Read(&m_bPowerL,sizeof(m_bPowerL));	// Place holder for Power Left
	myFile->Read(&m_bPowerR,sizeof(m_bPowerR));	// Place holder for Power Right
	myFile->Read(&m_bOutArm,sizeof(m_bOutArm));	// Place holder for Outfielder Arm
	myFile->Read(&m_bCatchArm,sizeof(m_bCatchArm));	// Place holder for Catcher Arm
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	return 1;
}

wxByte BatterStruct::GetCountBatter(wxString BatterFileName)
{
	wxByte count;
	wxFile myFile;
	myFile.Open( BatterFileName,wxFile::read);
	// Read Count
	myFile.Read(&count,sizeof(count));
	// Close file
	myFile.Close();
	return count;
}

int BatterStruct::CopyBatterFile(wxString inFileName, wxString outFileName)
{
	int i;
	wxByte count;
	long lSeekPlayerFile;
//	long lTeamSection = 74;
	wxFile inFile;
	wxFile outFile;
	wxChar cTemp[40];
	wxChar cTemp30[30];
	wxChar cTemp3[3];
	wxString strTemp;

	inFile.Open( inFileName, wxFile::read);
	outFile.Create( outFileName, TRUE);
	// Read Count
	inFile.Read(&count,sizeof(count));
	outFile.Write(&count,sizeof(count));
	// Read team name
	inFile.Read(cTemp,40);
	strTemp.Printf("%40s",cTemp);
	outFile.Write(strTemp,40);
	// Read short team name
	inFile.Read(cTemp3,3);
	strTemp.Printf("%c%c%c",cTemp3[0],cTemp3[1],cTemp3[2]);
	outFile.Write(strTemp,3);
	// Read Ballpark
	inFile.Read(cTemp30,30);
	strTemp.Printf("%30s",cTemp30);
	outFile.Write(strTemp,30);
	// Read all of the players
	for (i=0; i<count; i++)
	{
		lSeekPlayerFile = LENGTH_TEAM_SECTION + (i * m_RecordSize);
		inFile.Seek( lSeekPlayerFile, wxFromStart );
		BatterRead(&inFile);
		// Zero out fields
		m_2B = 0;
		m_3B = 0;
		m_AB = 0;
		m_CS = 0;
		m_Hits = 0;
		m_HomeRuns = 0;
		m_RBI = 0;
		m_ReachedOnError = 0;
		m_Runs = 0;
		m_Sacrifice = 0;
		m_StolenBase = 0;
		m_StrikeOut = 0;
		m_Walk = 0;
		m_HBP = 0;
		m_Games = 0;
		// Write Batter info at EOF, Already positioned there
		BatterWrite(&outFile);
	}
	// Close file
	inFile.Close();
	outFile.Close();
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// PitcherStruct

PitcherStruct::PitcherStruct()
{
	m_RecordSize = 30+     // Pitcher Name
		sizeof(m_Wins)+
		sizeof(m_Loss)+
		sizeof(m_Saves)+
		7+				// Innings Pitched 9999.99
		sizeof(m_ER)+
		sizeof(m_Hits)+
		sizeof(m_Walks)+
		sizeof(m_Strikeouts)+
		sizeof(m_HomeRuns)+
		sizeof(m_Filler1)+
		sizeof(m_Games)+
		sizeof(m_CompGames)+
		sizeof(m_Starts)+
		6+				// On Base Chance Basic
		6+				// On Base Chance Left
		6+				// On Base Chance Right
		6+				// On Base Chance Walk
		6+				// On Base Chance Single
		6+				// On Base Chance Double
		6+				// On Base Chance Triple
		6+				// On Base Chance HomeRun
		6+				// Chance Double Play
		6+				// On Base Chance Walk Right
		6+				// On Base Chance Single Right
		6+				// On Base Chance Double Right
		6+				// On Base Chance Triple Right
		6+				// On Base Chance HomeRun Right
		6+				// Chance Double Play Right
		6+				// On Base Chance Walk Left
		6+				// On Base Chance Single Left
		6+				// On Base Chance Double Left
		6+				// On Base Chance Triple Left
		6+				// On Base Chance HomeRun Left
		6+				// Chance Double Play Left
		6+				// Filler
		6+				// Filler
		6+				// Filler
		sizeof(m_Starter)+
		sizeof(m_Relief)+
		sizeof(m_Throws)+
		sizeof(m_Bunting)+
		sizeof(m_Hold)+
		sizeof(m_bWP)+
		sizeof(m_bBalk)+
		sizeof(m_bER1)+
		sizeof(m_PitcherField)+
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte)+	// Filler
		sizeof(wxByte);	// Filler

	m_Filler1 = 0;
	m_bFiller1 = 0;
	m_OBChanceBasic.Empty();
	m_OBChanceLeft.Empty();
	m_OBChanceRight.Empty();
}

PitcherStruct::~PitcherStruct()
{
}

int PitcherStruct::UpdatePitcher(wxString PitcherFileName, int number)
{
	long SeekPosition;
	wxFile wxFileTeam;

    SeekPosition = LENGTH_COUNT_SECTION + ( number * m_RecordSize );

	wxFileTeam.Open( PitcherFileName, wxFile::read_write );
	wxFileTeam.Seek( SeekPosition, wxFromStart );
	PitcherWrite( &wxFileTeam );
	// Close file
	wxFileTeam.Close();
	return 1;
}

int PitcherStruct::AddPitcher(wxString PitcherFileName)
{
	wxByte count;
	wxFile wxFileTeam;

	count = 0;
	// Open file and write new player
	wxFileTeam.Open( PitcherFileName, wxFile::read_write );
	wxFileTeam.Read( &count,sizeof(count) );
	count++;
	wxFileTeam.Seek( 0, wxFromStart );
	wxFileTeam.Write( &count,sizeof(count) );
	wxFileTeam.SeekEnd(0);
	PitcherWrite( &wxFileTeam );
	// Close file
	wxFileTeam.Close();
	return 1;
}

int PitcherStruct::GetPitcher(wxString PitcherFileName, int number)
{
	long SeekPosition;
	wxFile wxFileTeam;

    SeekPosition = LENGTH_COUNT_SECTION + ( number * m_RecordSize );

	wxFileTeam.Open( PitcherFileName, wxFile::read );
	wxFileTeam.Seek( SeekPosition, wxFromStart );
	PitcherRead( &wxFileTeam );
	// Close file
	wxFileTeam.Close();
	return 1;
}

wxArrayString PitcherStruct::ReturnAllPitchers(wxString PitcherFileName)
{
	wxArrayString my_PitcherArray;
	int count;
	int i;

	count = GetCountPitcher( PitcherFileName );

	for (i=0; i<count; i++)
	{
    	GetPitcher( PitcherFileName, i );
    	my_PitcherArray.Add( m_PitcherName.Mid(0,30) );
    }

	return my_PitcherArray;
}

int PitcherStruct::PitcherWrite(wxFile* myFile)
{
	myFile->Write(m_PitcherName,30);
	myFile->Write(&m_Wins,sizeof(m_Wins));
	myFile->Write(&m_Loss,sizeof(m_Loss));
	myFile->Write(&m_Saves,sizeof(m_Saves));
	myFile->Write(m_IP,7);
	myFile->Write(&m_ER,sizeof(m_ER));
	myFile->Write(&m_Hits,sizeof(m_Hits));
	myFile->Write(&m_Walks,sizeof(m_Walks));
	myFile->Write(&m_Strikeouts,sizeof(m_Strikeouts));
	myFile->Write(&m_HomeRuns,sizeof(m_HomeRuns));
	myFile->Write(&m_Filler1,sizeof(m_Filler1));
	myFile->Write(&m_Games,sizeof(m_Games));
	myFile->Write(&m_CompGames,sizeof(m_CompGames));
	myFile->Write(&m_Starts,sizeof(m_Starts));
	myFile->Write(m_OBChanceBasic,6);
	myFile->Write(m_OBChanceLeft,6);
	myFile->Write(m_OBChanceRight,6);
	myFile->Write(m_OBChanceWalk,6);
	myFile->Write(m_OBChanceSingle,6);
	myFile->Write(m_OBChanceDouble,6);
	myFile->Write(m_OBChanceTriple,6);
	myFile->Write(m_OBChanceHomeRun,6);
	myFile->Write(m_ChanceDoublePlay,6);
	myFile->Write(m_OBChanceWalkRight,6);
	myFile->Write(m_OBChanceSingleRight,6);
	myFile->Write(m_OBChanceDoubleRight,6);
	myFile->Write(m_OBChanceTripleRight,6);
	myFile->Write(m_OBChanceHomeRunRight,6);
	myFile->Write(m_ChanceDoublePlayRight,6);
	myFile->Write(m_OBChanceWalkLeft,6);
	myFile->Write(m_OBChanceSingleLeft,6);
	myFile->Write(m_OBChanceDoubleLeft,6);
	myFile->Write(m_OBChanceTripleLeft,6);
	myFile->Write(m_OBChanceHomeRunLeft,6);
	myFile->Write(m_ChanceDoublePlayLeft,6);
	myFile->Write("0     ",6);		// Filler
	myFile->Write("0     ",6);		// Filler
	myFile->Write("0     ",6);		// Filler
	myFile->Write(&m_Starter,sizeof(m_Starter));
	myFile->Write(&m_Relief,sizeof(m_Relief));
	myFile->Write(&m_Throws,sizeof(m_Throws));
	myFile->Write(&m_Bunting,sizeof(m_Bunting));
	myFile->Write(&m_Hold,sizeof(m_Hold));
	myFile->Write(&m_bWP,sizeof(m_bWP));
	myFile->Write(&m_bBalk,sizeof(m_bBalk));
	myFile->Write(&m_bER1,sizeof(m_bER1));
	myFile->Write(&m_PitcherField,sizeof(m_PitcherField));	// Pitcher field rating
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	myFile->Write(&m_bFiller1,sizeof(wxByte));	// Filler
	return 1;
}

int PitcherStruct::PitcherRead(wxFile* myFile)
{
	wxChar cTemp[30];
	wxChar cTemp7[7];
	wxChar cTemp6[6];
	wxByte bTemp;

	myFile->Read(cTemp,30);
	m_PitcherName.Printf("%30s",cTemp);
	myFile->Read(&m_Wins,sizeof(m_Wins));
	myFile->Read(&m_Loss,sizeof(m_Loss));
	myFile->Read(&m_Saves,sizeof(m_Saves));
	myFile->Read(cTemp7,7);
	m_IP.Printf("%c%c%c%c%c%c%c",cTemp7[0],cTemp7[1],cTemp7[2],cTemp7[3],cTemp7[4],cTemp7[5],cTemp7[6]);
	myFile->Read(&m_ER,sizeof(m_ER));
	myFile->Read(&m_Hits,sizeof(m_Hits));
	myFile->Read(&m_Walks,sizeof(m_Walks));
	myFile->Read(&m_Strikeouts,sizeof(m_Strikeouts));
	myFile->Read(&m_HomeRuns,sizeof(m_HomeRuns));
	myFile->Read(&m_Filler1,sizeof(m_Filler1));
	myFile->Read(&m_Games,sizeof(m_Games));
	myFile->Read(&m_CompGames,sizeof(m_CompGames));
	myFile->Read(&m_Starts,sizeof(m_Starts));
	myFile->Read(cTemp6,6);
	m_OBChanceBasic.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceWalk.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceSingle.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceDouble.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceTriple.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceHomeRun.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_ChanceDoublePlay.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceWalkRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceSingleRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceDoubleRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceTripleRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceHomeRunRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_ChanceDoublePlayRight.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceWalkLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceSingleLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceDoubleLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceTripleLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	m_OBChanceHomeRunLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp,6);
	m_ChanceDoublePlayLeft.Printf("%c%c%c%c%c%c",cTemp6[0],cTemp6[1],cTemp6[2],cTemp6[3],cTemp6[4],cTemp6[5]);
	myFile->Read(cTemp6,6);
	myFile->Read(cTemp6,6);
	myFile->Read(cTemp6,6);
	myFile->Read(&m_Starter,sizeof(m_Starter));
	myFile->Read(&m_Relief,sizeof(m_Relief));
	myFile->Read(&m_Throws,sizeof(m_Throws));
	myFile->Read(&m_Bunting,sizeof(m_Bunting));
	myFile->Read(&m_Hold,sizeof(m_Hold));
	myFile->Read(&m_bWP,sizeof(m_bWP));
	myFile->Read(&m_bBalk,sizeof(m_bBalk));
	myFile->Read(&m_bER1,sizeof(m_bER1));
	myFile->Read(&m_PitcherField,sizeof(m_PitcherField));
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	myFile->Read(&bTemp,sizeof(wxByte));	// Filler
	return 1;
}

wxByte PitcherStruct::GetCountPitcher(wxString PitcherFileName)
{
	wxByte count;
	wxFile myFile;
	myFile.Open( PitcherFileName,wxFile::read);
	// Read Count
	myFile.Read(&count,sizeof(count));
	// Close file
	myFile.Close();
	return count;
}

int PitcherStruct::CopyPitcherFile(wxString inFileName, wxString outFileName)
{
	int i;
	wxByte count;
	long lSeekPlayerFile;
//	long lCountSection = 1;
	wxFile inFile;
	wxFile outFile;

	inFile.Open( inFileName,wxFile::read);
	outFile.Create( outFileName, TRUE);
	// Read Count
	inFile.Read(&count,sizeof(count));
	outFile.Write(&count,sizeof(count));
	// Read all of the pitchers
	for (i=0; i<count; i++)
	{
		lSeekPlayerFile = LENGTH_COUNT_SECTION + (i*m_RecordSize);
		inFile.Seek( lSeekPlayerFile, wxFromStart );
		PitcherRead(&inFile);
		// Zero out fields
		m_ER = 0;
		m_Hits = 0;
		m_HomeRuns = 0;
		m_IP.Empty();
		m_IP = "0";
		m_Loss = 0;
		m_Saves = 0;
		m_Strikeouts = 0;
		m_Walks = 0;
		m_Wins = 0;
		m_Starts = 0;
		m_Games = 0;
		m_CompGames = 0;
		// Write Pitcher info at EOF, Already positioned there
		PitcherWrite(&outFile);
	}
	// Close file
	inFile.Close();
	outFile.Close();
	return 1;
}

///////////////////////////////////////////////////////////////////////
// LeagueStruct

LeagueStruct::LeagueStruct()
{
}

LeagueStruct::~LeagueStruct()
{
}

// Pass the file name of the League
int LeagueStruct::GetLeague(wxString LeagueFileName)
{
	wxFile wxFileLeague;
	wxFileLeague.Open( LeagueFileName, wxFile::read );
	LeagueRead( &wxFileLeague );
	// Close file
	wxFileLeague.Close();
	return 1;
}

// This section is far from tested 06/28/2007
// Pass the opened file for the league
int LeagueStruct::LeagueRead(wxFile* myFile)
{
	wxChar cTemp03[3];
	wxChar cTemp08[8];
	wxChar cTemp20[20];
	wxChar cTemp30[30];
	wxChar cTemp40[40];
	wxString strTemp01;
	int iTeams;
	int iDivisions;
	int iConferences;

	myFile->Read( &m_Version, sizeof(m_Version) );
	myFile->Read( &m_ConferenceNumber, sizeof(m_ConferenceNumber) );
	myFile->Read( cTemp30, 30 );
	m_LeagueName.Printf( "%30s", cTemp30 );

	for (iConferences=0; iConferences<m_ConferenceNumber; iConferences++)
	{
		myFile->Read( &m_DivisionNumber, sizeof(m_DivisionNumber) );
		myFile->Read( cTemp30, 30 );	// Read Conference Name
		strTemp01 = cTemp30;
		m_pConferences.Add( strTemp01 );

		for (iDivisions=0; iDivisions<m_DivisionNumber; iDivisions++)
		{
			myFile->Read( &m_TeamsNumber, sizeof(m_TeamsNumber) );
			myFile->Read( cTemp30, 30 );	// Read Division Name
			strTemp01 = cTemp30;
			m_pDivisions.Add( strTemp01 );

			// This array will only work for one division
			for (iTeams=0; iTeams<m_TeamsNumber; iTeams++)
			{
				myFile->Read( cTemp40, 40 );	// Read Team Name
				strTemp01 = cTemp40;
				m_pTeamNames.Add( strTemp01 );
				myFile->Read( cTemp08, 8 );	    // Read Team File Name
				strTemp01 = cTemp08;
				m_pTeamFileNames.Add( strTemp01 );
				myFile->Read( cTemp03, 3 );	    // Read Short Team Name
				strTemp01 = cTemp03;
				m_pShortTeamNames.Add( strTemp01 );
				myFile->Read( cTemp20, 20 );	// Read Ballpark Name
				strTemp01 = cTemp20;
				m_pTeamBallparks.Add( strTemp01 );
				// The following four need to be in arrays
				myFile->Read( &m_HomeWins, sizeof(m_HomeWins) );
				myFile->Read( &m_HomeLoss, sizeof(m_HomeLoss) );
				myFile->Read( &m_AwayWins, sizeof(m_AwayWins) );
				myFile->Read( &m_AwayLoss, sizeof(m_AwayLoss) );
			}
		}
	}
	return 1;
}

//  Parse the GetLeagueDir() subroutine as follows:
//
//	GetLeagueDir(TRUE); Returns all leagues and base
//	GetLeagueDir(FALSE); Returns only league directories
//
//  Return value is the directory name that contains the team files
//  this will be either a directory named "data" or a directory starting with L
//
wxString LeagueStruct::GetLeagueDir(bool baseFlag)
{
	wxFile myInFile;
	wxArrayString arrayFileNames;
	wxArrayString arrayLeagues;
	int i;
	wxString strLeagueName;
	wxString strLeagueDir;
	wxString strTemp;
	wxString strDataDir(wxGetApp().g_DataDir);
	wxByte version;
	wxByte count;
	char temp[30];
	wxFileName myTempFileName;

	//need to include <wx/filename.h> and <wx/stdpaths.h>

wxFileName f(wxStandardPaths::Get().GetExecutablePath());
wxString appPath(f.GetPath());

//	if ( wxDirExists(strDataDir) )
	if ( wxDirExists(appPath) )
	{
//        myTempFileName = wxFindFirstFile(strDataDir+"/l*.dat",wxFILE);
        myTempFileName = wxFindFirstFile(appPath+"/data/l*.dat",wxFILE);

   		wxMessageBox ( myTempFileName.GetPath(), "\n");
   		wxMessageBox ( appPath, "\n");
   		wxMessageBox ( strDataDir, "\n");

    	if (myTempFileName.FileExists())
    	{
    		while (myTempFileName.FileExists())
    		{
    			arrayFileNames.Add(myTempFileName.GetName());
    			myTempFileName = wxFindNextFile();
    		}

    		// Since the FindNextFile does not return the files in any order
    		// we must sort the file names
    		arrayFileNames.Sort();

    		for (i=0; i<(int)arrayFileNames.GetCount(); i++)
    		{
//    			myInFile.Open(strDataDir+"/"+arrayFileNames[i]+".dat",wxFile::read);
    			myInFile.Open(appPath+"/data"+"/"+arrayFileNames[i]+".dat",wxFile::read);
    			myInFile.Read(&version,sizeof(version));
    			myInFile.Read(&count,sizeof(count));
    			// Get the League name
    			myInFile.Read(temp,30);
    			myInFile.Close();
    			strLeagueName.Printf("%30s",temp);
    			arrayLeagues.Add(strLeagueName.Mid(0,30));
    		}
    		// If TRUE add in Base directory
    		if (baseFlag == TRUE)
    		{
    			//            123456789012345678901234567890
//    			strncpy(temp,"All Base Teams                ",30);
    			memcpy(temp,"All Base Teams                ",30);
    			strLeagueName.Printf("%30s",temp);
    			arrayLeagues.Add(strLeagueName.Mid(0,30));
    		}

            wxSingleChoiceDialog dialog1(NULL,
                                        _T("Please select a League"),
                                        _T("Please select a League"),
                                        arrayLeagues );

            if (dialog1.ShowModal() == wxID_OK)
            {
                if ( dialog1.GetSelection() == (int)arrayFileNames.GetCount() )
                {
//                    wxMessageDialog dialog2(NULL, dialog1.GetStringSelection()+ _T("\n")+
//                                           strDataDir,
//                                           _T("Got string"),
//                                           wxOK|wxICON_INFORMATION );
//                    dialog2.ShowModal();
                    // if returning base only return directory since there is no league
//                    strLeagueDir = strDataDir;
                    strLeagueDir = appPath+"/data";
                }
                else
                {
//                    wxMessageDialog dialog3(NULL, dialog1.GetStringSelection()+ _T("\n")+
//                                           arrayFileNames[dialog1.GetSelection()],
//                                           _T("Got string"),
//                                           wxOK|wxICON_INFORMATION );
//                    dialog3.ShowModal();
                    strLeagueDir = arrayFileNames[ dialog1.GetSelection() ];
                }
            }
    	}
    	else
    	{
    		// No Leagues found so display message
            wxMessageBox(_T("Could not find any Leagues files. Please create a League"),
                        _T("League"),
                        wxOK|wxICON_ERROR );
            // return null string
            strLeagueDir.Clear();
    		return strLeagueDir;
    	}
    }
    else
    {
    		// data directory not found so display message
//            wxMessageBox(_T("Directory \""+strDataDir+"\" could not be found"), _T("League"), wxOK);
            wxMessageBox(_T("Directory \""+appPath+"\\data\" could not be found"), _T("League"), wxOK);
            // return null string
            strLeagueDir.Clear();
    		return strLeagueDir;
    }
	return strLeagueDir;
}

//  Parse the GetLeagueName() subroutine as follows:
//
//  Return value is the league name.
//
wxString LeagueStruct::GetLeagueName()
{
	wxFile myInFile;
	wxArrayString arrayFileNames;
	wxArrayString arrayLeagues;
	int i;
	wxString strLeagueName;
	wxString strTemp;
	wxString strDataDir(wxGetApp().g_DataDir);
	wxByte version;
	wxByte count;
	wxChar temp[30];
	wxFileName myTempFileName;

	if ( wxDirExists(strDataDir) )
	{
        myTempFileName = wxFindFirstFile(strDataDir+"/l*.dat",wxFILE);
    	if (myTempFileName.FileExists())
    	{
    		while (myTempFileName.FileExists())
    		{
    			arrayFileNames.Add(myTempFileName.GetName());
    			myTempFileName = wxFindNextFile();
    		}

    		// Since the FindNextFile does not return the files in
    		// any order so we must sort the file names
    		arrayFileNames.Sort();

    		for (i=0; i<(int)arrayFileNames.GetCount(); i++)
    		{
    			myInFile.Open(strDataDir+"/"+arrayFileNames[i]+".dat",wxFile::read);
    			myInFile.Read(&version,sizeof(version));
    			myInFile.Read(&count,sizeof(count));
    			// Get the League name
    			myInFile.Read(temp,30);
    			myInFile.Close();
    			strLeagueName.Printf("%30s",temp);
    			arrayLeagues.Add(strLeagueName.Mid(0,30));
    		}

			wxSingleChoiceDialog dialog1(NULL,
				_T("Please select a League"),
				_T("Please select a League"),
				arrayLeagues );

			if (dialog1.ShowModal() == wxID_OK)
			{
				strLeagueName = dialog1.GetStringSelection();
			}
    	}
    	else
    	{
    		// No Leagues found so display message
            wxMessageBox(_T("Could not find any Leagues files. Please create a League"),
                        _T("League"),
                        wxOK|wxICON_ERROR );
            // return null string
            strLeagueName.Clear();
    		return strLeagueName;
    	}
	}
	else
	{
    		// data directory not found so display message
            wxMessageBox(_T("Directory \""+strDataDir+"\" could not be found"), _T("League"), wxOK);
            // return null string
            strLeagueName.Clear();
    		return strLeagueName;
    }
	return strLeagueName;
}

/////////////////////////////////////////////////////////////////////////////
// TeamStruct

TeamStruct::TeamStruct()
{
}

TeamStruct::~TeamStruct()
{
}

// Returns 40 character team name and full file name or null
wxString TeamStruct::GetTeamName(wxString strLeagueDir)
{
	wxFile myInFile;
	wxArrayString arrayFileNames;
	wxArrayString arrayTeamNames;
	int i;
	wxString strTeamNameFileName;
	wxString strTeamName;
	wxString strTemp;
	wxByte count;
	wxChar temp[40];
	wxFileName myTempFileName;

	if ( wxDirExists(strLeagueDir) )
	{
        myTempFileName = wxFindFirstFile(strLeagueDir+"/tb*.dat",wxFILE);
    	if (myTempFileName.FileExists())
    	{
    		while (myTempFileName.FileExists())
    		{
    			arrayFileNames.Add(myTempFileName.GetFullName());
    			myTempFileName = wxFindNextFile();
    		}

    		// Since the FindNextFile does not return the files in any order
    		// we must sort the file names
    		arrayFileNames.Sort();

    		for (i=0; i<(int)arrayFileNames.GetCount(); i++)
    		{
    			myInFile.Open(strLeagueDir+"/"+arrayFileNames[i],wxFile::read);
    			myInFile.Read(&count,sizeof(count));
    			// Get the Team name
    			myInFile.Read(temp,40);
    			myInFile.Close();
    			strTeamName.Printf("%40s",temp);
    			arrayTeamNames.Add(strTeamName.Mid(0,40));
    		}

            wxSingleChoiceDialog dialog1(NULL,
                                        _T("Please select a Team"),
                                        _T("Please select a Team"),
                                        arrayTeamNames );

            if (dialog1.ShowModal() == wxID_OK)
            {
//                wxMessageDialog dialog3(NULL, dialog1.GetStringSelection()+ _T("\n")+
//                                       arrayFileNames[dialog1.GetSelection()],
//                                       _T("Got string"),
//                                       wxOK|wxICON_INFORMATION );
//                dialog3.ShowModal();
                strTeamNameFileName = arrayTeamNames[ dialog1.GetSelection() ] +
                                  arrayFileNames[ dialog1.GetSelection() ];
            }
    	}
    	else
    	{
    		// No Leagues found so display message
            wxMessageBox(_T("Could not find any Team files. Please create a Team"),
                        _T("Team"),
                        wxOK|wxICON_ERROR );
            // return null string
            strTeamNameFileName.Clear();
    		return strTeamNameFileName;
    	}
    }
    else
    {
    		// data directory not found so display message
            wxMessageBox(_T("Directory \""+strLeagueDir+"\" could not be found"), _T("League"), wxOK);
            // return null string
            strTeamNameFileName.Clear();
    		return strTeamNameFileName;
    }
	return strTeamNameFileName;
}

// Returns array with 40 character team name and full file name or null
wxArrayString TeamStruct::ReturnTeamAndFile(wxString strLeagueDir)
{
	wxFile myInFile;
	wxArrayString arrayFileNames;
	wxArrayString arrayTeamNames;
	wxArrayString arrayTeamAndFile;
	int i;
	wxString strTeamNameFileName;
	wxString strTeamName;
	wxString strTemp;
	wxByte count;
	wxChar temp[40];
	wxFileName myTempFileName;

	if ( wxDirExists(strLeagueDir) )
	{
        myTempFileName = wxFindFirstFile(strLeagueDir+"/tb*.dat",wxFILE);
    	if (myTempFileName.FileExists())
    	{
    		while (myTempFileName.FileExists())
    		{
    			arrayFileNames.Add(myTempFileName.GetFullName());
    			myTempFileName = wxFindNextFile();
    		}

    		// Since the FindNextFile does not return the files in any order
    		// we must sort the file names
    		arrayFileNames.Sort();

    		for (i=0; i<(int)arrayFileNames.GetCount(); i++)
    		{
    			myInFile.Open(strLeagueDir+"/"+arrayFileNames[i],wxFile::read);
    			myInFile.Read(&count,sizeof(count));
    			// Get the Team name
    			myInFile.Read(temp,40);
    			myInFile.Close();
    			strTeamName.Printf("%40s",temp);
    			arrayTeamNames.Add(strTeamName.Mid(0,40));
                arrayTeamAndFile.Add( arrayTeamNames[ i ] + arrayFileNames[ i ] );
    		}
        }
    }

	return arrayTeamAndFile;
}

/////////////////////////////////////////////////////////////////////////////
// OptionFile

OptionFile::OptionFile()
{
// Windows directory will be
//    C:\Documents and Settings\gnew\Application Data\.wxApps
// Create the directory if nessary
    if (!wxDirExists(wxGetApp().g_OptionsDir))
        wxMkdir(wxGetApp().g_OptionsDir);
    wxTextFile file(wxGetApp().g_OptionsFile);

    if ( file.Exists() )
    {
        OptionFileRead();
    }
    else // Create the file
    {
		file.Create();
		file.Close();
		// Write default Options
		OptionFileWrite();
    }
}

OptionFile::~OptionFile()
{
}

// Get the next line in the Options file
wxString OptionFile::GetNextOptionLine()
{
	return "test";
}

void OptionFile::OptionFileRead()
{
    wxString s;

    wxTextFile file(wxGetApp().g_OptionsFile);

	m_arrayOptions.Clear();

    file.Open();
    for ( s = file.GetFirstLine(); !file.Eof(); s = file.GetNextLine() )
    {
        m_arrayOptions.Add(s);
    }
    file.Close();

    OptionFileReadParse();
}

void OptionFile::OptionFileReadParse()
{
    int i;
//        wxGetApp().g_DataDir = "data";
//        wxGetApp().g_HTML_Index = "index.htm";
//        wxGetApp().g_HTML_BG = "images/background.jpg";
//        wxGetApp().g_HTML_LinkColor = "Blue";
//        wxGetApp().g_HTML_VLinkColor = "Purple";
//        wxGetApp().g_HTML_BGColor = "White";
//        wxGetApp().g_HTML_TextColor = "Black";

    for (i=0; i<(int)m_arrayOptions.GetCount(); i++)
    {
        if (m_arrayOptions[i].StartsWith("DataDir"))
        {
//            wxMessageBox("Found DataDir Entry - " + m_arrayOptions[i],
//                "Found", wxOK, wxGetApp().pMyMainWindow);
            wxGetApp().g_DataDir = m_arrayOptions[i].Mid(10,wxString::npos);
        }
        if (m_arrayOptions[i].StartsWith("BackgroundPicture"))
        {
            wxGetApp().g_HTML_BG =
                m_arrayOptions[i].Mid(20,wxString::npos);
        }
        if (m_arrayOptions[i].StartsWith("Index"))
        {
            wxGetApp().g_HTML_Index =
                m_arrayOptions[i].Mid(8,wxString::npos);
        }
        if (m_arrayOptions[i].StartsWith("TextColor"))
        {
            wxGetApp().g_HTML_TextColor =
                m_arrayOptions[i].Mid(12,wxString::npos);
        }
        if (m_arrayOptions[i].StartsWith("BGColor"))
        {
            wxGetApp().g_HTML_BGColor =
                m_arrayOptions[i].Mid(10,wxString::npos);
        }
        if (m_arrayOptions[i].StartsWith("LinkColor"))
        {
            wxGetApp().g_HTML_LinkColor =
                m_arrayOptions[i].Mid(12,wxString::npos);
        }
        if (m_arrayOptions[i].StartsWith("VLinkColor"))
        {
            wxGetApp().g_HTML_VLinkColor =
                m_arrayOptions[i].Mid(13,wxString::npos);
        }
    }
}

void OptionFile::OptionFileWriteParse()
{
    m_arrayOptions.Clear();

    m_arrayOptions.Add( "DataDir = " + wxGetApp().g_DataDir );
    m_arrayOptions.Add( "BackgroundPicture = " + wxGetApp().g_HTML_BG );
    m_arrayOptions.Add( "Index = " + wxGetApp().g_HTML_Index );
    m_arrayOptions.Add( "TextColor = " + wxGetApp().g_HTML_TextColor );
    m_arrayOptions.Add( "BGColor = " + wxGetApp().g_HTML_BGColor );
    m_arrayOptions.Add( "LinkColor = " + wxGetApp().g_HTML_LinkColor );
    m_arrayOptions.Add( "VLinkColor = " + wxGetApp().g_HTML_VLinkColor );

    OptionFileWrite();
}

void OptionFile::OptionFileWrite()
{
    int i;

    wxTextFile file(wxGetApp().g_OptionsFile);

    // If no Options then set up defaults
    if ( m_arrayOptions.GetCount() == 0 )
    {
        // Setup defaults
        m_arrayOptions.Add("DataDir = data");
        m_arrayOptions.Add("BackgroundPicture = images/background.jpg");
        m_arrayOptions.Add("Index = index.htm");
        m_arrayOptions.Add("TextColor = Black");
        m_arrayOptions.Add("BGColor = White");
        m_arrayOptions.Add("LinkColor = Blue");
        m_arrayOptions.Add("VLinkColor = Purple");
        wxGetApp().g_DataDir = "data";
        wxGetApp().g_HTML_Index = "index.htm";
        wxGetApp().g_HTML_BG = "images/background.jpg";
        wxGetApp().g_HTML_LinkColor = "Blue";
        wxGetApp().g_HTML_VLinkColor = "Purple";
        wxGetApp().g_HTML_BGColor = "White";
        wxGetApp().g_HTML_TextColor = "Black";
    }

    // Open the file,
    //    Clear the contents,
    //        Add the new data,
    //            and then write it out
    file.Open();
    file.Clear();

    for (i=0; i<(int)m_arrayOptions.GetCount(); i++)
    {
        file.AddLine(m_arrayOptions[i]);
    }

    file.Write();
    file.Close();
}

/////////////////////////////////////////////////////////////////////////////
