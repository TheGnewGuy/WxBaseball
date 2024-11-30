/***************************************************************
 * Name:      WxBaseballApp.h
 * Purpose:   Defines Application Class
 * Author:    John Gnew (john.gnew@verizon.net)
 * Created:   2024-03-22
 * Copyright: John Gnew ()
 * License:
 **************************************************************/

#ifndef WXBASEBALLAPP_H
#define WXBASEBALLAPP_H

#include <wx/wx.h>
#include <wx/frame.h>

#include <wx/app.h>
#include <wx/cmndata.h>

#include "WxBaseballMain.h"
#include "DBRoutines.h"
#include "FileRoutines.h"
#include "dialogs.h"

//#define    APP_VERSION      111130
#define    APP_VERSION_STR  _T("1.1.11.30")

class WxBaseballApp : public wxApp
{
    public:
        virtual bool OnInit();
        int OnExit();

    // ---------------------------------------------------------------------------
    // global variables
    // ---------------------------------------------------------------------------
    WxBaseballFrame* pWxBaseballFrame;
//    wxFrame* pWxBaseballFrame;

    DBRoutines* pDBRoutines;
    FileRoutines* pFileRoutines;

    // Global print data, to remember settings during the session
    wxPrintData* g_printData;

    // Global page setup data
    wxPageSetupData* g_pageSetupData;

    // Global Data Directory without the trailing slash
    wxString g_DataDirectory;

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
};

DECLARE_APP(WxBaseballApp);

#endif // WXBASEBALLAPP_H
