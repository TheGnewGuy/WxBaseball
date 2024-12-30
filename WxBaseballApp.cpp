/***************************************************************
 * Name:      WxBaseballApp.cpp
 * Purpose:   Code for Application Class
 * Author:    John Gnew (john.gnew@verizon.net)
 * Created:   2024-03-22
 * Copyright: John Gnew ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "WxBaseballApp.h"
#include "WxBaseballMain.h"

IMPLEMENT_APP(WxBaseballApp);

//bool WxBaseballApp::OnInit()
//{
//    WxBaseballFrame* frame = new WxBaseballFrame(0L, _("wxWidgets Application Template"));
//
//    frame->Show();
//
//    return true;
//}
bool WxBaseballApp::OnInit()
{
//	wxDateTime dt;
//	myAppVersion.Printf( wxT("1.1.%i.24"), dt.GetDayOfYear());

	pWxBaseballFrame = ( WxBaseballFrame * ) NULL;

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

// Comment until OptionFile is setup in Main
//	OptionFile myOptionFile;

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

	pDBRoutines = new DBRoutines();

	pFileRoutines = new FileRoutines();

//    WxBaseballFrame* frame = new WxBaseballFrame(0L, _("wxWidgets Application Template"));
    pWxBaseballFrame = new WxBaseballFrame(0L, _("Baseball Statistics for Strat-O-Matic"));
//	pWxBaseballFrame = new WxBaseballFrame ( ( wxFrame * ) NULL, -1,
//	                                   _T ( "Baseball Statistics for Strat-O-Matic" ),
//	                                   wxDefaultPosition, wxSize ( 900, 650 ),
//	                                   wxDEFAULT_FRAME_STYLE );

// Comment until mycanvas is setup
	pWxBaseballFrame->pCanvas = new MyCanvas ( pWxBaseballFrame, wxID_ANY,
	                                        wxDefaultPosition, wxDefaultSize );

	pWxBaseballFrame->Centre ( wxBOTH );
	pWxBaseballFrame->Show ( TRUE );

	SetTopWindow ( pWxBaseballFrame );

	return TRUE;
}

int WxBaseballApp::OnExit()
{
	delete g_printData;
	delete g_pageSetupData;
	return 1;
}

