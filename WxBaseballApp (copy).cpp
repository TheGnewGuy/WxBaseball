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

bool WxBaseballApp::OnInit()
{
    WxBaseballFrame* frame = new WxBaseballFrame(0L, _("wxWidgets Application Template"));
    
    frame->Show();
    
    return true;
}
