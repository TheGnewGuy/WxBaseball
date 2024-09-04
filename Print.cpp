/////////////////////////////////////////////////////////////////////////////
// Name:        print.cpp                                                  //
// Purpose:     Printing                                                   //
// Author:      John Gnew                                                  //
// Created:     09/15/04                                                   //
// Copyright:   (c) John Gnew                                              //
// Modifications:                                                          //
//   Date       Description                                                //
// 01/01/06     Added comments for Cannon IP4200 Printer                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if !wxUSE_PRINTING_ARCHITECTURE
#error "You must set wxUSE_PRINTING_ARCHITECTURE to 1 in setup.h, and recompile the library."
#endif

// Set this to 1 if you want to test PostScript printing under MSW.
// However, you'll also need to edit src/msw/makefile.nt.
#define wxTEST_POSTSCRIPT_IN_MSW 0

//#include <ctype.h>
//#include <wx/metafile.h>
#include <wx/print.h>
#include <wx/printdlg.h>

//#include <wx/accel.h>

#if wxTEST_POSTSCRIPT_IN_MSW
#include <wx/generic/printps.h>
#include <wx/generic/prntdlgg.h>
#endif

//#include "main.h"
#include "WxBaseballMain.h"
#include "WxBaseballApp.h"
#include "Print.h"

bool MyPrintout::OnPrintPage(int page)
{
    wxDC* dc = GetDC();
    if (dc)
    {
        // Print 1 caned pages
        if (page == 1)
            DrawPageOne(dc);
//        else if (page == 2)
//            DrawPageTwo(dc);

        return true;
    }
    else
        return false;
}

MyPrintout::~MyPrintout()
{
}

// Called by framework. If overridden must call base class
bool MyPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;

    return true;
}

// Called by framework
void MyPrintout::GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo)
{
    *minPage = 1;
    *maxPage = 1;
    *selPageFrom = 1;
    *selPageTo = 1;
}

bool MyPrintout::HasPage(int pageNum)
{
    return (pageNum == 1 || pageNum == 2);
}

void MyPrintout::DrawPageOne(wxDC* dc)
{
//    // This is how someone thinks the scaleing should be done
//    // Init the device scale to inches.
//    int ppiPrinterX, ppiPrinterY;
//    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
//
//    int pageWidth, pageHeight;
//    GetPageSizePixels(&pageWidth, &pageHeight);
//
//    int w, h;
//    dc->GetSize(&w, &h);
//
//    dc->SetUserScale(1.0f/ppiPrinterX * (float)(w/(float)pageWidth), 1.0f/ppiPrinterY * (float)(h/(float)pageHeight));

//// Recommended from user form
//printout_->GetDC()->SetMapMode(wxMM_METRIC);
//double pixelspermmx=static_cast<double>(printout_->GetDC()->LogicalToDeviceXRel(1<<25))/static_cast<double>(1<<25);
//double pixelspermmy=static_cast<double>(printout_->GetDC()->LogicalToDeviceYRel(1<<25))/static_cast<double>(1<<25);
//printout_->GetDC()->SetMapMode(wxMM_POINTS);
//double pixelsperptx=static_cast<double>(printout_->GetDC()->LogicalToDeviceXRel(1<<25))/static_cast<double>(1<<25);
//double pixelsperpty=static_cast<double>(printout_->GetDC()->LogicalToDeviceYRel(1<<25))/static_cast<double>(1<<25);

//  ptspermmx = pixelspermmx/pixelsperptx;
//  ptspermmy = pixelspermmy/pixelsperpty;

    // Set the printer DC to ROUGHLY reflect the screen text size.
    int marginX;
    int marginY;
    int ppiScreenX, ppiScreenY;
    int ppiPrinterX, ppiPrinterY;
//    float scalex, scaley;
    int pageWidth, pageHeight;
    int w, h;
    float overallScalex, overallScaley;

    // Get the logical pixels per inch of screen and printer
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);

    // Set the margin to be 1/2 inch in logical pixels
    marginX = ppiPrinterX/2;
    marginY = ppiPrinterY/2;

    // This scales the DC so that the printout roughly represents the
    // the screen scaling. The text point size _should_ be the right size
    // but in fact is too small for some reason. This is a detail that will
    // need to be addressed at some point but can be fudged for the
    // moment.
//    scalex = (float)((float)ppiPrinterX/(float)ppiScreenX);
//    scaley = (float)((float)ppiPrinterY/(float)ppiScreenY);

    // Now we have to check in case our real page size is reduced
    // (e.g. because we're drawing to a print preview memory DC)
    // Might need dc->SetMapMode(wxMM_POINTS (1/72 of inch)
    // or wxMM_TWIPS(1/1440 of inch) );
    dc->GetSize(&w, &h);
    GetPageSizePixels(&pageWidth, &pageHeight);

    // Add the margin to the graphic size in inches
    pageWidth  += (2*marginX);
    pageHeight += (2*marginY);

    // If printer pageWidth == current DC width, then this doesn't
    // change. But w might be the preview bitmap width, so scale down.
//    overallScalex = scalex * (float)(w/(float)pageWidth);
//    overallScaley = scaley * (float)(h/(float)pageHeight);
// We need to scale based on the actual size of the graphic.
// Based on 800 x 1000 form size
// Left Margin 55
// Top Margin 50
// So Y = 1000 + 55 + 55 = 1110
// So X = 800 + 50 + 50 = 900
    overallScalex = (float)(w/(float)1110.0);
    overallScaley = (float)(h/(float)900.0);

// Use the following only if you do not have X and Y
// actualscale = sqrt((overallScalex*overallScalex+overallScaley*overallScaley)/2)
//    float actualscale = wxMin(overallScalex, overallScaley);

//	Used for debugging variables
//    wxString msg1;
//    wxString msg2;
//    wxString msg3;
//    wxString msg4;
//    wxString msg5;
//    wxString msg6;
//    msg1.Printf("ppiScreenX = %i, ppiScreenY = %i\n",
//        ppiScreenX,ppiScreenY);
//    msg2.Printf("ppiPrinterX = %i, ppiPrinterY = %i\n",
//        ppiPrinterX,ppiPrinterY);
//    msg3.Printf("scalex = %f, scaley = %f\n",
//        scalex,scaley);
//    msg4.Printf("pageWidth = %i, pageHeight = %i\n",
//        pageWidth,pageHeight);
//    msg5.Printf("w = %i, h = %i\n",
//        w,h);
//    msg6.Printf("ScaleX = %f, ScaleY = %f",
//        overallScalex,overallScaley);
//    (void)wxMessageBox(msg1+msg2+msg3+msg4+msg5+msg6);

    // Laptop Xerox
    // ppiScreen x = 96, y=96
    // ppiPrinter x = 600, y=600
    // scale x = 6.25 y = 6.25
    // pageWidth = 6994, pageHeight = 5494
    // w = 699 h = 535
    // overall X = .624643 y = .608618
    //
    // Laptop Deskjet 870cse
    // ppiScreen x = 96, y=96
    // ppiPrinter x = 300, y=300
    // scale x = 3.125 y = 3.125
    // pageWidth = 3412, pageHeight = 2700
    // w = 715 h = 551
    // overall X = .654858 y = .637731
    //
    // Desktop Deskjet 870cse
    // ppiScreen x = 120, y=120
    // ppiPrinter x = 300, y=300
    // scale x = 2.5 y = 2.5
    // pageWidth = 3412, pageHeight = 2700
    // w = 848 h = 654
    // overall X = .621336 y = .605556
    //
    // Desktop Deskjet 870cse Direct to printer
    // ppiScreen x = 120, y=120
    // ppiPrinter x = 300, y=300
    // scale x = 2.5 y = 2.5
    // pageWidth = 3412, pageHeight = 2700
    // w = 3112 h = 2400
    // overall X = 2.280188 y = 2.222222
    //
    // Desktop Cannon IP4200 Direct to printer
    // ppiScreen x = 120, y=120
    // ppiPrinter x = 600, y=600
    // scale x = 5.0 y = 5.0
    // pageWidth = 3412, pageHeight = 2700
    // w = 7011 h = 5400
    // overall X = 0.604051 y = 0.857037
    //
    // Laptop Cannon IP4200 Network to printer
    // ppiScreen x = 96, y=96
    // ppiPrinter x = 600, y=600
    // scale x = 6.25 y = 6.25
    // pageWidth = 7011, pageHeight = 5400
    // w = 715 h = 535
    // overall X = 0.637391 y = 0.619213
    //
// The following is way too large
//    dc->SetUserScale(scalex, scaley);
    dc->SetUserScale(overallScalex, overallScaley);
//    dc->SetUserScale(actualscale, actualscale);
    // Set the fontsize to a reasonable value after DC scaling, (1st term in pt size)
	// Set font to Times New Roman
//    wxFont myTimesNewRomanFont;
    wxFont myTimesNewRomanFont(8, wxFONTFAMILY_ROMAN , wxFONTSTYLE_NORMAL,
            wxFONTWEIGHT_NORMAL, false);
    wxString myOrigFontName;

    myOrigFontName = myTimesNewRomanFont.GetFaceName();
//    myTimesNewRomanFont.SetFamily(wxSWISS);
//    myTimesNewRomanFont.SetFaceName("Times New Roman");
//    myTimesNewRomanFont.SetStyle(wxNORMAL);
//    myTimesNewRomanFont.SetWeight(wxNORMAL);
//    myTimesNewRomanFont.SetPointSize(int(8/actualscale));

    dc->SetFont(myTimesNewRomanFont);

    // preview + .15 about (.75, .75)
    // real print add about .6 to both (2.880188, 2.822222)
//    dc->SetUserScale(2.880188, 2.822222);

    wxGetApp().pWxBaseballFrame->pCanvas->OnDraw(*dc);
}

