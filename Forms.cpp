/////////////////////////////////////////////////////////////////////////////
// Name:        Forms.cpp                                                  //
// Purpose:     Baseball statistics tracking tool for Strat-O-Matic        //
// Author:      John Gnew                                                  //
// Created:     09/15/04                                                   //
// Copyright:   (c) John Gnew                                              //
//                                                                         //
// Modifications:                                                          //
//   Date       Description                                                //
// 04/19/25     Changed FormA::Common to start the diammond in inning 1    //
//              instead of column FLDG                                     //
// 04/20/25     Changed top of print from 50 to 75 to allow for            //
//              3 hole punch                                               //
// 04/29/25     Created FormB which for Strat-o-matic should have been     //
//              FormA. So I mixed them up.                                 //
// 07/23/25     Made Team field larger in summery area                     //
//              Added Rest and Hold fields to FormA                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================

#define xLeftScoreSheet       55    // Left margin of sheet
#define xStartFLDG		      xLeftScoreSheet+156
//#define xLeftInningScoreSheet 144
#define xLeftInningScoreSheet 169
#define xBoxSize              37
#define xRightSumScoreSheet   xLeftInningScoreSheet+(15*xBoxSize)
// Positioned to allow for three hole punch
#define yTopScoreSheet	      75
#define yStartNames		      yTopScoreSheet+25
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

	wxGetApp().g_printData->SetOrientation ( wxLANDSCAPE );
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
	for (ix=xStartFLDG+xIncInning; ix<=xStopInning; ix+=xIncInning)
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

	// Draw box around scoresheet
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
    dc.DrawText(myBuffer,xLeftScoreSheet+40,ySummaryTop+5);
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

	// Batter Rest Box
	#define RestBoxLX 100-(6*xBoxSize)
	#define RestBoxRX RestBoxLX + 90
	dc.DrawLine(xStopStats-RestBoxRX,yGameNumberTop+75,xStopStats-RestBoxRX,yGameNumberTop);
	dc.DrawLine(xStopStats-RestBoxRX,yGameNumberTop,xStopStats-RestBoxLX,yGameNumberTop);
	dc.DrawLine(xStopStats-RestBoxLX,yGameNumberTop,xStopStats-RestBoxLX,yGameNumberTop+75);
	dc.DrawLine(xStopStats-RestBoxLX,yGameNumberTop+75,xStopStats-RestBoxRX,yGameNumberTop+75);
	dc.DrawLine(xStopStats-RestBoxLX,yGameNumberTop+25,xStopStats-RestBoxRX,yGameNumberTop+25);

    myBuffer.Clear();
    myBuffer.Printf("REST");
    dc.DrawText(myBuffer,xStopStats-RestBoxLX+30,+yGameNumberTop+5);

	// Hold Box
	#define HoldBoxLX RestBoxLX + 115
	#define HoldBoxRX RestBoxRX + 115
	dc.DrawLine(xStopStats-HoldBoxRX,yGameNumberTop+75,xStopStats-HoldBoxRX,yGameNumberTop);
	dc.DrawLine(xStopStats-HoldBoxRX,yGameNumberTop,xStopStats-HoldBoxLX,yGameNumberTop);
	dc.DrawLine(xStopStats-HoldBoxLX,yGameNumberTop,xStopStats-HoldBoxLX,yGameNumberTop+75);
	dc.DrawLine(xStopStats-HoldBoxLX,yGameNumberTop+75,xStopStats-HoldBoxRX,yGameNumberTop+75);
	dc.DrawLine(xStopStats-HoldBoxLX,yGameNumberTop+25,xStopStats-HoldBoxRX,yGameNumberTop+25);

    myBuffer.Clear();
    myBuffer.Printf("HOLD");
    dc.DrawText(myBuffer,xStopStats-HoldBoxLX+30,+yGameNumberTop+5);
}

FormB::FormB()
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

	wxGetApp().g_printData->SetOrientation ( wxPORTRAIT );
}

FormB::~FormB()
{
}

void FormB::OnDraw(wxDC& dc)
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
//    if ( wxGetApp().pWxBaseballFrame->pMenuBar->IsChecked(myID_SCORESHEET) )
//    {
//        Normal(dc);
//    }
//    else
//    {
//        Extended(dc);
//    }

    // Reset Font back to original font
    dc.SetFont(myOldFont);
}

void FormB::Common(wxDC& dc)
{
	// Batter section ends at Y 445 when it starts at 25 for a length or 420
	#define xStart		100
	#define yStart		50
	#define pxStart     550
	// Pitcher section Y starts at 460 ends at 640 so a length of 180
	int pyStart = yStart + 420 + 15;	// the 15 is seperator
	int bsyStart = pyStart + 180 + 15;
	#define bsyInc	40
	// Size of box sscore is 105, 25 for header and 2 times bsyInc
	int gameyStart = bsyStart + 105 + 15;
	int gamexEnd = xStart + 140;
	#define gameyInc	65
	#define bsSize		43
	#define xEnd		1050
	#define xInc		48
	#define yInc		20
	#define cols		16
	#define brows		21
	#define prows       9
	#define pcols		5
	#define name        182
	#define teamsize	176

	int i,j,jcol;
	int BoxScoreXInc;

	// Draw horizontal lines in batter part of form
	i = yStart;
	for (j=0; j<=brows; j++)
	{
//		dc.DrawLine(xStart,i,(xInc*cols)+(xStart+name),i);
		dc.DrawLine( xStart, i, xEnd, i);
		i+=yInc;
	}
	// Draw vertical lines in batter part of form
	// cols = 15, xEnd = 1050, name = 175
	dc.DrawLine( xStart, yStart, xStart, (yInc*brows)+yStart);
	jcol=xStart+name;
	for (j=0; j<=cols; j++)
	{
		dc.DrawLine( jcol, yStart, jcol, (yInc*brows)+yStart );
		jcol=jcol+xInc;
	}
    myBuffer.Clear();
    myBuffer.Printf("BATTER");
    dc.DrawText( myBuffer, xStart+60, yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("FLDG");
    dc.DrawText( myBuffer, xStart+name+5, yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("AB");
    dc.DrawText( myBuffer, xStart+name+15+(xInc*1), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("R");
    dc.DrawText( myBuffer, xStart+name+20+(xInc*2), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("H");
    dc.DrawText( myBuffer, xStart+name+20+(xInc*3), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("RBI");
    dc.DrawText( myBuffer, xStart+name+10+(xInc*4), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("2B");
    dc.DrawText( myBuffer, xStart+name+15+(xInc*5), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("3B");
    dc.DrawText( myBuffer, xStart+name+5+(xInc*6), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("HR");
    dc.DrawText( myBuffer, xStart+name+15+(xInc*7), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("W");
    dc.DrawText( myBuffer, xStart+name+20+(xInc*8), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("K");
    dc.DrawText( myBuffer, xStart+name+20+(xInc*9), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("E");
    dc.DrawText( myBuffer, xStart+name+20+(xInc*10), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("SB");
    dc.DrawText( myBuffer, xStart+name+15+(xInc*11), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("SF");
    dc.DrawText( myBuffer, xStart+name+15+(xInc*12), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("AVG");
    dc.DrawText( myBuffer, xStart+name+10+(xInc*13), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("SLG");
    dc.DrawText( myBuffer, xStart+name+10+(xInc*14), yStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("OB%%");
    dc.DrawText( myBuffer, xStart+name+9+(xInc*15), yStart+5 );

	// Pitcher section
	// Draw horizontal lines in pitcher part of form
	i = pyStart;
	int ipStart = xEnd - name - ( pcols * xInc );
	for (j=0; j<=prows; j++)
	{
		dc.DrawLine( ipStart, i, (xInc*pcols)+ipStart+name ,i );
		i+=yInc;
	}
	// Draw vertical lines in pitcher part of form
	dc.DrawLine( ipStart, pyStart, ipStart, (yInc*prows)+pyStart);
	jcol=ipStart+name;
	for (j=0; j<=pcols; j++)
	{
		dc.DrawLine(jcol,pyStart,jcol,(yInc*prows)+pyStart);
		jcol=jcol+xInc;
	}
    myBuffer.Clear();
    myBuffer.Printf("PITCHER");
    dc.DrawText( myBuffer, ipStart+60, pyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("IP");
    dc.DrawText( myBuffer, ipStart+name+15, pyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("H");
    dc.DrawText( myBuffer, ipStart+name+20+(xInc*1), pyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("W");
    dc.DrawText( myBuffer, ipStart+name+20+(xInc*2), pyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("K");
    dc.DrawText( myBuffer, ipStart+name+20+(xInc*3), pyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("ER");
    dc.DrawText( myBuffer, ipStart+name+15+(xInc*4), pyStart+5 );

	myBuffer.Clear();
    myBuffer.Printf("OTHER DATA:");
    dc.DrawText( myBuffer, xStart+5, pyStart+5 );
	myBuffer.Clear();
    myBuffer.Printf("COMMENTS:");
    dc.DrawText( myBuffer, xStart+5, pyStart+5+(yInc*4) );

	// Box Score section
	// Summary Boxes for Inning Scores
	// xRightBS = ( 50 * 15 ) + ( 50 + 175 ) = 975
//	int xRightBS = (xInc*cols)+(xStart+name);
	dc.DrawLine( xStart, bsyStart, xEnd, bsyStart );
	dc.DrawLine( xStart, bsyStart+25, xEnd, bsyStart+25 );
	dc.DrawLine( xStart, bsyStart+bsyInc+25, xEnd, bsyStart+bsyInc+25 );
	dc.DrawLine( xStart, bsyStart+(bsyInc*2)+25, xEnd, bsyStart+(bsyInc*2)+25 );
	// Draw Right Vertical line
	dc.DrawLine( xEnd, bsyStart, xEnd, bsyStart+(bsyInc*2)+25 );
	// Draw Left Vertical line
	dc.DrawLine( xStart, bsyStart, xStart, bsyStart+(bsyInc*2)+25 );
	// xEnd = 1050, BoxScoreXInc = 43, teamsize = 176
	BoxScoreXInc = ( xEnd - teamsize - xStart ) / 18;
	for (i=0; i<=18; i++)
	{
		dc.DrawLine( xStart+teamsize+(i*BoxScoreXInc), bsyStart, xStart+teamsize+(i*BoxScoreXInc), bsyStart+(bsyInc*2)+25 );
	}
    myBuffer.Clear();
    myBuffer.Printf("TEAMS");
    dc.DrawText(myBuffer,xStart+50,bsyStart+5);
    myBuffer.Clear();
    myBuffer.Printf("1");
    dc.DrawText(myBuffer,( xStart+teamsize+20 ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("2");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(1*BoxScoreXInc) ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("3");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(2*BoxScoreXInc) ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("4");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(3*BoxScoreXInc) ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("5");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(4*BoxScoreXInc) ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("6");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(5*BoxScoreXInc) ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("7");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(6*BoxScoreXInc) ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("8");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(7*BoxScoreXInc) ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("9");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(8*BoxScoreXInc) ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("R");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(15*BoxScoreXInc) ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("H");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(16*BoxScoreXInc) ), bsyStart+5 );
    myBuffer.Clear();
    myBuffer.Printf("E");
    dc.DrawText(myBuffer,( xStart+teamsize+20+(17*BoxScoreXInc) ), bsyStart+5 );

   	dc.DrawLine( xStart, gameyStart, gamexEnd, gameyStart );
	dc.DrawLine( xStart, gameyStart+25, gamexEnd, gameyStart+25 );
	dc.DrawLine( xStart, gameyStart+gameyInc, gamexEnd, gameyStart+gameyInc );

	dc.DrawLine( xStart, gameyStart, xStart, gameyStart+gameyInc );
	dc.DrawLine( gamexEnd, gameyStart, gamexEnd, gameyStart+gameyInc );

	myBuffer.Clear();
    myBuffer.Printf("GAME NUMBER");
    dc.DrawText( myBuffer, xStart+7, gameyStart+5 );
}

