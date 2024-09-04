/////////////////////////////////////////////////////////////////////////////
// Name:        main.h                                               //
// Purpose:     Baseball statistics tracking tool for Strat-O-Matic        //
// Author:      John Gnew                                                  //
// Created:     08/30/04                                                   //
// Copyright:   (c) John Gnew                                              //
// Modifications:                                                          //
//   Date       Description                                                //
// xx/xx/xx     xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
/////////////////////////////////////////////////////////////////////////////
#ifndef _MAIN_H_
#define _MAIN_H_

#include <wx/toolbar.h>

#include <wx/cmndata.h>

// Define a new canvas which can receive some events
class MyCanvas: public wxScrolledWindow
{
public:
    MyCanvas(wxFrame *frame, wxWindowID id, const wxPoint& pos, const wxSize& size);
    ~MyCanvas();
    // OnDraw
    virtual void OnDraw(wxDC& dc);

//    void OnPaint(wxPaintEvent& event);
//    void OnMouseEvent(wxMouseEvent& event);
//    void OnChar(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
private:
//    wxMenu *popupMenu;
};

// Define a new frame
class MyMainWindow : public wxFrame
{
private:

public:
    MyCanvas* pCanvas;
    wxTextCtrl* pTextWindow;
    wxMenuBar* pMenuBar;
//    wxPanel *m_pBatterNotebook;

    void GetTeam(wxString strLeagueDir);
    void GetTeamHTML(wxString strLeagueDir);
    void ExportFile(wxString strDir, wxString strTeamName);
    void ExportHTMLFile(wxString strDir, wxString strTeamName);
    void HTMLLeagueStats(wxString strLeague);
    void BuildPlayerArray(wxArrayString* arrayFileNames, wxString strLeagueName,
	wxString strDivisionName, wxString strConferenceName, int iDiv);
    wxString GetLeagues(bool baseFlag);

    MyMainWindow(wxWindow *parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos, const wxSize& size, const long style);

    void InitToolBar(wxToolBar* toolBar);

    void OnSize(wxSizeEvent& event);
    void OnAbout(wxCommandEvent& event);
    //    void OnNewWindow(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);
    // File menu
    void OnFileExportLeagueTeams(wxCommandEvent& event);
    void OnFileExportLeagueTeamsAll(wxCommandEvent& event);
    void OnExportHTML(wxCommandEvent& event);
    void OnExportHTMLAll(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    // View menu
    void OnScoreSheetCheck(wxCommandEvent& event);
    void OnScoreSheetCheckExtra(wxCommandEvent& event);
    void OnStatistics(wxCommandEvent& event);
    // League menu
    void OnLeaguesEditLeague(wxCommandEvent& event);
    // Players menu
    void OnPlayersAddEditBatter(wxCommandEvent& event);
    void OnPlayersAddEditPitcher(wxCommandEvent& event);

    // Print
    void OnPrint(wxCommandEvent& event);
    // Print Setup
    void OnPrintSetup(wxCommandEvent& event);
    // Print PreView
    void OnPrintPreview(wxCommandEvent& event);

    // Ststistics
    void OnStatisticsHTMLLeagueStats(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit();
    int OnExit();

    // ---------------------------------------------------------------------------
    // global variables
    // ---------------------------------------------------------------------------
    MyMainWindow* pMyMainWindow;
    wxFont m_testFont;

    // Global print data, to remember settings during the session
    wxPrintData* g_printData;

    // Global page setup data
    wxPageSetupData* g_pageSetupData;

    // Global Options Directory and file name
    //    OptionsFile contains Dir nad filename
    // Access - wxGetApp().g_OptionsDir
    //          wxGetApp().g_OptionsFile
    wxString g_OptionsDir;
    wxString g_OptionsFile;

    //Global Option Variables
    wxString g_DataDir;

    wxString g_HTML_BG;
    wxString g_HTML_Index;
    wxString g_HTML_TextColor;
    wxString g_HTML_BGColor;
    wxString g_HTML_LinkColor;
    wxString g_HTML_VLinkColor;

private:

};

DECLARE_APP(MyApp)

// menu items ids
enum
{
    myID_EXPORT = wxID_HIGHEST +1,
    myID_EXPORT_All,
    myID_EXPORT_HTML,
    myID_EXPORT_HTML_All,
    myID_TOOLBAR,
    myID_STATUSBAR,
    myID_SCORESHEET,
    myID_SCORESHEETXTRA,
    myID_STATISTICS,
    myID_ADDLEAGUE,
    myID_EDITLEAGUE,
    myID_LEAGUEOPTIONSHTML,
    myID_ADDTEAMS,
    myID_EDITTEAMS,
    myID_ADDBATTERS,
    myID_ADDPITCHERS,
    myID_DELBATTERS,
    myID_DELPITCHERS,
    myID_CREATESTATS,
    myID_UPDATESTATS,
    myID_HTMLSTATS,
    myID_TOOLBAR_NEW,
    myID_TOOLBAR_OPEN,
    myID_TOOLBAR_SAVE,
    myID_TOOLBAR_COPY,
    myID_TOOLBAR_CUT,
    myID_TOOLBAR_PASTE,
    myID_TOOLBAR_PRINT,
    myID_TOOLBAR_HELP,
    myID_OPTIONS
};

#endif // _MAIN_H_
