/////////////////////////////////////////////////////////////////////////////
// Name:        FileRoutines.h                                             //
// Purpose:     Baseball statistics tracking tool for Strat-O-Matic        //
// Author:      John Gnew                                                  //
// Created:     09/22/24                                                   //
// Copyright:   (c) John Gnew                                              //
// Modifications:                                                          //
//   Date       Description                                                //
// ??/??/??     X                                                          //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
/////////////////////////////////////////////////////////////////////////////

#ifndef _FILEROUTINES_H_
#define _FILEROUTINES_H_

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/wx.h>

#include <wx/file.h>
#include <wx/string.h>
#include "sqlite3.h"
//#include "DBRoutines.h"

class FileRoutines
{
	public:
		FileRoutines();
		virtual ~FileRoutines();
		int ExportTeam( int passedTeamID );
		void BuildPlayerStats( int leagueID, int conferenceID, int divisionID );

	protected:

	private:
};

#endif // _FILEROUTINES_H_
