/////////////////////////////////////////////////////////////////////////////
// Name:        print.h
// Purpose:     Printing
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

class MyPrintout: public wxPrintout
{
 public:
  MyPrintout(const wxString &title = "My printout")
        : wxPrintout(title) {}
  ~MyPrintout();

  bool OnPrintPage(int page);
  bool HasPage(int page);
  bool OnBeginDocument(int startPage, int endPage);
  void GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo);

  void DrawPageOne(wxDC* dc);
};

#define WXPRINT_QUIT            100
#define WXPRINT_PRINT           101
#define WXPRINT_PRINT_SETUP     102
#define WXPRINT_PAGE_SETUP      103
#define WXPRINT_PREVIEW         104

#define WXPRINT_PRINT_PS        105
#define WXPRINT_PRINT_SETUP_PS  106
#define WXPRINT_PAGE_SETUP_PS   107
#define WXPRINT_PREVIEW_PS      108

#define WXPRINT_ABOUT           109

