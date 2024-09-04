/***************************************************************
 * Name:      WxBaseballMain.h
 * Purpose:   Defines Application Frame
 * Author:    John Gnew (john.gnew@verizon.net)
 * Created:   2024-03-22
 * Copyright: John Gnew ()
 * License:
 **************************************************************/

#ifndef WXBASEBALLMAIN_H
#define WXBASEBALLMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/wx.h>
#include <wx/frame.h>

#include <wx/toolbar.h>

#include "MyCanvas.h"
#include "sqlite3.h"

class WxBaseballFrame: public wxFrame
{
    public:
        WxBaseballFrame(wxFrame *frame, const wxString& title);
//        WxBaseballFrame(wxFrame *frame, const wxWindowID id, const wxString& title,
//            const wxPoint& pos, const wxSize& size, const long style);
        ~WxBaseballFrame();

        void InitToolBar(wxToolBar* toolBar);
        MyCanvas* pCanvas;
        wxMenuBar* pMenuBar;

        // Sqlite3 defines
//        wxString m_DBFileName;
//        int m_dbOpen;
//        sqlite3 *m_db;
//        sqlite3_stmt *m_stmt;

    private:
        enum
        {
            idMenuQuit = 1000,
            idMenuAbout
        };
        void OnClose(wxCloseEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
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

#endif // WXBASEBALLMAIN_H
