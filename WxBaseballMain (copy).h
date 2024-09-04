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

#include "WxBaseballApp.h"

class WxBaseballFrame: public wxFrame
{
    public:
        WxBaseballFrame(wxFrame *frame, const wxString& title);
        ~WxBaseballFrame();
    private:
        enum
        {
            idMenuQuit = 1000,
            idMenuAbout
        };
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        DECLARE_EVENT_TABLE()
};


#endif // WXBASEBALLMAIN_H
