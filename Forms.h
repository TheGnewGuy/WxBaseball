/////////////////////////////////////////////////////////////////////////////
// Name:        Forms.h                                                    //
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

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface
#endif

#include <wx/dc.h>

class FormA
{
protected:

public:
	FormA();
	void OnDraw(wxDC& dc);
	void Common(wxDC& dc);
	void Normal(wxDC& dc);
	void Extended(wxDC& dc);
	~FormA();

private:
   	int xStopInning, xStopStats, yBottomScoreSheet;
	int yGameNumberTop;
	int ySummaryTop;
	int ix, iy;
	int i;
	wxString myBuffer;

    wxFont myTimesNewRomanFont;
    wxString myOrigFontName;

};

