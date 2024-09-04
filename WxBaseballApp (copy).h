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

#include <wx/app.h>

class WxBaseballApp : public wxApp
{
    public:
        virtual bool OnInit();
};

#endif // WXBASEBALLAPP_H
