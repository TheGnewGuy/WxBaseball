/////////////////////////////////////////////////////////////////////////////
// Name:          DBRoutines.cpp                                           //
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

#include <wx/choicdlg.h>
#include <wx/msgdlg.h>

#include "WxBaseballApp.h"
#include "DBRoutines.h"

DBRoutines::DBRoutines()
{
    m_DBFileName = _T("");
    m_dbOpen = 99;
}

DBRoutines::~DBRoutines()
{
    //dtor
}

// Set return to TRUE for On and FALSE for Off, and -1 for fail
int DBRoutines::DBSetForeginKeys(bool OnOffToggle)
{
    wxString Foobar;
    wxString sqlTRUE;
    wxString sqlFALSE;
    int rc;
    char *zErrMsg = 0;

    // Create SQL statement
    sqlTRUE = "PRAGMA foreign_keys = ON;";
    sqlFALSE = "PRAGMA foreign_keys = OFF;";

    // Execute SQL statement
//    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if (OnOffToggle == TRUE)
    {
        rc = sqlite3_exec(m_db, sqlTRUE, 0, 0, &zErrMsg);
    }
    else
    {
        rc = sqlite3_exec(m_db, sqlFALSE, 0, 0, &zErrMsg);
    }

    if( rc != SQLITE_OK )
    {
        if (OnOffToggle == TRUE)
        {
            Foobar.Printf( wxT("SQL error setting foreign_keys to TRUE: %s"), zErrMsg);
            wxMessageBox(Foobar);
        }
        else
        {
            Foobar.Printf( wxT("SQL error setting foreign_keys to FALSE: %s"), zErrMsg);
            wxMessageBox(Foobar);
        }
        sqlite3_free(zErrMsg);
        rc = -1;
    }
//    else
//    {
//        if (OnOffToggle == TRUE)
//        {
//            Foobar.Printf( wxT("Foregin_keys successfully turned ON"));
//            wxMessageBox(Foobar);
//            rc = TRUE;
//       }
//        else
//         {
//            Foobar.Printf( wxT("Foregin_keys successfully turned OFF"));
//            wxMessageBox(Foobar);
//            rc = FALSE;
//        }
//   }
   return rc;
}

// Check to see if DB is already open, If not prompt to open one
int DBRoutines::DBIsDBOpen()
{
    wxString MsgString;

    if( m_dbOpen )
    {
        MsgString.Printf( wxT("Database has not been opened"));
        wxMessageBox(MsgString);
        wxGetApp().pDBRoutines->DBOpen();
    }
}
int DBRoutines::DBOpen()
{
    int rc = TRUE;

//    wxFileDialog fileDlg(this, _("Choose the DB file"), wxEmptyString, wxEmptyString, _("DB file|*.db|All files|*.*"));
    wxFileDialog fileDlg(NULL, _("Choose the DB file"), wxEmptyString, wxEmptyString, _("DB file|*.db|All files|*.*"));
    if (fileDlg.ShowModal() == wxID_OK)
    {
        m_DBFileName = fileDlg.GetPath();
        wxGetApp().g_DataDirectory = fileDlg.GetDirectory();
        wxString myFile = fileDlg.GetFilename();
        wxString Foobar;
//        Foobar.Printf( wxT("Filename = %s\nPath = %s"), myFile, m_DBFileName);
//        wxMessageBox(Foobar);

        if (m_dbOpen == SQLITE_OK)
        {
            sqlite3_close(m_db);
            m_dbOpen = 99;
        }

        m_dbOpen = sqlite3_open(m_DBFileName, &m_db);

        if( m_dbOpen )
        {
            //fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            Foobar.Printf( wxT("Can't open database: %s"), sqlite3_errmsg(m_db));
            wxMessageBox(Foobar);
            //return(0);
        }
//        else
//        {
//            //fprintf(stderr, "Opened database successfully\n");
//            Foobar.Printf( wxT("Opened database successfully"));
//            wxMessageBox(Foobar);
//        }

        // The Sqlite3 default setting for foregin keys os off
        if (m_dbOpen == SQLITE_OK)
        {
            DBSetForeginKeys(TRUE);    // Turn foreign_keys ON
        }
    }
//    Destroy();
    return rc;
}

int DBRoutines::DBClose()
{
    int rc = TRUE;
    wxString Foobar;

    if (m_dbOpen == SQLITE_OK)
    {
        sqlite3_close(m_db);
        m_dbOpen = 99;

//        Foobar.Printf( wxT("Just Closed database"));
//        wxMessageBox(Foobar);
   }
//    Destroy();

    return rc;
}

int DBRoutines::DBGetATeamID()
{
	int intLeagueID;
	int intTeamID;
	int intBatterStatsID;
	int intPitcherStatsID;

    intLeagueID = DBGetLeague();
    intTeamID = DBGetTeamArray(intLeagueID);
    intBatterStatsID = DBGetBatterStatsID(intTeamID);
    intPitcherStatsID = DBGetPitcherStatsID(intTeamID);

	return intTeamID;
}

int DBRoutines::DBGetTeamNamesArray()
{
	int intLeagueID;
	int intTeamID;
	int intBatterStatsID;
	int intPitcherStatsID;

    intLeagueID = DBGetLeague();
    intTeamID = DBGetTeamArray(intLeagueID);
    intBatterStatsID = DBGetBatterStatsID(intTeamID);
    intPitcherStatsID = DBGetPitcherStatsID(intTeamID);
//    DBGetPitcherData(intPitcherStatsID);

	return intPitcherStatsID;
}

// Routine will return the selected LeagueID
int DBRoutines::DBGetLeague()
{
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlLeague;

	/* Create SQL statement */
	sqlLeague = "SELECT "  \
		"LeagueID," \
		"LeagueName," \
		"NumberOfConferences," \
		"NumberOfDivisions," \
		"BaseLeague" \
		" FROM LEAGUES " \
		" WHERE ActiveRec = ?1 " ;

	rc = sqlite3_prepare_v2(m_db, sqlLeague, strlen(sqlLeague), &m_stmt, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmt, 1, TRUE);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ActiveRec: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	m_arrayLeagueIDs.Clear();
	m_arrayLeagueNames.Clear();

	while (sqlite3_step(m_stmt) == SQLITE_ROW)
	{
        m_arrayLeagueIDs.Add(sqlite3_column_int(m_stmt, 0));
		m_arrayLeagueNames.Add(sqlite3_column_text(m_stmt, 1));
	}

	sqlite3_finalize(m_stmt);

	DBSortLeagueNames();

    wxSingleChoiceDialog dialogLeague(wxGetApp().GetTopWindow(),
                                "League List",
                                "Please select a League",
                                m_arrayLeagueNames);

    dialogLeague.SetSelection(0);

    if (dialogLeague.ShowModal() == wxID_OK)
    {
        m_strLeagueName = dialogLeague.GetStringSelection();
        m_intLeagueID = m_arrayLeagueIDs[dialogLeague.GetSelection()];
    }

    return m_intLeagueID;
}

// Routine will return the selected LeagueID
int DBRoutines::DBGetLeagueTeams( int passedLeagueID)
{
	int rc;
	wxString MsgBuffer;
	wxString sqlTeam;

	/* Create SQL statement */
	sqlTeam = "SELECT "  \
		"TeamID," \
		"TeamName" \
		" FROM TEAM " \
		" WHERE LeagueID = ?1 " ;

	rc = sqlite3_prepare_v2(m_db, sqlTeam, strlen(sqlTeam), &m_stmtTeam, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 1, passedLeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedLeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	m_arrayTeamIDs.Clear();
	m_arrayTeamNames.Clear();

	while (sqlite3_step(m_stmtTeam) == SQLITE_ROW)
	{
		m_arrayTeamIDs.Add(sqlite3_column_int(m_stmtTeam, 0));
		m_arrayTeamNames.Add(sqlite3_column_text(m_stmtTeam, 1));
	}

	sqlite3_finalize(m_stmtTeam);

    DBSortTeamNames();

    return passedLeagueID;
}

// Routine will return the selected League
int DBRoutines::DBGetLeague( int passedLeagueID )
{
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlLeague;

	/* Create SQL statement */
	sqlLeague = "SELECT "  \
		"LeagueID, " \
		"LeagueName, " \
		"NumberOfConferences, " \
		"NumberOfDivisions, " \
		"BaseLeague, " \
		"LeagueYear, " \
		"ActiveRec," \
		"CreateTime," \
		"LastUpdateTime" \
		" FROM LEAGUES " \
		" WHERE LeagueID = ?1 " ;

	rc = sqlite3_prepare_v2(m_db, sqlLeague, strlen(sqlLeague), &m_stmtLeague, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtLeague, 1, passedLeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedLeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	while (sqlite3_step(m_stmtLeague) == SQLITE_ROW)
	{
        structLeagueData.LeagueID = sqlite3_column_int(m_stmtLeague, 0);
		structLeagueData.LeagueName = sqlite3_column_text(m_stmtLeague, 1);
        structLeagueData.NumberOfConferences = sqlite3_column_int(m_stmtLeague, 2);
        structLeagueData.NumberOfDivisions = sqlite3_column_int(m_stmtLeague, 3);
        structLeagueData.BaseLeague = sqlite3_column_int(m_stmtLeague, 4);
        structLeagueData.LeagueYear = sqlite3_column_int(m_stmtLeague, 5);
        structLeagueData.ActiveRec = sqlite3_column_int(m_stmtLeague, 6);
		structLeagueData.CreateTime = sqlite3_column_text(m_stmtLeague, 7);
		structLeagueData.LastUpdateTime = sqlite3_column_text(m_stmtLeague, 8);
	}

	sqlite3_finalize(m_stmtLeague);

    return structLeagueData.LeagueID;
}

// Routine will return the selected Conference data
int DBRoutines::DBGetConference( int passedConferenceID )
{
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlConference;

	/* Create SQL statement */
	sqlConference = "SELECT "  \
		"ConferenceID, " \
		"ConferenceName, " \
		"LeagueID, " \
		"BaseConference, " \
		"ActiveRec," \
		"CreateTime," \
		"LastUpdateTime" \
		" FROM CONFERENCES " \
		" WHERE ConferenceID = ?1 " ;

	rc = sqlite3_prepare_v2(m_db, sqlConference, strlen(sqlConference), &m_stmtConference, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtConference, 1, passedConferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedConferenceID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	while (sqlite3_step(m_stmtConference) == SQLITE_ROW)
	{
        structConferenceData.ConferenceID = sqlite3_column_int(m_stmtConference, 0);
		structConferenceData.ConferenceName = sqlite3_column_text(m_stmtConference, 1);
        structConferenceData.LeagueID = sqlite3_column_int(m_stmtConference, 2);
        structConferenceData.BaseConference = sqlite3_column_int(m_stmtConference, 3);
        structConferenceData.ActiveRec = sqlite3_column_int(m_stmtConference, 4);
		structConferenceData.CreateTime = (sqlite3_column_text(m_stmtConference, 5));
		structConferenceData.LastUpdateTime = (sqlite3_column_text(m_stmtConference, 6));
	}

	sqlite3_finalize(m_stmtConference);

    return structConferenceData.ConferenceID;
}

// Routine will return the selected Division
int DBRoutines::DBGetDivision( int passedDivisionID )
{
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlLeague;

	/* Create SQL statement */
	sqlLeague = "SELECT "  \
		"DivisionID, " \
		"DivisionName, " \
		"LeagueID, " \
		"ConferenceID, " \
		"BaseDivisions, " \
		"ActiveRec," \
		"CreateTime," \
		"LastUpdateTime" \
		" FROM DIVISIONS " \
		" WHERE DivisionID = ?1 " ;

	rc = sqlite3_prepare_v2(m_db, sqlLeague, strlen(sqlLeague), &m_stmtDivision, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtDivision, 1, passedDivisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedDivisionID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	while (sqlite3_step(m_stmtDivision) == SQLITE_ROW)
	{
        structDivisionData.DivisionID = sqlite3_column_int(m_stmtDivision, 0);
		structDivisionData.DivisionName = sqlite3_column_text(m_stmtDivision, 1);
        structDivisionData.LeagueID = sqlite3_column_int(m_stmtDivision, 2);
        structDivisionData.ConferenceID = sqlite3_column_int(m_stmtDivision, 3);
        structDivisionData.BaseDivisions = sqlite3_column_int(m_stmtDivision, 4);
        structDivisionData.ActiveRec = sqlite3_column_int(m_stmtDivision, 5);
		structDivisionData.CreateTime = (sqlite3_column_text(m_stmtDivision, 6));
		structDivisionData.LastUpdateTime = (sqlite3_column_text(m_stmtDivision, 7));
	}

	sqlite3_finalize(m_stmtDivision);

    return structLeagueData.LeagueID;
}

// Routine will sort the League names and ID in seperate arrays
int DBRoutines::DBSortLeagueNames()
{
    wxString strTempLeagueName;
    int intTempLeagueID;
    int intArrayCount=0;
    int i;
    int j;

    intArrayCount = m_arrayLeagueNames.GetCount();

    for ( i=0; i<intArrayCount; i++ )
    {
        for ( j=i+1; j<intArrayCount; j++ )
        {
            if ( m_arrayLeagueNames[i] > m_arrayLeagueNames[j] )
            {
                strTempLeagueName = m_arrayLeagueNames[i];
                m_arrayLeagueNames[i] = m_arrayLeagueNames[j];
                m_arrayLeagueNames[j] = strTempLeagueName;
                intTempLeagueID = m_arrayLeagueIDs[i];
                m_arrayLeagueIDs[i] = m_arrayLeagueIDs[j];
                m_arrayLeagueIDs[j] = intTempLeagueID;
            }
        }
    }
    return 0;
}

// Routine will populate structConferenceData
int DBRoutines::DBGetConferenceID(wxString passedConferenceName, int passedLeagueID)
{
	int rc;
	wxString MsgBuffer;
	wxString sqlConference;

	/* Create SQL statement */
	sqlConference = "SELECT "  \
		"ConferenceID," \
		"ConferenceName," \
		"LeagueID," \
		"BaseConference," \
		"ActiveRec," \
		"CreateTime," \
		"LastUpdateTime" \
		" FROM CONFERENCES " \
		" WHERE ConferenceName = ?1 AND LeagueID = ?2 ";

	rc = sqlite3_prepare_v2(m_db, sqlConference, strlen(sqlConference), &m_stmtConference, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtConference, 1, passedConferenceName, strlen(passedConferenceName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ConferenceName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtConference, 2, passedLeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedLeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	// Now retrieve inserted record to get new ConferenceID value
	while (sqlite3_step(m_stmtConference) == SQLITE_ROW)
	{
		structConferenceData.ConferenceID = (sqlite3_column_int(m_stmtConference, 0));
		structConferenceData.ConferenceName = (sqlite3_column_text(m_stmtConference, 1));
		structConferenceData.LeagueID = (sqlite3_column_int(m_stmtConference, 2));
		structConferenceData.BaseConference = (sqlite3_column_int(m_stmtConference, 3));
		structConferenceData.ActiveRec = (sqlite3_column_int(m_stmtConference, 4));
		structConferenceData.CreateTime = (sqlite3_column_text(m_stmtConference, 5));
		structConferenceData.LastUpdateTime = (sqlite3_column_text(m_stmtConference, 6));
	}

	rc = sqlite3_finalize(m_stmtConference);

    // Return the new ConferenceID
    return structConferenceData.ConferenceID;
}

// Routine will return the selected ConferenceID
int DBRoutines::DBGetConferenceID( int passedLeagueID )
{
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlConference;

	/* Create SQL statement */
	sqlConference = "SELECT "  \
		"ConferenceID," \
		"ConferenceName" \
		" FROM CONFERENCES " \
		" WHERE LeagueID = ?1 ";

	rc = sqlite3_prepare_v2(m_db, sqlConference, strlen(sqlConference), &m_stmt, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmt, 1, passedLeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedLeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	m_arrayConferenceIDs.Clear();
	m_arrayConferenceNames.Clear();

	while (sqlite3_step(m_stmt) == SQLITE_ROW)
	{
        m_arrayConferenceIDs.Add(sqlite3_column_int(m_stmt, 0));
		m_arrayConferenceNames.Add(sqlite3_column_text(m_stmt, 1));
	}

	sqlite3_finalize(m_stmt);

	DBSortConferenceNames();

    wxSingleChoiceDialog dialogConference(wxGetApp().GetTopWindow(),
                                "Conference List",
                                "Please select a Conference",
                                m_arrayConferenceNames);

    dialogConference.SetSelection(0);

    if (dialogConference.ShowModal() == wxID_OK)
    {
        m_strConferenceName = dialogConference.GetStringSelection();
        m_intConferenceID = m_arrayConferenceIDs[dialogConference.GetSelection()];
    }

    return m_intConferenceID;
}

// Routine will sort the Conference names and ID in seperate arrays
int DBRoutines::DBSortConferenceNames()
{
    wxString strTempConferenceName;
    int intTempConferenceID;
    int intArrayCount=0;
    int i;
    int j;

    intArrayCount = m_arrayConferenceNames.GetCount();

    for ( i=0; i<intArrayCount; i++ )
    {
        for ( j=i+1; j<intArrayCount; j++ )
        {
            if ( m_arrayConferenceNames[i] > m_arrayConferenceNames[j] )
            {
                strTempConferenceName = m_arrayConferenceNames[i];
                m_arrayConferenceNames[i] = m_arrayConferenceNames[j];
                m_arrayConferenceNames[j] = strTempConferenceName;
                intTempConferenceID = m_arrayConferenceIDs[i];
                m_arrayConferenceIDs[i] = m_arrayConferenceIDs[j];
                m_arrayConferenceIDs[j] = intTempConferenceID;
            }
        }
    }
    return 0;
}

// Routine will return the selected DivisionID
int DBRoutines::DBGetDivisionID( int passedConferenceID )
{
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlDivision;

	/* Create SQL statement */
	sqlDivision = "SELECT "  \
		"DivisionID," \
		"DivisionName" \
		" FROM DIVISIONS " \
		" WHERE ConferenceID = ?1 ";

	rc = sqlite3_prepare_v2(m_db, sqlDivision, strlen(sqlDivision), &m_stmt, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmt, 1, passedConferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedConferenceID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	m_arrayDivisionIDs.Clear();
	m_arrayDivisionNames.Clear();

	while (sqlite3_step(m_stmt) == SQLITE_ROW)
	{
        m_arrayDivisionIDs.Add(sqlite3_column_int(m_stmt, 0));
		m_arrayDivisionNames.Add(sqlite3_column_text(m_stmt, 1));
	}

	sqlite3_finalize(m_stmt);

	if ( m_arrayDivisionNames.GetCount() == 0 )
	{
        m_arrayDivisionIDs.Add(1);
		m_arrayDivisionNames.Add("DEFAULT");
	}

	DBSortDivisionNames();

    wxSingleChoiceDialog dialogDivision(wxGetApp().GetTopWindow(),
                                "Division List",
                                "Please select a Division",
                                m_arrayDivisionNames);

    dialogDivision.SetSelection(0);

    if (dialogDivision.ShowModal() == wxID_OK)
    {
        m_strDivisionName = dialogDivision.GetStringSelection();
        m_intDivisionID = m_arrayDivisionIDs[dialogDivision.GetSelection()];
    }

    return m_intDivisionID;
}

// Routine will sort the Division names and ID in seperate arrays
int DBRoutines::DBSortDivisionNames()
{
    wxString strTempDivisionName;
    int intTempDivisionID;
    int intArrayCount=0;
    int i;
    int j;

    intArrayCount = m_arrayDivisionNames.GetCount();

    for ( i=0; i<intArrayCount; i++ )
    {
        for ( j=i+1; j<intArrayCount; j++ )
        {
            if ( m_arrayDivisionNames[i] > m_arrayDivisionNames[j] )
            {
                strTempDivisionName = m_arrayDivisionNames[i];
                m_arrayDivisionNames[i] = m_arrayDivisionNames[j];
                m_arrayDivisionNames[j] = strTempDivisionName;
                intTempDivisionID = m_arrayDivisionIDs[i];
                m_arrayDivisionIDs[i] = m_arrayDivisionIDs[j];
                m_arrayDivisionIDs[j] = intTempDivisionID;
            }
        }
    }
    return 0;
}

// Routine will populate structConferenceData
int DBRoutines::DBGetDivisionID(wxString passedDivisionName, int passedLeagueID)
{
	int rc;
	wxString MsgBuffer;
	wxString sqlDivision;

	/* Create SQL statement */
	sqlDivision = "SELECT "  \
		"DivisionID," \
		"DivisionName," \
		"LeagueID," \
		"ConferenceID," \
		"BaseDivisions," \
		"ActiveRec," \
		"CreateTime," \
		"LastUpdateTime" \
		" FROM DIVISIONS " \
		" WHERE DivisionName = ?1 AND LeagueID = ?2 ";

	rc = sqlite3_prepare_v2(m_db, sqlDivision, strlen(sqlDivision), &m_stmtDivision, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtDivision, 1, passedDivisionName, strlen(passedDivisionName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind DivisionName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtDivision, 2, passedLeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedLeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	// Now retrieve inserted record to get new DivisionID value
	while (sqlite3_step(m_stmtDivision) == SQLITE_ROW)
	{
		structDivisionData.DivisionID = (sqlite3_column_int(m_stmtDivision, 0));
		structDivisionData.DivisionName = (sqlite3_column_text(m_stmtDivision, 1));
		structDivisionData.LeagueID = (sqlite3_column_int(m_stmtDivision, 2));
		structDivisionData.ConferenceID = (sqlite3_column_int(m_stmtDivision, 2));
		structDivisionData.BaseDivisions = (sqlite3_column_int(m_stmtDivision, 4));
		structDivisionData.ActiveRec = (sqlite3_column_int(m_stmtDivision, 5));
		structDivisionData.CreateTime = (sqlite3_column_text(m_stmtDivision, 6));
		structDivisionData.LastUpdateTime = (sqlite3_column_text(m_stmtDivision, 7));
	}

	sqlite3_finalize(m_stmtDivision);

    // Return the new DivisionID
    return structDivisionData.DivisionID;
}

// Routine will populate arrays with teams in the league
int DBRoutines::DBGetLeagueID(wxString passedLeagueName)
{
	int rc;
	wxString MsgBuffer;
	wxString sqlLeague;

	/* Create SQL statement */
	sqlLeague = "SELECT "  \
		"LeagueID " \
		" FROM LEAGUES " \
		" WHERE LeagueName = ?1 " ;

	rc = sqlite3_prepare_v2(m_db, sqlLeague, strlen(sqlLeague), &m_stmt, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmt, 1, passedLeagueName, strlen(passedLeagueName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeagueName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	// If there are multiple Leagues with the same name
	// then return the last one retrieved
	while (sqlite3_step(m_stmt) == SQLITE_ROW)
	{
		m_intLeagueID = (sqlite3_column_int(m_stmt, 0));
		structLeagueData.LeagueID = m_intLeagueID;
	}

	sqlite3_finalize(m_stmt);

    return m_intLeagueID;
}

// Routine will populate arrays with teams in the league
int DBRoutines::DBGetTeam(int passedTeamID)
{
    // Now get a list of teams in this league
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlTeam;

	/* Create SQL statement */
	sqlTeam = "SELECT "  \
		"TeamID," \
		"TeamName," \
		"TeamNameShort," \
		"BallparkName," \
		"TotalWins," \
		"TotalLosses," \
		"HomeWins," \
		"HomeLosses," \
		"AwayWins," \
		"AwayLosses," \
		"LeagueID," \
		"ConferenceID," \
		"DivisionID," \
		"TeamYear," \
		"BaseTeam," \
		"ActiveRec" \
		" FROM TEAM " \
		" WHERE TeamID = ?1 " ;

	rc = sqlite3_prepare_v2(m_db, sqlTeam, strlen(sqlTeam), &m_stmtTeam, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	// Bind the data to field '1' which is the first '?' in the INSERT statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtTeam, 1, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	while (sqlite3_step(m_stmtTeam) == SQLITE_ROW)
	{
		structTeamData.TeamID = sqlite3_column_int(m_stmtTeam, 0);
		structTeamData.TeamName = sqlite3_column_text(m_stmtTeam, 1);
		structTeamData.TeamNameShort = sqlite3_column_text(m_stmtTeam, 2);
		structTeamData.BallparkName = sqlite3_column_text(m_stmtTeam, 3);
		structTeamData.TotalWins = sqlite3_column_int(m_stmtTeam, 4);
		structTeamData.TotalLosses = sqlite3_column_int(m_stmtTeam, 5);
		structTeamData.HomeWins = sqlite3_column_int(m_stmtTeam, 6);
		structTeamData.HomeLosses = sqlite3_column_int(m_stmtTeam, 7);
		structTeamData.AwayWins = sqlite3_column_int(m_stmtTeam, 8);
		structTeamData.AwayLosses = sqlite3_column_int(m_stmtTeam, 9);
		structTeamData.LeagueID = sqlite3_column_int(m_stmtTeam, 10);
		structTeamData.ConferenceID = sqlite3_column_int(m_stmtTeam, 11);
		structTeamData.DivisionID = sqlite3_column_int(m_stmtTeam, 12);
		structTeamData.TeamYear = sqlite3_column_int(m_stmtTeam, 13);
		structTeamData.BaseTeam = sqlite3_column_int(m_stmtTeam, 14);
		structTeamData.ActiveRec = sqlite3_column_int(m_stmtTeam, 15);

//        Foobar.Printf( wxT("Team Name: %s"), strTeamName);
//        wxMessageBox(Foobar);

	}

	// structTeamData.TeamID is the same as the passedTeamID.
	// Making sure that m_intTeamID is set to the same team we just populated with
	m_intTeamID = structTeamData.TeamID;

	sqlite3_finalize(m_stmtTeam);

    return m_intTeamID;
}

// Routine will return the selected TeamID
int DBRoutines::DBSelectTeam(int passedLeagueID, int passedConferenceID, int passedDivisionID)
{
    // Now get a list of teams in this league
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlTeam;

	/* Create SQL statement */
	sqlTeam = "SELECT "  \
		"TeamID," \
		"TeamName" \
		" FROM TEAM " \
		" WHERE LeagueID = ?1 AND ConferenceID = ?2 AND DivisionID = ?3" ;

	rc = sqlite3_prepare_v2(m_db, sqlTeam, strlen(sqlTeam), &m_stmtTeam, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtTeam, 1, passedLeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedLeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 2, passedConferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedConferenceID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 3, passedDivisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedDivisionID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	m_arrayTeamIDs.Clear();
	m_arrayTeamNames.Clear();

	while (sqlite3_step(m_stmtTeam) == SQLITE_ROW)
	{
		m_arrayTeamIDs.Add(sqlite3_column_int(m_stmtTeam, 0));
		m_arrayTeamNames.Add(sqlite3_column_text(m_stmtTeam, 1));

//        Foobar.Printf( wxT("Team Name: %s"), strTeamName);
//        wxMessageBox(Foobar);

	}

	sqlite3_finalize(m_stmtTeam);

    DBSortTeamNames();

    // Display a dialog to allow team selection from the array of teams
    wxSingleChoiceDialog dialogTeam(wxGetApp().GetTopWindow(),
                                "Team List",
                                "Please select a Team",
                                m_arrayTeamNames);

// DEFAULT team name has no teams associated with it and causes and error
    if (m_strLeagueName != "DEFAULT")
    {
        dialogTeam.SetSelection(0);
    }

    if (dialogTeam.ShowModal() == wxID_OK)
    {
        m_strTeamName = dialogTeam.GetStringSelection();
        m_intTeamID = m_arrayTeamIDs[dialogTeam.GetSelection()];

        // Get all of the team data and place it into structTeamData
        DBGetTeam( m_intTeamID );

//        wxMessageDialog dialogTeam2(wxGetApp().GetTopWindow(), m_strTeamName, "Got Team");
//        dialogTeam2.ShowModal();
    }
    return m_intTeamID;
}

// Routine will populate arrays with teams in the league
int DBRoutines::DBGetTeamArray(int passedLeagueID)
{
    // Now get a list of teams in this league
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlTeam;

	/* Create SQL statement */
	sqlTeam = "SELECT "  \
		"TeamID," \
		"TeamName" \
		" FROM TEAM " \
		" WHERE LeagueID = ?1 " ;

	rc = sqlite3_prepare_v2(m_db, sqlTeam, strlen(sqlTeam), &m_stmtTeam, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
//	else
//	{
//		MsgBuffer.Printf( wxT("Prepare for TEAMS Select OK:\n"));
//        wxMessageBox(MsgBuffer);
//	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtTeam, 1, passedLeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedLeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	m_arrayTeamIDs.Clear();
	m_arrayTeamNames.Clear();

	while (sqlite3_step(m_stmtTeam) == SQLITE_ROW)
	{
		m_arrayTeamIDs.Add(sqlite3_column_int(m_stmtTeam, 0));
		m_arrayTeamNames.Add(sqlite3_column_text(m_stmtTeam, 1));

//        Foobar.Printf( wxT("Team Name: %s"), strTeamName);
//        wxMessageBox(Foobar);

	}

	sqlite3_finalize(m_stmtTeam);

    DBSortTeamNames();

    // Display a dialog to allow team selection from the array of teams
    wxSingleChoiceDialog dialogTeam(wxGetApp().GetTopWindow(),
                                "Team List",
                                "Please select a Team",
                                m_arrayTeamNames);

// DEFAULT team name has no teams associated with it and causes and error
    if (m_strLeagueName != "DEFAULT")
    {
        dialogTeam.SetSelection(0);
    }

    if (dialogTeam.ShowModal() == wxID_OK)
    {
        m_strTeamName = dialogTeam.GetStringSelection();
        m_intTeamID = m_arrayTeamIDs[dialogTeam.GetSelection()];

        // Get all of the team data and place it into structTeamData
        DBGetTeam( m_intTeamID );

//        wxMessageDialog dialogTeam2(wxGetApp().GetTopWindow(), m_strTeamName, "Got Team");
//        dialogTeam2.ShowModal();
    }
    return m_intTeamID;
}

// Routine will sort the Team names and ID in seperate arrays
int DBRoutines::DBSortTeamNames()
{
    wxString strTempTeamName;
    int intTempTeamID;
    int intArrayCount=0;
    int i;
    int j;

    intArrayCount = m_arrayTeamNames.GetCount();

    for ( i=0; i<intArrayCount; i++ )
    {
        for ( j=i+1; j<intArrayCount; j++ )
        {
            if ( m_arrayTeamNames[i] > m_arrayTeamNames[j] )
            {
                strTempTeamName = m_arrayTeamNames[i];
                m_arrayTeamNames[i] = m_arrayTeamNames[j];
                m_arrayTeamNames[j] = strTempTeamName;
                intTempTeamID = m_arrayTeamIDs[i];
                m_arrayTeamIDs[i] = m_arrayTeamIDs[j];
                m_arrayTeamIDs[j] = intTempTeamID;
            }
        }
    }
    return 0;
}

// Routine will return the BatterStatsID in a given team
int DBRoutines::DBGetBatterStatsID(int passedTeamID)
{
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlBatter;
	wxString sqlBatterStats;
	wxString strTemp;

	// Create SQL statement for PitcherStats retrival
	sqlBatterStats = "SELECT "  \
		"S.BatterStatsID," \
		"B.BatterID," \
		"B.FirstName," \
		"B.LastName " \
		" FROM BATTERSTATS AS S " \
		" JOIN BATTER AS B " \
		" ON S.BatterID=B.BatterID " \
		" WHERE S.TeamID = ?1 ";

	rc = sqlite3_prepare_v2(m_db, sqlBatterStats, strlen(sqlBatterStats), &m_stmtBatter, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtBatter, 1, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	m_arrayBatterStatsIDs.Clear();
	m_arrayBatterFirstNames.Clear();
	m_arrayBatterLastNames.Clear();
	m_arrayBatterFullNames.Clear();

	while (sqlite3_step(m_stmtBatter) == SQLITE_ROW)
	{
		m_arrayBatterStatsIDs.Add(sqlite3_column_int(m_stmtBatter, 0));
		m_arrayBatterFirstNames.Add(sqlite3_column_text(m_stmtBatter, 2));
		m_arrayBatterLastNames.Add(sqlite3_column_text(m_stmtBatter, 3));
		strTemp.Clear();
		strTemp.Append(sqlite3_column_text(m_stmtBatter, 2));
		strTemp.Append(" ");
		strTemp.Append(sqlite3_column_text(m_stmtBatter, 3));
		m_arrayBatterFullNames.Add(strTemp);
	}

	sqlite3_finalize(m_stmtBatter);

	DBSortBatterNames();

    return m_intBatterStatsID;
}

// Routine will return the BatterStatsID in a given player on a team
int DBRoutines::DBGetBatterStatsID(int passedTeamID, int passedBatterID)
{
	int rc;
	wxString MsgBuffer;
	wxString sqlBatterData;

    // Create SQL statement for BatterStats retrival
    sqlBatterData = "SELECT "  \
		" BatterStatsID " \
		" FROM BATTERSTATS " \
		" WHERE TeamID = ?1 " \
		" and " \
		" BatterID = ?2 ";

	rc = sqlite3_prepare_v2(m_db, sqlBatterData, strlen(sqlBatterData), &m_stmtBatter, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	// Bind the data to field '1' which is the first '?' in the SELECT statement
	rc = sqlite3_bind_int(m_stmtBatter, 1, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatter, 2, passedBatterID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedBatterID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	while (sqlite3_step(m_stmtBatter) == SQLITE_ROW)
	{
		m_intBatterStatsID = sqlite3_column_int(m_stmtBatter, 0);
	}

	sqlite3_finalize(m_stmtBatter);

    return m_intBatterStatsID;
}

// Routine will sort the Pitcher names and ID in seperate arrays
int DBRoutines::DBSortBatterNames()
{
    wxString strTempBatterFirstName;
    wxString strTempBatterLastName;
    wxString strTempBatterFullName;
    int intTempBatterStatsID;
    int intArrayCount=0;
    int i;
    int j;

    intArrayCount = m_arrayBatterFullNames.GetCount();

    for ( i=0; i<intArrayCount; i++ )
    {
        for ( j=i+1; j<intArrayCount; j++ )
        {
            if ( m_arrayBatterFullNames[i] > m_arrayBatterFullNames[j] )
            {
                strTempBatterFullName = m_arrayBatterFullNames[i];
                m_arrayBatterFullNames[i] = m_arrayBatterFullNames[j];
                m_arrayBatterFullNames[j] = strTempBatterFullName;
                strTempBatterFirstName = m_arrayBatterFirstNames[i];
                m_arrayBatterFirstNames[i] = m_arrayBatterFirstNames[j];
                m_arrayBatterFirstNames[j] = strTempBatterFirstName;
                strTempBatterLastName = m_arrayBatterLastNames[i];
                m_arrayBatterLastNames[i] = m_arrayBatterLastNames[j];
                m_arrayBatterLastNames[j] = strTempBatterLastName;
                intTempBatterStatsID = m_arrayBatterStatsIDs[i];
                m_arrayBatterStatsIDs[i] = m_arrayBatterStatsIDs[j];
                m_arrayBatterStatsIDs[j] = intTempBatterStatsID;
            }
        }
    }
    return 0;
}

// Routine will return the BatterStatsID
int DBRoutines::DBGetBatterData(int passedBatterStatsID)
{
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlBatterData;
	wxString strTemp;

		// Create SQL statement for BatterStats retrival
	sqlBatterData = "SELECT "  \
		"S.BatterStatsID," \
		"S.AB," \
		"S.Runs," \
		"S.Hits," \
		"S.RBI," \
		"S.Doubles," \
		"S.Triples," \
		"S.HomeRuns," \
		"S.Walk," \
		"S.Stirkeout," \
		"S.ReachedOnError," \
		"S.Sacrifice," \
		"S.StolenBase," \
		"S.CS," \
		"S.Games," \
		"S.HBP," \
		"S.AVG," \
		"S.SLG," \
		"S.OBP," \
		"S.BatterID," \
		"S.TeamID," \
		"B.BatterID," \
		"B.FirstName," \
		"B.LastName," \
		"B.Pitcher," \
		"B.Catcher," \
		"B.FirstBase," \
		"B.SecondBase," \
		"B.ShortStop," \
		"B.ThirdBase," \
		"B.LeftField," \
		"B.CenterField," \
		"B.RightField," \
		"B.Bunting," \
		"B.HitRun," \
		"B.Running," \
		"B.Stealing," \
		"B.CatchArm," \
		"B.OutArm," \
		"B.PowerRight," \
		"B.PowerLeft," \
		"B.Pass," \
		"B.TRate," \
		"B.ER1," \
		"B.ER2," \
		"B.ER3," \
		"B.ER4," \
		"B.ER5," \
		"B.ER6," \
		"B.ER7," \
		"B.ER8," \
		"B.ER9," \
		"B.BatterHits," \
		"B.TeamID," \
		"B.OBChanceHomeRun," \
		"B.OBChanceTriple," \
		"B.OBChanceDouble," \
		"B.OBChanceSingle," \
		"B.OBChanceWalk," \
		"B.ChanceDoublePlay," \
		"B.OBChanceHomeRunRight," \
		"B.OBChanceTripleRight," \
		"B.OBChanceDoubleRight," \
		"B.OBChanceSingleRight," \
		"B.OBChanceWalkRight," \
		"B.ChanceDoublePlayRight," \
		"B.OBChanceHomeRunLeft," \
		"B.OBChanceTripleLeft," \
		"B.OBChanceDoubleLeft," \
		"B.OBChanceSingleLeft," \
		"B.OBChanceWalkLeft," \
		"B.ChanceDoublePlayLeft," \
		"B.OBChanceBasic," \
		"B.OBChanceLeft," \
		"B.OBChanceRight" \
		" FROM BATTERSTATS AS S " \
		" JOIN BATTER AS B " \
		" ON S.BatterID=B.BatterID " \
		" WHERE S.BatterStatsID = ?1 ";

	rc = sqlite3_prepare_v2(m_db, sqlBatterData, strlen(sqlBatterData), &m_stmtBatter, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtBatter, 1, passedBatterStatsID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedBatterStatsID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	while (sqlite3_step(m_stmtBatter) == SQLITE_ROW)
	{
		structBatterStats.BatterStatsID = sqlite3_column_int(m_stmtBatter, 0);
		structBatterStats.AB = sqlite3_column_int(m_stmtBatter, 1);
		structBatterStats.Runs = sqlite3_column_int(m_stmtBatter, 2);
		structBatterStats.Hits = sqlite3_column_int(m_stmtBatter, 3);
		structBatterStats.RBI = sqlite3_column_int(m_stmtBatter, 4);
		structBatterStats.Doubles = sqlite3_column_int(m_stmtBatter, 5);
		structBatterStats.Triples = sqlite3_column_int(m_stmtBatter, 6);
        structBatterStats.HomeRuns = sqlite3_column_int(m_stmtBatter, 7);
        structBatterStats.Walk = sqlite3_column_int(m_stmtBatter, 8);
        structBatterStats.Strikeout = sqlite3_column_int(m_stmtBatter, 9);
        structBatterStats.ReachedOnError = sqlite3_column_int(m_stmtBatter, 10);
        structBatterStats.Sacrifice = sqlite3_column_int(m_stmtBatter, 11);
        structBatterStats.StollenBase = sqlite3_column_int(m_stmtBatter, 12);
        structBatterStats.CS = (float)sqlite3_column_double(m_stmtBatter, 13);
        structBatterStats.Games = (float)sqlite3_column_double(m_stmtBatter, 14);
        structBatterStats.HBP = sqlite3_column_int(m_stmtBatter, 15);
		structBatterStats.AVG = (float)sqlite3_column_double(m_stmtBatter, 16);
		structBatterStats.SLG = (float)sqlite3_column_double(m_stmtBatter, 17);
		structBatterStats.OBP = (float)sqlite3_column_double(m_stmtBatter, 18);
        structBatterStats.BatterID = sqlite3_column_int(m_stmtBatter, 19);
        structBatterStats.TeamID = sqlite3_column_int(m_stmtBatter, 20);

        structBatterData.BatterID = sqlite3_column_int(m_stmtBatter, 21);
        structBatterData.FirstName = sqlite3_column_text(m_stmtBatter, 22);
        structBatterData.LastName = sqlite3_column_text(m_stmtBatter, 23);
        structBatterData.Pitcher = sqlite3_column_int(m_stmtBatter, 24);
        structBatterData.Catcher = sqlite3_column_int(m_stmtBatter, 25);
        structBatterData.FirstBase = sqlite3_column_int(m_stmtBatter, 26);
        structBatterData.SecondBase = sqlite3_column_int(m_stmtBatter, 27);
        structBatterData.Shortstop = sqlite3_column_int(m_stmtBatter, 28);
        structBatterData.ThirdBase = sqlite3_column_int(m_stmtBatter, 29);
        structBatterData.LeftField = sqlite3_column_int(m_stmtBatter, 30);
        structBatterData.CenterField = sqlite3_column_int(m_stmtBatter, 31);
        structBatterData.RightField = sqlite3_column_int(m_stmtBatter, 32);
        structBatterData.Bunting = sqlite3_column_int(m_stmtBatter, 33);
        structBatterData.HitRun = sqlite3_column_int(m_stmtBatter, 34);
        structBatterData.Running = sqlite3_column_int(m_stmtBatter, 35);
        structBatterData.Stealing = sqlite3_column_int(m_stmtBatter, 36);
        structBatterData.CatchArm = sqlite3_column_int(m_stmtBatter, 37);
        structBatterData.OutArm = sqlite3_column_int(m_stmtBatter, 38);
        structBatterData.PowerRight = sqlite3_column_int(m_stmtBatter, 39);
        structBatterData.PowerLeft = sqlite3_column_int(m_stmtBatter, 40);
        structBatterData.Pass = sqlite3_column_int(m_stmtBatter, 41);
        structBatterData.TRate = sqlite3_column_int(m_stmtBatter, 42);
        structBatterData.ER1 = sqlite3_column_int(m_stmtBatter, 43);
        structBatterData.ER2 = sqlite3_column_int(m_stmtBatter, 44);
        structBatterData.ER3 = sqlite3_column_int(m_stmtBatter, 45);
        structBatterData.ER4 = sqlite3_column_int(m_stmtBatter, 46);
        structBatterData.ER5 = sqlite3_column_int(m_stmtBatter, 47);
        structBatterData.ER6 = sqlite3_column_int(m_stmtBatter, 48);
        structBatterData.ER7 = sqlite3_column_int(m_stmtBatter, 49);
        structBatterData.ER8 = sqlite3_column_int(m_stmtBatter, 50);
        structBatterData.ER9 = sqlite3_column_int(m_stmtBatter, 51);
        structBatterData.BatterHits = sqlite3_column_int(m_stmtBatter, 52);
        structBatterData.TeamID = sqlite3_column_int(m_stmtBatter, 53);

        structBatterData.OBChanceHomeRun = (float)sqlite3_column_double(m_stmtBatter, 54);
        structBatterData.OBChanceTriple = (float)sqlite3_column_double(m_stmtBatter, 55);
        structBatterData.OBChanceDouble = (float)sqlite3_column_double(m_stmtBatter, 56);
        structBatterData.OBChanceSingle = (float)sqlite3_column_double(m_stmtBatter, 57);
        structBatterData.OBChanceWalk = (float)sqlite3_column_double(m_stmtBatter, 58);
        structBatterData.ChanceDoublePlay = (float)sqlite3_column_double(m_stmtBatter, 59);
        structBatterData.OBChanceHomeRunRight = (float)sqlite3_column_double(m_stmtBatter, 60);
        structBatterData.OBChanceTripleRight = (float)sqlite3_column_double(m_stmtBatter, 61);
        structBatterData.OBChanceDoubleRight = (float)sqlite3_column_double(m_stmtBatter, 62);
        structBatterData.OBChanceSingleRight = (float)sqlite3_column_double(m_stmtBatter, 63);
        structBatterData.OBChanceWalkRight = (float)sqlite3_column_double(m_stmtBatter, 64);
        structBatterData.ChanceDoublePlayRight = (float)sqlite3_column_double(m_stmtBatter, 65);
        structBatterData.OBChanceHomeRunLeft = (float)sqlite3_column_double(m_stmtBatter, 66);
        structBatterData.OBChanceTripleLeft = (float)sqlite3_column_double(m_stmtBatter, 67);
        structBatterData.OBChanceDoubleLeft = (float)sqlite3_column_double(m_stmtBatter, 68);
        structBatterData.OBChanceSingleLeft = (float)sqlite3_column_double(m_stmtBatter, 69);
        structBatterData.OBChanceWalkLeft = (float)sqlite3_column_double(m_stmtBatter, 70);
        structBatterData.ChanceDoublePlayLeft = (float)sqlite3_column_double(m_stmtBatter, 71);
        structBatterData.OBChanceBasic = (float)sqlite3_column_double(m_stmtBatter, 72);
        structBatterData.OBChanceLeft = (float)sqlite3_column_double(m_stmtBatter, 73);
        structBatterData.OBChanceRight = (float)sqlite3_column_double(m_stmtBatter, 74);
	}

	sqlite3_finalize(m_stmtBatter);

	m_intBatterStatsID = structBatterStats.BatterStatsID;

    return m_intBatterStatsID;
}

// Routine will return the PitcherID based on TeamID and FIrst and Last names
int DBRoutines::DBGetBatterID(int passedTeamID, wxString passedFirstName, wxString passedLastName)
{
	int rc;
	wxString MsgBuffer;
	wxString sqlBatterData;

    // Create SQL statement for BatterID retrival
    sqlBatterData = "SELECT "  \
		" BatterID " \
		" FROM BATTER " \
		" WHERE TeamID = ?1 " \
		" and " \
		" FirstName = ?2 " \
		" and " \
		" LastName = ?3 ";

	rc = sqlite3_prepare_v2(m_db, sqlBatterData, strlen(sqlBatterData), &m_stmtBatter, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	// Bind the data to field '1' which is the first '?' in the SELECT statement
	rc = sqlite3_bind_int(m_stmtBatter, 1, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtBatter, 2, structBatterData.FirstName, strlen(passedFirstName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind FirstName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtBatter, 3, structBatterData.LastName, strlen(passedLastName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LastName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	while (sqlite3_step(m_stmtBatter) == SQLITE_ROW)
	{
		m_intBatterID = sqlite3_column_int(m_stmtBatter, 0);
	}

	sqlite3_finalize(m_stmtBatter);

    return m_intBatterID;
}

// Routine will Update the data in the batter data record
int DBRoutines::DBUpdateBatterData(int passedBatterID)
{
    int rc;
	wxString MsgBuffer;
    wxString sqlBatter;

    // Create SQL statement for sqlBatter update
	sqlBatter = "UPDATE BATTER "  \
		"SET " \
		"FirstName = ?1," \
		"LastName = ?2," \
		"Pitcher = ?3," \
		"Catcher = ?4," \
		"FirstBase = ?5," \
		"SecondBase = ?6," \
		"ShortStop = ?7," \
		"ThirdBase = ?8," \
		"LeftField = ?9," \
		"CenterField = ?10," \
		"RightField = ?11," \
		"Bunting = ?12," \
		"HitRun = ?13," \
		"Running = ?14," \
		"Stealing = ?15," \
		"CatchArm = ?16," \
		"OutArm = ?17," \
		"PowerRight = ?18," \
		"PowerLeft = ?19," \
		"Pass = ?20," \
		"TRate = ?21," \
		"ER1 = ?22," \
		"ER2 = ?23," \
		"ER3 = ?24," \
		"ER4 = ?25," \
		"ER5 = ?26," \
		"ER6 = ?27," \
		"ER7 = ?28," \
		"ER8 = ?29," \
		"ER9 = ?30," \
		"BatterHits = ?31," \
		"TeamID = ?32," \
		"OBChanceHomeRun = ?33," \
		"OBChanceTriple = ?34," \
		"OBChanceDouble = ?35," \
		"OBChanceSingle = ?36," \
		"OBChanceWalk = ?37," \
		"ChanceDoublePlay = ?38," \
		"OBChanceHomeRunRight = ?39," \
		"OBChanceTripleRight = ?40," \
		"OBChanceDoubleRight = ?41," \
		"OBChanceSingleRight = ?42," \
		"OBChanceWalkRight = ?43," \
		"ChanceDoublePlayRight = ?44," \
		"OBChanceHomeRunLeft = ?45," \
		"OBChanceTripleLeft = ?46," \
		"OBChanceDoubleLeft = ?47," \
		"OBChanceSingleLeft = ?48," \
		"OBChanceWalkLeft = ?49," \
		"ChanceDoublePlayLeft = ?50," \
		"OBChanceBasic = ?51," \
		"OBChanceLeft = ?52," \
		"OBChanceRight = ?53," \
		"LastUpdateTime = datetime('NOW','localtime')" \
		" WHERE BatterID = ?54 ";

	rc = sqlite3_prepare_v2(m_db, sqlBatter, strlen(sqlBatter), &m_stmtBatterUpdateData, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_text(m_stmtBatterUpdateData, 1, structBatterData.FirstName, strlen(structBatterData.FirstName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind FirstName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtBatterUpdateData, 2, structBatterData.LastName, strlen(structBatterData.LastName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LastName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 3, structBatterData.Pitcher);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Pitcher: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 4, structBatterData.Catcher);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Catcher: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 5, structBatterData.FirstBase);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind FirstBase: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 6, structBatterData.SecondBase);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind SecondBase: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 7, structBatterData.Shortstop);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Shortstop: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 8, structBatterData.ThirdBase);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ThirdBase: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 9, structBatterData.LeftField);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeftField: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 10, structBatterData.CenterField);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind CenterField: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 11, structBatterData.RightField);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind RightField: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 12, structBatterData.Bunting);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Bunting: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 13, structBatterData.HitRun);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HitRun: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 14, structBatterData.Running);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Running: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 15, structBatterData.Stealing);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Stealing: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 16, structBatterData.CatchArm);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind CatchArm: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 17, structBatterData.OutArm);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OutArm: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 18, structBatterData.PowerRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind PowerRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 19, structBatterData.PowerLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind PowerLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 20, structBatterData.Pass);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Pass: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 21, structBatterData.TRate);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TRate: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 22, structBatterData.ER1);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER1: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 23, structBatterData.ER2);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER2: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 24, structBatterData.ER3);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER3: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 25, structBatterData.ER4);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER4: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 26, structBatterData.ER5);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER5: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 27, structBatterData.ER6);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER6: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 28, structBatterData.ER7);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER7: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 29, structBatterData.ER8);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER8: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 30, structBatterData.ER9);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER9: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 31, structBatterData.BatterHits);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind BatterHits: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 32, structBatterData.TeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 33, structBatterData.OBChanceHomeRun);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRun: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 34, structBatterData.OBChanceTriple);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTriple: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 35, structBatterData.OBChanceDouble);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDouble: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 36, structBatterData.OBChanceSingle);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingle: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 37, structBatterData.OBChanceWalk);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalk: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 38, structBatterData.ChanceDoublePlay);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlay: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 39, structBatterData.OBChanceHomeRunRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRunRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 40, structBatterData.OBChanceTripleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTripleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 41, structBatterData.OBChanceDoubleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDoubleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 42, structBatterData.OBChanceSingleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 43, structBatterData.OBChanceWalkRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalkRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 44, structBatterData.ChanceDoublePlayRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlayRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 45, structBatterData.OBChanceHomeRunLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRunLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 46, structBatterData.OBChanceTripleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTripleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 47, structBatterData.OBChanceDoubleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDoubleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 48, structBatterData.OBChanceSingleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 49, structBatterData.OBChanceWalkLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalkLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 50, structBatterData.ChanceDoublePlayLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlayLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 51, structBatterData.OBChanceBasic);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceBasic: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 52, structBatterData.OBChanceLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateData, 53, structBatterData.OBChanceRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateData, 54, structBatterData.BatterID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind BatterID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtBatterUpdateData);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Update BatterData %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtBatterUpdateData);

    return TRUE;

}

// Routine will Update the data in the batter stats record
int DBRoutines::DBUpdateBatterStats(int passedBatterStatsID)
{
    int rc;
	wxString MsgBuffer;
    wxString sqlBatterStats;

    // Create SQL statement for sqlBatterStats update
	sqlBatterStats = "UPDATE BATTERSTATS "  \
        "SET "  \
		"AB = ?1," \
		"Runs = ?2," \
		"Hits = ?3," \
		"RBI = ?4," \
		"Doubles = ?5," \
		"Triples = ?6," \
		"HomeRuns = ?7," \
		"Walk = ?8," \
		"Stirkeout = ?9," \
		"ReachedOnError = ?10," \
		"Sacrifice = ?11," \
		"StolenBase = ?12," \
		"CS = ?13," \
		"Games = ?14," \
		"HBP = ?15," \
		"AVG = ?16," \
		"SLG = ?17," \
		"OBP = ?18," \
		"BatterID = ?19," \
		"TeamID = ?20," \
		"LastUpdateTime = datetime('NOW','localtime')" \
		" WHERE BatterStatsID = ?21 ";

//    wxString Foobar;
//    Foobar.Printf( wxT("Pitcher Starts = %i"), structPitcherStats.Starts);
//    wxMessageBox(Foobar);
//    Foobar.Printf( wxT("Pitcher StatsID = %i"), passedPitcherStatsID);
//    wxMessageBox(Foobar);

	rc = sqlite3_prepare_v2(m_db, sqlBatterStats, strlen(sqlBatterStats), &m_stmtBatterUpdateStats, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 1, structBatterStats.AB);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind AB: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 2, structBatterStats.Runs);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Runs: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 3, structBatterStats.Hits);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Hits: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 4, structBatterStats.RBI);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind RBI: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 5, structBatterStats.Doubles);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Doubles: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 6, structBatterStats.Triples);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Triples: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 7, structBatterStats.HomeRuns);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HomeRuns: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 8, structBatterStats.Walk);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Walk: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 9, structBatterStats.Strikeout);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Strikeout: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 10, structBatterStats.ReachedOnError);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ReachedOnError: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 11, structBatterStats.Sacrifice);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Sacrifice: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 12, structBatterStats.StollenBase);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind StollenBase: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 13, structBatterStats.CS);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind CS: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 14, structBatterStats.Games);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Games: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 15, structBatterStats.HBP);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HBP: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateStats, 16, structBatterStats.AVG);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind AVG: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateStats, 17, structBatterStats.SLG);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind SLG: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterUpdateStats, 18, structBatterStats.OBP);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBP: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 19, structBatterStats.BatterID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind BatterID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 20, structBatterStats.TeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterUpdateStats, 21, passedBatterStatsID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedBatterStatsID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtBatterUpdateStats);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Update BatterStats %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtBatterUpdateStats);

    return TRUE;
}

// Routine will return the PitcherStatsID in a given team
int DBRoutines::DBGetPitcherStatsID(int passedTeamID)
{
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
	wxString sqlPitcher;
	wxString sqlPitcherStats;
	wxString strTemp;

	// Create SQL statement for PitcherStats retrival
	sqlPitcherStats = "SELECT "  \
		"S.PitcherStatsID," \
		"P.PitcherID," \
		"P.FirstName," \
		"P.LastName " \
		" FROM PITCHERSTATS AS S " \
		" JOIN PITCHER AS P " \
		" ON S.PitcherID=P.PitcherID " \
		" WHERE S.TeamID = ?1 ";

	rc = sqlite3_prepare_v2(m_db, sqlPitcherStats, strlen(sqlPitcherStats), &m_stmtPitcher, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtPitcher, 1, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	m_arrayPitcherStatsIDs.Clear();
	m_arrayPitcherFirstNames.Clear();
	m_arrayPitcherLastNames.Clear();
	m_arrayPitcherFullNames.Clear();

	while (sqlite3_step(m_stmtPitcher) == SQLITE_ROW)
	{
		m_arrayPitcherStatsIDs.Add(sqlite3_column_int(m_stmtPitcher, 0));
		m_arrayPitcherFirstNames.Add(sqlite3_column_text(m_stmtPitcher, 2));
		m_arrayPitcherLastNames.Add(sqlite3_column_text(m_stmtPitcher, 3));
		strTemp.Clear();
		strTemp.Append(sqlite3_column_text(m_stmtPitcher, 2));
		strTemp.Append(" ");
		strTemp.Append(sqlite3_column_text(m_stmtPitcher, 3));
		m_arrayPitcherFullNames.Add(strTemp);
	}

	sqlite3_finalize(m_stmtPitcher);

	DBSortPitcherNames();

    return m_intPitcherStatsID;
}

// Routine will sort the Pitcher names and ID in seperate arrays
int DBRoutines::DBSortPitcherNames()
{
    wxString strTempPitcherFirstName;
    wxString strTempPitcherLastName;
    wxString strTempPitcherFullName;
    int intTempPitcherStatsID;
    int intArrayCount=0;
    int i;
    int j;

    intArrayCount = m_arrayPitcherFullNames.GetCount();

    for ( i=0; i<intArrayCount; i++ )
    {
        for ( j=i+1; j<intArrayCount; j++ )
        {
            if ( m_arrayPitcherFullNames[i] > m_arrayPitcherFullNames[j] )
            {
                strTempPitcherFullName = m_arrayPitcherFullNames[i];
                m_arrayPitcherFullNames[i] = m_arrayPitcherFullNames[j];
                m_arrayPitcherFullNames[j] = strTempPitcherFullName;
                strTempPitcherFirstName = m_arrayPitcherFirstNames[i];
                m_arrayPitcherFirstNames[i] = m_arrayPitcherFirstNames[j];
                m_arrayPitcherFirstNames[j] = strTempPitcherFirstName;
                strTempPitcherLastName = m_arrayPitcherLastNames[i];
                m_arrayPitcherLastNames[i] = m_arrayPitcherLastNames[j];
                m_arrayPitcherLastNames[j] = strTempPitcherLastName;
                intTempPitcherStatsID = m_arrayPitcherStatsIDs[i];
                m_arrayPitcherStatsIDs[i] = m_arrayPitcherStatsIDs[j];
                m_arrayPitcherStatsIDs[j] = intTempPitcherStatsID;
            }
        }
    }
    return 0;
}

// Routine will return the PitcherStatsID
int DBRoutines::DBGetPitcherData(int passedPitcherStatsID)
{
	int rc;
	wxString MsgBuffer;
	wxString Foobar;
//	wxString sqlPitcher;
	wxString sqlPitcherData;
	wxString strTemp;

		// Create SQL statement for BatterStats retrival
	sqlPitcherData = "SELECT "  \
		"S.PitcherStatsID," \
		"S.Wins," \
		"S.Loss," \
		"S.Saves," \
		"S.InningsPitched," \
		"S.ER," \
		"S.Hits," \
		"S.Walks," \
		"S.Strikeouts," \
		"S.HomeRuns," \
		"S.Games," \
		"S.CompleteGames," \
		"S.Starts," \
		"S.ERA," \
		"S.WHIP," \
		"S.PitcherID," \
		"S.TeamID," \
		"P.PitcherID," \
		"P.FirstName," \
		"P.LastName, " \
		"P.OBChanceHomeRun, " \
		"P.OBChanceTriple, " \
		"P.OBChanceDouble, " \
		"P.OBChanceSingle, " \
		"P.OBChanceWalk, " \
		"P.ChanceDoublePlay, " \
		"P.OBChanceHomeRunRight, " \
		"P.OBChanceTripleRight, " \
		"P.OBChanceDoubleRight, " \
		"P.OBChanceSingleRight, " \
		"P.OBChanceWalkRight, " \
		"P.ChanceDoublePlayRight, " \
		"P.OBChanceHomeRunLeft, " \
		"P.OBChanceTripleLeft, " \
		"P.OBChanceDoubleLeft, " \
		"P.OBChanceSingleLeft, " \
		"P.OBChanceWalkLeft, " \
		"P.ChanceDoublePlayLeft, " \
		"P.OBChanceBasic, " \
		"P.OBChanceLeft, " \
		"P.OBChanceRight, " \
		"P.Starter, " \
		"P.Relief, " \
		"P.Throws, " \
		"P.Bunting, " \
		"P.Hold, " \
		"P.WP, " \
		"P.Balk, " \
		"P.Pitcher, " \
		"P.ER1, " \
		"P.TeamID" \
		" FROM PITCHERSTATS AS S " \
		" JOIN PITCHER AS P " \
		" ON S.PitcherID=P.PitcherID " \
		" WHERE S.PitcherStatsID = ?1 ";

	rc = sqlite3_prepare_v2(m_db, sqlPitcherData, strlen(sqlPitcherData), &m_stmtPitcher, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
//	else
//	{
//		MsgBuffer.Printf( wxT("Prepare for Pitcher Select OK:\n"));
//        wxMessageBox(MsgBuffer);
//	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtPitcher, 1, passedPitcherStatsID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
//	else
//	{
//		MsgBuffer.Printf( wxT("Bind for Pitcher Select OK:\n"));
//        wxMessageBox(MsgBuffer);
//	}

//	m_arrayPitcherStatsIDs.Clear();
//	m_arrayPitcherFirstNames.Clear();
//	m_arrayPitcherLastNames.Clear();
//	m_arrayPitcherFullNames.Clear();
//
	while (sqlite3_step(m_stmtPitcher) == SQLITE_ROW)
	{
		structPitcherStats.PitcherStatsID = sqlite3_column_int(m_stmtPitcher, 0);
		structPitcherStats.Wins = sqlite3_column_int(m_stmtPitcher, 1);
		structPitcherStats.Loss = sqlite3_column_int(m_stmtPitcher, 2);
		structPitcherStats.Saves = sqlite3_column_int(m_stmtPitcher, 3);
//		structPitcherStats.InningsPitched = sqlite3_column_real(m_stmtPitcher, 4);
		structPitcherStats.InningsPitched = (float)sqlite3_column_double(m_stmtPitcher, 4);
		structPitcherStats.ER = sqlite3_column_int(m_stmtPitcher, 5);
		structPitcherStats.Hits = sqlite3_column_int(m_stmtPitcher, 6);
        structPitcherStats.Walks = sqlite3_column_int(m_stmtPitcher, 7);
        structPitcherStats.Strikeouts = sqlite3_column_int(m_stmtPitcher, 8);
        structPitcherStats.HomeRuns = sqlite3_column_int(m_stmtPitcher, 9);
        structPitcherStats.Games = sqlite3_column_int(m_stmtPitcher, 10);
        structPitcherStats.CompleteGames = sqlite3_column_int(m_stmtPitcher, 11);
        structPitcherStats.Starts = sqlite3_column_int(m_stmtPitcher, 12);
        structPitcherStats.ERA = (float)sqlite3_column_double(m_stmtPitcher, 13);
        structPitcherStats.WHIP = (float)sqlite3_column_double(m_stmtPitcher, 14);
        structPitcherStats.PitcherID = sqlite3_column_int(m_stmtPitcher, 15);
        structPitcherStats.TeamID = sqlite3_column_int(m_stmtPitcher, 16);

        structPitcherData.PitcherID = sqlite3_column_int(m_stmtPitcher, 17);
        structPitcherData.FirstName = sqlite3_column_text(m_stmtPitcher, 18);
        structPitcherData.LastName = sqlite3_column_text(m_stmtPitcher, 19);
        structPitcherData.OBChanceHomeRun = (float)sqlite3_column_double(m_stmtPitcher, 20);
        structPitcherData.OBChanceTriple = (float)sqlite3_column_double(m_stmtPitcher, 21);
        structPitcherData.OBChanceDouble = (float)sqlite3_column_double(m_stmtPitcher, 22);
        structPitcherData.OBChanceSingle = (float)sqlite3_column_double(m_stmtPitcher, 23);
        structPitcherData.OBChanceWalk = (float)sqlite3_column_double(m_stmtPitcher, 24);
        structPitcherData.ChanceDoublePlay = (float)sqlite3_column_double(m_stmtPitcher, 25);
        structPitcherData.OBChanceHomeRunRight = (float)sqlite3_column_double(m_stmtPitcher, 26);
        structPitcherData.OBChanceTripleRight = (float)sqlite3_column_double(m_stmtPitcher, 27);
        structPitcherData.OBChanceDoubleRight = (float)sqlite3_column_double(m_stmtPitcher, 28);
        structPitcherData.OBChanceSingleRight = (float)sqlite3_column_double(m_stmtPitcher, 29);
        structPitcherData.OBChanceWalkRight = (float)sqlite3_column_double(m_stmtPitcher, 30);
        structPitcherData.ChanceDoublePlayRight = (float)sqlite3_column_double(m_stmtPitcher, 31);
        structPitcherData.OBChanceHomeRunLeft = (float)sqlite3_column_double(m_stmtPitcher, 32);
        structPitcherData.OBChanceTripleLeft = (float)sqlite3_column_double(m_stmtPitcher, 33);
        structPitcherData.OBChanceDoubleLeft = (float)sqlite3_column_double(m_stmtPitcher, 34);
        structPitcherData.OBChanceSingleLeft = (float)sqlite3_column_double(m_stmtPitcher, 35);
        structPitcherData.OBChanceWalkLeft = (float)sqlite3_column_double(m_stmtPitcher, 36);
        structPitcherData.ChanceDoublePlayLeft = (float)sqlite3_column_double(m_stmtPitcher, 37);
        structPitcherData.OBChanceBasic = (float)sqlite3_column_double(m_stmtPitcher, 38);
        structPitcherData.OBChanceLeft = (float)sqlite3_column_double(m_stmtPitcher, 39);
        structPitcherData.OBChanceRight = (float)sqlite3_column_double(m_stmtPitcher, 40);
        structPitcherData.Starter = sqlite3_column_int(m_stmtPitcher, 41);
        structPitcherData.Relief = sqlite3_column_int(m_stmtPitcher, 42);
        structPitcherData.Throws = sqlite3_column_int(m_stmtPitcher, 43);
        structPitcherData.Bunting = sqlite3_column_int(m_stmtPitcher, 44);
        structPitcherData.Hold = sqlite3_column_int(m_stmtPitcher, 45);
        structPitcherData.WP = sqlite3_column_int(m_stmtPitcher, 46);
        structPitcherData.Balk = sqlite3_column_int(m_stmtPitcher, 47);
        structPitcherData.Pitcher = sqlite3_column_int(m_stmtPitcher, 48);
        structPitcherData.ER1 = sqlite3_column_int(m_stmtPitcher, 49);
        structPitcherData.TeamID = sqlite3_column_int(m_stmtPitcher, 50);
//
//        Foobar.Printf( wxT("Pitcher Name: %s"), strTemp);
//        wxMessageBox(Foobar);
	}

	sqlite3_finalize(m_stmtPitcher);

	m_intPitcherStatsID = structPitcherStats.PitcherStatsID;

    return m_intPitcherStatsID;
}

// Routine will return the PitcherID based on TeamID and FIrst and Last names
int DBRoutines::DBGetPitcherID(int passedTeamID, wxString passedFirstName, wxString passedLastName)
{
	int rc;
	wxString MsgBuffer;
	wxString sqlPitcherData;

    // Create SQL statement for BatterStats retrival
    sqlPitcherData = "SELECT "  \
		" PitcherID " \
		" FROM PITCHER " \
		" WHERE TeamID = ?1 " \
		" and " \
		" FirstName = ?2 " \
		" and " \
		" LastName = ?3 ";

	rc = sqlite3_prepare_v2(m_db, sqlPitcherData, strlen(sqlPitcherData), &m_stmtPitcher, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	// Bind the data to field '1' which is the first '?' in the SELECT statement
	rc = sqlite3_bind_int(m_stmtPitcher, 1, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtPitcher, 2, structPitcherData.FirstName, strlen(passedFirstName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind FirstName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtPitcher, 3, structPitcherData.LastName, strlen(passedLastName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LastName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	while (sqlite3_step(m_stmtPitcher) == SQLITE_ROW)
	{
		m_intPitcherID = sqlite3_column_int(m_stmtPitcher, 0);
	}

	sqlite3_finalize(m_stmtPitcher);

    return m_intPitcherID;
}

// Routine will return the PitcherStatsID in a given team
int DBRoutines::DBGetPitcherStatsID(int passedTeamID, int passedPitcherID)
{
	int rc;
	wxString MsgBuffer;
	wxString sqlPitcherData;

    // Create SQL statement for BatterStats retrival
    sqlPitcherData = "SELECT "  \
		" PitcherStatsID " \
		" FROM PITCHERSTATS " \
		" WHERE TeamID = ?1 " \
		" and " \
		" PitcherID = ?2 ";

	rc = sqlite3_prepare_v2(m_db, sqlPitcherData, strlen(sqlPitcherData), &m_stmtPitcher, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	// Bind the data to field '1' which is the first '?' in the SELECT statement
	rc = sqlite3_bind_int(m_stmtPitcher, 1, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcher, 2, passedPitcherID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedPitcherID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	while (sqlite3_step(m_stmtPitcher) == SQLITE_ROW)
	{
		m_intPitcherStatsID = sqlite3_column_int(m_stmtPitcher, 0);
	}

	sqlite3_finalize(m_stmtPitcher);

    return m_intPitcherStatsID;
}

// Routine will Insert the data in the batter data record
int DBRoutines::DBInsertBatterData(int passedTeamID)
{
    int rc;
	wxString MsgBuffer;
	wxString sqlBatterData;

	/* Create SQL statement */
	sqlBatterData = "INSERT INTO BATTER("  \
		"FirstName," \
		"LastName," \
		"Pitcher," \
		"Catcher," \
		"FirstBase," \
		"SecondBase," \
		"ShortStop," \
		"ThirdBase," \
		"LeftField," \
		"CenterField," \
		"RightField," \
		"Bunting," \
		"HitRun," \
		"Running," \
		"Stealing," \
		"CatchArm," \
		"OutArm," \
		"PowerRight," \
		"PowerLeft," \
		"Pass," \
		"TRate," \
		"ER1," \
		"ER2," \
		"ER3," \
		"ER4," \
		"ER5," \
		"ER6," \
		"ER7," \
		"ER8," \
		"ER9," \
		"BatterHits," \
		"TeamID," \
		"OBChanceHomeRun," \
		"OBChanceTriple," \
		"OBChanceDouble," \
		"OBChanceSingle," \
		"OBChanceWalk," \
		"ChanceDoublePlay," \
		"OBChanceHomeRunRight," \
		"OBChanceTripleRight," \
		"OBChanceDoubleRight," \
		"OBChanceSingleRight," \
		"OBChanceWalkRight," \
		"ChanceDoublePlayRight," \
		"OBChanceHomeRunLeft," \
		"OBChanceTripleLeft," \
		"OBChanceDoubleLeft," \
		"OBChanceSingleLeft," \
		"OBChanceWalkLeft," \
		"ChanceDoublePlayLeft," \
		"OBChanceBasic," \
		"OBChanceLeft," \
		"OBChanceRight" \
		")" \
		"VALUES (" \
		"?1," \
		"?2," \
		"?3," \
		"?4," \
		"?5," \
		"?6," \
		"?7," \
		"?8," \
		"?9," \
		"?10," \
		"?11," \
		"?12," \
		"?13," \
		"?14," \
		"?15," \
		"?16," \
		"?17," \
		"?18," \
		"?19," \
		"?20," \
		"?21," \
		"?22," \
		"?23," \
		"?24," \
		"?25," \
		"?26," \
		"?27," \
		"?28," \
		"?29," \
		"?30," \
		"?31," \
		"?32," \
		"?33," \
		"?34," \
		"?35," \
		"?36," \
		"?37," \
		"?38," \
		"?39," \
		"?40," \
		"?41," \
		"?42," \
		"?43," \
		"?44," \
		"?45," \
		"?46," \
		"?47," \
		"?48," \
		"?49," \
		"?50," \
		"?51," \
		"?52," \
		"?53" \
		");";

	rc = sqlite3_prepare_v2(m_db, sqlBatterData, strlen(sqlBatterData), &m_stmtBatterInsertData, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for Batter insert data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_text(m_stmtBatterInsertData, 1, structBatterData.FirstName, strlen(structBatterData.FirstName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind FirstName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtBatterInsertData, 2, structBatterData.LastName, strlen(structBatterData.LastName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LastName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 3, structBatterData.Pitcher);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Pitcher: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 4, structBatterData.Catcher);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Catcher: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 5, structBatterData.FirstBase);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind FirstBase: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 6, structBatterData.SecondBase);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind SecondBase: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 7, structBatterData.Shortstop);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Shortstop: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 8, structBatterData.ThirdBase);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ThirdBase: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 9, structBatterData.LeftField);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeftField: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 10, structBatterData.CenterField);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind CenterField: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 11, structBatterData.RightField);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind RightField: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 12, structBatterData.Bunting);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Bunting: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 13, structBatterData.HitRun);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HitRun: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 14, structBatterData.Running);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Running: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 15, structBatterData.Stealing);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Stealing: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 16, structBatterData.CatchArm);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind CatchArm: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 17, structBatterData.OutArm);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OutArm: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 18, structBatterData.PowerRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind PowerRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 19, structBatterData.PowerLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind PowerLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 20, structBatterData.Pass);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Pass: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 21, structBatterData.TRate);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TRate: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 22, structBatterData.ER1);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER1: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 23, structBatterData.ER2);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER1: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 24, structBatterData.ER3);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER3: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 25, structBatterData.ER4);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER4: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 26, structBatterData.ER5);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER5: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 27, structBatterData.ER6);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER6: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 28, structBatterData.ER7);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER7: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 29, structBatterData.ER8);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER8: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 30, structBatterData.ER9);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER9: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 31, structBatterData.BatterHits);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind BatterHits: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertData, 32, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 33, structBatterData.OBChanceHomeRun);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRun: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 34, structBatterData.OBChanceTriple);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTriple: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 35, structBatterData.OBChanceDouble);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDouble: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 36, structBatterData.OBChanceSingle);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingle: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 37, structBatterData.OBChanceWalk);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalk: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 38, structBatterData.ChanceDoublePlay);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlay: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 39, structBatterData.OBChanceHomeRunRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRunRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 40, structBatterData.OBChanceTripleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTripleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 41, structBatterData.OBChanceDoubleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDoubleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 42, structBatterData.OBChanceSingleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 43, structBatterData.OBChanceWalkRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalkRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 44, structBatterData.ChanceDoublePlayRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlayRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 45, structBatterData.OBChanceHomeRunLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRunLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 46, structBatterData.OBChanceTripleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTripleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 47, structBatterData.OBChanceDoubleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDoubleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 48, structBatterData.OBChanceSingleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 49, structBatterData.OBChanceWalkLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalkLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 50, structBatterData.ChanceDoublePlayLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlayLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 51, structBatterData.OBChanceBasic);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceBasic: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 52, structBatterData.OBChanceLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertData, 53, structBatterData.OBChanceRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtBatterInsertData);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Insert BatterData %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtBatterInsertData);

	// Now retrieve inserted record to get new batterID value

    m_intBatterID = DBGetBatterID( passedTeamID, structBatterData.FirstName, structBatterData.LastName );
    DBInsertBatterStats( m_intBatterID, passedTeamID );

    // Return the new m_intBatterID
    return m_intBatterID;
}

// Routine will Insert the data in the batter stats record
int DBRoutines::DBInsertBatterStats(int passedBatterID, int passedTeamID)
{
	int rc;
	wxString MsgBuffer;
	wxString sqlBatterStats;

	/* Create SQL statement */
	sqlBatterStats = "INSERT INTO BATTERSTATS("  \
		"AB," \
		"Runs," \
		"Hits," \
		"RBI," \
		"Doubles," \
		"Triples," \
		"HomeRuns," \
		"Walk," \
		"Stirkeout," \
		"ReachedOnError," \
		"Sacrifice," \
		"StolenBase," \
		"CS," \
		"Games," \
		"HBP," \
		"AVG," \
		"SLG," \
		"OBP," \
		"BatterID," \
		"TeamID" \
		")" \
		"VALUES (" \
		"?1," \
		"?2," \
		"?3," \
		"?4," \
		"?5," \
		"?6," \
		"?7," \
		"?8," \
		"?9," \
		"?10," \
		"?11," \
		"?12," \
		"?13," \
		"?14," \
		"?15," \
		"?16," \
		"?17," \
		"?18," \
		"?19," \
		"?20" \
		");";

	rc = sqlite3_prepare_v2(m_db, sqlBatterStats, strlen(sqlBatterStats), &m_stmtBatterInsertStats, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for batter insert stats: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 1, structBatterStats.AB);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind AB: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 2, structBatterStats.Runs);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Runs: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 3, structBatterStats.Hits);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Hits: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 4, structBatterStats.RBI);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind RBI: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 5, structBatterStats.Doubles);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Doubles: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 6, structBatterStats.Triples);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Triples: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 7, structBatterStats.HomeRuns);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HomeRuns: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 8, structBatterStats.Walk);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Walk: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 9, structBatterStats.Strikeout);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Strikeout: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 10, structBatterStats.ReachedOnError);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ReachedOnError: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 11, structBatterStats.Sacrifice);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Sacrifice: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 12, structBatterStats.StollenBase);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind StollenBase: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 13, structBatterStats.CS);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind CS: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 14, structBatterStats.Games);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Games: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 15, structBatterStats.HBP);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HBP: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertStats, 16, structBatterStats.AVG);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind AVG: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertStats, 17, structBatterStats.SLG);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind SLG: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtBatterInsertStats, 18, structBatterStats.OBP);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBP: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 19, passedBatterID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedBatterID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtBatterInsertStats, 20, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtBatterInsertStats);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Insert BatterStats: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtBatterInsertStats);

    m_intBatterStatsID = DBGetBatterStatsID( passedTeamID, passedBatterID);

    // Return the new batter stats ID
    return m_intBatterStatsID;
}

// Routine will Insert the data in the pitcher data record
int DBRoutines::DBInsertPitcherData(int passedTeamID)
{
    int rc;
	wxString MsgBuffer;
	wxString sqlPitcherData;

	/* Create SQL statement */
	sqlPitcherData = "INSERT INTO PITCHER("  \
		"FirstName," \
		"LastName," \
		"OBChanceHomeRun," \
		"OBChanceTriple," \
		"OBChanceDouble," \
		"OBChanceSingle," \
		"OBChanceWalk," \
		"ChanceDoublePlay," \
		"OBChanceHomeRunRight," \
		"OBChanceTripleRight," \
		"OBChanceDoubleRight," \
		"OBChanceSingleRight," \
		"OBChanceWalkRight," \
		"ChanceDoublePlayRight," \
		"OBChanceHomeRunLeft," \
		"OBChanceTripleLeft," \
		"OBChanceDoubleLeft," \
		"OBChanceSingleLeft," \
		"OBChanceWalkLeft," \
		"ChanceDoublePlayLeft," \
		"OBChanceBasic," \
		"OBChanceLeft," \
		"OBChanceRight," \
		"Starter," \
		"Relief," \
		"Throws," \
		"Bunting," \
		"Hold," \
		"WP," \
		"Balk," \
		"Pitcher," \
		"ER1," \
		"TeamID" \
		")" \
		"VALUES (" \
		"?1," \
		"?2," \
		"?3," \
		"?4," \
		"?5," \
		"?6," \
		"?7," \
		"?8," \
		"?9," \
		"?10," \
		"?11," \
		"?12," \
		"?13," \
		"?14," \
		"?15," \
		"?16," \
		"?17," \
		"?18," \
		"?19," \
		"?20," \
		"?21," \
		"?22," \
		"?23," \
		"?24," \
		"?25," \
		"?26," \
		"?27," \
		"?28," \
		"?29," \
		"?30," \
		"?31," \
		"?32," \
		"?33"  \
		");";

	rc = sqlite3_prepare_v2(m_db, sqlPitcherData, strlen(sqlPitcherData), &m_stmtPitcherInsertData, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for pitcher insert data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_text(m_stmtPitcherInsertData, 1, structPitcherData.FirstName, strlen(structPitcherData.FirstName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind FirstName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtPitcherInsertData, 2, structPitcherData.LastName, strlen(structPitcherData.LastName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LastName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 3, structPitcherData.OBChanceHomeRun);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRun: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 4, structPitcherData.OBChanceTriple);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTriple: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 5, structPitcherData.OBChanceDouble);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDouble: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 6, structPitcherData.OBChanceSingle);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingle: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 7, structPitcherData.OBChanceWalk);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalk: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 8, structPitcherData.ChanceDoublePlay);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlay: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 9, structPitcherData.OBChanceHomeRunRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRunRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 10, structPitcherData.OBChanceTripleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTripleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 11, structPitcherData.OBChanceDoubleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDoubleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 12, structPitcherData.OBChanceSingleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 13, structPitcherData.OBChanceWalkRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalkRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 14, structPitcherData.ChanceDoublePlayRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlayRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 15, structPitcherData.OBChanceHomeRunLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRunLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 16, structPitcherData.OBChanceTripleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTripleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 17, structPitcherData.OBChanceDoubleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDoubleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 18, structPitcherData.OBChanceSingleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 19, structPitcherData.OBChanceWalkLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalkLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 20, structPitcherData.ChanceDoublePlayLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlayLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 21, structPitcherData.OBChanceBasic);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceBasic: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 22, structPitcherData.OBChanceLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertData, 23, structPitcherData.OBChanceRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertData, 24, structPitcherData.Starter);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Starter: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertData, 25, structPitcherData.Relief);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Relief: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertData, 26, structPitcherData.Throws);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Throws: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertData, 27, structPitcherData.Bunting);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Bunting: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertData, 28, structPitcherData.Hold);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Hold: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertData, 29, structPitcherData.WP);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind WP: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertData, 30, structPitcherData.Balk);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Balk: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertData, 31, structPitcherData.Pitcher);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Pitcher: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertData, 32, structPitcherData.ER1);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER1: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertData, 33, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtPitcherInsertData);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Insert PitcherData %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtPitcherInsertData);

	// Now retrieve inserted record to get new pitcherID value

    m_intPitcherID = DBGetPitcherID( passedTeamID, structPitcherData.FirstName, structPitcherData.LastName );
    DBInsertPitcherStats( m_intPitcherID, passedTeamID );

    // Return the new pitcherID
    return m_intPitcherID;
}

// Routine will Insert the data in the pitcher stats record
int DBRoutines::DBInsertPitcherStats(int passedPitcherID, int passedTeamID)
{
	int rc;
	wxString MsgBuffer;
	wxString sqlPitcherStats;

	/* Create SQL statement */
	sqlPitcherStats = "INSERT INTO PITCHERSTATS("  \
		"Wins," \
		"Loss," \
		"Saves," \
		"InningsPitched," \
		"ER," \
		"Hits," \
		"Walks," \
		"Strikeouts," \
		"HomeRuns," \
		"Games," \
		"CompleteGames," \
		"Starts," \
		"ERA," \
		"WHIP," \
		"PitcherID," \
		"TeamID" \
		")" \
		"VALUES (" \
		"?1," \
		"?2," \
		"?3," \
		"?4," \
		"?5," \
		"?6," \
		"?7," \
		"?8," \
		"?9," \
		"?10," \
		"?11," \
		"?12," \
		"?13," \
		"?14," \
		"?15," \
		"?16" \
		");";

	rc = sqlite3_prepare_v2(m_db, sqlPitcherStats, strlen(sqlPitcherStats), &m_stmtPitcherInsertStats, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for pitcher insert stats: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 1, structPitcherStats.Wins);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Wins: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 2, structPitcherStats.Loss);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Loss: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 3, structPitcherStats.Saves);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Saves: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertStats, 4, structPitcherStats.InningsPitched);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Saves: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 5, structPitcherStats.ER);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 6, structPitcherStats.Hits);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Hits: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 7, structPitcherStats.Walks);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Walks: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 8, structPitcherStats.Strikeouts);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Strikeouts: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 9, structPitcherStats.HomeRuns);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HomeRuns: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 10, structPitcherStats.Games);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Games: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 11, structPitcherStats.CompleteGames);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind CompleteGames: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 12, structPitcherStats.Starts);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Starts: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertStats, 13, structPitcherStats.ERA);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ERA: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherInsertStats, 14, structPitcherStats.WHIP);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind WHIP: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 15, passedPitcherID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind PitcherID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherInsertStats, 16, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtPitcherInsertStats);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Insert PitcherStats: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtPitcherInsertStats);

    m_intPitcherStatsID = DBGetPitcherStatsID( passedTeamID, passedPitcherID);

    // Return the new pitcher stats ID
    return m_intPitcherStatsID;
}

// Routine will Insert the data in the conference data record
int DBRoutines::DBInsertConference()
{
    int rc;
	wxString MsgBuffer;
	wxString sqlConference;

	/* Create SQL statement */
	sqlConference = "INSERT INTO CONFERENCES("  \
		"ConferenceName," \
		"LeagueID," \
		"BaseConference" \
		")" \
		"VALUES (" \
		"?1," \
		"?2," \
		"?3" \
		");";

	rc = sqlite3_prepare_v2(m_db, sqlConference, strlen(sqlConference), &m_stmtConference, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for Conference insert data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	rc = sqlite3_bind_text(m_stmtConference, 1, structConferenceData.ConferenceName, strlen(structConferenceData.ConferenceName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ConferenceName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtConference, 2, structConferenceData.LeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtConference, 3, structConferenceData.BaseConference);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind BaseConference: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtConference);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Insert ConferenceData %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtConference);

	// Now retrieve inserted record to get new ConferenceID value

    m_intConferenceID = DBGetConferenceID( structConferenceData.ConferenceName, structConferenceData.LeagueID );
    structConferenceData.ConferenceID = m_intConferenceID;

    // Return the new ConferenceID
    return m_intConferenceID;
}

// Routine will Insert the data in the division data record
int DBRoutines::DBInsertDivision()
{
    int rc;
	wxString MsgBuffer;
	wxString sqlDivision;

	/* Create SQL statement */
	sqlDivision = "INSERT INTO DIVISIONS("  \
		"DivisionName," \
		"LeagueID," \
		"ConferenceID," \
		"BaseDivisions" \
		")" \
		"VALUES (" \
		"?1," \
		"?2," \
		"?3," \
		"?4" \
		");";

	rc = sqlite3_prepare_v2(m_db, sqlDivision, strlen(sqlDivision), &m_stmtDivision, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for Division insert data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	rc = sqlite3_bind_text(m_stmtDivision, 1, structDivisionData.DivisionName, strlen(structDivisionData.DivisionName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind DivisionName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtDivision, 2, structDivisionData.LeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtDivision, 3, structDivisionData.ConferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ConferenceID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtDivision, 4, structDivisionData.BaseDivisions);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind BaseConference: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtDivision);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Insert DivisionData %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtDivision);

	// Now retrieve inserted record to get new ConferenceID value

    m_intDivisionID = DBGetDivisionID( structDivisionData.DivisionName, structDivisionData.LeagueID );

    // Return the new ConferenceID
    return m_intDivisionID;
}

// Routine will Insert the data in the League data record
int DBRoutines::DBInsertLeague()
{
    int rc;
	wxString MsgBuffer;
	wxString sqlLeague;

	/* Create SQL statement */
	sqlLeague = "INSERT INTO LEAGUES("  \
		"LeagueName," \
		"NumberOfConferences," \
		"NumberOfDivisions," \
		"BaseLeague," \
		"LeagueYear" \
		")" \
		"VALUES (" \
		"?1," \
		"?2," \
		"?3," \
		"?4," \
		"?5" \
		");";

	rc = sqlite3_prepare_v2(m_db, sqlLeague, strlen(sqlLeague), &m_stmtLeague, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for League insert data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_text(m_stmtLeague, 1, structLeagueData.LeagueName, strlen(structLeagueData.LeagueName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeagueName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtLeague, 2, structLeagueData.NumberOfConferences);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind NumberOfConferences: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtLeague, 3, structLeagueData.NumberOfDivisions);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind NumberOfDivisions: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtLeague, 4, structLeagueData.BaseLeague);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind BaseLeague: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtLeague, 5, structLeagueData.LeagueYear);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeagueYear: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtLeague);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Insert LeagueData %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtLeague);

	// Now retrieve inserted record to get new LeagueID value

    m_intLeagueID = DBGetLeagueID( structLeagueData.LeagueName );

    // Return the new LeagueID
    return m_intLeagueID;
}

// Routine will Insert the data in the League data record
int DBRoutines::DBUpdateLeague( int passedLeagueID )
{
    int rc;
	wxString MsgBuffer;
	wxString sqlLeague;

	/* Create SQL statement */
	sqlLeague = "UPDATE LEAGUES "  \
		"SET " \
		"LeagueName = ?1," \
		"NumberOfConferences = ?2," \
		"NumberOfDivisions = ?3," \
		"BaseLeague = ?4," \
		"LeagueYear = ?5, "
		"LastUpdateTime = datetime('NOW','localtime') " \
		"WHERE LeagueID = ?6 ";

	rc = sqlite3_prepare_v2(m_db, sqlLeague, strlen(sqlLeague), &m_stmtLeague, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for League insert data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_text(m_stmtLeague, 1, structLeagueData.LeagueName, strlen(structLeagueData.LeagueName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeagueName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtLeague, 2, structLeagueData.NumberOfConferences);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind NumberOfConferences: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtLeague, 3, structLeagueData.NumberOfDivisions);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind NumberOfDivisions: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtLeague, 4, structLeagueData.BaseLeague);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind BaseLeague: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtLeague, 5, structLeagueData.LeagueYear);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeagueYear: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtLeague, 6, passedLeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtLeague);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Update LeagueData %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtLeague);

    // Return the new LeagueID
    return structLeagueData.LeagueID;
}

// Routine will Insert the data in the Team data record
int DBRoutines::DBInsertTeam()
{
    int rc;
	wxString MsgBuffer;
	wxString sqlTeam;

	/* Create SQL statement */
	sqlTeam = "INSERT INTO TEAM("  \
		"TeamName," \
		"TeamNameShort," \
		"BallparkName," \
		"TotalWins," \
		"TotalLosses," \
		"HomeWins," \
		"HomeLosses," \
		"AwayWins," \
		"AwayLosses," \
		"LeagueID," \
		"ConferenceID," \
		"DivisionID," \
		"TeamYear," \
		"BaseTeam" \
		")" \
		"VALUES (" \
		"?1," \
		"?2," \
		"?3," \
		"?4," \
		"?5," \
		"?6," \
		"?7," \
		"?8," \
		"?9," \
		"?10," \
		"?11," \
		"?12," \
		"?13," \
		"?14" \
		");";

	rc = sqlite3_prepare_v2(m_db, sqlTeam, strlen(sqlTeam), &m_stmtTeam, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for Team insert data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_text(m_stmtTeam, 1, structTeamData.TeamName, strlen(structTeamData.TeamName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtTeam, 2, structTeamData.TeamNameShort, strlen(structTeamData.TeamNameShort), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamNameShort: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtTeam, 3, structTeamData.BallparkName, strlen(structTeamData.BallparkName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind BallparkName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 4, structTeamData.TotalWins);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TotalWins: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 5, structTeamData.TotalLosses);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TotalLosses: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 6, structTeamData.HomeWins);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HomeWins: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 7, structTeamData.HomeLosses);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HomeLosses: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 8, structTeamData.AwayWins);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind AwayWins: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 9, structTeamData.AwayLosses);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind AwayLosses: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 10, structTeamData.LeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 11, structTeamData.ConferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ConferenceID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 12, structTeamData.DivisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind DivisionID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 13, structTeamData.TeamYear);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamYear: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 14, structTeamData.BaseTeam);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamYear: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtTeam);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Insert TeamData %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtTeam);

	// Now retrieve inserted record to get new LeagueID value

    m_intTeamID = DBGetTeamID( structTeamData.TeamName, structTeamData.LeagueID,
			structTeamData.ConferenceID, structTeamData.DivisionID );

    // Return the new LeagueID
    return m_intTeamID;
}

// Routine will Update the data in the Team data record
int DBRoutines::DBUpdateTeam( int passedTeamID )
{
    int rc;
	wxString MsgBuffer;
	wxString sqlTeam;

	/* Create SQL statement */
	sqlTeam = "UPDATE TEAM "  \
		"SET " \
		"TeamName = ?1," \
		"TeamNameShort = ?2," \
		"BallparkName = ?3," \
		"TotalWins = ?4," \
		"TotalLosses = ?5," \
		"HomeWins = ?6," \
		"HomeLosses = ?7," \
		"AwayWins = ?8," \
		"AwayLosses = ?9," \
		"LeagueID = ?10," \
		"ConferenceID = ?11," \
		"DivisionID = ?12," \
		"TeamYear = ?13," \
		"BaseTeam = ?14," \
		"LastUpdateTime = datetime('NOW','localtime') " \
		"WHERE TeamID = ?15 ";

	rc = sqlite3_prepare_v2(m_db, sqlTeam, strlen(sqlTeam), &m_stmtTeam, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for Team UPDATE data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_text(m_stmtTeam, 1, structTeamData.TeamName, strlen(structTeamData.TeamName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtTeam, 2, structTeamData.TeamNameShort, strlen(structTeamData.TeamNameShort), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamNameShort: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtTeam, 3, structTeamData.BallparkName, strlen(structTeamData.BallparkName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind BallparkName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 4, structTeamData.TotalWins);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TotalWins: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 5, structTeamData.TotalLosses);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TotalLosses: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 6, structTeamData.HomeWins);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HomeWins: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 7, structTeamData.HomeLosses);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HomeLosses: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 8, structTeamData.AwayWins);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind AwayWins: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 9, structTeamData.AwayLosses);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind AwayLosses: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 10, structTeamData.LeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 11, structTeamData.ConferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ConferenceID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 12, structTeamData.DivisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind DivisionID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 13, structTeamData.TeamYear);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamYear: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 14, structTeamData.BaseTeam);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamYear: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 15, passedTeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedTeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtTeam);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Update TeamData %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtTeam);

    // Return the TeamID
    return passedTeamID;
}

// Routine will retrieve a teamID
int DBRoutines::DBGetTeamID( wxString PassedTeamName, int PassedLeagueID, int PassedConferenceID, int PassedDivisionID )
{
    int rc;
	wxString MsgBuffer;
	wxString sqlTeam;

	/* Create SQL statement */
	sqlTeam = "SELECT "  \
		"TeamID, " \
		"TeamName," \
		"TeamNameShort," \
		"BallparkName," \
		"TotalWins," \
		"TotalLosses," \
		"HomeWins," \
		"HomeLosses," \
		"AwayWins," \
		"AwayLosses," \
		"LeagueID," \
		"ConferenceID," \
		"DivisionID," \
		"TeamYear," \
		"BaseTeam," \
		"ActiveRec," \
		"CreateTime," \
		"LastUpdateTime" \
		" FROM TEAM " \
		" WHERE TeamName = ?1 AND LeagueID = ?2 AND ConferenceID = ?3 AND DivisionID = ?4" ;

	rc = sqlite3_prepare_v2(m_db, sqlTeam, strlen(sqlTeam), &m_stmtTeam, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare for Team SELECT data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_bind_text(m_stmtTeam, 1, structTeamData.TeamName, strlen(structTeamData.TeamName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 2, PassedLeagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind PassedLeagueID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 3, PassedConferenceID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind PassedConferenceID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtTeam, 4, PassedDivisionID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind PassedDivisionID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	while (sqlite3_step(m_stmtTeam) == SQLITE_ROW)
	{
		m_intTeamID = (sqlite3_column_int(m_stmtTeam, 0));
		structTeamData.TeamID = m_intTeamID;
		structTeamData.TeamName = (sqlite3_column_text(m_stmtTeam, 1));
		structTeamData.TeamNameShort = (sqlite3_column_text(m_stmtTeam, 2));
		structTeamData.BallparkName = (sqlite3_column_text(m_stmtTeam, 3));
		structTeamData.TotalWins = (sqlite3_column_int(m_stmtTeam, 4));
		structTeamData.TotalLosses = (sqlite3_column_int(m_stmtTeam, 5));
		structTeamData.HomeWins = (sqlite3_column_int(m_stmtTeam, 6));
		structTeamData.HomeLosses = (sqlite3_column_int(m_stmtTeam, 7));
		structTeamData.AwayWins = (sqlite3_column_int(m_stmtTeam, 8));
		structTeamData.AwayLosses = (sqlite3_column_int(m_stmtTeam, 9));
		structTeamData.LeagueID = (sqlite3_column_int(m_stmtTeam, 10));
		structTeamData.ConferenceID = (sqlite3_column_int(m_stmtTeam, 11));
		structTeamData.DivisionID = (sqlite3_column_int(m_stmtTeam, 12));
		structTeamData.TeamYear = (sqlite3_column_int(m_stmtTeam, 13));
		structTeamData.BaseTeam = (sqlite3_column_int(m_stmtTeam, 14));
		structTeamData.ActiveRec = (sqlite3_column_int(m_stmtTeam, 15));
		structTeamData.CreateTime = (sqlite3_column_text(m_stmtTeam, 16));
		structTeamData.LastUpdateTime = (sqlite3_column_text(m_stmtTeam, 17));
	}

	sqlite3_finalize(m_stmtTeam);

    // Return the new TeamID
    return m_intTeamID;
}

// Routine will Update the data in the pitcher stats record
int DBRoutines::DBUpdatePitcherStats(int passedPitcherStatsID)
{
    int rc;
	wxString MsgBuffer;
    wxString sqlPitcherStats;

    // Create SQL statement for sqlPitcherStats retrival
	sqlPitcherStats = "UPDATE PITCHERSTATS "  \
        "SET "  \
		"Wins = ?1," \
		"Loss = ?2," \
		"Saves = ?3," \
		"InningsPitched = ?4," \
		"ER = ?5," \
		"Hits = ?6," \
		"Walks = ?7," \
		"Strikeouts = ?8," \
		"HomeRuns = ?9," \
		"Games = ?10," \
		"CompleteGames = ?11," \
		"Starts = ?12," \
		"ERA = ?13," \
		"WHIP = ?14," \
		"PitcherID = ?15," \
		"TeamID = ?16," \
		"LastUpdateTime = datetime('NOW','localtime') " \
		"WHERE PitcherStatsID = ?17 ";

//    wxString Foobar;
//    Foobar.Printf( wxT("Pitcher Starts = %i"), structPitcherStats.Starts);
//    wxMessageBox(Foobar);
//    Foobar.Printf( wxT("Pitcher StatsID = %i"), passedPitcherStatsID);
//    wxMessageBox(Foobar);

	rc = sqlite3_prepare_v2(m_db, sqlPitcherStats, strlen(sqlPitcherStats), &m_stmtPitcherUpdateStats, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to prepare data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 1, structPitcherStats.Wins);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Wins: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 2, structPitcherStats.Loss);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Loss: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 3, structPitcherStats.Saves);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Saves: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateStats, 4, structPitcherStats.InningsPitched);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Saves: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 5, structPitcherStats.ER);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 6, structPitcherStats.Hits);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Hits: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 7, structPitcherStats.Walks);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Walks: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 8, structPitcherStats.Strikeouts);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Strikeouts: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 9, structPitcherStats.HomeRuns);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind HomeRuns: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 10, structPitcherStats.Games);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Games: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 11, structPitcherStats.CompleteGames);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind CompleteGames: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 12, structPitcherStats.Starts);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Starts: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateStats, 13, structPitcherStats.ERA);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ERA: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateStats, 14, structPitcherStats.WHIP);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind WHIP: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 15, structPitcherStats.PitcherID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind PitcherID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 16, structPitcherStats.TeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateStats, 17, passedPitcherStatsID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedPitcherStatsID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtPitcherUpdateStats);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Update PitcherStats: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtPitcherUpdateStats);

    return TRUE;
}

// Routine will Update the data in the base pitcher record
int DBRoutines::DBUpdatePitcherData(int passedPitcherID)
{
    int rc;
	wxString MsgBuffer;
    wxString sqlPitcherData;

    	/* Create SQL statement */
	sqlPitcherData = "UPDATE PITCHER "  \
		"SET " \
		"FirstName = ?1," \
		"LastName = ?2," \
		"OBChanceHomeRun = ?3," \
		"OBChanceTriple = ?4," \
		"OBChanceDouble = ?5," \
		"OBChanceSingle = ?6," \
		"OBChanceWalk = ?7," \
		"ChanceDoublePlay = ?8," \
		"OBChanceHomeRunRight = ?9," \
		"OBChanceTripleRight = ?10," \
		"OBChanceDoubleRight = ?11," \
		"OBChanceSingleRight = ?12," \
		"OBChanceWalkRight = ?13," \
		"ChanceDoublePlayRight = ?14," \
		"OBChanceHomeRunLeft = ?15," \
		"OBChanceTripleLeft = ?16," \
		"OBChanceDoubleLeft = ?17," \
		"OBChanceSingleLeft = ?18," \
		"OBChanceWalkLeft = ?19," \
		"ChanceDoublePlayLeft = ?20," \
		"OBChanceBasic = ?21," \
		"OBChanceLeft = ?22," \
		"OBChanceRight = ?23," \
		"Starter = ?24," \
		"Relief = ?25," \
		"Throws = ?26," \
		"Bunting = ?27," \
		"Hold = ?28," \
		"WP = ?29," \
		"Balk = ?30," \
		"Pitcher = ?31," \
		"ER1 = ?32," \
		"TeamID = ?33," \
		"LastUpdateTime = datetime('NOW','localtime')" \
		" WHERE PitcherID = ?34 ";

    ////////////////////////////////////////////////////////////////
    // This routine needs to be verified and tested
    ////////////////////////////////////////////////////////////////

    wxString Foobar;
    Foobar.Printf( wxT("Pitcher On Base Chance = %f"), structPitcherData.OBChanceBasic);
    wxMessageBox(Foobar);
    Foobar.Printf( wxT("Pitcher ID = %i"), passedPitcherID);
    wxMessageBox(Foobar);

	rc = sqlite3_prepare_v2(m_db, sqlPitcherData, strlen(sqlPitcherData), &m_stmtPitcherUpdateData, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

 	// Bind the data to field '1' which is the first '?' in the UPDATE statement
	//rc = sqlite3_bind_text(m_stmtTeam, 1, strLeagueName, strlen(strLeagueName), NULL );
	rc = sqlite3_bind_text(m_stmtPitcherUpdateData, 1, structPitcherData.FirstName, strlen(structPitcherData.FirstName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind FirstName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_text(m_stmtPitcherUpdateData, 2, structPitcherData.LastName, strlen(structPitcherData.LastName), SQLITE_STATIC);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind LastName: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 3, structPitcherData.OBChanceHomeRun);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRun: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 4, structPitcherData.OBChanceTriple);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTriple: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 5, structPitcherData.OBChanceDouble);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDouble: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 6, structPitcherData.OBChanceSingle);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingle: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 7, structPitcherData.OBChanceWalk);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalk: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 8, structPitcherData.ChanceDoublePlay);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlay: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 9, structPitcherData.OBChanceHomeRunRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRunRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 10, structPitcherData.OBChanceTripleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTripleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 11, structPitcherData.OBChanceDoubleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDoubleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 12, structPitcherData.OBChanceSingleRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingleRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 13, structPitcherData.OBChanceWalkRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalkRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 14, structPitcherData.ChanceDoublePlayRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlayRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 15, structPitcherData.OBChanceHomeRunLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceHomeRunLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 16, structPitcherData.OBChanceTripleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceTripleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 17, structPitcherData.OBChanceDoubleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceDoubleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 18, structPitcherData.OBChanceSingleLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceSingleLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 19, structPitcherData.OBChanceWalkLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceWalkLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 20, structPitcherData.ChanceDoublePlayLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ChanceDoublePlayLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 21, structPitcherData.OBChanceBasic);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceBasic: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 22, structPitcherData.OBChanceLeft);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceLeft: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_double(m_stmtPitcherUpdateData, 23, structPitcherData.OBChanceRight);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind OBChanceRight: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 24, structPitcherData.Starter);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Starter: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 25, structPitcherData.Relief);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Relief: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 26, structPitcherData.Throws);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Throws: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 27, structPitcherData.Bunting);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Bunting: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 28, structPitcherData.Hold);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Hold: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 29, structPitcherData.WP);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind WP: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 30, structPitcherData.Balk);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Balk: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 31, structPitcherData.Pitcher);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind Pitcher: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 32, structPitcherData.ER1);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind ER1: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 33, structPitcherData.TeamID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind TeamID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	rc = sqlite3_bind_int(m_stmtPitcherUpdateData, 34, passedPitcherID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind passedPitcherID: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rc = sqlite3_step(m_stmtPitcherUpdateData);

	if (rc != SQLITE_DONE)
	{
		MsgBuffer.Printf( wxT("Failed to Update PitcherData %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	sqlite3_finalize(m_stmtPitcherUpdateData);

    return TRUE;
}

// This routine will look at the League for Conferences and Divisions
// It will rotate through the Conferences and Divisions involking
// wxGetApp().pFileRoutines->BuildPlayerStats(leagueID, conferenceID, divisionID)
// with the approprate information.
void DBRoutines::DBGetLeagueConferenceDivision( int leagueID )
{
//	m_LeagueRecord leagueRecord;
//	m_ConferenceRecord conferenceRecord;
//	m_DivisionRecord divisionRecord;
	int rcSqlStepConf = 0;
	int rcSqlStepDiv = 0;
	int rc = 0;
//	CHAR buffer[100];
	wxString sqlConference;
	wxString sqlDivision;
	sqlite3_stmt *localStmtConf;
	sqlite3_stmt *localStmtDivision;
	int conferenceID = 0;
	int divisionID = 0;
	wxString MsgBuffer;

	/* Create SQL statement */
	sqlConference = "SELECT "  \
		"ConferenceID " \
		" from CONFERENCES "
		" WHERE LeagueID = ?1 ";

	rc = sqlite3_prepare_v2(m_db, sqlConference, strlen(sqlConference), &localStmtConf, 0);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}
	else
	{
//		MsgBuffer.Printf( wxT("Prepare for CONFERENCES Select OK:\n"), sqlite3_errmsg(m_db));
//        wxMessageBox(MsgBuffer);
	}
	// Bind the data to field '1' which is the first '?' in the INSERT statement
	rc = sqlite3_bind_int(localStmtConf, 1, leagueID);
	if (rc != SQLITE_OK)
	{
		MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(m_db));
        wxMessageBox(MsgBuffer);
	}

	rcSqlStepConf = sqlite3_step(localStmtConf);
	while (rcSqlStepConf == SQLITE_ROW)
	{
		// Get ID of conference
		conferenceID = sqlite3_column_int(localStmtConf, 0);

		/* Create SQL statement */
		sqlDivision = "SELECT "  \
			"DivisionID " \
			" from DIVISIONS "
			" WHERE LeagueID = ?1 AND ConferenceID = ?2";

		rc = sqlite3_prepare_v2(m_db, sqlDivision, strlen(sqlDivision), &localStmtDivision, 0);
		if (rc != SQLITE_OK)
		{
			MsgBuffer.Printf( wxT("Failed to fetch data: %s\n"), sqlite3_errmsg(m_db));
			wxMessageBox(MsgBuffer);
		}
		else
		{
//			MsgBuffer.Printf( wxT("Prepare for CONFERENCES Select OK:\n"), sqlite3_errmsg(m_db));
//			wxMessageBox(MsgBuffer);
		}
		// Bind the data to field '1' which is the first '?' in the INSERT statement
		rc = sqlite3_bind_int(localStmtDivision, 1, leagueID);
		if (rc != SQLITE_OK)
		{
			MsgBuffer.Printf( wxT("Could not bind leagueID int: %s\n"), sqlite3_errmsg(m_db));
			wxMessageBox(MsgBuffer);
		}
		rc = sqlite3_bind_int(localStmtDivision, 2, conferenceID);
		if (rc != SQLITE_OK)
		{
			MsgBuffer.Printf( wxT("Could not bind conferenceID int: %s\n"), sqlite3_errmsg(m_db));
			wxMessageBox(MsgBuffer);
		}

		rcSqlStepDiv = sqlite3_step(localStmtDivision);
		if (rcSqlStepDiv == SQLITE_DONE)
		{
			// Process League data with default division
			wxGetApp().pFileRoutines->BuildPlayerStats(leagueID, conferenceID, 1);
		}
		else
		{
			while (rcSqlStepDiv == SQLITE_ROW)
			{
				// Get ID of division
				divisionID = sqlite3_column_int(localStmtDivision, 0);

				// Process League data
				wxGetApp().pFileRoutines->BuildPlayerStats(leagueID, conferenceID, divisionID);

				// Get next Division
				rcSqlStepDiv = sqlite3_step(localStmtDivision);
			}
		}
		sqlite3_finalize(localStmtDivision);

		// Get next Conference
		rcSqlStepConf = sqlite3_step(localStmtConf);
	}
	sqlite3_finalize(localStmtConf);
}

