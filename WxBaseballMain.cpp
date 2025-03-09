/////////////////////////////////////////////////////////////////////////////
// Name:		WxBaseballMain.cpp                                         //
// Purpose:		Baseball statistics tracking tool for Strat-O-Matic        //
// Author:		John Gnew (john.gnew@verizon.net)                          //
// Created:		2024-03-22                                                 //
// Copyright:	(c) John Gnew                                              //
/////////////////////////////////////////////////////////////////////////////
// Modifications:                                                          //
//   Date       Description                                                //
// 03/04/25  Changed return test in OnTeamsAddTeam to check for			   //
//           wxID_CANCEL                                                   //
// 03/05/25  Changed return test in OnTeamsEditTeam,                       //
//           OnStatisticsHTMLLeagueStats, OnFileExportLeagueTeamsAll       //
//           to check for wxID_CANCEL                                      //
// 03/09/25  Changed OnTeamsEditTeam to check for false instead of         //
//           wxID_CANCEL. wxID_CANCEL is 5101 which might be a valid       //
//           DB index but false which is '0' is not valid                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include <wx/print.h>
#include <wx/printdlg.h>
#include "WxBaseballMain.h"
#include "WxBaseballApp.h"
#include "Print.h"
#include "dialogs.h"
#include "FileRoutines.h"

//#if !defined(__WXMSW__)
//    #include "bitmaps/Baseball.xpm"
//#else
//    #include "bitmaps/app.xpm"
//#endif
#include "bitmaps/Baseball.xpm"

#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/help.xpm"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(WxBaseballFrame, wxFrame)
//    EVT_CLOSE(WxBaseballFrame::OnClose)
//    EVT_MENU(idMenuQuit, WxBaseballFrame::OnQuit)
//    EVT_MENU(idMenuAbout, WxBaseballFrame::OnAbout)
	//File Menu
	EVT_MENU ( wxID_OPEN, WxBaseballFrame::OnOpen )
	// If this should only close the DB then changes need to be made from OnQuit but
	// OnClose is triggered with the X in the top right.
	EVT_MENU ( wxID_CLOSE, WxBaseballFrame::OnQuit )
	EVT_MENU ( wxID_PRINT, WxBaseballFrame::OnPrint )
	EVT_MENU ( wxID_PREVIEW, WxBaseballFrame::OnPrintPreview )
	EVT_MENU ( wxID_PRINT_SETUP, WxBaseballFrame::OnPrintSetup )
	EVT_MENU ( myID_EXPORT, WxBaseballFrame::OnFileExportLeagueTeams )
	EVT_MENU ( myID_EXPORT_All, WxBaseballFrame::OnFileExportLeagueTeamsAll )
	EVT_MENU ( myID_EXPORT_HTML, WxBaseballFrame::OnExportHTML )
	EVT_MENU ( myID_EXPORT_HTML_All, WxBaseballFrame::OnExportHTMLAll )
	EVT_MENU ( myID_OPTIONS, WxBaseballFrame::OnOptions )
	EVT_MENU ( wxID_EXIT, WxBaseballFrame::OnQuit )
	// Edit Menu
	// View Menu
	EVT_MENU ( myID_SCORESHEET, WxBaseballFrame::OnScoreSheetCheck )
	EVT_MENU ( myID_SCORESHEETXTRA, WxBaseballFrame::OnScoreSheetCheckExtra )
	EVT_MENU ( myID_STATISTICS, WxBaseballFrame::OnStatistics )
	// Leagues Menu
	EVT_MENU ( myID_ADDLEAGUE, WxBaseballFrame::OnLeaguesAddLeague )
	EVT_MENU ( myID_EDITLEAGUE, WxBaseballFrame::OnLeaguesEditLeague )
	//    EVT_MENU(myID_LEAGUEOPTIONSHTML, WxBaseballFrame::OnLeagueOptionsHTML)
	// Teams Menu
	EVT_MENU( myID_ADDTEAMS, WxBaseballFrame::OnTeamsAddTeam )
	EVT_MENU( myID_EDITTEAMS, WxBaseballFrame::OnTeamsEditTeam )
	EVT_MENU( myID_CREATETEAM, WxBaseballFrame::OnTeamsCreateTeam )
	// Players Menu
	EVT_MENU ( myID_ADDBATTERS, WxBaseballFrame::OnPlayersAddEditBatter )
	EVT_MENU ( myID_ADDPITCHERS, WxBaseballFrame::OnPlayersAddEditPitcher )
	// Statistics Menu
	//    EVT_MENU(myID_CREATESTATS, WxBaseballFrame::OnCreateStats)
	//    EVT_MENU(myID_UPDATESTATS, WxBaseballFrame::OnUpdateStats)
	EVT_MENU ( myID_HTMLSTATS, WxBaseballFrame::OnStatisticsHTMLLeagueStats )

	// Window Menu
	// Help Menu
	EVT_MENU ( wxID_ABOUT, WxBaseballFrame::OnAbout )

	//    EVT_TOOL(myID_TOOLBAR_NEW, WxBaseballFrame::OnNew)
	//    EVT_TOOL(myID_TOOLBAR_OPEN, WxBaseballFrame::OnOpen)
	//    EVT_TOOL(myID_TOOLBAR_SAVE, WxBaseballFrame::OnSave)
	//    EVT_TOOL(myID_TOOLBAR_COPY, WxBaseballFrame::OnCopy)
	//    EVT_TOOL(myID_TOOLBAR_CUT, WxBaseballFrame::OnCut)
	//    EVT_TOOL(myID_TOOLBAR_PASTE, WxBaseballFrame::OnPaste)
	EVT_TOOL ( myID_TOOLBAR_PRINT, WxBaseballFrame::OnPrint )
	EVT_TOOL ( myID_TOOLBAR_HELP, WxBaseballFrame::OnAbout )

	EVT_CLOSE ( WxBaseballFrame::OnClose )

END_EVENT_TABLE()

WxBaseballFrame::WxBaseballFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title, wxDefaultPosition, wxSize ( 900, 650 ), wxDEFAULT_FRAME_STYLE )
//WxBaseballFrame::WxBaseballFrame(wxFrame *frame, int,
//                    const wxString& title, const wxPoint &pos=wxDefaultPosition,
//                    const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE )
{

//#if wxUSE_TOOLBAR
	//Create a ToolBar
	CreateToolBar ( wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL );
	InitToolBar ( GetToolBar() );
//#endif // end wxUSE_TOOLBAR

	// Accelerators
	wxAcceleratorEntry entries[9];
	entries[0].Set ( wxACCEL_CTRL, ( int ) 'O', wxID_OPEN );
	entries[1].Set ( wxACCEL_CTRL, WXK_F4, wxID_EXIT );
	entries[2].Set ( wxACCEL_CTRL, ( int ) 'P', wxID_PRINT );
	entries[3].Set ( wxACCEL_ALT, WXK_F4, wxID_EXIT );
	entries[4].Set ( wxACCEL_CTRL, ( int ) 'Z', wxID_UNDO );
	entries[5].Set ( wxACCEL_CTRL, ( int ) 'X', wxID_CUT );
	entries[6].Set ( wxACCEL_CTRL, ( int ) 'C', wxID_COPY );
	entries[7].Set ( wxACCEL_CTRL, ( int ) 'V', wxID_PASTE );
	entries[8].Set ( wxACCEL_NORMAL, WXK_F1, wxID_ABOUT );
	wxAcceleratorTable accel ( 9, entries );
	SetAcceleratorTable ( accel );

	// Give it an icon
//#if !defined(__WXMSW__)
//	SetIcon ( wxIcon ( Baseball_xpm ) );
//#else
//	SetIcon ( wxIcon ( App_xpm ) );
//#endif
	SetIcon ( wxIcon ( Baseball_xpm ) );

	// Make a menubar
//#if wxUSE_MENUS
	wxMenu* pFileMenu = new wxMenu;

	pFileMenu->Append ( wxID_OPEN, _T ( "&Open\tCTRL-O" ), _T ( "Open League" ) );
	pFileMenu->Append ( wxID_CLOSE, _T ( "&Close\tALT-F4" ), _T ( "Close League" ) );
	pFileMenu->AppendSeparator();
	pFileMenu->Append ( wxID_PRINT, _T ( "&Print\tCTRL-P" ), _T ( "Print" ) );
	pFileMenu->Append ( wxID_PREVIEW, _T ( "&Print Preview" ), _T ( "Print Preview" ) );
	pFileMenu->Append ( wxID_PRINT_SETUP, _T ( "&Print Setup" ), _T ( "Print Setup" ) );
	pFileMenu->AppendSeparator();
	pFileMenu->Append ( myID_EXPORT, _T ( "&Export" ), _T ( "Export Teams" ) );
	pFileMenu->Append ( myID_EXPORT_All, _T ( "&Export League" ), _T ( "Export League Teams All" ) );
	pFileMenu->Append ( myID_EXPORT_HTML, _T ( "&Export HTML Teams" ), _T ( "Export HTML Teams" ) );
	pFileMenu->Append ( myID_EXPORT_HTML_All, _T ( "&Export HTML Teams All" ), _T ( "Export HTML Teams All" ) );
	pFileMenu->AppendSeparator();
	pFileMenu->Append ( myID_OPTIONS, _T ( "Options" ), _T ( "Options" ) );
	pFileMenu->AppendSeparator();
	pFileMenu->Append ( wxID_EXIT, _T ( "&Exit\tALT-F4" ), _T ( "Quit the program" ) );

	wxMenu* pEditMenu = new wxMenu;
	pEditMenu->Append ( wxID_UNDO, _T ( "&Undo\tCTRL-Z" ), _T ( "Undo" ) );
	pEditMenu->AppendSeparator();
	pEditMenu->Append ( wxID_CUT, _T ( "&Cut\tCTRL-X" ), _T ( "Cut" ) );
	pEditMenu->Append ( wxID_COPY, _T ( "&Copy\tCTRL-C" ), _T ( "Copy" ) );
	pEditMenu->Append ( wxID_PASTE, _T ( "&Paste\tCTRL-V" ), _T ( "Paste" ) );
	pEditMenu->Enable ( wxID_UNDO, FALSE );
	pEditMenu->Enable ( wxID_CUT, FALSE );
	pEditMenu->Enable ( wxID_COPY, FALSE );
	pEditMenu->Enable ( wxID_PASTE, FALSE );

	wxMenu* pViewMenu = new wxMenu;
	pViewMenu->AppendCheckItem ( myID_TOOLBAR, _T ( "&ToolBar" ), _T ( "ToolBar" ) );
	pViewMenu->AppendCheckItem ( myID_STATUSBAR, _T ( "&StatusBar" ), _T ( "StatusBar" ) );
	pViewMenu->Check ( myID_TOOLBAR, TRUE );
	pViewMenu->Check ( myID_STATUSBAR, TRUE );
	pViewMenu->AppendSeparator();
	pViewMenu->AppendCheckItem ( myID_SCORESHEET, _T ( "&Scoresheet Form" ), _T ( "Scoresheet" ) );
	pViewMenu->AppendCheckItem ( myID_SCORESHEETXTRA, _T ( "&Scoresheet Form Xtra" ), _T ( "Scoresheet Xtra" ) );
	pViewMenu->AppendCheckItem ( myID_STATISTICS, _T ( "&Statistics Form" ), _T ( "Statistics Form" ) );
	pViewMenu->Check ( myID_SCORESHEET, TRUE );
	pViewMenu->Check ( myID_SCORESHEETXTRA, FALSE );
	pViewMenu->Check ( myID_STATISTICS, FALSE );

	wxMenu* pLeaguesMenu = new wxMenu;
	pLeaguesMenu->Append ( myID_ADDLEAGUE, _T ( "&Add League" ), _T ( "Add League" ) );
	pLeaguesMenu->Append ( myID_EDITLEAGUE, _T ( "&Edit League" ), _T ( "Edit League" ) );
	pLeaguesMenu->Append ( myID_LEAGUEOPTIONSHTML, _T ( "&Options" ), _T ( "Options" ) );
//	pLeaguesMenu->Enable ( myID_ADDLEAGUE, FALSE );
	pLeaguesMenu->Enable ( myID_LEAGUEOPTIONSHTML, FALSE );

	wxMenu* pTeamsMenu = new wxMenu;
	pTeamsMenu->Append ( myID_ADDTEAMS, _T ( "&Add Teams" ), _T ( "Add Teams" ) );
	pTeamsMenu->Append ( myID_EDITTEAMS, _T ( "&Edit Teams" ), _T ( "Edit Teams" ) );
	pTeamsMenu->AppendSeparator();
	pTeamsMenu->Append ( myID_CREATETEAM, _T ( "&Create Team" ), _T ( "Create Team" ) );
//	pTeamsMenu->Enable ( myID_ADDTEAMS, FALSE );
//	pTeamsMenu->Enable ( myID_EDITTEAMS, FALSE );

	wxMenu* pPlayersMenu = new wxMenu;
	pPlayersMenu->Append ( myID_ADDBATTERS, _T ( "&Add/Edit Batters" ), _T ( "Add/Edit Batters" ) );
	pPlayersMenu->Append ( myID_ADDPITCHERS, _T ( "&Add/Edit Pitchers" ), _T ( "Add/Edit Pitchers" ) );
	pPlayersMenu->AppendSeparator();
	pPlayersMenu->Append ( myID_DELBATTERS, _T ( "&Delete Batters" ), _T ( "Delete Batters" ) );
	pPlayersMenu->Append ( myID_DELPITCHERS, _T ( "&Delete Pitchers" ), _T ( "Delete Pitchers" ) );
	pPlayersMenu->Enable ( myID_DELBATTERS, FALSE );
	pPlayersMenu->Enable ( myID_DELPITCHERS, FALSE );

	wxMenu* pStatisticsMenu = new wxMenu;
	pStatisticsMenu->Append ( myID_CREATESTATS, _T ( "&Create Statistics" ), _T ( "Create Statistics" ) );
	pStatisticsMenu->Append ( myID_UPDATESTATS, _T ( "&Update Statistics" ), _T ( "Update Statistics" ) );
	pStatisticsMenu->Append ( myID_HTMLSTATS, _T ( "&League Statistics" ), _T ( "League Statistics" ) );
	pStatisticsMenu->Enable ( myID_CREATESTATS, FALSE );
	pStatisticsMenu->Enable ( myID_UPDATESTATS, FALSE );

	wxMenu* pWindowMenu = new wxMenu;

	wxMenu* pHelpMenu = new wxMenu;
	pHelpMenu->Append ( wxID_ABOUT, _T ( "&About" ) );

	pMenuBar = new wxMenuBar;

	pMenuBar->Append ( pFileMenu, _T ( "&File" ) );
	pMenuBar->Append ( pEditMenu, _T ( "&Edit" ) );
	pMenuBar->Append ( pViewMenu, _T ( "&View" ) );
	pMenuBar->Append ( pLeaguesMenu, _T ( "&Leagues" ) );
	pMenuBar->Append ( pTeamsMenu, _T ( "&Teams" ) );
	pMenuBar->Append ( pPlayersMenu, _T ( "&Players" ) );
	pMenuBar->Append ( pStatisticsMenu, _T ( "&Statistics" ) );
	pMenuBar->Append ( pWindowMenu, _T ( "&Window" ) );
	pMenuBar->Append ( pHelpMenu, _T ( "&Help" ) );

	// Associate the menu bar with pMyMainWindow
	SetMenuBar ( pMenuBar );
//#endif // wxUSE_MENUS

//#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user!"),0);
    SetStatusText(wxbuildinfo(short_f), 1);
//#endif // wxUSE_STATUSBAR

//sqlite3 files
//	m_DBFileName = _T("");
//	m_dbOpen = 99;		// SQLITE_OK is set to '0'
}


WxBaseballFrame::~WxBaseballFrame()
{
}

// Called when clicking the X in top right corner
void WxBaseballFrame::OnClose(wxCloseEvent &event)
{
    wxString Foobar;

//    Foobar.Printf( wxT("OnClose"));
//    wxMessageBox(Foobar);

    wxGetApp().pDBRoutines->DBClose();

    // Continue with closing the frame.
    event.Skip();
}

void WxBaseballFrame::OnOpen(wxCommandEvent &event)
{
//    wxMessageBox ( _T ( "You entered OnOpen" ),
//                _T ( "OnOpen" ), wxOK|wxICON_INFORMATION );
    wxGetApp().pDBRoutines->DBOpen();
}

void WxBaseballFrame::OnQuit(wxCommandEvent &event)
{
    wxString Foobar;

//    Foobar.Printf( wxT("OnQuit"));
//    wxMessageBox(Foobar);

    wxGetApp().pDBRoutines->DBClose();

    Destroy();
}

void WxBaseballFrame::OnAbout(wxCommandEvent &event)
{
    wxString msg;
    wxString strWxVersion = wxbuildinfo(long_f);
    msg.Printf( wxT( "WxBaseball Tracking System\nVersion:  %s\nWxWidgets Version: %s"), APP_VERSION_STR, strWxVersion );
//    msg.Printf( wxT( "WxBaseball Tracking System\nVersion:  %s\nWxWidgets Version: %s"), wxGetApp().myAppVersion, strWxVersion );
    wxMessageBox(msg, _("Welcome to WxBaseball"));
}

void WxBaseballFrame::OnPrint ( wxCommandEvent& event )
{
	wxPrintDialogData printDialogData ( * wxGetApp().g_printData );

	wxPrinter printer ( & printDialogData );
	MyPrintout printout ( _T ( "My printout" ) );
	if ( !printer.Print ( this, &printout, true /*prompt*/ ) )
	{
		if ( wxPrinter::GetLastError() == wxPRINTER_ERROR )
			wxMessageBox (
			    _T ( "There was a problem printing.\nPerhaps your current printer is not set correctly?" ),
			    _T ( "Printing" ), wxOK|wxICON_INFORMATION );
		else
			wxMessageBox ( _T ( "You canceled printing" ),
			               _T ( "Printing" ), wxOK|wxICON_INFORMATION );
	}
	else
	{
		( *wxGetApp().g_printData ) = printer.GetPrintDialogData().GetPrintData();
	}
}

void WxBaseballFrame::OnPrintPreview ( wxCommandEvent& event )
{
	// Pass two printout objects: for preview, and possible printing.
	wxPrintDialogData printDialogData ( * wxGetApp().g_printData );
	wxPrintPreview *preview = new wxPrintPreview ( new MyPrintout, new MyPrintout, & printDialogData );
	if ( !preview->Ok() )
	{
		delete preview;
		wxMessageBox (
		    _T ( "There was a problem previewing.\nPerhaps your current printer is not set correctly?" ),
		    _T ( "Previewing" ), wxOK|wxICON_INFORMATION );
		return;
	}

	// This size will display the entire score sheet without scroll bars in most cases
	wxPreviewFrame *frame = new wxPreviewFrame ( preview, this, _T ( "Print Preview" ),
	                        wxPoint ( 50, 50 ), wxSize ( 800, 710 ) );
	frame->Centre ( wxBOTH );
	frame->Initialize();
	frame->Show();
}

void WxBaseballFrame::OnPrintSetup ( wxCommandEvent& event )
{
	wxPrintDialogData printDialogData ( * wxGetApp().g_printData );
	wxPrintDialog printerDialog ( this, & printDialogData );

	// show print setup dialog
	printerDialog.ShowModal();

	( * wxGetApp().g_printData ) = printerDialog.GetPrintDialogData().GetPrintData();
}

void WxBaseballFrame::OnFileExportLeagueTeams ( wxCommandEvent& event )
{
	int teamID;
	wxString msg;

	// Check is DB is open, if not, open one
    if( !wxGetApp().pDBRoutines->DBIsDBOpen() )
    {
        msg.Printf( wxT("Database has not been opened"));
        wxMessageBox(msg);
    }

	// Use a dialog to select a league then a team.
	teamID = wxGetApp().pDBRoutines->DBGetATeamID();
	if ( teamID != false )
		wxGetApp().pFileRoutines->ExportTeam( teamID );
}

// Create a list of all teams in a league, then export all of them
void WxBaseballFrame::OnFileExportLeagueTeamsAll ( wxCommandEvent& event )
{
	int leagueID;
	int i;
	wxString msg;

	// Check is DB is open, if not, open one
    if( !wxGetApp().pDBRoutines->DBIsDBOpen() )
    {
        msg.Printf( wxT("Database has not been opened"));
        wxMessageBox(msg);
    }

	// Use a dialog to select a league then create the array of the teams in the league
	leagueID = wxGetApp().pDBRoutines->DBGetLeague();
	if ( leagueID != wxID_CANCEL )
	{
		wxGetApp().pDBRoutines->DBGetLeagueTeams( leagueID );

		for ( i=0; i<(int)wxGetApp().pDBRoutines->m_arrayTeamIDs.GetCount(); i++ )
		{
			wxGetApp().pFileRoutines->ExportTeam( wxGetApp().pDBRoutines->m_arrayTeamIDs[i] );
		}
	}
}

void WxBaseballFrame::OnExportHTML ( wxCommandEvent& event )
{
}

void WxBaseballFrame::OnExportHTMLAll ( wxCommandEvent& event )
{
}

void WxBaseballFrame::OnOptions ( wxCommandEvent& event )
{
}

//  WxWidgits already has changed the status of the checked item before
//  this code get control.
void WxBaseballFrame::OnScoreSheetCheck ( wxCommandEvent& event )
{
	if ( pMenuBar->IsChecked ( myID_SCORESHEET ) )
	{
		pMenuBar->Check ( myID_SCORESHEETXTRA, FALSE );
		pMenuBar->Check ( myID_STATISTICS, FALSE );
		pCanvas->Refresh();
	}
	else
	{
		pMenuBar->Check ( myID_SCORESHEETXTRA, TRUE );
		pMenuBar->Check ( myID_STATISTICS, FALSE );
		pCanvas->Refresh();
	}
}

//  WxWidgits already has changed the status of the checked item before
//  this code get control.
void WxBaseballFrame::OnScoreSheetCheckExtra ( wxCommandEvent& event )
{
	if ( pMenuBar->IsChecked ( myID_SCORESHEETXTRA ) )
	{
		pMenuBar->Check ( myID_SCORESHEET, FALSE );
		pMenuBar->Check ( myID_STATISTICS, FALSE );
		pCanvas->Refresh();
	}
	else
	{
		pMenuBar->Check ( myID_SCORESHEET, TRUE );
		pMenuBar->Check ( myID_STATISTICS, FALSE );
		pCanvas->Refresh();
	}
}

//  WxWidgits already has changed the status of the checked item before
//  this code get control.
void WxBaseballFrame::OnStatistics ( wxCommandEvent& event )
{
	if ( pMenuBar->IsChecked ( myID_SCORESHEET ) )
	{
		// until Statistics score sheet is created, leave default to normal scoresheet
		pMenuBar->Check ( myID_SCORESHEET, TRUE );
		pMenuBar->Check ( myID_SCORESHEETXTRA, FALSE );
		pMenuBar->Check ( myID_STATISTICS, FALSE );
		pCanvas->Refresh();
	}
	else
	{
		// until Statistics score sheet is created, leave default to normal scoresheet
		pMenuBar->Check ( myID_SCORESHEET, TRUE );
		pMenuBar->Check ( myID_SCORESHEETXTRA, FALSE );
		pMenuBar->Check ( myID_STATISTICS, FALSE );
		pCanvas->Refresh();
	}
}

void WxBaseballFrame::OnStatisticsHTMLLeagueStats ( wxCommandEvent& event )
{
	int LeagueID;
    wxString msg;

	// Check is DB is open, if not, open one
    if( !wxGetApp().pDBRoutines->DBIsDBOpen() )
    {
        msg.Printf( wxT("Database has not been opened"));
        wxMessageBox(msg);
    }
//    msg.Printf( wxT( "DB opened in Frame") );
//    wxMessageBox(msg);

	// DBGetLeague will prompt for the league
	LeagueID = wxGetApp().pDBRoutines->DBGetLeague();
	if (LeagueID != wxID_CANCEL)
	{
		// This will populate the structLeagueData
		wxGetApp().pDBRoutines->DBGetLeague( LeagueID );
		// The following will look at the League for Conferences and Divisions
		// which in turn will involk pFileRoutines->BuildPlayerStats to
		// create the stats report for each division
		wxGetApp().pDBRoutines->DBGetLeagueConferenceDivision( LeagueID );
	//	wxGetApp().pFileRoutines->BuildPlayerStats( 1, 2, 3 );
	}
}

void WxBaseballFrame::OnLeaguesAddLeague ( wxCommandEvent& event )
{
	// Check is DB is open, if not, open one
	wxGetApp().pDBRoutines->DBIsDBOpen();

	LeagueDialogAdd( this );
}

void WxBaseballFrame::OnLeaguesEditLeague ( wxCommandEvent& event )
{
}

void WxBaseballFrame::OnTeamsAddTeam ( wxCommandEvent& event )
{
	int LeagueID;
	int ConferenceID;
	int DivisionID;

	// Check is DB is open, if not, open one
	wxGetApp().pDBRoutines->DBIsDBOpen();

	// DBGetLeague will prompt for the league
	LeagueID = wxGetApp().pDBRoutines->DBGetLeague();
	if (LeagueID != wxID_CANCEL)\
	{
		// This will populate the structLeagueData
		wxGetApp().pDBRoutines->DBGetLeague( LeagueID );

		// Get Conference, check to see if valid, then
		// get Division and check to see if its valid
		ConferenceID = wxGetApp().pDBRoutines->DBGetConferenceID( LeagueID );
		if (ConferenceID != wxID_CANCEL)
		{
			DivisionID = wxGetApp().pDBRoutines->DBGetDivisionID( ConferenceID );
			if (DivisionID != wxID_CANCEL)
			{
				TeamDialogAdd( this );
			}
		}
	}
}

void WxBaseballFrame::OnTeamsEditTeam ( wxCommandEvent& event )
{
	int LeagueID;
	int ConferenceID;
	int DivisionID;
	int TeamID;

	// Check is DB is open, if not, open one
	wxGetApp().pDBRoutines->DBIsDBOpen();

	// DBGetLeague will prompt for the league
	LeagueID = wxGetApp().pDBRoutines->DBGetLeague();
	if (LeagueID != wxID_CANCEL)
	{
		// This will populate the structLeagueData
		wxGetApp().pDBRoutines->DBGetLeague( LeagueID );

		// Get Conference, check to see if valid, then
		// get Division and check to see if its valid
		ConferenceID = wxGetApp().pDBRoutines->DBGetConferenceID( LeagueID );
		if (ConferenceID != wxID_CANCEL)
		{
			DivisionID = wxGetApp().pDBRoutines->DBGetDivisionID( ConferenceID );
			if (DivisionID != wxID_CANCEL)
			{
				TeamID = wxGetApp().pDBRoutines->DBSelectTeam( LeagueID, ConferenceID, DivisionID );
				// wxID_CANCEL is 5101 so if an actual teamID was 5101 there would be a problem.
				// So, we are returning false '0'. All DB indexes start with 1 so zero is ok.
				if (TeamID != false)
				{
					wxGetApp().pDBRoutines->DBGetTeam( TeamID );

					TeamDialogApply( this );
				}
			}
		}
	}
}

void WxBaseballFrame::OnTeamsCreateTeam ( wxCommandEvent& event )
{

	// Check is DB is open, if not, open one
//	wxGetApp().pDBRoutines->DBIsDBOpen();

	TeamDialogCreate( this );
}

void WxBaseballFrame::OnPlayersAddEditBatter ( wxCommandEvent& event )
{
	BatterDialog ( this );
}

void WxBaseballFrame::OnPlayersAddEditPitcher ( wxCommandEvent& event )
{
	PitcherDialog ( this );
}

//#if wxUSE_TOOLBAR
void WxBaseballFrame::InitToolBar ( wxToolBar* pToolBar )
{
	wxBitmap* pBitmaps[8];

	pBitmaps[0] = new wxBitmap ( new_xpm );
	pBitmaps[1] = new wxBitmap ( open_xpm );
	pBitmaps[2] = new wxBitmap ( save_xpm );
	pBitmaps[3] = new wxBitmap ( copy_xpm );
	pBitmaps[4] = new wxBitmap ( cut_xpm );
	pBitmaps[5] = new wxBitmap ( paste_xpm );
	pBitmaps[6] = new wxBitmap ( print_xpm );
	pBitmaps[7] = new wxBitmap ( help_xpm );

	int width = 24;
	int currentX = 5;

	currentX += width + 5;

    pToolBar->AddTool (myID_TOOLBAR_OPEN, "Open", *pBitmaps[1], "Open", wxITEM_NORMAL);
	currentX += width + 5;
    pToolBar->AddTool (myID_TOOLBAR_SAVE, "Save", *pBitmaps[2], "Save", wxITEM_NORMAL);
	currentX += width + 5;

	pToolBar->AddSeparator();

    pToolBar->AddTool (myID_TOOLBAR_COPY, "Copy", *pBitmaps[3], "Copy", wxITEM_NORMAL);
	currentX += width + 5;
    pToolBar->AddTool (myID_TOOLBAR_CUT, "Cut", *pBitmaps[4], "Cut", wxITEM_NORMAL);
	currentX += width + 5;
    pToolBar->AddTool (myID_TOOLBAR_PASTE, "Paste", *pBitmaps[5], "Paste", wxITEM_NORMAL);
	currentX += width + 5;

	pToolBar->AddSeparator();

    pToolBar->AddTool (myID_TOOLBAR_PRINT, "Print", *pBitmaps[6], "Print", wxITEM_NORMAL);
	currentX += width + 5;

	pToolBar->AddSeparator();

    pToolBar->AddTool (myID_TOOLBAR_HELP, "Help", *pBitmaps[7], "Help", wxITEM_NORMAL);

	pToolBar->Realize();

	int i;
	for ( i = 0; i < 8; i++ )
		delete pBitmaps[i];
}
//#endif // end wxUSE_TOOLBAR
