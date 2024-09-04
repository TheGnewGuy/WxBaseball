/***************************************************************
 * Name:      MyCanvas.h
 * Purpose:   Defines Application Class
 * Author:    John Gnew (john.gnew@verizon.net)
 * Created:   2024-03-22
 * Copyright: John Gnew ()
 * License:
 **************************************************************/

#ifndef MYCANVAS_H
#define MYCANVAS_H

#include <wx/wx.h>
//#include <wx/dc.h>
#include <wx/cmndata.h>

// Define a new canvas which can receive some events
class MyCanvas: public wxScrolledWindow
{
    public:
        MyCanvas(wxFrame *frame, wxWindowID id, const wxPoint& pos, const wxSize& size);
        virtual ~MyCanvas();
    // OnDraw
        virtual void OnDraw(wxDC& dc);

//    void OnPaint(wxPaintEvent& event);
//    void OnMouseEvent(wxMouseEvent& event);
//    void OnChar(wxKeyEvent& event);

        DECLARE_EVENT_TABLE()

    protected:

    private:
};

#endif // MYCANVAS_H
