/***************************************************************
 * Name:      MyCanvas.cpp
 * Purpose:   Code for Canvas
 * Author:    John Gnew (john.gnew@verizon.net)
 * Created:   2024-03-22
 * Copyright: John Gnew ()
 * License:
 **************************************************************/

#include "MyCanvas.h"
#include "Forms.h"


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
