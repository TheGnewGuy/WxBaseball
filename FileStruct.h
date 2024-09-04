/////////////////////////////////////////////////////////////////////////////
// Name:          FileStruct.h                                             //
// Purpose:       Baseball statistics tracking tool for Strat-O-Matic      //
// Author:        John Gnew                                                //
// Created:       08/01/05                                                 //
// Copyright:     (c) John Gnew                                            //
// Modifications:                                                          //
//   Date       Description                                                //
// xx/xx/xx     xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
/////////////////////////////////////////////////////////////////////////////
#ifndef _FILESTRUCT_H_
#define _FILESTRUCT_H_

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

#include <wx/dc.h>
#include <wx/textfile.h>

/////////////////////////////////////////////////////////////////////////////
// BatterStruct window

class BatterStruct : public wxObject
{
// Construction
public:
	BatterStruct();

// Attributes
public:
	int		m_RecordSize;	// Calculated in constructor

	wxString	m_PlayerName;	// Length of 30
	short	m_AB;
	short	m_Runs;
	short	m_Hits;
	short	m_RBI;
	short	m_2B;
	short	m_3B;
	short	m_HomeRuns;
	short	m_Walk;
	short	m_StrikeOut;
	short	m_ReachedOnError;
	short	m_Sacrifice;
	short	m_StolenBase;
	short	m_CS;
	short	m_Filler1;		// For Growth if needed
	short	m_Filler2;		// For Growth if needed
	short	m_Games;
	short	m_HBP;
	wxByte	m_Pitcher;
	wxByte	m_Catcher;
	wxByte	m_FirstBase;
	wxByte	m_SecondBase;
	wxByte	m_ShortStop;
	wxByte	m_ThirdBase;
	wxByte	m_LeftField;
	wxByte	m_CenterField;
	wxByte	m_RightField;
	wxByte	m_bBunting;
	wxByte	m_bHitRun;
	wxByte	m_bRunning;
	wxByte	m_bStealing;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(BatterStruct)
	//}}AFX_VIRTUAL

// Implementation
public:
	wxByte m_bCatchArm;
	wxByte m_bOutArm;
	wxByte m_bPowerR;
	wxByte m_bPowerL;
	wxByte m_bPass;
	wxByte m_bTRate;
	wxByte m_bER9;
	wxByte m_bER8;
	wxByte m_bER7;
	wxByte m_bER6;
	wxByte m_bER5;
	wxByte m_bER4;
	wxByte m_bER3;
	wxByte m_bER2;
	wxByte m_bER1;
	wxString m_OBChanceHomeRun;
	wxString m_OBChanceTriple;
	wxString m_OBChanceDouble;
	wxString m_OBChanceSingle;
	wxString m_OBChanceWalk;
	wxString m_ChanceDoublePlay;
	wxString m_OBChanceHomeRunRight;
	wxString m_OBChanceTripleRight;
	wxString m_OBChanceDoubleRight;
	wxString m_OBChanceSingleRight;
	wxString m_OBChanceWalkRight;
	wxString m_ChanceDoublePlayRight;
	wxString m_OBChanceHomeRunLeft;
	wxString m_OBChanceTripleLeft;
	wxString m_OBChanceDoubleLeft;
	wxString m_OBChanceSingleLeft;
	wxString m_OBChanceWalkLeft;
	wxString m_ChanceDoublePlayLeft;
	int CopyBatterFile(wxString inFileName, wxString outFileName);
	wxByte GetCountBatter(wxString BatterFileName);
	wxByte m_bBatterHits;
	wxString m_OBChanceRight;
	wxString m_OBChanceLeft;
	wxString m_OBChanceBasic;
	wxByte m_bFiller1;
	int GetBatter(wxString BatterFileName, int number);
   wxArrayString ReturnAllBatters(wxString BatterFileName);
	int AddBatter(wxString BatterFileName);
	int UpdateBatter(wxString BatterFileName, int number);
	virtual ~BatterStruct();

	// Generated message map functions
protected:
private:
	int BatterRead(wxFile* myFile);
	int BatterWrite(wxFile* myFile);
protected:
	// Generated message map functions
};

/////////////////////////////////////////////////////////////////////////////
// PitcherStruct window

class PitcherStruct : public wxObject
{
// Construction
public:
	PitcherStruct();

// Attributes
public:
	int		m_RecordSize;	// Calculated in constructor

	wxByte	m_PitcherNumber;// Count of Pitchers in file
	wxString m_PitcherName;
	wxString m_PitcherFirstName;
	wxString m_PitcherLastName;
	short	m_Wins;
	short	m_Loss;
	short	m_Saves;
	short	m_ER;
	wxString	m_IP;
	short	m_Hits;
	short	m_Walks;
	short	m_Strikeouts;
	short	m_HomeRuns;
	short	m_Filler1;		// For Growth if needed
	short	m_Games;
	short	m_CompGames;
	short	m_Starts;
	wxByte	m_Starter;
	wxByte	m_Throws;
	wxByte	m_Relief;
	wxByte	m_Bunting;
	wxByte	m_Hold;
	wxByte	m_PitcherField;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PitcherStruct)
	//}}AFX_VIRTUAL

// Implementation
public:
	wxByte m_bER1;
	wxByte m_bBalk;
	wxByte m_bWP;
	wxString m_OBChanceHomeRun;
	wxString m_OBChanceTriple;
	wxString m_OBChanceDouble;
	wxString m_OBChanceSingle;
	wxString m_OBChanceWalk;
	wxString m_ChanceDoublePlay;
	wxString m_OBChanceHomeRunRight;
	wxString m_OBChanceTripleRight;
	wxString m_OBChanceDoubleRight;
	wxString m_OBChanceSingleRight;
	wxString m_OBChanceWalkRight;
	wxString m_ChanceDoublePlayRight;
	wxString m_OBChanceHomeRunLeft;
	wxString m_OBChanceTripleLeft;
	wxString m_OBChanceDoubleLeft;
	wxString m_OBChanceSingleLeft;
	wxString m_OBChanceWalkLeft;
	wxString m_ChanceDoublePlayLeft;
	int CopyPitcherFile(wxString inFileName, wxString outFileName);
	wxByte m_bFiller1;
	wxString m_OBChanceRight;
	wxString m_OBChanceLeft;
	wxString m_OBChanceBasic;
	int GetPitcher(wxString PitcherFileName, int number);
	wxByte GetCountPitcher(wxString PitcherFileName);
	int AddPitcher(wxString PitcherFileName);
	int UpdatePitcher(wxString PitcherFileName, int number);
   wxArrayString ReturnAllPitchers(wxString BatterFileName);
	virtual ~PitcherStruct();

protected:
	// Generated message map functions
private:
	int PitcherWrite(wxFile* myFile);
	int PitcherRead(wxFile* myFile);
};

/////////////////////////////////////////////////////////////////////////////
// LeagueStruct window

class LeagueStruct : public wxObject
{
// Construction
public:
	LeagueStruct();

// Attributes
public:
	// We will always have 1 Conference and one Division. If these
	// are not specified, then they will be assigne NULL
	//
	// Record Layout varies, each indentation will duplicat number:
    //  BYTE Version number
	//	BYTE Number of Conferences
	//	30 char League Name
	//		BYTE Number of Divisions
	//		30 char Conference Name (Always 1, Name may be NULL)
	//			BYTE Number of teams
	//			30 char Division Name (Always 1, Name may be NULL)
	//				40 char Team Name
	//				8 char Team File Name no extension
	//				3 char Short Team Name
	//				20 char Ballpark Name
	//				Home short Wins
	//				Home short Loss
	//				Away short Wins
	//				Away short Loss
	wxByte         m_Version;
	wxByte	       m_ConferenceNumber;
	wxByte	       m_DivisionNumber;
	wxByte	       m_TeamsNumber;
	wxByte	       m_HomeWins;
	wxByte	       m_HomeLoss;
	wxByte	       m_AwayWins;
	wxByte	       m_AwayLoss;
	wxString	   m_LeagueName;
	wxArrayString  m_pConferences;
	wxArrayString  m_pDivisions;
	wxArrayString  m_pTeamNames;
	wxArrayString  m_pTeamFileNames;
	wxArrayString  m_pShortTeamNames;
	wxArrayString  m_pTeamBallparks;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LeagueStruct)
	//}}AFX_VIRTUAL

// Implementation
public:
    int GetLeague(wxString LeagueFileName);
    int LeagueRead(wxFile* myFile);
    // Returns directory of league
    wxString GetLeagueDir(bool baseFlag);
    wxString GetLeagueName();
	virtual ~LeagueStruct();

protected:
	// Generated message map functions
};

/////////////////////////////////////////////////////////////////////////////
// TeamStruct window

class TeamStruct : public wxObject
{
// Construction
public:
	TeamStruct();

// Attributes
public:
	// Record Structure
	//	BYTE Number of Batters
	//	BYTE Number of Pitchers
	//	40 char Team Name
	//		Batter records based on number of batters

	wxByte	m_BatterNumber;
	wxString	m_TeamName;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TeamStruct)
	//}}AFX_VIRTUAL

// Implementation
public:
    // Returns 40 character team name and then full file name or NULL
    wxString GetTeamName(wxString strLeagueDir);
	wxArrayString ReturnTeamAndFile(wxString strLeagueDir);
    virtual ~TeamStruct();

protected:
	// Generated message map functions
};


/////////////////////////////////////////////////////////////////////////////
// TeamStruct window

class OptionFile : public wxObject
{
// Construction
public:
	OptionFile();
	virtual ~OptionFile();
	void OptionFileRead();
	void OptionFileReadParse();
	void OptionFileWriteParse();
	void OptionFileWrite();

// Attributes
public:
//	wxString m_strOptionFileName;
//	wxTextFile m_OptionFile;
    wxArrayString m_arrayOptions;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TeamStruct)
	//}}AFX_VIRTUAL

// Implementation
public:
	wxString GetNextOptionLine();
};

#endif // _FILESTRUCT_H_

