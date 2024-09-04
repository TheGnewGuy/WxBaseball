/////////////////////////////////////////////////////////////////////////////
// Name:        Forms.cpp                                                  //
// Purpose:     Baseball statistics tracking tool for Strat-O-Matic        //
// Author:      John Gnew                                                  //
// Created:     09/15/04                                                   //
// Copyright:   (c) John Gnew                                              //
// Modifications:                                                          //
//   Date       Description                                                //
// xx/xx/xx     xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx                           //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================


#define xLeftScoreSheet       55    // Left margin of sheet
#define xStartFLDG		      xLeftScoreSheet+156
#define xLeftInningScoreSheet 144
#define xBoxSize              37
#define xRightSumScoreSheet   xLeftInningScoreSheet+(15*xBoxSize)
#define yTopScoreSheet	      50
#define yStartNames		      75
#define xIncInning	          44
#define yIncInning	          44
#define xIncStats	          25
#define yIncStats	          44

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------
#include "WxBaseballMain.h"
#include "WxBaseballApp.h"
#include "Forms.h"

FormA::FormA()
{
	// SetUp font to Times New Roman
//    myOrigFontName = myTimesNewRomanFont.GetFaceName();
//    myTimesNewRomanFont.SetFamily(wxSWISS);
//    myTimesNewRomanFont.SetFaceName("Times New Roman");
//    myTimesNewRomanFont.SetStyle(wxNORMAL);
//    myTimesNewRomanFont.SetWeight(wxNORMAL);
//    myTimesNewRomanFont.SetPointSize(8);
    myTimesNewRomanFont = wxFont(8, wxFONTFAMILY_SWISS,
        wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL,
        FALSE, "Times New Roman", wxFONTENCODING_SYSTEM);
}

FormA::~FormA()
{
}

void FormA::OnDraw(wxDC& dc)
{
	wxFont myOldFont;

    //  Each logical unit is 1 pixel - wxMM_TEXT is the default MapMode
	dc.SetMapMode(wxMM_TEXT);

	// Save original Font so that we can restore it latter
    myOldFont = dc.GetFont();
	// Set font to Times New Roman
    dc.SetFont(myTimesNewRomanFont);

    Common(dc);

    // Insert inning numbers
    if ( wxGetApp().pWxBaseballFrame->pMenuBar->IsChecked(myID_SCORESHEET) )
    {
        Normal(dc);
    }
    else
    {
        Extended(dc);
    }

    // Reset Font back to original font
    dc.SetFont(myOldFont);
}

void FormA::Normal(wxDC& dc)
{
    // Insert inning numbers
    for (i=1; i<=9; i++)
    {
        myBuffer.Clear();
        myBuffer.Printf("%i", i);
        dc.DrawText(myBuffer,xStartFLDG+20+(i*xIncInning),yTopScoreSheet+5);
    }
    for (i=10; i<=12; i++)
    {
        myBuffer.Clear();
        myBuffer.Printf("%i", i);
        dc.DrawText(myBuffer,xStartFLDG+15+(i*xIncInning),yTopScoreSheet+5);
    }

    for (i=1; i<=9; i++)
    {
        myBuffer.Clear();
        myBuffer.Printf("%i", i);
        // 144 = position on 1st box
        // 37 is size of box
        // 15 = displacement into box
        dc.DrawText(myBuffer,(xLeftInningScoreSheet-xBoxSize+15)+(i*xBoxSize),ySummaryTop+5);
    }
    for (i=10; i<=12; i++)
    {
        myBuffer.Clear();
        myBuffer.Printf("%i", i);
        dc.DrawText(myBuffer,(xLeftInningScoreSheet-xBoxSize+11)+(i*xBoxSize),ySummaryTop+5);
    }
}

void FormA::Extended(wxDC& dc)
{
    // Insert inning numbers
    for (i=13; i<=24; i++)
    {
        myBuffer.Clear();
        myBuffer.Printf("%i", i);
        dc.DrawText(myBuffer,xStartFLDG+15+((i-12)*xIncInning),yTopScoreSheet+5);
    }

    for (i=13; i<=24; i++)
    {
        myBuffer.Clear();
        myBuffer.Printf("%i", i);
        // 144 = position on 1st box
        // 37 is size of box
        // 15 = displacement into box
        dc.DrawText(myBuffer,(xLeftInningScoreSheet-xBoxSize+11)+((i-12)*xBoxSize),ySummaryTop+5);
    }
}

void FormA::Common(wxDC& dc)
{
	// Based on 800 x 1000

//	Used for debugging variables
//    wxString msg;
//    msg.Printf("%i",mymapmode);
//    dc.DrawText(msg,20,20);

	xStopInning = (12*xIncInning)+xStartFLDG;
	yBottomScoreSheet = (12*yIncInning)+yStartNames;
	yGameNumberTop = yBottomScoreSheet+13+(5*25)+13;
	ySummaryTop = yGameNumberTop+75-125;

	// Draw diamond graphics for innings starting with FLDG box
    dc.SetPen( *wxLIGHT_GREY_PEN );
	for (ix=xStartFLDG; ix<=xStopInning; ix+=xIncInning)
	{
    	for (iy=yTopScoreSheet+25; iy<yBottomScoreSheet; iy+=yIncInning)
	    {
            dc.DrawLine(ix+10, iy+(yIncInning/2), ix+(xIncInning/2), iy+10);
            dc.DrawLine(ix+(xIncInning/2), iy+10, ix+xIncInning-10, iy+(yIncInning/2));
            dc.DrawLine(ix+10, iy+(yIncInning/2), ix+(xIncInning/2), iy+yIncInning-10);
            dc.DrawLine(ix+(xIncInning/2), iy+yIncInning-10, ix+xIncInning-10, iy+(yIncInning/2));
        }
	}

	// Set pen back to BLACK
    dc.SetPen( *wxBLACK_PEN );

    // Draw vertical lines for innings starting with FLDG box
	for (ix=xStartFLDG; ix<=xStopInning; ix+=xIncInning)
	{
		dc.DrawLine(ix, yTopScoreSheet, ix, yBottomScoreSheet);
	}

	// Draw vertical lines for statistics starting after innings boxes
	xStopStats = (11*xIncStats) + ix;
	// ix=ix was needed to get rid of a warning "Statement with no effect"
	for ( ix=ix; ix<=xStopStats; ix+=xIncStats )
	{
		dc.DrawLine(ix, yTopScoreSheet, ix, yBottomScoreSheet);
	}
	ix-=xIncStats; // ix = 1058

//    wxString msg6;
//    msg6.Printf("Left Side = %i",ix);
//    (void)wxMessageBox(msg6);    // prints 1058

    // Split the SB/CS column diagonaly
    dc.DrawLine(ix-xIncStats, (yTopScoreSheet+25), ix, yTopScoreSheet);
    for (i=yTopScoreSheet+25; i<yBottomScoreSheet; i+=yIncStats)
	{
		dc.DrawLine(ix-xIncStats, (i+yIncStats), ix, i);
	}


	// Draw inning boxes horizontal lines in main sheet
	for (iy=yStartNames; iy<=yBottomScoreSheet; iy+=yIncInning)
	{
		dc.DrawLine(xLeftScoreSheet, iy, xStopStats, iy);
	}

	dc.DrawLine(xLeftScoreSheet, yTopScoreSheet, xLeftScoreSheet, yBottomScoreSheet);
	dc.DrawLine(xLeftScoreSheet, yBottomScoreSheet, xStopStats, yBottomScoreSheet);
	dc.DrawLine(xStopStats, yBottomScoreSheet, xStopStats, yTopScoreSheet);
	dc.DrawLine(xStopStats, yTopScoreSheet, xLeftScoreSheet, yTopScoreSheet);

	myBuffer.Clear();
    myBuffer.Printf("FLDG");
    dc.DrawText(myBuffer,xStartFLDG+5,yTopScoreSheet+5);

    // Insert statistics labels
    myBuffer.Clear();
    myBuffer.Printf("AB");
    dc.DrawText(myBuffer,xStartFLDG+4+(13*xIncInning),yTopScoreSheet+5);
    myBuffer.Clear();
    myBuffer.Printf("R");
    dc.DrawText(myBuffer,xStartFLDG+8+(13*xIncInning)+(1*xIncStats),yTopScoreSheet+5);
    myBuffer.Clear();
    myBuffer.Printf("H");
    dc.DrawText(myBuffer,xStartFLDG+8+(13*xIncInning)+(2*xIncStats),yTopScoreSheet+5);
    myBuffer.Clear();
    myBuffer.Printf("RBI");
    dc.DrawText(myBuffer,xStartFLDG+2+(13*xIncInning)+(3*xIncStats),yTopScoreSheet+5);
    myBuffer.Clear();
    myBuffer.Printf("2B");
    dc.DrawText(myBuffer,xStartFLDG+4+(13*xIncInning)+(4*xIncStats),yTopScoreSheet+5);
    myBuffer.Clear();
    myBuffer.Printf("3B");
    dc.DrawText(myBuffer,xStartFLDG+4+(13*xIncInning)+(5*xIncStats),yTopScoreSheet+5);
    myBuffer.Clear();
    myBuffer.Printf("HR");
    dc.DrawText(myBuffer,xStartFLDG+4+(13*xIncInning)+(6*xIncStats),yTopScoreSheet+5);
    myBuffer.Clear();
    myBuffer.Printf("W");
    dc.DrawText(myBuffer,xStartFLDG+8+(13*xIncInning)+(7*xIncStats),yTopScoreSheet+5);
    myBuffer.Clear();
    myBuffer.Printf("K");
    dc.DrawText(myBuffer,xStartFLDG+8+(13*xIncInning)+(8*xIncStats),yTopScoreSheet+5);
    myBuffer.Clear();
    myBuffer.Printf("RE");
    dc.DrawText(myBuffer,xStartFLDG+4+(13*xIncInning)+(9*xIncStats),yTopScoreSheet+5);
    myBuffer.Clear();
    myBuffer.Printf("SB");
    dc.DrawText(myBuffer,xStartFLDG+2+(13*xIncInning)+(10*xIncStats),yTopScoreSheet+1);
    myBuffer.Clear();
    myBuffer.Printf("CS");
    dc.DrawText(myBuffer,xStartFLDG+12+(13*xIncInning)+(10*xIncStats),yTopScoreSheet+13);

	// Summary Boxes for Inning Scores
	dc.DrawLine(xLeftScoreSheet,ySummaryTop,xRightSumScoreSheet,ySummaryTop);
	dc.DrawLine(xLeftScoreSheet,ySummaryTop+25,xRightSumScoreSheet,ySummaryTop+25);
	dc.DrawLine(xLeftScoreSheet,ySummaryTop+75,xRightSumScoreSheet,ySummaryTop+75);
	dc.DrawLine(xLeftScoreSheet,ySummaryTop+125,xRightSumScoreSheet,ySummaryTop+125);
	dc.DrawLine(xRightSumScoreSheet,ySummaryTop,xRightSumScoreSheet,ySummaryTop+125);
	dc.DrawLine(xLeftScoreSheet,ySummaryTop,xLeftScoreSheet,ySummaryTop+125);
	for (i=xLeftInningScoreSheet; i<xRightSumScoreSheet; i+=xBoxSize)
	{
		dc.DrawLine(i,ySummaryTop,i,ySummaryTop+125);
	}
    myBuffer.Clear();
    myBuffer.Printf("TEAMS");
    dc.DrawText(myBuffer,xLeftScoreSheet+15,ySummaryTop+5);
    myBuffer.Clear();
    myBuffer.Printf("R");
    dc.DrawText(myBuffer,(xLeftInningScoreSheet-xBoxSize+15)+(13*xBoxSize),ySummaryTop+5);
    myBuffer.Clear();
    myBuffer.Printf("H");
    dc.DrawText(myBuffer,(xLeftInningScoreSheet-xBoxSize+15)+(14*xBoxSize),ySummaryTop+5);
    myBuffer.Clear();
    myBuffer.Printf("E");
    dc.DrawText(myBuffer,(xLeftInningScoreSheet-xBoxSize+15)+(15*xBoxSize),ySummaryTop+5);

	// Pitcher Stats
	dc.DrawLine(xStopStats,yBottomScoreSheet+13,xStopStats,yBottomScoreSheet+13+(5*25));
	dc.DrawLine(xStopStats,yBottomScoreSheet+13+(5*25),xStopStats-100-(6*xBoxSize),
        yBottomScoreSheet+13+(5*25));
	dc.DrawLine(xStopStats-100-(6*xBoxSize),yBottomScoreSheet+13+(5*25),
        xStopStats-100-(6*xBoxSize),yBottomScoreSheet+13);
	dc.DrawLine(xStopStats-100-(6*xBoxSize),yBottomScoreSheet+13,xStopStats,
        yBottomScoreSheet+13);

	for (i=xStopStats-(6*xBoxSize);i<=xStopStats;i+=xBoxSize)
	{
		dc.DrawLine(i,yBottomScoreSheet+13,i,yBottomScoreSheet+13+(5*25));
	}
	for (i=yBottomScoreSheet+13+25; i<=yBottomScoreSheet+13+(5*25); i+=25)
	{
		dc.DrawLine(xStopStats-100-(6*xBoxSize),i,xStopStats,i);
	}

    myBuffer.Clear();
    myBuffer.Printf("PITCHER");
    dc.DrawText(myBuffer,xStopStats-100-(6*xBoxSize)+25,yBottomScoreSheet+13+5);
    myBuffer.Clear();
    myBuffer.Printf("IP");
    dc.DrawText(myBuffer,xStopStats-(6*xBoxSize)+11,yBottomScoreSheet+13+5);
    myBuffer.Clear();
    myBuffer.Printf("H");
    dc.DrawText(myBuffer,xStopStats-(6*xBoxSize)+15+(1*xBoxSize),yBottomScoreSheet+13+5);
    myBuffer.Clear();
    myBuffer.Printf("W");
    dc.DrawText(myBuffer,xStopStats-(6*xBoxSize)+12+(2*xBoxSize),yBottomScoreSheet+13+5);
    myBuffer.Clear();
    myBuffer.Printf("K");
    dc.DrawText(myBuffer,xStopStats-(6*xBoxSize)+15+(3*xBoxSize),yBottomScoreSheet+13+5);
    myBuffer.Clear();
    myBuffer.Printf("HR");
    dc.DrawText(myBuffer,xStopStats-(6*xBoxSize)+10+(4*xBoxSize),yBottomScoreSheet+13+5);
    myBuffer.Clear();
    myBuffer.Printf("ER");
    dc.DrawText(myBuffer,xStopStats-(6*xBoxSize)+10+(5*xBoxSize),yBottomScoreSheet+13+5);

	// Game Number box
	dc.DrawLine(xStopStats,yGameNumberTop+75,xStopStats,yGameNumberTop);
	dc.DrawLine(xStopStats,yGameNumberTop,xStopStats-100,yGameNumberTop);
	dc.DrawLine(xStopStats-100,yGameNumberTop,xStopStats-100,yGameNumberTop+75);
	dc.DrawLine(xStopStats-100,yGameNumberTop+75,xStopStats,yGameNumberTop+75);
	dc.DrawLine(xStopStats-100,yGameNumberTop+25,xStopStats,yGameNumberTop+25);

    myBuffer.Clear();
    myBuffer.Printf("GAME NUMBER");
    dc.DrawText(myBuffer,xStopStats-100+10,+yGameNumberTop+5);
}

