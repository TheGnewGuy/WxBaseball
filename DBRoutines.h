/////////////////////////////////////////////////////////////////////////////
// Name:          DBRoutines.h                                             //
// Purpose:       Baseball statistics tracking tool for Strat-O-Matic      //
// Author:        John Gnew                                                //
// Created:       2024-06-24                                               //
// Copyright:     (c) John Gnew                                            //
/////////////////////////////////////////////////////////////////////////////
// Modifications:                                                          //
//   Date       Description                                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifndef DBROUTINES_H
#define DBROUTINES_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/wx.h>

#include "sqlite3.h"
#include "dialogs.h"

class DBRoutines
{
    public:
        DBRoutines();
        virtual ~DBRoutines();
        int DBSetForeginKeys(bool OnOffToggle);
        int DBOpen();
        int DBClose();
        int DBGetBatterStatsID(int TeamID);
        int DBGetBatterStatsID(int passedTeamID, int BatterID);
        int DBGetConference( int passedConferenceID );
        int DBGetDivision( int passedDivisionID );
        int DBGetTeamNamesArray();
        int DBGetLeague();
        int DBGetLeague(int passedLeagueID);
        int DBGetLeagueID(wxString passedLeagueName);
        int DBGetLeagueTeams(int passedLeagueID);
        int DBGetATeamID();
        int DBGetTeam(int TeamID);
        int DBGetTeamArray(int LeagueID);
        int DBGetBatterData(int BatterStatsID);
        int DBGetBatterID(int TeamID, wxString FirstName, wxString LastName);
        int DBGetPitcherID(int TeamID, wxString FirstName, wxString LastName);
        int DBGetPitcherStatsID(int TeamID);
        int DBGetPitcherStatsID(int passedTeamID, int PitcherID);
        int DBGetPitcherData(int PitcherStatsID);
        int DBInsertBatterData(int TeamID);
        int DBInsertBatterStats(int BatterID, int TeamID);
        int DBInsertPitcherData(int TeamID);
        int DBInsertPitcherStats(int PitcherID, int TeamID);
        int DBIsDBOpen();
        int DBSortLeagueNames();
        int DBSortTeamNames();
        int DBSortBatterNames();
        int DBSortPitcherNames();
        int DBUpdateBatterStats(int BatterStatsID);
        int DBUpdateBatterData(int BatterID);
        int DBUpdatePitcherStats(int PitcherStatsID);
        int DBUpdatePitcherData(int PitcherID);

        int m_dbOpen;
        sqlite3 *m_db;
        sqlite3_stmt *m_stmt;
        sqlite3_stmt *m_stmtConference;
        sqlite3_stmt *m_stmtDivision;
        sqlite3_stmt *m_stmtLeague;
        sqlite3_stmt *m_stmtTeam;
        sqlite3_stmt *m_stmtBatter;
        sqlite3_stmt *m_stmtBatterUpdateData;
        sqlite3_stmt *m_stmtBatterUpdateStats;
        sqlite3_stmt *m_stmtBatterInsertData;
        sqlite3_stmt *m_stmtBatterInsertStats;
        sqlite3_stmt *m_stmtPitcher;
        sqlite3_stmt *m_stmtPitcherInsertData;
        sqlite3_stmt *m_stmtPitcherInsertStats;
        sqlite3_stmt *m_stmtPitcherUpdateData;
        sqlite3_stmt *m_stmtPitcherUpdateStats;
        // Directory and filename
        wxString m_DBFileName;

        int m_intLeagueID;
        int m_intTeamID;
        int m_intBatterStatsID;
        int m_intBatterID;
        int m_intPitcherStatsID;
        int m_intPitcherID;
        wxString m_strLeagueName;
        wxString m_strTeamName;
        wxString m_strPitcherFirstName;
        wxString m_strPitcherLastName;
        wxArrayString m_arrayLeagueNames;
        wxArrayInt m_arrayLeagueIDs;
        wxArrayString m_arrayTeamNames;
        wxArrayInt m_arrayTeamIDs;
        wxArrayString m_arrayPitcherFirstNames;
        wxArrayString m_arrayPitcherLastNames;
        wxArrayString m_arrayPitcherFullNames;
        wxArrayInt m_arrayPitcherStatsIDs;
        wxArrayString m_arrayBatterFirstNames;
        wxArrayString m_arrayBatterLastNames;
        wxArrayString m_arrayBatterFullNames;
        wxArrayInt m_arrayBatterStatsIDs;

        struct{
            int BatterID;
            wxString FirstName;
            wxString LastName;
            int Pitcher;
            int Catcher;
            int FirstBase;
            int SecondBase;
            int Shortstop;
            int ThirdBase;
            int LeftField;
            int CenterField;
            int RightField;
            int Bunting;
            int HitRun;
            int Running;
            int Stealing;
            int CatchArm;
            int OutArm;
            int PowerRight;
            int PowerLeft;
            int Pass;
            int TRate;
            int ER1;
            int ER2;
            int ER3;
            int ER4;
            int ER5;
            int ER6;
            int ER7;
            int ER8;
            int ER9;
            bool BatterHits;
            int TeamID;
            float OBChanceHomeRun;
            float OBChanceTriple;
            float OBChanceDouble;
            float OBChanceSingle;
            float OBChanceWalk;
            float ChanceDoublePlay;
            float OBChanceHomeRunRight;
            float OBChanceTripleRight;
            float OBChanceDoubleRight;
            float OBChanceSingleRight;
            float OBChanceWalkRight;
            float ChanceDoublePlayRight;
            float OBChanceHomeRunLeft;
            float OBChanceTripleLeft;
            float OBChanceDoubleLeft;
            float OBChanceSingleLeft;
            float OBChanceWalkLeft;
            float ChanceDoublePlayLeft;
            float OBChanceBasic;
            float OBChanceLeft;
            float OBChanceRight;
            bool ActiveRec;
            char CreateTime[20];
            char LastUpdateTime[20];
        } structBatterData;

        struct{
            int BatterStatsID;
            int AB;
            int Runs;
            int Hits;
            int RBI;
            int Doubles;
            int Triples;
            int HomeRuns;
            int Walk;
            int Strikeout;
            int ReachedOnError;
            int Sacrifice;
            int StollenBase;
            int CS;
            int Games;
            int HBP;
            float AVG;
            float SLG;
            float OBP;
            int BatterID;
            int TeamID;
            bool ActiveRec;
            char CreateTime[20];
            char LastUpdateTime[20];
        } structBatterStats;

        struct{
            int PitcherID;
            wxString FirstName;
            wxString LastName;
            float OBChanceHomeRun;
            float OBChanceTriple;
            float OBChanceDouble;
            float OBChanceSingle;
            float OBChanceWalk;
            float ChanceDoublePlay;
            float OBChanceHomeRunRight;
            float OBChanceTripleRight;
            float OBChanceDoubleRight;
            float OBChanceSingleRight;
            float OBChanceWalkRight;
            float ChanceDoublePlayRight;
            float OBChanceHomeRunLeft;
            float OBChanceTripleLeft;
            float OBChanceDoubleLeft;
            float OBChanceSingleLeft;
            float OBChanceWalkLeft;
            float ChanceDoublePlayLeft;
            float OBChanceBasic;
            float OBChanceLeft;
            float OBChanceRight;
            int Starter;
            int Relief;
            int Throws;
            int Bunting;
            int Hold;
            int WP;
            int Balk;
            int Pitcher;
            int ER1;
            int TeamID;
            bool ActiveRec;
            char CreateTime[20];
            char LastUpdateTime[20];
        } structPitcherData;

        struct{
            int PitcherStatsID;
            int Wins;
            int Loss;
            int Saves;
            float InningsPitched;
            int ER;
            int Hits;
            int Walks;
            int Strikeouts;
            int HomeRuns;
            int Games;
            int CompleteGames;
            int Starts;
            float ERA;
            float WHIP;
            int PitcherID;
            int TeamID;
            bool ActiveRec;
            char CreateTime[20];
            char LastUpdateTime[20];
        } structPitcherStats;

        struct{
            int TeamID;
            wxString TeamName;
            wxString TeamNameShort;
            wxString BallparkName;
            int TotalWins;
            int TotalLosses;
            int HomeWins;
            int HomeLosses;
            int AwayWins;
            int AwayLosses;
            int LeagueID;
            int ConferenceID;
            int DivisionID;
            int TeamYear;
            bool BaseTeam;
            bool ActiveRec;
            char CreateTime[20];
            char LastUpdateTime[20];
		} structTeamData;

		struct{
            int LeagueID;
            wxString LeagueName;
            int TotalWins;
            int TotalLosses;
            int NumberOfConferences;
            int NumberOfDivisions;
            bool BaseLeague;
            int LeagueYear;
            bool ActiveRec;
            char CreateTime[20];
            char LastUpdateTime[20];
		} structLeagueData;

		struct{
            int ConferenceID;
            wxString ConferenceName;
            int LeagueID;
            bool BaseConference;
            bool ActiveRec;
            char CreateTime[20];
            char LastUpdateTime[20];
		} structConferenceData;

		struct{
            int DivisionID;
            wxString DivisionName;
            int LeagueID;
            int ConferenceID;
            bool BaseDivisions;
            bool ActiveRec;
            char CreateTime[20];
            char LastUpdateTime[20];
		} structDivisionData;

    protected:

    private:
};

#endif // DBROUTINES_H
