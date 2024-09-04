/////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp                                                   //
// Purpose:     Baseball statistics tracking tool for Strat-O-Matic        //
// Author:      John Gnew                                                  //
// Created:     08/30/04                                                   //
// Copyright:   (c) John Gnew                                              //
// Modifications:                                                          //
//   Date       Description                                                //
// 08/26/09     Corrected seticon error on Baseball_xpm not being used.    //
//              Set the defaults on the following fields                   //
//              in BuildPlayerArray: fBA=fSLG=fOBP=0.0f                    //
// 06/28/07     Added BuildPlayerArray, OnStatisticsHTMLLeagueStats, and   //
//              OnStatisticsHTMLLeagueStats                                //
// 06/27/07     Added OnExportHTMLAll                                      //
//              Hard coded HTML defaults                                   //
// 06/26/07     Added OnExportHTML, GetTeamHTML and ExportHTMLFile         //
// 02/02/05     Changed "League Options HTML" to "Options"                 //
//              Changed "Edit/Edit" to "Add/Edit" for pitchers menu        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
//       In MyCanvas::MyCanvas the following line was comented to not      //
//       display a green background.                                       //
//           SetBackgroundColour( _T("SEA GREEN"));                        //
//                                                                         //
//   Date       Description                                                //
// 06/26/07     Done - Update ExportHTMLFile HTML defaults                 //
//              Figure out options profile                                 //
//              Done - Setup statstics menu options                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#define __WXGTK__

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#include <wx/print.h>
#include <wx/printdlg.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/filefn.h>
//    #include <wx/toolbar.h>
//    #include <wx/splash.h>
#endif

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

#include "main.h"
#include "about.h"
#include "Print.h"
#include "Forms.h"
#include "dialogs.h"
#include "FileStruct.h"

#include <time.h>

IMPLEMENT_APP ( MyApp )

// ---------------------------------------------------------------------------
// global variables
// ---------------------------------------------------------------------------
#define HTML_DISP_LINES 10

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE ( MyMainWindow, wxFrame )
	//File Menu
	//    EVT_MENU(wxID_OPEN, MyMainWindow::OnOpen)
	EVT_MENU ( wxID_CLOSE, MyMainWindow::OnQuit )
	EVT_MENU ( wxID_PRINT, MyMainWindow::OnPrint )
	EVT_MENU ( wxID_PREVIEW, MyMainWindow::OnPrintPreview )
	EVT_MENU ( wxID_PRINT_SETUP, MyMainWindow::OnPrintSetup )
	EVT_MENU ( myID_EXPORT, MyMainWindow::OnFileExportLeagueTeams )
	EVT_MENU ( myID_EXPORT_All, MyMainWindow::OnFileExportLeagueTeamsAll )
	EVT_MENU ( myID_EXPORT_HTML, MyMainWindow::OnExportHTML )
	EVT_MENU ( myID_EXPORT_HTML_All, MyMainWindow::OnExportHTMLAll )
	EVT_MENU ( myID_OPTIONS, MyMainWindow::OnOptions )
	EVT_MENU ( wxID_EXIT, MyMainWindow::OnQuit )
	// Edit Menu
	// View Menu
	EVT_MENU ( myID_SCORESHEET, MyMainWindow::OnScoreSheetCheck )
	EVT_MENU ( myID_SCORESHEETXTRA, MyMainWindow::OnScoreSheetCheckExtra )
	EVT_MENU ( myID_STATISTICS, MyMainWindow::OnStatistics )
	// Leagues Menu
	//    EVT_MENU(myID_ADDLEAGUE, MyMainWindow::OnLeaguesAddLeague)
	EVT_MENU ( myID_EDITLEAGUE, MyMainWindow::OnLeaguesEditLeague )
	//    EVT_MENU(myID_LEAGUEOPTIONSHTML, MyMainWindow::OnLeagueOptionsHTML)
	// Teams Menu
	//    EVT_MENU(myID_ADDTEAMS, MyMainWindow::OnAddTeams)
	//    EVT_MENU(myID_EDITTEAMS, MyMainWindow::OnEditTeams)
	// Players Menu
	EVT_MENU ( myID_ADDBATTERS, MyMainWindow::OnPlayersAddEditBatter )
	EVT_MENU ( myID_ADDPITCHERS, MyMainWindow::OnPlayersAddEditPitcher )
	// Statistics Menu
	//    EVT_MENU(myID_CREATESTATS, MyMainWindow::OnCreateStats)
	//    EVT_MENU(myID_UPDATESTATS, MyMainWindow::OnUpdateStats)
	EVT_MENU ( myID_HTMLSTATS, MyMainWindow::OnStatisticsHTMLLeagueStats )

	// Window Menu
	// Help Menu
	EVT_MENU ( wxID_ABOUT, MyMainWindow::OnAbout )

	//    EVT_TOOL(myID_TOOLBAR_NEW, MyMainWindow::OnNew)
	//    EVT_TOOL(myID_TOOLBAR_OPEN, MyMainWindow::OnOpen)
	//    EVT_TOOL(myID_TOOLBAR_SAVE, MyMainWindow::OnSave)
	//    EVT_TOOL(myID_TOOLBAR_COPY, MyMainWindow::OnCopy)
	//    EVT_TOOL(myID_TOOLBAR_CUT, MyMainWindow::OnCut)
	//    EVT_TOOL(myID_TOOLBAR_PASTE, MyMainWindow::OnPaste)
	EVT_TOOL ( myID_TOOLBAR_PRINT, MyMainWindow::OnPrint )
	EVT_TOOL ( myID_TOOLBAR_HELP, MyMainWindow::OnAbout )

	EVT_CLOSE ( MyMainWindow::OnClose )
END_EVENT_TABLE()

// ===========================================================================
// Implementation
// ===========================================================================

// ---------------------------------------------------------------------------
// MyApp
// ---------------------------------------------------------------------------

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
	pMyMainWindow = ( MyMainWindow * ) NULL;

	// Global print data, to remember settings during the session
	g_printData = ( wxPrintData * ) NULL ;
	g_printData = new wxPrintData;
	g_printData->SetOrientation ( wxLANDSCAPE );
	g_printData->SetPaperId ( wxPAPER_LETTER );

	// Global page setup data
	g_pageSetupData = ( wxPageSetupData * ) NULL;
	g_pageSetupData = new wxPageSetupDialogData;

	// Global Options directory and file name setup
	g_OptionsDir = wxGetHomeDir()+"/.wxApps";
	g_OptionsFile = g_OptionsDir+"/"+"wxBaseball.ini";

    // Setup Global options varaibles and/or creat options file
	OptionFile myOptionFile;

//    // Display Splash screen - Fails in link for wxSplashScreen
//    wxBitmap bitmap;
////    if (bitmap.LoadFile("splash16.png", wxBITMAP_TYPE_PNG))
//    if (bitmap.LoadFile("bitmaps/app.xpm", wxBITMAP_TYPE_XPM))
//    {
//        wxSplashScreen* splash = new wxSplashScreen(bitmap,
//            wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
//            6000, NULL, -1, wxDefaultPosition, wxDefaultSize,
//            wxSIMPLE_BORDER|wxSTAY_ON_TOP);
//    }
//    wxYield();

	// Create the main pMyMainWindow window

	pMyMainWindow = new MyMainWindow ( ( wxFrame * ) NULL, -1,
	                                   _T ( "Baseball Statistics for Strat-O-Matic" ),
	                                   wxDefaultPosition, wxSize ( 900, 650 ),
	                                   wxDEFAULT_FRAME_STYLE );

	pMyMainWindow->pCanvas = new MyCanvas ( pMyMainWindow, wxID_ANY,
	                                        wxDefaultPosition, wxDefaultSize );

	pMyMainWindow->Centre ( wxBOTH );
	pMyMainWindow->Show ( TRUE );

	SetTopWindow ( pMyMainWindow );

	return TRUE;
}

int MyApp::OnExit()
{
	delete g_printData;
	delete g_pageSetupData;
	return 1;
}

// ---------------------------------------------------------------------------
// MyMainWindow
// ---------------------------------------------------------------------------
MyMainWindow::MyMainWindow ( wxWindow* parent,
                             const wxWindowID id,
                             const wxString& title,
                             const wxPoint& pos,
                             const wxSize& size,
                             const long style )
		: wxFrame ( parent, id, title, pos, size,
		            style | wxNO_FULL_REPAINT_ON_RESIZE )
{

#if wxUSE_TOOLBAR
	//Create a ToolBar
	CreateToolBar ( wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL );
	InitToolBar ( GetToolBar() );
#endif // end wxUSE_TOOLBAR

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
	pLeaguesMenu->Enable ( myID_ADDLEAGUE, FALSE );
	pLeaguesMenu->Enable ( myID_LEAGUEOPTIONSHTML, FALSE );

	wxMenu* pTeamsMenu = new wxMenu;
	pTeamsMenu->Append ( myID_ADDTEAMS, _T ( "&Add Teams" ), _T ( "Add Teams" ) );
	pTeamsMenu->Append ( myID_EDITTEAMS, _T ( "&Edit Teams" ), _T ( "Edit Teams" ) );
	pTeamsMenu->Enable ( myID_ADDTEAMS, FALSE );
	pTeamsMenu->Enable ( myID_EDITTEAMS, FALSE );

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
	pStatisticsMenu->Append ( myID_HTMLSTATS, _T ( "&League Statistics HTML" ), _T ( "League Statistics HTML" ) );
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

	CreateStatusBar();
}

//  WxWidgits already has changed the status of the checked item before
//  this code get control.
void MyMainWindow::OnScoreSheetCheck ( wxCommandEvent& event )
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
void MyMainWindow::OnScoreSheetCheckExtra ( wxCommandEvent& event )
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
void MyMainWindow::OnStatistics ( wxCommandEvent& event )
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

void MyMainWindow::OnLeaguesEditLeague ( wxCommandEvent& event )
{
	wxString strLeagueDirName;
	LeagueStruct my_LeagueStruct;

	strLeagueDirName = my_LeagueStruct.GetLeagueDir ( FALSE );
	if ( strLeagueDirName.IsEmpty() )
	{
		wxMessageBox ( _T ( "No Leagues were found" ),
		               _T ( "League" ), wxOK|wxICON_INFORMATION );
	}
	else
	{
		// Todo - Process league
		wxMessageBox ( strLeagueDirName, "\n");
	}

}

void MyMainWindow::OnClose ( wxCloseEvent& event )
{
	event.Skip();
}

void MyMainWindow::OnQuit ( wxCommandEvent& WXUNUSED ( event ) )
{
	Close();
}

void MyMainWindow::OnAbout ( wxCommandEvent& WXUNUSED ( event ) )
{
//    (void)wxMessageBox(_T("Baseball Statistics for Strat-O-Matic\n")
//                       _T("Author: John Gnew (c) 2005\n")
//                       _T("Usage: baseball.exe"), _T("About Baseball Statistics"));
	AppAbout ( this );
}

void MyMainWindow::OnPrint ( wxCommandEvent& event )
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

// Print PreView
void MyMainWindow::OnPrintPreview ( wxCommandEvent& event )
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

// Print Setup
void MyMainWindow::OnPrintSetup ( wxCommandEvent& event )
{
	wxPrintDialogData printDialogData ( * wxGetApp().g_printData );
	wxPrintDialog printerDialog ( this, & printDialogData );

	// show print setup dialog
	printerDialog.ShowModal();

	( * wxGetApp().g_printData ) = printerDialog.GetPrintDialogData().GetPrintData();
}

#if wxUSE_TOOLBAR
void MyMainWindow::InitToolBar ( wxToolBar* pToolBar )
{
//	wxBitmap* pBitmaps[8];
//
//	pBitmaps[0] = new wxBitmap ( new_xpm );
//	pBitmaps[1] = new wxBitmap ( open_xpm );
//	pBitmaps[2] = new wxBitmap ( save_xpm );
//	pBitmaps[3] = new wxBitmap ( copy_xpm );
//	pBitmaps[4] = new wxBitmap ( cut_xpm );
//	pBitmaps[5] = new wxBitmap ( paste_xpm );
//	pBitmaps[6] = new wxBitmap ( print_xpm );
//	pBitmaps[7] = new wxBitmap ( help_xpm );
//
//	int width = 24;
//	int currentX = 5;
//
////	pToolBar->AddTool ( myID_TOOLBAR_NEW, * ( pBitmaps[0] ), wxNullBitmap, FALSE, currentX, -1,
////	                    ( wxObject * ) NULL, _T ( "New file" ) );
//	currentX += width + 5;
//	pToolBar->AddTool ( myID_TOOLBAR_OPEN, *pBitmaps[1], wxNullBitmap, FALSE, currentX, -1,
//	                    ( wxObject * ) NULL, _T ( "Open file" ) );
//	currentX += width + 5;
//	pToolBar->AddTool ( myID_TOOLBAR_SAVE, *pBitmaps[2], wxNullBitmap, FALSE, currentX, -1,
//	                    ( wxObject * ) NULL, _T ( "Save file" ) );
//	currentX += width + 5;
//
//	pToolBar->AddSeparator();
//
//	pToolBar->AddTool ( myID_TOOLBAR_COPY, *pBitmaps[3], wxNullBitmap, FALSE, currentX, -1,
//	                    ( wxObject * ) NULL, _T ( "Copy" ) );
//	currentX += width + 5;
//	pToolBar->AddTool ( myID_TOOLBAR_CUT, *pBitmaps[4], wxNullBitmap, FALSE, currentX, -1,
//	                    ( wxObject * ) NULL, _T ( "Cut" ) );
//	currentX += width + 5;
//	pToolBar->AddTool ( myID_TOOLBAR_PASTE, *pBitmaps[5], wxNullBitmap, FALSE, currentX, -1,
//	                    ( wxObject * ) NULL, _T ( "Paste" ) );
//	currentX += width + 5;
//
//	pToolBar->AddSeparator();
//
//	pToolBar->AddTool ( myID_TOOLBAR_PRINT, *pBitmaps[6], wxNullBitmap, FALSE, currentX, -1,
//	                    ( wxObject * ) NULL, _T ( "Print" ) );
//	currentX += width + 5;
//
//	pToolBar->AddSeparator();
//
//	pToolBar->AddTool ( myID_TOOLBAR_HELP, *pBitmaps[7], wxNullBitmap, TRUE, currentX, -1,
//	                    ( wxObject * ) NULL, _T ( "Help" ) );
//
//	pToolBar->Realize();
//
//	int i;
//	for ( i = 0; i < 8; i++ )
//		delete pBitmaps[i];
}
#endif // end wxUSE_TOOLBAR

void MyMainWindow::OnFileExportLeagueTeams ( wxCommandEvent& event )
{
	wxString strLeagueDirName;
	LeagueStruct my_LeagueStruct;

	strLeagueDirName = my_LeagueStruct.GetLeagueDir ( FALSE );
	if ( strLeagueDirName.IsEmpty() )
	{
		wxMessageBox ( _T ( "No Leagues were found" ),
		               _T ( "League" ), wxOK|wxICON_INFORMATION );
	}
	else
	{
		GetTeam ( strLeagueDirName );
	}
}

void MyMainWindow::OnFileExportLeagueTeamsAll ( wxCommandEvent& event )
{
	wxArrayString arrayTeamNames;
	wxArrayString arrayFileNames;
	wxFile myInFile;
	wxByte count;
	char temp[40];
	wxString strTemp;
	wxString strTeamName;
	int i;

	wxString strLeague;
	wxString strLeagueName;
	wxString strLeagueFile;
	wxString strLeagueDir;
	wxFileName myTempFileName;
	LeagueStruct my_LeagueStruct;

	strLeagueDir = my_LeagueStruct.GetLeagueDir ( FALSE );

	arrayTeamNames.Clear();
	if ( wxDirExists ( strLeagueDir ) )
	{
		// We only search for /tb*.dat files because in the exportfile
		// routine we create the TB and TP file names for processing.
		myTempFileName = wxFindFirstFile ( strLeagueDir+"/tb*.dat",wxFILE );
		if ( myTempFileName.FileExists() )
		{
			while ( myTempFileName.FileExists() )
			{
				arrayFileNames.Add ( myTempFileName.GetFullName() );
				myTempFileName = wxFindNextFile();
			}

			// Since the FindNextFile does not return the files in any order
			// we must sort the file names
			arrayFileNames.Sort();

			for ( i=0; i< ( int ) arrayFileNames.GetCount(); i++ )
			{
				myInFile.Open ( strLeagueDir+"/"+arrayFileNames[i],wxFile::read );
				myInFile.Read ( &count,sizeof ( count ) );
				// Get the Team name
				myInFile.Read ( temp,40 );
				myInFile.Close();
				strTeamName.Printf ( "%40s",temp );
				arrayTeamNames.Add ( strTeamName.Mid ( 0,40 ) +arrayFileNames[i] );
			}
		}
		else
		{
			// No Leagues found so display message
			wxMessageBox ( _T ( "Could not find any Team files. Please create a Team" ),
			               _T ( "Team" ),
			               wxOK|wxICON_ERROR );
		}
	}
	else
	{
		// data directory not found so display message
		wxMessageBox ( _T ( "Directory \""+strLeagueDir+"\" could not be found" ), _T ( "League" ),
		               wxOK );
	}

	for ( i=0; i< ( int ) arrayFileNames.GetCount(); i++ )
	{
		ExportFile ( strLeagueDir, arrayTeamNames[i] );
	}
}

void MyMainWindow::GetTeam ( wxString strLeagueDir )
{
	wxString strTeamFileName;
	TeamStruct my_TeamStruct;

	strTeamFileName = my_TeamStruct.GetTeamName ( strLeagueDir );

	if ( strTeamFileName.IsEmpty() )
	{
		wxMessageBox ( _T ( "No Teams were found" ),
		               _T ( "Teams" ), wxOK|wxICON_INFORMATION );
	}
	else
	{
		ExportFile ( strLeagueDir, strTeamFileName );
	}
}

void MyMainWindow::ExportFile ( wxString strDir, wxString strTeamName )
{
	BatterStruct structBatter;
	PitcherStruct structPitcher;
	wxByte count;
	wxFile exportBatter;
	wxFile exportPitcher;
	wxString exportFileName;
	char exportData[200];
	wxString strexportData;
	float fBattingAverage, fSLG, fOnBasePercentage;
	int i;
	wxString strTemp;
	wxString strTemp1;
	wxString strTemp2;
	wxString myFileName;
	double fIP;
	double fERA;
	double fTRG;
	char Bunting[5] = {'A','B','C','D','E'};
	char HitRun[4] = {'A','B','C','D'};
	char Stealing[7] = {'3','2','A','B','C','D','E'};
	char BatterHits[3] = {'L','R','S'};
	char Power[2] = {'N','W'};
	int Running[15] = {5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
	int TRate[21] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
	int Pass[21] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
	int OutArm[12] = {-6,-5,-4,-3,-2,-1,0,1,2,3,4,5};
	int CatchArm[10] = {-4,-3,-2,-1,0,1,2,3,4,5};
	int PHold[16] = {9,8,7,6,5,4,3,2,1,0,-1,-2,-3,-4,-5,-6};

	count = 0;

	// Process Batter file
	strTemp = "XB" + strTeamName.Left ( 20 );
	exportFileName = strDir+"/" + strTemp+".txt"; // dir/xb000001.txt
	myFileName = strDir+"/tb"+strTeamName.Right ( 10 );
	exportBatter.Create ( exportFileName,TRUE );
	sprintf ( exportData,"Team Name,Batter Name,,AB,Runs,Hits,RBI,2B,3B,HR,W,K,RE," );
	strexportData = exportData;
	exportBatter.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"SF,SB,CS,Bunting,Stealing,Running,Hit Run,LRS,P,C," );
	strexportData = exportData;
	exportBatter.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"1B,2B,SS,3B,LF,CF,RF,ChB,ChL,ChR," );
	strexportData = exportData;
	exportBatter.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"AVG,SLG,OBP,Games,HBP," );
	strexportData = exportData;
	exportBatter.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"Ch1B,Ch2B,Ch3B,ChHR,ChW,ChDP," );
	strexportData = exportData;
	exportBatter.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"Ch1BL,Ch2BL,Ch3BL,ChHRL,ChWL,ChDPL," );
	strexportData = exportData;
	exportBatter.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"Ch1BR,Ch2BR,Ch3BR,ChHRR,ChWR,ChDPR," );
	strexportData = exportData;
	exportBatter.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"ER1,ER2,ER3,ER4,ER5,ER6,ER7,ER8,ER9," );
	strexportData = exportData;
	exportBatter.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"T Rating,Passball,Power Left,Power Right,Outfield Arm,Catcher Arm\n" );
	strexportData = exportData;
	exportBatter.Write ( strexportData,strexportData.Len() );

	// A Team was selected so export all of the players
	count = structBatter.GetCountBatter ( myFileName );
	for ( i=0; i<count; i++ )
	{
		structBatter.GetBatter ( myFileName, i );
		strexportData.Empty();
		if ( structBatter.m_AB == 0 )
		{
			fBattingAverage = 0.0f;
			fSLG = 0.0f;
			fOnBasePercentage = 0.0f;
		}
		else
		{
			fBattingAverage = ( float ) structBatter.m_Hits/structBatter.m_AB;

			fSLG = ( float )
			       ( ( structBatter.m_Hits - ( structBatter.m_2B + structBatter.m_3B
			                                   + structBatter.m_HomeRuns ) ) + ( 2*structBatter.m_2B )
			         + ( 3*structBatter.m_3B ) + ( 4*structBatter.m_HomeRuns ) )
			       / ( structBatter.m_AB );

			fOnBasePercentage = ( float )
			                    ( structBatter.m_Hits + structBatter.m_Walk
			                      + structBatter.m_ReachedOnError + structBatter.m_Sacrifice
			                      + structBatter.m_StolenBase )
			                    / ( structBatter.m_AB+structBatter.m_Walk
			                        + structBatter.m_ReachedOnError + structBatter.m_Sacrifice
			                        + structBatter.m_StolenBase );
		}

		strTemp1 = strTeamName.Left ( 40 );
		strTemp2 = structBatter.m_PlayerName.Left ( 30 );
		sprintf ( exportData,"%s,%s,",
		          strTemp1.c_str(),
		          strTemp2.c_str() );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,",
		          structBatter.m_AB,
		          structBatter.m_Runs,
		          structBatter.m_Hits,
		          structBatter.m_RBI,
		          structBatter.m_2B,
		          structBatter.m_3B,
		          structBatter.m_HomeRuns,
		          structBatter.m_Walk,
		          structBatter.m_StrikeOut,
		          structBatter.m_ReachedOnError );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%i,%i,%i,%c,%c,%i,%c,%c,%i,%i,",
		          structBatter.m_Sacrifice,
		          structBatter.m_StolenBase,
		          structBatter.m_CS,
		          Bunting[structBatter.m_bBunting],
		          Stealing[structBatter.m_bStealing],
		          Running[structBatter.m_bRunning],
		          HitRun[structBatter.m_bHitRun],
		          BatterHits[structBatter.m_bBatterHits],
		          structBatter.m_Pitcher,
		          structBatter.m_Catcher );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%i,%i,%i,%i,%i,%i,%i,%s,%s,%s,",
		          structBatter.m_FirstBase,
		          structBatter.m_SecondBase,
		          structBatter.m_ShortStop,
		          structBatter.m_ThirdBase,
		          structBatter.m_LeftField,
		          structBatter.m_CenterField,
		          structBatter.m_RightField,
		          structBatter.m_OBChanceBasic.c_str(),
		          structBatter.m_OBChanceLeft.c_str(),
		          structBatter.m_OBChanceRight.c_str() );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%1.3f,%1.3f,%1.3f,",
		          fBattingAverage,
		          fSLG,
		          fOnBasePercentage );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%i,%i,",
		          structBatter.m_Games,
		          structBatter.m_HBP );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%s,%s,%s,%s,%s,%s,",
		          structBatter.m_OBChanceSingle.c_str(),
		          structBatter.m_OBChanceDouble.c_str(),
		          structBatter.m_OBChanceTriple.c_str(),
		          structBatter.m_OBChanceHomeRun.c_str(),
		          structBatter.m_OBChanceWalk.c_str(),
		          structBatter.m_ChanceDoublePlay.c_str() );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%s,%s,%s,%s,%s,%s,",
		          structBatter.m_OBChanceSingleLeft.c_str(),
		          structBatter.m_OBChanceDoubleLeft.c_str(),
		          structBatter.m_OBChanceTripleLeft.c_str(),
		          structBatter.m_OBChanceHomeRunLeft.c_str(),
		          structBatter.m_OBChanceWalkLeft.c_str(),
		          structBatter.m_ChanceDoublePlayLeft.c_str() );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%s,%s,%s,%s,%s,%s,",
		          structBatter.m_OBChanceSingleRight.c_str(),
		          structBatter.m_OBChanceDoubleRight.c_str(),
		          structBatter.m_OBChanceTripleRight.c_str(),
		          structBatter.m_OBChanceHomeRunRight.c_str(),
		          structBatter.m_OBChanceWalkRight.c_str(),
		          structBatter.m_ChanceDoublePlayRight.c_str() );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%i,%i,%i,%i,%i,%i,%i,%i,%i,",
		          structBatter.m_bER1,
		          structBatter.m_bER2,
		          structBatter.m_bER3,
		          structBatter.m_bER4,
		          structBatter.m_bER5,
		          structBatter.m_bER6,
		          structBatter.m_bER7,
		          structBatter.m_bER8,
		          structBatter.m_bER9 );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%i,%i,%c,%c,%i,%i\n",
		          TRate[structBatter.m_bTRate],
		          Pass[structBatter.m_bPass],
		          Power[structBatter.m_bPowerL],
		          Power[structBatter.m_bPowerR],
		          OutArm[structBatter.m_bOutArm],
		          CatchArm[structBatter.m_bCatchArm] );
		strexportData = exportData;
		exportBatter.Write ( strexportData,strexportData.Len() );
	}
	exportBatter.Close();

	// Process Pitcher file
	strTemp = "XP" + strTeamName.Left ( 20 );
	exportFileName = strDir+"/" + strTemp+".txt"; // dir/XP000001.txt
	myFileName = strDir+"/tp"+strTeamName.Right ( 10 );
	exportPitcher.Create ( exportFileName,TRUE );
	sprintf ( exportData,"Team Name,Pitcher Name,,IP,ER,Hits,Walks,Strikeouts," );
	strexportData = exportData;
	exportPitcher.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"Homeruns,Hold,Wins,Loss,Saves,Starter,Relief,Throws," );
	strexportData = exportData;
	exportPitcher.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"ChB,ChL,ChR," );
	strexportData = exportData;
	exportPitcher.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"ERA,TRG," );
	strexportData = exportData;
	exportPitcher.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"Starts," );
	strexportData = exportData;
	exportPitcher.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"Games,Completed Games," );
	strexportData = exportData;
	exportPitcher.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"Ch1B,Ch2B,Ch3B,ChHR,ChW,ChDP," );
	strexportData = exportData;
	exportPitcher.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"Ch1BL,Ch2BL,Ch3BL,ChHRL,ChWL,ChDPL," );
	strexportData = exportData;
	exportPitcher.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"Ch1BR,Ch2BR,Ch3BR,ChHRR,ChWR,ChDPR," );
	strexportData = exportData;
	exportPitcher.Write ( strexportData,strexportData.Len() );
	sprintf ( exportData,"Wild Pitch,Balk,Fielding,ErrorRating,Bunting\n" );
	strexportData = exportData;
	exportPitcher.Write ( strexportData,strexportData.Len() );

	// A Team was selected so export all of the players
	count = structPitcher.GetCountPitcher ( myFileName );
	for ( i=0; i<count; i++ )
	{
		structPitcher.GetPitcher ( myFileName, i );
		strexportData.Empty();

		fIP = atof ( structPitcher.m_IP );

		if ( fIP == 0 )
		{
			fERA = 0;
			fTRG = 0;
		}
		else
		{
			fERA = ( double ) ( structPitcher.m_ER*9 ) /fIP;
			fTRG = ( double ) ( ( structPitcher.m_Hits+structPitcher.m_Walks ) *9 ) /fIP;
		}

		strTemp1 = strTeamName.Left ( 40 );
		strTemp2 = structPitcher.m_PitcherName.Left ( 30 );
		sprintf ( exportData,"%s,%s,",
		          strTemp1.c_str(),
		          strTemp2.c_str() );
		strexportData = exportData;
		exportPitcher.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%1.2f,%i,%i,%i,%i,%i,%i,%i,%i,",
		          fIP,
		          structPitcher.m_ER,
		          structPitcher.m_Hits,
		          structPitcher.m_Walks,
		          structPitcher.m_Strikeouts,
		          structPitcher.m_HomeRuns,
		          PHold[structPitcher.m_Hold],
		          structPitcher.m_Wins,
		          structPitcher.m_Loss );
		strexportData = exportData;
		exportPitcher.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%i,%i,%i,%c,%s,%s,%s,",
		          structPitcher.m_Saves,
		          structPitcher.m_Starter,
		          structPitcher.m_Relief,
		          BatterHits[structPitcher.m_Throws],
		          structPitcher.m_OBChanceBasic.c_str(),
		          structPitcher.m_OBChanceLeft.c_str(),
		          structPitcher.m_OBChanceRight.c_str() );
		strexportData = exportData;
		exportPitcher.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%1.2f,%1.2f,",
		          fERA,
		          fTRG );
		strexportData = exportData;
		exportPitcher.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%i,",
		          structPitcher.m_Starts );
		strexportData = exportData;
		exportPitcher.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%i,%i,",
		          structPitcher.m_Games,structPitcher.m_CompGames );
		strexportData = exportData;
		exportPitcher.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%s,%s,%s,%s,%s,%s,",
		          structPitcher.m_OBChanceSingle.c_str(),
		          structPitcher.m_OBChanceDouble.c_str(),
		          structPitcher.m_OBChanceTriple.c_str(),
		          structPitcher.m_OBChanceHomeRun.c_str(),
		          structPitcher.m_OBChanceWalk.c_str(),
		          structPitcher.m_ChanceDoublePlay.c_str() );
		strexportData = exportData;
		exportPitcher.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%s,%s,%s,%s,%s,%s,",
		          structPitcher.m_OBChanceSingleLeft.c_str(),
		          structPitcher.m_OBChanceDoubleLeft.c_str(),
		          structPitcher.m_OBChanceTripleLeft.c_str(),
		          structPitcher.m_OBChanceHomeRunLeft.c_str(),
		          structPitcher.m_OBChanceWalkLeft.c_str(),
		          structPitcher.m_ChanceDoublePlayLeft.c_str() );
		strexportData = exportData;
		exportPitcher.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%s,%s,%s,%s,%s,%s,",
		          structPitcher.m_OBChanceSingleRight.c_str(),
		          structPitcher.m_OBChanceDoubleRight.c_str(),
		          structPitcher.m_OBChanceTripleRight.c_str(),
		          structPitcher.m_OBChanceHomeRunRight.c_str(),
		          structPitcher.m_OBChanceWalkRight.c_str(),
		          structPitcher.m_ChanceDoublePlayRight.c_str() );
		strexportData = exportData;
		exportPitcher.Write ( strexportData,strexportData.Len() );
		sprintf ( exportData,"%i,%i,%i,%i,%c\n",
		          structPitcher.m_bWP,
		          structPitcher.m_bBalk,
		          structPitcher.m_PitcherField,
		          structPitcher.m_bER1,
		          Bunting[structPitcher.m_Bunting] );
		strexportData = exportData;
		exportPitcher.Write ( strexportData,strexportData.Len() );
	}
	exportPitcher.Close();
}

void MyMainWindow::OnExportHTML ( wxCommandEvent& event )
{
	wxString strLeagueDirName;
	LeagueStruct my_LeagueStruct;

	strLeagueDirName = my_LeagueStruct.GetLeagueDir ( FALSE );
	if ( strLeagueDirName.IsEmpty() )
	{
		wxMessageBox ( _T ( "No Leagues were found" ),
		               _T ( "League" ), wxOK|wxICON_INFORMATION );
	}
	else
	{
		GetTeamHTML ( strLeagueDirName );
	}
}

void MyMainWindow::OnExportHTMLAll ( wxCommandEvent& event )
{
	wxArrayString arrayTeamNames;
	wxArrayString arrayFileNames;
	wxFile myInFile;
	wxByte count;
	char temp[40];
	wxString strTemp;
	wxString strTeamName;
	int i;

	wxString strLeague;
	wxString strLeagueName;
	wxString strLeagueFile;
	wxString strLeagueDir;
	wxFileName myTempFileName;
	LeagueStruct my_LeagueStruct;

	strLeagueDir = my_LeagueStruct.GetLeagueDir ( FALSE );

	arrayTeamNames.Clear();
	if ( wxDirExists ( strLeagueDir ) )
	{
		// We only search for /tb*.dat files because in the exportfile
		// routine we create the TB and TP file names for processing.
		myTempFileName = wxFindFirstFile ( strLeagueDir+"/tb*.dat",wxFILE );
		if ( myTempFileName.FileExists() )
		{
			while ( myTempFileName.FileExists() )
			{
				arrayFileNames.Add ( myTempFileName.GetFullName() );
				myTempFileName = wxFindNextFile();
			}

			// Since the FindNextFile does not return the files in any order
			// we must sort the file names
			arrayFileNames.Sort();

			for ( i=0; i< ( int ) arrayFileNames.GetCount(); i++ )
			{
				myInFile.Open ( strLeagueDir+"/"+arrayFileNames[i],wxFile::read );
				myInFile.Read ( &count,sizeof ( count ) );
				// Get the Team name
				myInFile.Read ( temp,40 );
				myInFile.Close();
				strTeamName.Printf ( "%40s",temp );
				arrayTeamNames.Add ( strTeamName.Mid ( 0,40 ) +arrayFileNames[i] );
			}
		}
		else
		{
			// No Leagues found so display message
			wxMessageBox ( _T ( "Could not find any Team files. Please create a Team" ),
			               _T ( "Team" ),
			               wxOK|wxICON_ERROR );
		}
	}
	else
	{
		// data directory not found so display message
		wxMessageBox ( _T ( "Directory \""+strLeagueDir+"\" could not be found" ), _T ( "League" ), wxOK );
	}

	for ( i=0; i< ( int ) arrayFileNames.GetCount(); i++ )
	{
		ExportHTMLFile ( strLeagueDir, arrayTeamNames[i] );
	}
}

void MyMainWindow::GetTeamHTML ( wxString strLeagueDir )
{
	wxString strTeamFileName;
	TeamStruct my_TeamStruct;

	strTeamFileName = my_TeamStruct.GetTeamName ( strLeagueDir );

	if ( strTeamFileName.IsEmpty() )
	{
		wxMessageBox ( _T ( "No Teams were found" ),
		               _T ( "Teams" ), wxOK|wxICON_INFORMATION );
	}
	else
	{
		ExportHTMLFile ( strLeagueDir, strTeamFileName );
	}
}

void MyMainWindow::ExportHTMLFile ( wxString strDir, wxString strTeamName )
{
	wxByte count;
	char HTMLData[200];
	wxString HTMLFileName;
	wxString strHTMLData;
	wxString myFileName;
	wxString strTemp;
	wxString strTemp1;
	wxString strHTML;
	wxString strTempHTML;
	wxFile HTMLPlayer;
	BatterStruct structBatter;
	PitcherStruct structPitcher;
	int i,j;
	float fBattingAverage, fSLG, fOnBasePercentage;
	double fIP;
	double fERA;
	double fTRG;
	int *aiCount,tiCount;
	int *aiAB,tiAB;
	float *afBA,tfBA;
	double *adERA,tdERA;
	double *adIP,tdIP;

	struct tm *newTime;
	time_t szClock;

	wxString strIndex;
	wxString strBackgroundPicture;
	wxString strLinkColor;
	wxString strVLinkColor;
	wxString strBGColor;
	wxString strTextColor;

//	strIndex = "index.htm";
//	strBackgroundPicture = "images/background.jpg";
//	strLinkColor = "Blue";
//	strVLinkColor = "Purple";
//	strBGColor = "White";
//	strTextColor = "Black";
	strIndex = wxGetApp().g_HTML_Index;
	strBackgroundPicture = wxGetApp().g_HTML_BG;
	strLinkColor = wxGetApp().g_HTML_LinkColor;
	strVLinkColor = wxGetApp().g_HTML_VLinkColor;
	strBGColor = wxGetApp().g_HTML_BGColor;
	strTextColor = wxGetApp().g_HTML_TextColor;

	// Process Batter file
	strTemp = "XB" + strTeamName.Left ( 20 );
	HTMLFileName = strDir+"/" + strTemp+".htm"; // dir/xb000001.txt
	myFileName = strDir+"/tb"+strTeamName.Right ( 10 );
	HTMLPlayer.Create ( HTMLFileName,TRUE );
	strTemp1 = strTeamName.Left ( 40 );
	sprintf ( HTMLData,"<HTML>\n<HEAD>\n<TITLE>%s</TITLE>\n</HEAD>\n",
	          strTemp1.c_str() );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<BODY TEXT=%s LINK=%s VLINK=%s BGCOLOR=%s BACKGROUND=\"%s\">\n",
	          strTextColor.c_str(),strLinkColor.c_str(),strVLinkColor.c_str(),
	          strBGColor.c_str(),strBackgroundPicture.c_str() );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	strTemp1 = strTeamName.Left ( 40 );
	sprintf ( HTMLData,"<H2>%s</H2>\n",strTemp1.c_str() );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<A href=\"%s\">Back</A><BR><BR>\n",strIndex.c_str() );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TABLE BORDER>\n<TR>\n" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = LEFT><B>Batter</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>AB</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>R</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>H</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>RBI</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>2B</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>3B</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>HR</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>W</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>K</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>RE</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>CS</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>SB</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>AVG</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>SLG</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>OB%%</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"</TR>\n" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	// Insert table below here

	// A Team was selected so export all of the players
	count = structBatter.GetCountBatter ( myFileName );

	afBA = ( float * ) calloc ( count, sizeof ( float ) );
	memset ( afBA, '\0', count*sizeof ( float ) );
	aiAB = ( int * ) calloc ( count, sizeof ( int ) );
	memset ( aiAB, '\0', count*sizeof ( int ) );
	aiCount = ( int * ) calloc ( count, sizeof ( int ) );
	memset ( aiCount, '\0', count*sizeof ( int ) );
	// Fill arrays for count, AB and BA
	for ( i=0; i<count; i++ )
	{
		structBatter.GetBatter ( myFileName, i );
		strHTMLData.Empty();
		aiCount[i] = i;

		aiAB[i] = structBatter.m_AB;
		if ( aiAB[i] == 0 )
		{
			afBA[i] = 0.0f;
		}
		else
		{
			afBA[i] = ( float ) structBatter.m_Hits/aiAB[i];
		}
	}
	// Sort at bats then batting average
	for ( i=0; i<count; i++ )
	{
		for ( j=0; j<count-1; j++ )
		{
			if ( aiAB[j] < aiAB[j+1] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
				tiAB = aiAB[j];
				aiAB[j] = aiAB[j+1];
				aiAB[j+1] = tiAB;
				tfBA = afBA[j];
				afBA[j] = afBA[j+1];
				afBA[j+1] = tfBA;
			}
		}
	}
	for ( i=0; i<count; i++ )
	{
		for ( j=0; j<count-1; j++ )
		{
			if ( afBA[j] < afBA[j+1] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
				tiAB = aiAB[j];
				aiAB[j] = aiAB[j+1];
				aiAB[j+1] = tiAB;
				tfBA = afBA[j];
				afBA[j] = afBA[j+1];
				afBA[j+1] = tfBA;
			}
		}
	}

	for ( i=0; i<count; i++ )
	{
		structBatter.GetBatter ( myFileName, aiCount[i] );
		strHTMLData.Empty();
		if ( structBatter.m_AB == 0 )
		{
			fBattingAverage = 0.0f;
			fSLG = 0.0f;
			fOnBasePercentage = 0.0f;
		}
		else
		{
			fBattingAverage = ( float ) structBatter.m_Hits / structBatter.m_AB;

			fSLG = ( float )
			       ( ( structBatter.m_Hits - ( structBatter.m_2B + structBatter.m_3B
			                                   + structBatter.m_HomeRuns ) )
			         + ( 2*structBatter.m_2B ) + ( 3*structBatter.m_3B )
			         + ( 4*structBatter.m_HomeRuns )
			       ) / ( structBatter.m_AB );

			fOnBasePercentage =
			    ( float ) ( structBatter.m_Hits + structBatter.m_Walk
			                + structBatter.m_ReachedOnError + structBatter.m_Sacrifice
			                + structBatter.m_StolenBase ) / ( structBatter.m_AB
			                                                  + structBatter.m_Walk + structBatter.m_ReachedOnError
			                                                  + structBatter.m_Sacrifice + structBatter.m_StolenBase );
		}

		sprintf ( HTMLData,"<TR>" );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		strTemp1 = structBatter.m_PlayerName.Left ( 30 );
		sprintf ( HTMLData,"<TD>%s</TD>",
		          strTemp1.c_str() );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structBatter.m_AB );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structBatter.m_Runs );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structBatter.m_Hits );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structBatter.m_RBI );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structBatter.m_2B );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structBatter.m_3B );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structBatter.m_HomeRuns );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structBatter.m_Walk );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structBatter.m_StrikeOut );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structBatter.m_ReachedOnError );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD><TD>%i</TD>",
		          structBatter.m_CS,
		          structBatter.m_StolenBase );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%1.3f</TD>",
		          fBattingAverage );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%1.3f</TD>",
		          fSLG );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%1.3f</TD>",
		          fOnBasePercentage );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"</TR>\n" );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	}
	sprintf ( HTMLData,"</TABLE>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<BR><BR><TABLE BORDER>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	free ( afBA );
	free ( aiAB );
	free ( aiCount );

	// Process Pitcher file
	strTemp = "XP" + strTeamName.Left ( 20 );
	myFileName = strDir+"/tp"+strTeamName.Right ( 10 );
	sprintf ( HTMLData,"<TR>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TR><TD ALIGN = LEFT><B>Pitcher Name</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>IP</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>ER</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>H</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>W</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>K</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>HR</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>Start</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>Win</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>Loss</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>Saves</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>ERA</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TD ALIGN = CENTER><B>TRG</B></TD>" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"</TR>\n" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );

	// A Team was selected so export a players
	count = structPitcher.GetCountPitcher ( myFileName );

	adERA = ( double * ) calloc ( count, sizeof ( double ) );
	memset ( adERA, '\0', count*sizeof ( double ) );
	adIP = ( double * ) calloc ( count, sizeof ( double ) );
	memset ( adIP, '\0', count*sizeof ( double ) );
	aiCount = ( int * ) calloc ( count, sizeof ( int ) );
	memset ( aiCount, '\0', count*sizeof ( int ) );
	// Fill arrays for count, era and ip
	for ( i=0; i<count; i++ )
	{
		structPitcher.GetPitcher ( myFileName, i );
		strHTMLData.Empty();
		aiCount[i] = i;
		adIP[i] = atof ( structPitcher.m_IP );
		if ( adIP[i] == 0 )
		{
			adERA[i] = 0;
		}
		else
		{
			adERA[i] = ( double ) ( structPitcher.m_ER*9 ) /adIP[i];
		}
	}
	// Sort innings pitched and ERA
	for ( i=0; i<count; i++ )
	{
		for ( j=0; j<count-1; j++ )
		{
			if ( adIP[j] > adIP[j+1] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
				tdERA = adERA[j];
				adERA[j] = adERA[j+1];
				adERA[j+1] = tdERA;
				tdIP = adIP[j];
				adIP[j] = adIP[j+1];
				adIP[j+1] = tdIP;
			}
		}
	}
	for ( i=0; i<count; i++ )
	{
		for ( j=0; j<count-1; j++ )
		{
			if ( adERA[j] > adERA[j+1] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
				tdERA = adERA[j];
				adERA[j] = adERA[j+1];
				adERA[j+1] = tdERA;
				tdIP = adIP[j];
				adIP[j] = adIP[j+1];
				adIP[j+1] = tdIP;
			}
		}
	}

	for ( i=0; i<count; i++ )
	{
		structPitcher.GetPitcher ( myFileName, aiCount[i] );
		strHTMLData.Empty();

		fIP = atof ( structPitcher.m_IP );

		if ( fIP == 0 )
		{
			fERA = 0;
			fTRG = 0;
		}
		else
		{
			fERA = ( double ) ( structPitcher.m_ER*9 ) /fIP;
			fTRG = ( double ) ( ( structPitcher.m_Hits+structPitcher.m_Walks ) *9 ) /fIP;
		}

		strTemp1 = structPitcher.m_PitcherName.Left ( 30 );
		sprintf ( HTMLData,"<TR><TD>%s</TD>",
		          strTemp1.c_str() );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%1.2f</TD>",
		          fIP );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structPitcher.m_ER );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structPitcher.m_Hits );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structPitcher.m_Walks );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structPitcher.m_Strikeouts );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structPitcher.m_HomeRuns );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structPitcher.m_Starts );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structPitcher.m_Wins );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structPitcher.m_Loss );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%i</TD>",
		          structPitcher.m_Saves );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%1.2f</TD>",
		          fERA );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"<TD>%1.2f</TD>",
		          fTRG );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
		sprintf ( HTMLData,"</TR>\n" );
		strHTMLData = HTMLData;
		HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	}
	// Insert table above here
	sprintf ( HTMLData,"</TABLE>\n" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );

// Get time in seconds
	time ( &szClock );

// Convert time to struct tm form
	newTime = localtime ( &szClock );

	sprintf ( HTMLData, "<BR>Last Updated on %s<BR>\n", asctime ( newTime ) );

	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	// End of HTML file
	sprintf ( HTMLData,"</BODY>\n</HTML>\n" );
	strHTMLData = HTMLData;
	HTMLPlayer.Write ( strHTMLData,strHTMLData.Len() );
	HTMLPlayer.Close();
	free ( adERA );
	free ( adIP );
	free ( aiCount );
}

void MyMainWindow::OnStatisticsHTMLLeagueStats ( wxCommandEvent& event )
{
	wxString strLeague;
	wxString strLeagueName;
	wxString strLeagueFile;
	wxString strLeagueDir;

	strLeague = GetLeagues ( FALSE );

	if ( !strLeague.IsEmpty() )
	{
		strLeagueName = strLeague.Left ( 30 );
		if ( strncmp ( strLeagueName,"All",3 ) )
		{
			strLeagueFile = strLeague.Right ( 12 );
			strLeagueDir = strLeagueFile.Left ( 8 );
			HTMLLeagueStats ( strLeague );
		}
		else
		{
			// This is the base directory
			wxMessageBox ( _T ( "The Base directory can not be selected. Please Select a League Directory" ),
			               _T ( "Leagues" ), wxOK|wxICON_INFORMATION );
		}
	}
	else
	{
		wxMessageBox ( _T ( "No League was selected" ),
		               _T ( "Leagues" ), wxOK|wxICON_INFORMATION );
	}
}

void MyMainWindow::HTMLLeagueStats ( wxString strLeague )
{
	// strLeague contains
	// Left 30 League name
	// Right 12 File name L0000001.dat
	wxString strLeagueDir;
	wxString strLeagueName;
	wxString strLeagueFile;
	wxString strTeamFileName;
	wxString strDivisionName;
	wxString strConferenceName;
	wxString strTemp;
	wxArrayString* arrayFileNames = new wxArrayString;
	wxFile LeagueFile;
	wxByte version;
	wxByte NumDiv;
	wxByte NumConf;
	wxByte NumTeams;
	short HomeWin,HomeLoss,AwayWin,AwayLoss;
	char temp[41];
	int iDiv,iConf,iTeam;

	strLeagueName = strLeague.Left ( 30 );
	strLeagueFile = strLeague.Right ( 12 );
	strLeagueDir = strLeagueFile.Left ( 8 );
	LeagueFile.Open ( wxGetApp().g_DataDir+"/"+strLeagueFile,wxFile::read );
	LeagueFile.Read ( &version,sizeof ( version ) );
	LeagueFile.Read ( &NumConf,sizeof ( NumConf ) );
	LeagueFile.Read ( temp,30 );		// League Name
	temp[30] = 0x00;
	strLeagueName = temp;

	for ( iConf=0; iConf<NumConf; iConf++ )
	{
		LeagueFile.Read ( &NumDiv,sizeof ( NumDiv ) );
		LeagueFile.Read ( temp,30 );		// Conference Name
		temp[30] = 0x00;
		strConferenceName = temp;
		for ( iDiv=0; iDiv<NumDiv; iDiv++ )
		{
			LeagueFile.Read ( &NumTeams,sizeof ( NumTeams ) );
			LeagueFile.Read ( temp,30 );		// Division Name
			temp[30] = 0x00;
			strDivisionName = temp;
			arrayFileNames->Clear();
			for ( iTeam=0; iTeam<NumTeams; iTeam++ )
			{
				LeagueFile.Read ( temp,40 );		// Team Name
				LeagueFile.Read ( temp,8 );		// Team File
				temp[8] = 0x00;
				strTemp = temp;
				strTeamFileName = strLeagueDir + "/" + strTemp + ".dat";
				arrayFileNames->Add ( strTeamFileName );
				LeagueFile.Read ( temp,3 );		// Short Team Name Name
				LeagueFile.Read ( temp,20 );		// Ballpark
				LeagueFile.Read ( &HomeWin,sizeof ( HomeWin ) );		// Home Wins
				LeagueFile.Read ( &HomeLoss,sizeof ( HomeLoss ) );	// Home Loss
				LeagueFile.Read ( &AwayWin,sizeof ( AwayWin ) );		// Away Wins
				LeagueFile.Read ( &AwayLoss,sizeof ( AwayLoss ) );	// Away Loss
			}
			BuildPlayerArray ( arrayFileNames,strLeagueName,strDivisionName,strConferenceName,iDiv );
		}
	}
	LeagueFile.Close();
	delete arrayFileNames;
}

void MyMainWindow::BuildPlayerArray ( wxArrayString* arrayFileNames, wxString strLeagueName, wxString strDivisionName, wxString strConferenceName, int iDiv )
{
	// An array of the file names is passed
	// L0000002\TB000001.dat
	int arrayFileNamesSize;
	int iarraySize;
	wxFile TeamFile;
	wxFile PitcherFile;
	wxFile HTMLFile;
	wxByte count;
	wxArrayString cstrPlayerName;
	wxArrayString cstrPitcherName;
	wxArrayString cstrShortTeamName;
	wxArrayString cstrShortTeamNameP;
	wxArrayString cstrTeamData;
	wxArrayString cstrTeamPData;
	wxArrayString cstr1,cstr2,cstr3;
	wxArrayString cstrTemp1,cstrTemp2,cstrTemp3;
	wxString strBatterFileName;
	wxString strPitcherFileName;
	wxString strShortTeamName;
	wxString strTeamName;
	wxString strTemp;
	wxString strTemp1;
	wxString strTemp2;
	wxString strTemp3;
	wxString strTeamData;
	wxString strAVG,strSLG,strOBP;
	wxString strERA,strTRG,strIP;
	wxString str20Blank ( "                    " );
	char temp[41];
	int commafind;
	int *aiCount,tiCount,*aiPCount,*aiTCount;
	int *aiAB,*aiRuns,*aiHits,*aiRBI,*ai2B,*ai3B,*aiHR,*aiStolenBase,*aiCS;
	int *aiStrikeOut,*aiWalk;
	int iAB,iRuns,iHits,iRBI,i2B,i3B,iHR,iStolenBase,iCS,iWalk,iROE,iSacrifice;
	int iTAB,iTRuns,iTHits,iTRBI,iT2B,iT3B,iTHR,iTStolenBase,iTCS,iTWalk,iTROE,iTSacrifice;
	int iPER,iPHits,iPWalks,iPK,iPWins,iPLoss,iPStarts,iPSaves,iPHR;
	int iTPER,iTPHits,iTPWalks,iTPK,iTPWins,iTPLoss,iTPStarts,iTPSaves,iTPHR;
	int *aiPER,*aiPHits,*aiPWalks,*aiPK,*aiPWins,*aiPLoss,*aiPStarts,*aiPSaves,*aiPHR;
	float *afBA,*afSLG,*afOBP,*afTBA;
	float fBA,fSLG,fOBP;
	float fTBA,fTSLG,fTOBP;
	float *adERA,*adIP,*adTRG,*adTERA;
	double dERA,dIP,dTRG,dTemp;
	double dTERA,dTIP,dTTRG;
	int totCount = 0;
	int totPCount = 0;
	int i,ii,iii,j;
	int getSize1,getSize2,getSize3,getSizeMax;
	BatterStruct structBatter;
	PitcherStruct structPitcher;
	char HTMLData[200];
	wxString strHTMLData;
	wxString strIndex;
	wxString strBackgroundPicture;
	wxString strLinkColor;
	wxString strVLinkColor;
	wxString strBGColor;
	wxString strTextColor;
	wxString strDir;

	struct tm *newTime;
	time_t szClock;

	strDir = arrayFileNames->Item ( 0 ).Left ( 8 );
//	Registry myReg(strDir + REGLEAGUE);

//	strLinkColor	= myReg.GetRegistryOptionLinkColor();
//	strVLinkColor	= myReg.GetRegistryOptionVLinkColor();
//	strBGColor		= myReg.GetRegistryOptionBGColor();
//	strTextColor	= myReg.GetRegistryOptionTextColor();
//	strIndex		= myReg.GetRegistryOptionIndex();
//	strBackgroundPicture = myReg.GetRegistryOptionBackgroundPicture();
//	strIndex = "index.htm";
//	strBackgroundPicture = "images/background.jpg";
//	strLinkColor = "Blue";
//	strVLinkColor = "Purple";
//	strBGColor = "White";
//	strTextColor = "Black";
	strIndex = wxGetApp().g_HTML_Index;
	strBackgroundPicture = wxGetApp().g_HTML_BG;
	strLinkColor = wxGetApp().g_HTML_LinkColor;
	strVLinkColor = wxGetApp().g_HTML_VLinkColor;
	strBGColor = wxGetApp().g_HTML_BGColor;
	strTextColor = wxGetApp().g_HTML_TextColor;

	int m_StatCount = HTML_DISP_LINES;	// Counter for number of lines to be displayed

	arrayFileNamesSize = arrayFileNames->GetCount();
	for ( iarraySize=0; iarraySize < arrayFileNamesSize; iarraySize++ )
	{
		strBatterFileName = arrayFileNames->Item ( iarraySize );
		strBatterFileName.SetChar ( 10,'b' );	// Lowercase for unix
		strBatterFileName.SetChar ( 9,'t' );
		count = structBatter.GetCountBatter ( strBatterFileName );
		totCount += count;
		strPitcherFileName = arrayFileNames->Item ( iarraySize );
		strPitcherFileName.SetChar ( 10,'p' );	// Lowercase for unix
		strPitcherFileName.SetChar ( 9,'t' );
		count = structPitcher.GetCountPitcher ( strPitcherFileName );
		totPCount += count;
	}
	cstrPlayerName.Clear();
	aiCount = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( aiCount, '\0', totCount*sizeof ( int ) );
	aiPCount = ( int * ) calloc ( totPCount, sizeof ( int ) );
	memset ( aiPCount, '\0', totPCount*sizeof ( int ) );
	aiTCount = ( int * ) calloc ( arrayFileNamesSize, sizeof ( int ) );
	memset ( aiTCount, '\0', arrayFileNamesSize*sizeof ( int ) );
	aiAB = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( aiAB, '\0', totCount*sizeof ( int ) );
	aiRuns = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( aiRuns, '\0', totCount*sizeof ( int ) );
	aiHits = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( aiHits, '\0', totCount*sizeof ( int ) );
	aiRBI = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( aiRBI, '\0', totCount*sizeof ( int ) );
	ai2B = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( ai2B, '\0', totCount*sizeof ( int ) );
	ai3B = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( ai3B, '\0', totCount*sizeof ( int ) );
	aiHR = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( aiHR, '\0', totCount*sizeof ( int ) );
	afBA = ( float * ) calloc ( totCount, sizeof ( float ) );
	memset ( afBA, '\0', totCount*sizeof ( float ) );
	afTBA = ( float * ) calloc ( totCount, sizeof ( float ) );
	memset ( afTBA, '\0', totCount*sizeof ( float ) );
	afSLG = ( float * ) calloc ( totCount, sizeof ( float ) );
	memset ( afSLG, '\0', totCount*sizeof ( float ) );
	afOBP = ( float * ) calloc ( totCount, sizeof ( float ) );
	memset ( afOBP, '\0', totCount*sizeof ( float ) );
	aiStolenBase = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( aiStolenBase, '\0', totCount*sizeof ( int ) );
	aiCS = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( aiCS, '\0', totCount*sizeof ( int ) );
	aiStrikeOut = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( aiStrikeOut, '\0', totCount*sizeof ( int ) );
	aiWalk = ( int * ) calloc ( totCount, sizeof ( int ) );
	memset ( aiWalk, '\0', totCount*sizeof ( int ) );
	adERA = ( float * ) calloc ( totPCount, sizeof ( float ) );
	memset ( adERA, '\0', totPCount*sizeof ( float ) );
	adTERA = ( float * ) calloc ( totPCount, sizeof ( float ) );
	memset ( adTERA, '\0', totPCount*sizeof ( float ) );
	adTRG = ( float * ) calloc ( totPCount, sizeof ( float ) );
	memset ( adTRG, '\0', totPCount*sizeof ( float ) );
	adIP = ( float * ) calloc ( totPCount, sizeof ( float ) );
	memset ( adIP, '\0', totPCount*sizeof ( float ) );
	aiPER = ( int * ) calloc ( totPCount, sizeof ( int ) );
	memset ( aiPER, '\0', totPCount*sizeof ( int ) );
	aiPHits = ( int * ) calloc ( totPCount, sizeof ( int ) );
	memset ( aiPHits, '\0', totPCount*sizeof ( int ) );
	aiPWalks = ( int * ) calloc ( totPCount, sizeof ( int ) );
	memset ( aiPWalks, '\0', totPCount*sizeof ( int ) );
	aiPK = ( int * ) calloc ( totPCount, sizeof ( int ) );
	memset ( aiPK, '\0', totPCount*sizeof ( int ) );
	aiPWins = ( int * ) calloc ( totPCount, sizeof ( int ) );
	memset ( aiPWins, '\0', totPCount*sizeof ( int ) );
	aiPLoss = ( int * ) calloc ( totPCount, sizeof ( int ) );
	memset ( aiPLoss, '\0', totPCount*sizeof ( int ) );
	aiPStarts = ( int * ) calloc ( totPCount, sizeof ( int ) );
	memset ( aiPStarts, '\0', totPCount*sizeof ( int ) );
	aiPSaves = ( int * ) calloc ( totPCount, sizeof ( int ) );
	memset ( aiPSaves, '\0', totPCount*sizeof ( int ) );
	aiPHR = ( int * ) calloc ( totPCount, sizeof ( int ) );
	memset ( aiPHR, '\0', totPCount*sizeof ( int ) );

	// Fill arrays for count
	for ( i=0; i<totCount; i++ )
		aiCount[i] = i;
	for ( i=0; i<totPCount; i++ )
		aiPCount[i] = i;
	for ( i=0; i<arrayFileNamesSize; i++ )
		aiTCount[i] = i;
	iTAB=iTRuns=iTHits=iTRBI=iT2B=iT3B=iTHR=iTStolenBase=iTCS=iTWalk=iTROE=iTSacrifice=0;
	fTBA=fTOBP=fTSLG=0.0f;
	fBA=fSLG=fOBP=0.0f;
	iTPER=iTPHits=iTPWalks=iTPK=iTPWins=iTPLoss=iTPStarts=iTPSaves=iTPHR=0;
	dTIP=dTTRG=dTERA=0.0f;
	ii = 0;
	iii = 0;
	// Process all teams in division
	for ( iarraySize=0; iarraySize < arrayFileNamesSize; iarraySize++ )
	{
		// L0000002\TB000001.dat Player file
		// Set up Batter File
		strBatterFileName = arrayFileNames->Item ( iarraySize );
		strBatterFileName.SetChar ( 9,'t' );	// Lowercase for unix
		strBatterFileName.SetChar ( 10,'b' );
		// Set up Pitcher File
		strPitcherFileName = arrayFileNames->Item ( iarraySize );
		strPitcherFileName.SetChar ( 9,'t' );	// Lowercase for unix
		strPitcherFileName.SetChar ( 10,'p' );
		// Open Batter file
		TeamFile.Open ( strBatterFileName, wxFile::read );
		TeamFile.Read ( &count,sizeof ( count ) );
		TeamFile.Read ( temp,40 );		// Team Name
		temp[40] = 0x00;
		strTeamName = temp;
		TeamFile.Read ( temp,3 );		// Short Team Name
		temp[3] = 0x00;
		strShortTeamName = temp;
		TeamFile.Close();
		iAB=iRuns=iHits=iRBI=i2B=i3B=iHR=iStolenBase=iCS=iWalk=iROE=iSacrifice=0;
		iPER=iPHits=iPWalks=iPK=iPWins=iPLoss=iPStarts=iPSaves=iPHR=0;
		dIP=dTRG=dERA=0.0f;
		// Process all batters in file
		for ( i=0; i<count; i++ )
		{
			structBatter.GetBatter ( strBatterFileName, i );
			cstrShortTeamName.Add ( strShortTeamName );
			commafind = structBatter.m_PlayerName.Find ( ',' );
			strTemp.Empty();
			strTemp	= structBatter.m_PlayerName.GetChar ( commafind+2 );
			strTemp	+= ".";
			strTemp	+= structBatter.m_PlayerName.Left ( commafind );
			strTemp	+= str20Blank;
			strTemp	+= str20Blank;
			cstrPlayerName.Add ( strTemp );
			aiAB[ii]	= structBatter.m_AB;
			aiCS[ii]	= structBatter.m_CS;
			aiHits[ii]	= structBatter.m_Hits;
			ai2B[ii]	= structBatter.m_2B;
			ai3B[ii]	= structBatter.m_3B;
			aiHR[ii]	= structBatter.m_HomeRuns;
			aiRBI[ii]	= structBatter.m_RBI;
			aiRuns[ii]	= structBatter.m_Runs;
			aiStolenBase[ii] = structBatter.m_StolenBase;
			aiStrikeOut[ii]	= structBatter.m_StrikeOut;
			aiWalk[ii]	= structBatter.m_Walk;
			iAB			+= structBatter.m_AB;
			iCS			+= structBatter.m_CS;
			iHits		+= structBatter.m_Hits;
			i2B			+= structBatter.m_2B;
			i3B			+= structBatter.m_3B;
			iHR			+= structBatter.m_HomeRuns;
			iRBI		+= structBatter.m_RBI;
			iROE		+= structBatter.m_ReachedOnError;
			iRuns		+= structBatter.m_Runs;
			iSacrifice	+= structBatter.m_Sacrifice;
			iStolenBase	+= structBatter.m_StolenBase;
			iWalk		+= structBatter.m_Walk;
			if ( structBatter.m_AB == 0 )
			{
				afBA[ii]	= 0.0f;
				afSLG[ii]	= 0.0f;
				afOBP[ii]	= 0.0f;
			}
			else
			{
				afBA[ii]	= ( float ) structBatter.m_Hits/structBatter.m_AB;
				afSLG[ii]	= ( float ) ( ( structBatter.m_Hits- ( structBatter.m_2B+structBatter.m_3B+structBatter.m_HomeRuns ) ) + ( 2*structBatter.m_2B ) + ( 3*structBatter.m_3B ) + ( 4*structBatter.m_HomeRuns ) ) / ( structBatter.m_AB );
				afOBP[ii]	= ( float ) ( structBatter.m_Hits+structBatter.m_Walk+structBatter.m_ReachedOnError+structBatter.m_Sacrifice+structBatter.m_StolenBase ) / ( structBatter.m_AB+structBatter.m_Walk+structBatter.m_ReachedOnError+structBatter.m_Sacrifice+structBatter.m_StolenBase );
				fBA		= ( float ) iHits/iAB;
				fSLG	= ( float ) ( ( iHits- ( i2B+i3B+iHR ) ) + ( 2*i2B ) + ( 3*i3B ) + ( 4*iHR ) ) / ( iAB );
				fOBP	= ( float ) ( iHits+iWalk+iROE+iSacrifice+iStolenBase ) / ( iAB+iWalk+iROE+iSacrifice+iStolenBase );
			}
			ii++;
		}	// End of batter processing

		// Open Pitcher file to get count
		count = structPitcher.GetCountPitcher ( strPitcherFileName );

		// Fill arrays for count, era and ip
		for ( i=0; i<count; i++ )
		{
			structPitcher.GetPitcher ( strPitcherFileName, i );
			strHTMLData.Empty();

			commafind = structPitcher.m_PitcherName.Find ( ',' );
			strTemp.Empty();
			strTemp = structPitcher.m_PitcherName.GetChar ( commafind+2 );
			strTemp += ".";
			strTemp += structPitcher.m_PitcherName.Left ( commafind );
			strTemp += str20Blank;
			strTemp += str20Blank;
			cstrPitcherName.Add ( strTemp );
			cstrShortTeamNameP.Add ( strShortTeamName );
			aiPER[iii]		= structPitcher.m_ER;
			aiPHits[iii]	= structPitcher.m_Hits;
			aiPHR[iii]		= structPitcher.m_HomeRuns;
			adIP[iii]		= ( float ) atof ( structPitcher.m_IP );
			aiPLoss[iii]	= structPitcher.m_Loss;
			aiPSaves[iii]	= structPitcher.m_Saves;
			aiPStarts[iii]	= structPitcher.m_Starts;
			aiPK[iii]		= structPitcher.m_Strikeouts;
			aiPWalks[iii]	= structPitcher.m_Walks;
			aiPWins[iii]	= structPitcher.m_Wins;
			iPER	+=	structPitcher.m_ER;
			iPHits	+=	structPitcher.m_Hits;
			iPHR	+=	structPitcher.m_HomeRuns;
			dTemp	=	atof ( structPitcher.m_IP );
			dIP		+=	dTemp;
			iPLoss	+=	structPitcher.m_Loss;
			iPSaves	+=	structPitcher.m_Saves;
			iPStarts +=	structPitcher.m_Starts;
			iPK		+=	structPitcher.m_Strikeouts;
			iPWalks	+=	structPitcher.m_Walks;
			iPWins	+=	structPitcher.m_Wins;

			if ( adIP[iii] == 0 )
			{
				adERA[iii]	= 0.0f;
				adTRG[iii]	= 0.0f;
				dERA	+= 0.0f;
				dTRG	+= 0.0f;
			}
			else
			{
				adERA[iii]	= ( float ) ( structPitcher.m_ER*9 ) /adIP[iii];
				adTRG[iii]	= ( float ) ( ( structPitcher.m_Hits+structPitcher.m_Walks ) *9 ) /adIP[iii];
				dERA	= ( double ) ( iPER*9 ) /dIP;
				dTRG	= ( double ) ( ( iPHits+iPWalks ) *9 ) /dIP;
			}
			iii++;
		}

		adTERA[iarraySize]	= ( float ) dERA;	// Team ERA for latter sorting
		afTBA[iarraySize]	= fBA;	// Team Batting Average for latter sorting
		sprintf ( HTMLData,"%1.3f",fBA );
		strTemp = HTMLData;
		strAVG  = strTemp.Right ( 4 );
		sprintf ( HTMLData,"%1.3f",fSLG );
		strTemp = HTMLData;
		strSLG  = strTemp.Right ( 4 );
		sprintf ( HTMLData,"%1.3f",fOBP );
		strTemp = HTMLData;
		strOBP  = strTemp.Right ( 4 );
		sprintf ( HTMLData,"%s %.15s %s %s %s %5i %4i %5i %4i %4i %4i %3i %4i %4i\n",
		          strShortTeamName.c_str(),strTeamName.c_str(),strAVG.c_str(),strSLG.c_str(),
		          strOBP.c_str(),iAB,iRuns,iHits,iRBI,
		          i2B,i3B,iHR,iStolenBase,iCS );
		strTeamData = HTMLData;
		cstrTeamData.Add ( strTeamData );
		sprintf ( HTMLData,"%5.2f",dERA );
		strTemp = HTMLData;
		strERA  = strTemp;
		sprintf ( HTMLData,"%4.1f",dTRG );
		strTemp = HTMLData;
		strTRG  = strTemp;
		sprintf ( HTMLData,"%6.1f",dIP );
		strTemp = HTMLData;
		strIP  = strTemp;
		sprintf ( HTMLData,"%s %.15s %5s %4s   %3i   %3i %6s %5i %4i %3i  %4i %4i\n",
		          strShortTeamName.c_str(),strTeamName.c_str(),strERA.c_str(),strTRG.c_str(),
		          iPWins,iPLoss,strIP.c_str(),iPHits,
		          iPER,iPHR,iPWalks,iPK );
		strTeamData = HTMLData;
		cstrTeamPData.Add ( strTeamData );

		iTAB		+= iAB;
		iTRuns		+= iRuns;
		iTHits		+= iHits;
		iTRBI		+= iRBI;
		iT2B		+= i2B;
		iT3B		+= i3B;
		iTHR		+= iHR;
		iTStolenBase += iStolenBase;
		iTCS		+= iCS;
		iTWalk		+= iWalk;
		iTSacrifice	+= iSacrifice;
		iTROE		+= iROE;
		if ( iTAB == 0 )
		{
			fTBA  = 0.0f;
			fTSLG = 0.0f;
			fTOBP = 0.0f;
		}
		else
		{
			fTBA  = ( float ) iTHits/iTAB;
			fTSLG = ( float ) ( ( iTHits- ( iT2B+iT3B+iTHR ) ) + ( 2*iT2B ) + ( 3*iT3B ) + ( 4*iTHR ) ) / ( iTAB );
			fTOBP = ( float ) ( iTHits+iTWalk+iTROE+iTSacrifice+iTStolenBase ) / ( iTAB+iTWalk+iTROE+iTSacrifice+iTStolenBase );
		}

		iTPER		+= iPER;
		iTPHits		+= iPHits;
		iTPHR		+= iPHR;
		dTIP		+= dIP;
		iTPLoss		+= iPLoss;
		iTPSaves	+= iPSaves;
		iTPStarts	+= iPStarts;
		iTPK		+= iPK;
		iTPWalks	+= iPWalks;
		iTPWins		+= iPWins;
		if ( dTIP == 0 )
		{
			dTERA = 0.0f;
			dTTRG = 0.0f;
		}
		else
		{
			dTERA = ( float ) ( iTPER*9 ) /dTIP;
			dTTRG = ( float ) ( ( iTPHits+iTPWalks ) *9 ) /dTIP;
		}
	}	// End processing of all teams in division
	// Open file and create headers
	strLeagueName.Trim();
	strConferenceName.Trim();
	strDivisionName.Trim();
	if ( !strcmp ( "NULL",strDivisionName.Left ( 4 ) ) )
	{
		if ( !strcmp ( "NULL",strConferenceName.Left ( 4 ) ) )
		{
			// Create file name only with League name
			HTMLFile.Create ( arrayFileNames->Item ( 0 ).Left ( 8 ) +"/"+strLeagueName+".htm", TRUE );
		}
		else
		{
			// Create file name with League name and Conference name
			HTMLFile.Create ( arrayFileNames->Item ( 0 ).Left ( 8 ) +"/"+strLeagueName+strConferenceName+".htm", TRUE );
		}
	}
	else
	{
		if ( !strcmp ( "NULL",strConferenceName.Left ( 4 ) ) )
		{
			// Create file name with League name and Division name
			HTMLFile.Create ( arrayFileNames->Item ( 0 ).Left ( 8 ) +"/"+strLeagueName+strDivisionName+".htm", TRUE );
		}
		else
		{
			// Create file name with League name, Conference name, and Division name
			HTMLFile.Create ( arrayFileNames->Item ( 0 ).Left ( 8 ) +"/"+strLeagueName+strConferenceName+strDivisionName+".htm", TRUE );
		}
	}
	sprintf ( HTMLData,"<HTML>\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<HEAD>\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<TITLE>%s</TITLE>\n", strLeagueName.c_str() );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"</HEAD>\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );

	sprintf ( HTMLData,"<BODY TEXT=%s LINK=%s VLINK=%s BGCOLOR=%s BACKGROUND=\"%s\">\n",
	          strTextColor.c_str(), strLinkColor.c_str(), strVLinkColor.c_str(),
	          strBGColor.c_str(),	strBackgroundPicture.c_str() );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<A href=\"%s\">Back</A><BR><BR>\n",strIndex.c_str() );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"<H2>League: %s</H2>\n", strLeagueName.c_str() );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	if ( strcmp ( "NULL",strConferenceName.Left ( 4 ) ) )
	{
		sprintf ( HTMLData,"Conference: <B>%s</B><BR>\n", strConferenceName.c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	if ( strcmp ( "NULL",strDivisionName.Left ( 4 ) ) )
	{
		sprintf ( HTMLData,"Division: <B>%s</B><BR>\n", strDivisionName.c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	sprintf ( HTMLData,"<PRE><B>\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"BATTING\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"    TEAM             AVG  SLG  OBP    AB    R     H  RBI   2B   3B  HR   SB   CS\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	for ( i=0; i<arrayFileNamesSize-1; i++ )
	{
		for ( j=0; j<arrayFileNamesSize-1; j++ )
		{
			if ( afTBA[aiTCount[j]] < afTBA[aiTCount[j+1]] )
			{
				tiCount = aiTCount[j];
				aiTCount[j] = aiTCount[j+1];
				aiTCount[j+1] = tiCount;
			}
		}
	}
	for ( iarraySize=0; iarraySize < arrayFileNamesSize; iarraySize++ )
	{
		strTemp = cstrTeamData.Item ( aiTCount[iarraySize] );
		HTMLFile.Write ( strTemp,strTemp.Len() );
	}
	sprintf ( HTMLData,"--- --------------- ---- ---- ---- ----- ---- ----- ---- ---- ---- --- ---- ----\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"%1.3f",fTBA );
	strTemp = HTMLData;
	strAVG  = strTemp.Right ( 4 );
	sprintf ( HTMLData,"%1.3f",fTSLG );
	strTemp = HTMLData;
	strSLG  = strTemp.Right ( 4 );
	sprintf ( HTMLData,"%1.3f",fTOBP );
	strTemp = HTMLData;
	strOBP  = strTemp.Right ( 4 );
	sprintf ( HTMLData,"LEAGUE TOTALS       %s %s %s %5i %4i %5i %4i %4i %4i %3i %4i %4i\n",
	          strAVG.c_str(),strSLG.c_str(),strOBP.c_str(),iTAB,iTRuns,iTHits,iTRBI,
	          iT2B,iT3B,iTHR,iTStolenBase,iTCS );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );

	sprintf ( HTMLData,"\nPITCHING\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"    TEAM             ERA   TRG  Wins  Loss     IP  Hits   ER  HR Walks    K\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	for ( i=0; i<arrayFileNamesSize-1; i++ )
	{
		for ( j=0; j<arrayFileNamesSize-1; j++ )
		{
			if ( adTERA[aiTCount[j]] > adTERA[aiTCount[j+1]] )
			{
				tiCount = aiTCount[j];
				aiTCount[j] = aiTCount[j+1];
				aiTCount[j+1] = tiCount;
			}
		}
	}
	for ( iarraySize=0; iarraySize < arrayFileNamesSize; iarraySize++ )
	{
		strTemp = cstrTeamPData.Item ( aiTCount[iarraySize] );
		HTMLFile.Write ( strTemp,strTemp.Len() );
	}
	sprintf ( HTMLData,"--- --------------- ----- ----   ---   --- ------ ----- ---- ---  ---- ----\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"%5.2f",dTERA );
	strTemp = HTMLData;
	strERA  = strTemp;
	sprintf ( HTMLData,"%4.1f",dTTRG );
	strTemp = HTMLData;
	strTRG  = strTemp.Right ( 4 );
	sprintf ( HTMLData,"%6.1f",dTIP );
	strTemp = HTMLData;
	strIP  = strTemp;
	sprintf ( HTMLData,"LEAGUE TOTALS       %5s %4s   %3i   %3i %6s %5i %4i %3i  %4i %4i\n\n",
	          strERA.c_str(),strTRG.c_str(),iTPWins,iTPLoss,strIP.c_str(),
	          iTPHits,iTPER,iTPHR,iTPWalks,iTPK );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );

	cstr1.Clear();
	cstr2.Clear();
	cstr3.Clear();
	// Sort at bats
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( aiAB[aiCount[j]] < aiAB[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiAB[aiCount[i]] == aiAB[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiAB[aiCount[i]] );
			cstr1.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s %5i", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),aiAB[aiCount[i]] );
			cstr1.Add ( HTMLData );
		}
	}
	// Sort runs
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( aiRuns[aiCount[j]] < aiRuns[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiRuns[aiCount[i]] == aiRuns[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiRuns[aiCount[i]] );
			cstr2.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),aiRuns[aiCount[i]] );
			cstr2.Add ( HTMLData );
		}
	}
	// Sort Hits
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( aiHits[aiCount[j]] < aiHits[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiHits[aiCount[i]] == aiHits[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiHits[aiCount[i]] );
			cstr3.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s %5i\n", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),aiHits[aiCount[i]] );
			cstr3.Add ( HTMLData );
		}
	}
//	                    00000000011111111112222222222333333333344444444445555555555666666666677777777778
//	                    12345678901234567890123456789012345678901234567890123456789012345678901234567890
	sprintf ( HTMLData,"\nAt Bats                    Runs                       Hits\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	getSize1 = cstr1.GetCount();
	getSize2 = cstr2.GetCount();
	getSize3 = cstr3.GetCount();
	getSizeMax = getSize1;
	if ( getSizeMax < getSize2 )
	{
		getSizeMax = getSize2;
	}
	if ( getSizeMax < getSize3 )
	{
		getSizeMax = getSize3;
	}
	if ( getSize1 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize1;i++ )
			cstr1.Add ( str20Blank+str20Blank );
	}
	if ( getSize2 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize2;i++ )
			cstr2.Add ( str20Blank+str20Blank );
	}
	if ( getSize3 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize3;i++ )
			cstr3.Add ( str20Blank+str20Blank );
	}
	for ( i=0; i < getSizeMax;i++ )
	{
		sprintf ( HTMLData,"%.26s %.26s %.26s\n",cstr1.Item ( i ).c_str(),
		          cstr2.Item ( i ).c_str(),cstr3.Item ( i ).c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	cstr1.Clear();
	cstr2.Clear();
	cstr3.Clear();

	// Sort RBI
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( aiRBI[aiCount[j]] < aiRBI[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiRBI[aiCount[i]] == aiRBI[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiRBI[aiCount[i]] );
			cstr1.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),aiRBI[aiCount[i]] );
			cstr1.Add ( HTMLData );
		}
	}
	// Sort Batting Averages
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( afBA[aiCount[j]] < afBA[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( afBA[aiCount[i]] == afBA[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH     %1.3f", afBA[aiCount[i]] );
			cstr2.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s %1.3f", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),afBA[aiCount[i]] );
			cstr2.Add ( HTMLData );
		}
	}
	// Sort Slugging Percentage
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( afSLG[aiCount[j]] < afSLG[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( afSLG[aiCount[i]] == afSLG[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH     %1.3f", afSLG[aiCount[i]] );
			cstr3.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s %1.3f", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),afSLG[aiCount[i]] );
			cstr3.Add ( HTMLData );
		}
	}
//	                    00000000011111111112222222222333333333344444444445555555555666666666677777777778
//	                    12345678901234567890123456789012345678901234567890123456789012345678901234567890
	sprintf ( HTMLData,"\nRBI's                      Batting Averages           Slugging Percentages\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	getSize1 = cstr1.GetCount();
	getSize2 = cstr2.GetCount();
	getSize3 = cstr3.GetCount();
	getSizeMax = getSize1;
	if ( getSizeMax < getSize2 )
	{
		getSizeMax = getSize2;
	}
	if ( getSizeMax < getSize3 )
	{
		getSizeMax = getSize3;
	}
	if ( getSize1 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize1;i++ )
			cstr1.Add ( str20Blank+str20Blank );
	}
	if ( getSize2 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize2;i++ )
			cstr2.Add ( str20Blank+str20Blank );
	}
	if ( getSize3 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize3;i++ )
			cstr3.Add ( str20Blank+str20Blank );
	}
	for ( i=0; i < getSizeMax;i++ )
	{
		sprintf ( HTMLData,"%.26s %.26s %.26s\n",cstr1.Item ( i ).c_str(),
		          cstr2.Item ( i ).c_str(),cstr3.Item ( i ).c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	cstr1.Clear();
	cstr2.Clear();
	cstr3.Clear();

	// Sort On Base Percentage
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( afOBP[aiCount[j]] < afOBP[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( afOBP[aiCount[i]] == afOBP[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH     %1.3f", afOBP[aiCount[i]] );
			cstr1.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s %1.3f", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),afOBP[aiCount[i]] );
			cstr1.Add ( HTMLData );
		}
	}
	// Sort Doubles
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( ai2B[aiCount[j]] < ai2B[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( ai2B[aiCount[i]] == ai2B[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", ai2B[aiCount[i]] );
			cstr2.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),ai2B[aiCount[i]] );
			cstr2.Add ( HTMLData );
		}
	}
	// Sort Triples
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( ai3B[aiCount[j]] < ai3B[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( ai3B[aiCount[i]] == ai3B[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", ai3B[aiCount[i]] );
			cstr3.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),ai3B[aiCount[i]] );
			cstr3.Add ( HTMLData );
		}
	}
//	                    00000000011111111112222222222333333333344444444445555555555666666666677777777778
//	                    12345678901234567890123456789012345678901234567890123456789012345678901234567890
	sprintf ( HTMLData,"\nOn Base Percentage         Doubles                    Triples\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	getSize1 = cstr1.GetCount();
	getSize2 = cstr2.GetCount();
	getSize3 = cstr3.GetCount();
	getSizeMax = getSize1;
	if ( getSizeMax < getSize2 )
	{
		getSizeMax = getSize2;
	}
	if ( getSizeMax < getSize3 )
	{
		getSizeMax = getSize3;
	}
	if ( getSize1 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize1;i++ )
			cstr1.Add ( str20Blank+str20Blank );
	}
	if ( getSize2 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize2;i++ )
			cstr2.Add ( str20Blank+str20Blank );
	}
	if ( getSize3 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize3;i++ )
			cstr3.Add ( str20Blank+str20Blank );
	}
	for ( i=0; i < getSizeMax;i++ )
	{
		sprintf ( HTMLData,"%.26s %.26s %.26s\n",cstr1.Item ( i ).c_str(),
		          cstr2.Item ( i ).c_str(),cstr3.Item ( i ).c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	cstr1.Clear();
	cstr2.Clear();
	cstr3.Clear();

	// Sort Home Runs
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( aiHR[aiCount[j]] < aiHR[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiHR[aiCount[i]] == aiHR[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiHR[aiCount[i]] );
			cstr1.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s   %3i", cstrShortTeamName.Item ( aiCount[i] ).c_str(), cstrPlayerName.Item ( aiCount[i] ).c_str(),aiHR[aiCount[i]] );
			cstr1.Add ( HTMLData );
		}
	}
	// Sort StolenBases
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( aiStolenBase[aiCount[j]] < aiStolenBase[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiStolenBase[aiCount[i]] == aiStolenBase[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiStolenBase[aiCount[i]] );
			cstr2.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),aiStolenBase[aiCount[i]] );
			cstr2.Add ( HTMLData );
		}
	}
	// Sort Caught Stealing
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( aiCS[aiCount[j]] < aiCS[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiCS[aiCount[i]] == aiCS[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiCS[aiCount[i]] );
			cstr3.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),aiCS[aiCount[i]] );
			cstr3.Add ( HTMLData );
		}
	}
//	                    00000000011111111112222222222333333333344444444445555555555666666666677777777778
//	                    12345678901234567890123456789012345678901234567890123456789012345678901234567890
	sprintf ( HTMLData,"\nHome Runs                  Stolen Basses              Caught Stealing\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	getSize1 = cstr1.GetCount();
	getSize2 = cstr2.GetCount();
	getSize3 = cstr3.GetCount();
	getSizeMax = getSize1;
	if ( getSizeMax < getSize2 )
	{
		getSizeMax = getSize2;
	}
	if ( getSizeMax < getSize3 )
	{
		getSizeMax = getSize3;
	}
	if ( getSize1 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize1;i++ )
			cstr1.Add ( str20Blank+str20Blank );
	}
	if ( getSize2 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize2;i++ )
			cstr2.Add ( str20Blank+str20Blank );
	}
	if ( getSize3 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize3;i++ )
			cstr3.Add ( str20Blank+str20Blank );
	}
	for ( i=0; i < getSizeMax;i++ )
	{
		sprintf ( HTMLData,"%.26s %.26s %.26s\n",cstr1.Item ( i ).c_str(),
		          cstr2.Item ( i ).c_str(),cstr3.Item ( i ).c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	cstr1.Clear();
	cstr2.Clear();
	cstr3.Clear();

	// Sort Strike Outs
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( aiStrikeOut[aiCount[j]] < aiStrikeOut[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiStrikeOut[aiCount[i]] == aiStrikeOut[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiStrikeOut[aiCount[i]] );
			cstr1.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),aiStrikeOut[aiCount[i]] );
			cstr1.Add ( HTMLData );
		}
	}
	// Sort Walks
	for ( i=0; i<totCount-1; i++ )
	{
		for ( j=0; j<totCount-1; j++ )
		{
			if ( aiWalk[aiCount[j]] < aiWalk[aiCount[j+1]] )
			{
				tiCount = aiCount[j];
				aiCount[j] = aiCount[j+1];
				aiCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiWalk[aiCount[i]] == aiWalk[aiCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiWalk[aiCount[i]] );
			cstr2.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamName.Item ( aiCount[i] ).c_str(),
			          cstrPlayerName.Item ( aiCount[i] ).c_str(),aiWalk[aiCount[i]] );
			cstr2.Add ( HTMLData );
		}
	}
	// Earned Runs
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( aiPER[aiPCount[j]] < aiPER[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiPER[aiPCount[i]] == aiPER[aiPCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiPER[aiPCount[i]] );
			cstr3.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamNameP.Item ( aiPCount[i] ).c_str(),
			          cstrPitcherName.Item ( aiPCount[i] ).c_str(),aiPER[aiPCount[i]] );
			cstr3.Add ( HTMLData );
		}
	}
//	                    00000000011111111112222222222333333333344444444445555555555666666666677777777778
//	                    12345678901234567890123456789012345678901234567890123456789012345678901234567890
	sprintf ( HTMLData,"\nStrike Outs                Walks                      Earned Runs\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	getSize1 = cstr1.GetCount();
	getSize2 = cstr2.GetCount();
	getSize3 = cstr3.GetCount();
	getSizeMax = getSize1;
	if ( getSizeMax < getSize2 )
	{
		getSizeMax = getSize2;
	}
	if ( getSizeMax < getSize3 )
	{
		getSizeMax = getSize3;
	}
	if ( getSize1 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize1;i++ )
			cstr1.Add ( str20Blank+str20Blank );
	}
	if ( getSize2 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize2;i++ )
			cstr2.Add ( str20Blank+str20Blank );
	}
	if ( getSize3 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize3;i++ )
			cstr3.Add ( str20Blank+str20Blank );
	}
	for ( i=0; i < getSizeMax;i++ )
	{
		sprintf ( HTMLData,"%.26s %.26s %.26s\n",cstr1.Item ( i ).c_str(),
		          cstr2.Item ( i ).c_str(),cstr3.Item ( i ).c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	cstr1.Clear();
	cstr2.Clear();
	cstr3.Clear();

	// Earned Run Average
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( adIP[aiPCount[j]] < adIP[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( adERA[aiPCount[j]] > adERA[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	cstrTemp1.Clear();
	cstrTemp2.Clear();
	cstrTemp3.Clear();
	for ( i=0; i<totPCount; i++ )
	{
		if ( adERA[aiPCount[i]] != 0 )
		{
			cstrTemp1.Add ( cstrShortTeamNameP.Item ( aiPCount[i] ) );
			cstrTemp2.Add ( cstrPitcherName.Item ( aiPCount[i] ) );
			sprintf ( HTMLData,"%5.2f",adERA[aiPCount[i]] );
			cstrTemp3.Add ( HTMLData );
		}
	}
	// if ERA is all zeros, then fill array with zeros
	if ( !cstrTemp3.GetCount() )
	{
		for ( i=0; i<m_StatCount+1; i++ )
		{
			sprintf ( HTMLData,"%5.2f",adERA[aiPCount[i]] );
			cstrTemp3.Add ( HTMLData );
		}
	}

	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( !strcmp ( cstrTemp3.Item ( i ), cstrTemp3.Item ( m_StatCount ) ) ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH     %.5s", cstrTemp3.Item ( i ).c_str() );
			cstr1.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s %.5s", cstrTemp1.Item ( i ).c_str(),
			          cstrTemp2.Item ( i ).c_str(),cstrTemp3.Item ( i ).c_str() );
			cstr1.Add ( HTMLData );
		}
	}
	// Innings Pitched
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( adIP[aiPCount[j]] < adIP[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( adIP[aiPCount[i]] == adIP[aiPCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4.1f", adIP[aiPCount[i]] );
			cstr2.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4.1f", cstrShortTeamNameP.Item ( aiPCount[i] ).c_str(),
			          cstrPitcherName.Item ( aiPCount[i] ).c_str(),adIP[aiPCount[i]] );
			cstr2.Add ( HTMLData );
		}
	}
	// Hits
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( aiPHits[aiPCount[j]] < aiPHits[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiPHits[aiPCount[i]] == aiPHits[aiPCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiPHits[aiPCount[i]] );
			cstr3.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamNameP.Item ( aiPCount[i] ).c_str(),
			          cstrPitcherName.Item ( aiPCount[i] ).c_str(),aiPHits[aiPCount[i]] );
			cstr3.Add ( HTMLData );
		}
	}
//	                    00000000011111111112222222222333333333344444444445555555555666666666677777777778
//	                    12345678901234567890123456789012345678901234567890123456789012345678901234567890
	sprintf ( HTMLData,"\nEarned Run Average         Innings Pitched            Hits Allowed\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	getSize1 = cstr1.GetCount();
	getSize2 = cstr2.GetCount();
	getSize3 = cstr3.GetCount();
	getSizeMax = getSize1;
	if ( getSizeMax < getSize2 )
	{
		getSizeMax = getSize2;
	}
	if ( getSizeMax < getSize3 )
	{
		getSizeMax = getSize3;
	}
	if ( getSize1 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize1;i++ )
			cstr1.Add ( str20Blank+str20Blank );
	}
	if ( getSize2 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize2;i++ )
			cstr2.Add ( str20Blank+str20Blank );
	}
	if ( getSize3 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize3;i++ )
			cstr3.Add ( str20Blank+str20Blank );
	}
	for ( i=0; i < getSizeMax;i++ )
	{
		sprintf ( HTMLData,"%.26s %.26s %.26s\n",cstr1.Item ( i ).c_str(),
		          cstr2.Item ( i ).c_str(),cstr3.Item ( i ).c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	cstr1.Clear();
	cstr2.Clear();
	cstr3.Clear();

	// TRG
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( adTRG[aiPCount[j]] > adTRG[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	cstrTemp1.Clear();
	cstrTemp2.Clear();
	cstrTemp3.Clear();
	for ( i=0; i<totPCount; i++ )
	{
		if ( adTRG[aiPCount[i]] != 0 )
		{
			cstrTemp1.Add ( cstrShortTeamNameP.Item ( aiPCount[i] ) );
			cstrTemp2.Add ( cstrPitcherName.Item ( aiPCount[i] ) );
			sprintf ( HTMLData,"%4.1f",adTRG[aiPCount[i]] );
			cstrTemp3.Add ( HTMLData );
		}
	}
	// if TRG is all zeros, then fill array with zeros
	if ( !cstrTemp3.GetCount() )
	{
		for ( i=0; i<m_StatCount+1; i++ )
		{
			sprintf ( HTMLData,"%5.2f",adERA[aiPCount[i]] );
			cstrTemp3.Add ( HTMLData );
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( !strcmp ( cstrTemp3.Item ( i ), cstrTemp3.Item ( m_StatCount ) ) ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %.4s", cstrTemp3.Item ( i ).c_str() );
			cstr1.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %.4s", cstrTemp1.Item ( i ).c_str(),
			          cstrTemp2.Item ( i ).c_str(),cstrTemp3.Item ( i ).c_str() );
			cstr1.Add ( HTMLData );
		}
	}
	// Walks
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( aiPWalks[aiPCount[j]] < aiPWalks[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiPWalks[aiPCount[i]] == aiPWalks[aiPCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiPWalks[aiPCount[i]] );
			cstr2.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamNameP.Item ( aiPCount[i] ).c_str(),
			          cstrPitcherName.Item ( aiPCount[i] ).c_str(),aiPWalks[aiPCount[i]] );
			cstr2.Add ( HTMLData );
		}
	}
	// Strikeouts
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( aiPK[aiPCount[j]] < aiPK[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiPK[aiPCount[i]] == aiPK[aiPCount[6]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiPK[aiPCount[i]] );
			cstr3.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamNameP.Item ( aiPCount[i] ).c_str(),
			          cstrPitcherName.Item ( aiPCount[i] ).c_str(),aiPK[aiPCount[i]] );
			cstr3.Add ( HTMLData );
		}
	}
//	                    00000000011111111112222222222333333333344444444445555555555666666666677777777778
//	                    12345678901234567890123456789012345678901234567890123456789012345678901234567890
	sprintf ( HTMLData,"\nTRG                        Walks Allowed              Strikeouts\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	getSize1 = cstr1.GetCount();
	getSize2 = cstr2.GetCount();
	getSize3 = cstr3.GetCount();
	getSizeMax = getSize1;
	if ( getSizeMax < getSize2 )
	{
		getSizeMax = getSize2;
	}
	if ( getSizeMax < getSize3 )
	{
		getSizeMax = getSize3;
	}
	if ( getSize1 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize1;i++ )
			cstr1.Add ( str20Blank+str20Blank );
	}
	if ( getSize2 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize2;i++ )
			cstr2.Add ( str20Blank+str20Blank );
	}
	if ( getSize3 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize3;i++ )
			cstr3.Add ( str20Blank+str20Blank );
	}
	for ( i=0; i < getSizeMax;i++ )
	{
		sprintf ( HTMLData,"%.26s %.26s %.26s\n",cstr1.Item ( i ).c_str(),
		          cstr2.Item ( i ).c_str(),cstr3.Item ( i ).c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	cstr1.Clear();
	cstr2.Clear();
	cstr3.Clear();

	// Wins
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( aiPWins[aiPCount[j]] < aiPWins[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiPWins[aiPCount[i]] == aiPWins[aiPCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiPWins[aiPCount[i]] );
			cstr1.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamNameP.Item ( aiPCount[i] ).c_str(),
			          cstrPitcherName.Item ( aiPCount[i] ).c_str(),aiPWins[aiPCount[i]] );
			cstr1.Add ( HTMLData );
		}
	}
	// Loss
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( aiPLoss[aiPCount[j]] < aiPLoss[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiPLoss[aiPCount[i]] == aiPLoss[aiPCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiPLoss[aiPCount[i]] );
			cstr2.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamNameP.Item ( aiPCount[i] ).c_str(),
			          cstrPitcherName.Item ( aiPCount[i] ).c_str(),aiPLoss[aiPCount[i]] );
			cstr2.Add ( HTMLData );
		}
	}
	// Starts
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( aiPStarts[aiPCount[j]] < aiPStarts[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiPStarts[aiPCount[i]] == aiPStarts[aiPCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiPStarts[aiPCount[i]] );
			cstr3.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamNameP.Item ( aiPCount[i] ).c_str(),
			          cstrPitcherName.Item ( aiPCount[i] ).c_str(),aiPStarts[aiPCount[i]] );
			cstr3.Add ( HTMLData );
		}
	}
//	                    00000000011111111112222222222333333333344444444445555555555666666666677777777778
//	                    12345678901234567890123456789012345678901234567890123456789012345678901234567890
	sprintf ( HTMLData,"\nWins                       Loss                       Starts\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	getSize1 = cstr1.GetCount();
	getSize2 = cstr2.GetCount();
	getSize3 = cstr3.GetCount();
	getSizeMax = getSize1;
	if ( getSizeMax < getSize2 )
	{
		getSizeMax = getSize2;
	}
	if ( getSizeMax < getSize3 )
	{
		getSizeMax = getSize3;
	}
	if ( getSize1 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize1;i++ )
			cstr1.Add ( str20Blank+str20Blank );
	}
	if ( getSize2 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize2;i++ )
			cstr2.Add ( str20Blank+str20Blank );
	}
	if ( getSize3 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize3;i++ )
			cstr3.Add ( str20Blank+str20Blank );
	}
	for ( i=0; i < getSizeMax;i++ )
	{
		sprintf ( HTMLData,"%.26s %.26s %.26s\n",cstr1.Item ( i ).c_str(),
		          cstr2.Item ( i ).c_str(),cstr3.Item ( i ).c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	cstr1.Clear();
	cstr2.Clear();
	cstr3.Clear();

	// Saves
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( aiPSaves[aiPCount[j]] < aiPSaves[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiPSaves[aiPCount[i]] == aiPSaves[aiPCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiPSaves[aiPCount[i]] );
			cstr1.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamNameP.Item ( aiPCount[i] ).c_str(),
			          cstrPitcherName.Item ( aiPCount[i] ).c_str(),aiPSaves[aiPCount[i]] );
			cstr1.Add ( HTMLData );
		}
	}
	// Homeruns
	for ( i=0; i<totPCount-1; i++ )
	{
		for ( j=0; j<totPCount-1; j++ )
		{
			if ( aiPHR[aiPCount[j]] < aiPHR[aiPCount[j+1]] )
			{
				tiCount = aiPCount[j];
				aiPCount[j] = aiPCount[j+1];
				aiPCount[j+1] = tiCount;
			}
		}
	}
	for ( i=0; i<m_StatCount; i++ )
	{
		if ( ( aiPHR[aiPCount[i]] == aiPHR[aiPCount[m_StatCount]] ) )
		{
			sprintf ( HTMLData,"OTHERS TIED WITH      %4i", aiPHR[aiPCount[i]] );
			cstr2.Add ( HTMLData );
			break;
		}
		else
		{
			sprintf ( HTMLData,"%s %.16s  %4i", cstrShortTeamNameP.Item ( aiPCount[i] ).c_str(),
			          cstrPitcherName.Item ( aiPCount[i] ).c_str(),aiPHR[aiPCount[i]] );
			cstr2.Add ( HTMLData );
		}
	}
//	                    00000000011111111112222222222333333333344444444445555555555666666666677777777778
//	                    12345678901234567890123456789012345678901234567890123456789012345678901234567890
	sprintf ( HTMLData,"\nSaves                      HomeRuns Allowed\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	getSize1 = cstr1.GetCount();
	getSize2 = cstr2.GetCount();
	getSize3 = cstr3.GetCount();
	getSizeMax = getSize1;
	if ( getSizeMax < getSize2 )
	{
		getSizeMax = getSize2;
	}
	if ( getSizeMax < getSize3 )
	{
		getSizeMax = getSize3;
	}
	if ( getSize1 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize1;i++ )
			cstr1.Add ( str20Blank+str20Blank );
	}
	if ( getSize2 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize2;i++ )
			cstr2.Add ( str20Blank+str20Blank );
	}
	if ( getSize3 < getSizeMax )
	{
		for ( i=0; i<getSizeMax-getSize3;i++ )
			cstr3.Add ( str20Blank+str20Blank );
	}
	for ( i=0; i < getSizeMax;i++ )
	{
		sprintf ( HTMLData,"%.26s %.26s %.26s\n",cstr1.Item ( i ).c_str(),
		          cstr2.Item ( i ).c_str(),cstr3.Item ( i ).c_str() );
		strHTMLData = HTMLData;
		HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	}
	cstr1.Clear();
	cstr2.Clear();
	cstr3.Clear();

	sprintf ( HTMLData,"\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );
	sprintf ( HTMLData,"</B></PRE>\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );

	// Write the Last Updated line n the HTML file

// Get time in seconds
	time ( &szClock );

// Convert time to struct tm form
	newTime = localtime ( &szClock );

	sprintf ( HTMLData, "<BR>Last Updated on %s<BR>\n", asctime ( newTime ) );

	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );

	sprintf ( HTMLData,"</BODY></HTML>\n" );
	strHTMLData = HTMLData;
	HTMLFile.Write ( strHTMLData,strHTMLData.Len() );

	HTMLFile.Close();

	// Free up all of the dynamic arrays
	free ( aiCount );
	free ( aiPCount );
	free ( aiTCount );
	free ( aiAB );
	free ( aiRuns );
	free ( aiHits );
	free ( aiRBI );
	free ( ai2B );
	free ( ai3B );
	free ( aiHR );
	free ( afBA );
	free ( afTBA );
	free ( afSLG );
	free ( afOBP );
	free ( aiStolenBase );
	free ( aiCS );
	free ( aiStrikeOut );
	free ( aiWalk );
	free ( adERA );
	free ( adTERA );
	free ( adTRG );
	free ( adIP );
	free ( aiPER );
	free ( aiPHits );
	free ( aiPWalks );
	free ( aiPK );
	free ( aiPWins );
	free ( aiPLoss );
	free ( aiPStarts );
	free ( aiPSaves );
	free ( aiPHR );
}

// Parse the GetLeagues() subroutine as follows:
//
//	CString strLeague;
//	CString strLeagueName;
//	CString strLeagueFile;
//	CString strLeagueDir;
//
//	strLeague = GetLeagues(TRUE);
//
//	strLeagueName = strLeague.Left(30);
//	if (strncmp(strLeagueName,"All",3))
//	{
//		strLeagueFile = strLeague.Right(12);
//		strLeagueDir = strLeagueFile.Left(8);
//	}
//	else
//	{
//		// This is the base directory
//		strLeagueDir = "data";
//	}
//
wxString MyMainWindow::GetLeagues ( bool baseFlag )
{
	wxFile myInFile;
	wxArrayString arrayFileNames;
	wxArrayString arrayLeagueNames;
	int sortflag;
	int i;
	wxString strLeagueName;
	wxString strTemp;
	wxByte version;
	wxByte count;
	char temp[41];
	wxFileName myTempFileName;

	myTempFileName = wxFindFirstFile ( wxGetApp().g_DataDir+"/l*.dat" );
	if ( myTempFileName.FileExists() )
	{
		while ( myTempFileName.FileExists() )
		{
			arrayFileNames.Add ( myTempFileName.GetFullName() );
			myTempFileName = wxFindNextFile();
		}

		// Since the FindNextFile does not return the files in any order
		// we must sort the file names
		sortflag = 1;
		while ( sortflag )
		{
			sortflag = 0;
			for ( i=0; i< ( int ) ( arrayFileNames.GetCount()-1 ); i++ )
			{
				if ( arrayFileNames[i].Cmp ( arrayFileNames[i+1] ) == 1 )
				{
					strTemp = arrayFileNames[i];
					arrayFileNames[i] = arrayFileNames[i+1];
					arrayFileNames[i+1] = strTemp;
					sortflag = 1;
				}
			}
		}

		for ( i=0; i< ( int ) arrayFileNames.GetCount(); i++ )
		{
			myInFile.Open ( wxGetApp().g_DataDir+"/"+arrayFileNames[i],wxFile::read );
			myInFile.Read ( &version,sizeof ( version ) );
			myInFile.Read ( &count,sizeof ( count ) );
			myInFile.Read ( temp,30 );
			myInFile.Close();
			temp[30] = 0x00;
			strLeagueName = temp;
			strLeagueName = strLeagueName+"\t"+arrayFileNames[i];
			arrayLeagueNames.Add ( strLeagueName );
		}
		// If TRUE add in Base directory
		if ( baseFlag == TRUE )
		{
			//           123456789012345678901234567890
			strcpy ( temp,"All Base Teams                " );
			temp[30] = 0x00;
			strLeagueName = temp;
			strLeagueName = strLeagueName+"\t"+"data";
			arrayLeagueNames.Add ( strLeagueName );
		}
		wxSingleChoiceDialog dialog1 ( NULL,
		                               _T ( "Please select a League" ),
		                               _T ( "Please select a League" ),
		                               arrayLeagueNames );

		strLeagueName.Empty();

		if ( dialog1.ShowModal() == wxID_OK )
		{
			strLeagueName = arrayLeagueNames[ dialog1.GetSelection() ];
		}
	}
	else
	{
		// No Leagues found so display message
		wxMessageBox ( _T ( "Could not find any Leagues files. Please create a League" ),
		               _T ( "Leagues" ), wxID_OK|wxICON_INFORMATION );
		return "";
	}
	return strLeagueName;
}

void MyMainWindow::OnPlayersAddEditBatter ( wxCommandEvent& event )
{
	BatterDialog ( this );
}

void MyMainWindow::OnPlayersAddEditPitcher ( wxCommandEvent& event )
{
	PitcherDialog ( this );
}

void MyMainWindow::OnOptions ( wxCommandEvent& event )
{
    int rc;

	OptionFile myOptionFile;
	OptionsDialog dlg( this );
    rc = dlg.ShowModal();
// Possible RC values are wxID_APPLY, wxID_OK, and wxID_CANCEL
    if ( rc == wxID_APPLY )
    {
        myOptionFile.OptionFileWriteParse();
    }
}

BEGIN_EVENT_TABLE ( MyCanvas, wxWindow )
	//    EVT_MOUSE_EVENTS(MyCanvas::OnMouseEvent)
	//    EVT_CHAR(MyCanvas::OnChar)
	//EVT_PAINT ( MyCanvas::OnPaint )
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas ( wxFrame* frame, wxWindowID id, const wxPoint& pos, const wxSize& size ) :
		wxScrolledWindow ( frame, id, pos, size, wxHSCROLL | wxVSCROLL, _T ( "scrolledWindow" ) )
{
//    SetBackgroundColour( _T("SEA GREEN"));
	SetScrollbars ( 72, 72, 16, 12, 0, 0, FALSE );
}

MyCanvas::~MyCanvas()
{}

// OnDraw
void MyCanvas::OnDraw ( wxDC& dc )
{
	FormA* pMyForm = new FormA();
	pMyForm->OnDraw ( dc );
	delete pMyForm;
}

