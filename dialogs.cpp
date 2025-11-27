/////////////////////////////////////////////////////////////////////////////
// Name:          dialogs.cpp                                              //
// Purpose:       Baseball statistics tracking tool for Strat-O-Matic      //
// Author:        John Gnew                                                //
// Created:       12/14/04                                                 //
// Copyright:     (c) John Gnew                                            //
// Modifications:                                                          //
//   Date       Description                                                //
// 06/27/07     Correct combo box sizes for batter and pitcher info dialgs //
// 12/02/05     Wrote code to update batters and pitchers                  //
// 09/30/05     Wrote code to create the pitcher notebook                  //
// 09/28/05     Wrote code to populate the batters dialog fields           //
//              and to set field change flags.                             //
// 07/28/05     Added comments for Todo list                               //
// 09/15/24     Corrected code for OutArm, CatchArm and Running            //
// 09/19/24     Added batter insert code                                   //
// 12/02/24     Changed bool UpdateBatterNames to void                     //
//              Changed bool UpdatePitcherNames to void                    //
//              The routines in the release mode did not return a value    //
//              and were causing the app to close                          //
//              Power Right text was 75, change to 85                      //
//              Updated BatterNotebook::GetNotebookData to calculate       //
//              AVG, SLG and OBP and store in the structure                //
//              Updated BatterNotebook::OnComboPitcherSelect to load       //
//              AVG, SLG and OBP from the structure                        //
//              Updated PitcherNotebook::GetNotebookData to calculate      //
//              ERA and WHIP and store in structure                        //
//              Updated PitcherNotebook::OnComboPitcherSelect to load      //
//              ERA and WHIP from the structure                            //
// 12/08/24     Set values for relief pitcher to be 0 to 5 insetad of      //
//              0 to 4                                                     //
// 01/09/25     Set defaults for OnNew pitcher starter and Relief to 0     //
//              Instead of 1                                               //
// 01/16/25     Changed default chance from "0     " to "0.0"              //
// 03/18/25     Changed GetTeamNamesArray calls so that notebooks for      //
//              batters and pitchers do not display when league and team   //
//              selections are canceled                                    //
//              In BatterNotebook::GetNotebookData changed the calculation //
//              for OBP to (H+BB+HBP)/(AB+BB+HBP+SF). Previously it was    //
//              including multiple other fields.                           //
// 03/24/25     Set fields on the batter and pitcher stat pages to display //
//              RED when changed and them bake to BLACK when done          //
//              Changes were to select B/P, ON Apply, On New.              //
//              Special consideratoin were for Pitcher IP as it is a text  //
//              field and not a spin.                                      //
// 04/21/25     Set code to disable the ADD button until NEW is pressed    //
//              adding batters and pitchers. This involved changing        //
//              BuildControlButtons for unique variables for each page     //
// 07/06/25     Set default notebooks page to Stat for Batters and         //
//              Pitchers. Then for New, set default to Info.               //
// 09/15/25     Centered various dialogs using CenterOnParent();           //
//              This became a problem after rebuilding Linux and using     //
//              WxWidgets 3.2.8 which upgraded from 3.2.6                  //
// 09/17/25     Added field HittingCard to Pitcher data and Dialogs        //
// 10/06/25     Created a post event in batter and pitcher notebooks       //
//              OnApply routines to update and display the new stat        //
//              information. Before we needed to select a different        //
//              player and then go back to the updated player.             //
// 11/27/25     Created CheckEntriesDialog dialog to allow for the         //
//              checking of data entered for Hits, Walks, Strikeouts       //
//              and HRs. This displays the batter and pitcher values       //
//              which should be the same.                                  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
//       Correct messages when Apply is pressed                            //
//       Force NEW to be pressed prior to allowing the activation of the   //
//          Add button                                                     //
//       In AddBatter, think about renumbering tabs since 4 became tab 1   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// declarations
// ===========================================================================
//#define __WXGTK__


// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/dialog.h>
    #include <wx/statline.h>
    #include <wx/dynarray.h>
    #include <wx/file.h>
#endif

#include "dialogs.h"
#include "WxBaseballMain.h"
#include "WxBaseballApp.h"
#include "FileStruct.h"
#include "DBRoutines.h"

// ---------------------------------------------------------------------------
// events
// ---------------------------------------------------------------------------

// Batter enum
enum
{
    ID_NOTEBOOK_BATTER          = wxID_HIGHEST + 1,   // Start at 6000
    ID_LISTBOX_BATTER_CLEAR,
    ID_COMBO_BATTER,
    ID_COMBO_BATTER_TEAM,
    ID_POSITION_BATTER_01,
    ID_POSITION_BATTER_02,
    ID_POSITION_BATTER_03,
    ID_POSITION_BATTER_04,
    ID_POSITION_BATTER_05,
    ID_POSITION_BATTER_06,
    ID_POSITION_BATTER_07,
    ID_POSITION_BATTER_08,
    ID_POSITION_BATTER_09,
    ID_POSITION_BATTER_ER01,
    ID_POSITION_BATTER_ER02,
    ID_POSITION_BATTER_ER03,
    ID_POSITION_BATTER_ER04,
    ID_POSITION_BATTER_ER05,
    ID_POSITION_BATTER_ER06,
    ID_POSITION_BATTER_ER07,
    ID_POSITION_BATTER_ER08,
    ID_POSITION_BATTER_ER09,
    ID_CHANCE_BATTER_BASIC,
    ID_CHANCE_BATTER_RIGHT,
    ID_CHANCE_BATTER_LEFT,
    ID_CHANCE_BATTER_B1B,
    ID_CHANCE_BATTER_B2B,
    ID_CHANCE_BATTER_B3B,
    ID_CHANCE_BATTER_BHR,
    ID_CHANCE_BATTER_BW,
    ID_CHANCE_BATTER_BDP,
    ID_CHANCE_BATTER_L1B,
    ID_CHANCE_BATTER_L2B,
    ID_CHANCE_BATTER_L3B,
    ID_CHANCE_BATTER_LHR,
    ID_CHANCE_BATTER_LW,
    ID_CHANCE_BATTER_LDP,
    ID_CHANCE_BATTER_R1B,
    ID_CHANCE_BATTER_R2B,
    ID_CHANCE_BATTER_R3B,
    ID_CHANCE_BATTER_RHR,
    ID_CHANCE_BATTER_RW,
    ID_CHANCE_BATTER_RDP,
    ID_STATS_BATTER_AB,
    ID_STATS_BATTER_2B,
    ID_STATS_BATTER_3B,
    ID_STATS_BATTER_HR,
    ID_STATS_BATTER_HITS,
    ID_STATS_BATTER_SACRICIFE,
    ID_STATS_BATTER_SB,
    ID_STATS_BATTER_CS,
    ID_STATS_BATTER_K,
    ID_STATS_BATTER_W,
    ID_STATS_BATTER_HBP,
    ID_STATS_BATTER_GAMES,
    ID_STATS_BATTER_RBI,
    ID_STATS_BATTER_RUNS,
    ID_STATS_BATTER_RE,
    ID_STATS_BATTER_AVG,
    ID_STATS_BATTER_SLG,
    ID_STATS_BATTER_OBP,
    ID_INFO_BATTER_FIRSTNAME,
    ID_INFO_BATTER_LASTNAME,
    ID_INFO_BATTER_BATTERHITS,
    ID_INFO_BATTER_POWERLEFT,
    ID_INFO_BATTER_POWERRIGHT,
    ID_INFO_BATTER_OUTFIELDARM,
    ID_INFO_BATTER_CATCHERARM,
    ID_INFO_BATTER_PASSBALL,
    ID_INFO_BATTER_STEALING,
    ID_INFO_BATTER_BUNTING,
    ID_INFO_BATTER_HITRUN,
    ID_INFO_BATTER_RUNNING,
    ID_INFO_BATTER_TRATING
};

// Pitcher enum
enum
{
    ID_NOTEBOOK_PITCHER          = wxID_HIGHEST + 200,   // Start at 6200
    ID_LISTBOX_PITCHER_CLEAR,
    ID_COMBO_PITCHER,
    ID_COMBO_PITCHER_TEAM,
    ID_CHANCE_PITCHER_BASIC,
    ID_CHANCE_PITCHER_RIGHT,
    ID_CHANCE_PITCHER_LEFT,
    ID_CHANCE_PITCHER_B1B,
    ID_CHANCE_PITCHER_B2B,
    ID_CHANCE_PITCHER_B3B,
    ID_CHANCE_PITCHER_BHR,
    ID_CHANCE_PITCHER_BW,
    ID_CHANCE_PITCHER_BDP,
    ID_CHANCE_PITCHER_L1B,
    ID_CHANCE_PITCHER_L2B,
    ID_CHANCE_PITCHER_L3B,
    ID_CHANCE_PITCHER_LHR,
    ID_CHANCE_PITCHER_LW,
    ID_CHANCE_PITCHER_LDP,
    ID_CHANCE_PITCHER_R1B,
    ID_CHANCE_PITCHER_R2B,
    ID_CHANCE_PITCHER_R3B,
    ID_CHANCE_PITCHER_RHR,
    ID_CHANCE_PITCHER_RW,
    ID_CHANCE_PITCHER_RDP,
    ID_STATS_PITCHER_WINS,
    ID_STATS_PITCHER_LOSS,
    ID_STATS_PITCHER_STARTS,
    ID_STATS_PITCHER_SAVE,
    ID_STATS_PITCHER_IP,
    ID_STATS_PITCHER_HITS,
    ID_STATS_PITCHER_GAMES,
    ID_STATS_PITCHER_CGAMES,
    ID_STATS_PITCHER_K,
    ID_STATS_PITCHER_W,
    ID_STATS_PITCHER_HR,
    ID_STATS_PITCHER_ER,
    ID_STATS_PITCHER_ERA,
    ID_STATS_PITCHER_WHIP,
    ID_INFO_PITCHER_FIRSTNAME,
    ID_INFO_PITCHER_LASTNAME,
    ID_INFO_PITCHER_STARTER,
    ID_INFO_PITCHER_RELIEF,
    ID_INFO_PITCHER_BALK,
    ID_INFO_PITCHER_P,
    ID_INFO_PITCHER_HITTINGCARD,
    ID_INFO_PITCHER_ERP,
    ID_INFO_PITCHER_WP,
    ID_INFO_PITCHER_THROWS,
    ID_INFO_PITCHER_BUNTING,
    ID_INFO_PITCHER_HOLD,
};
// Pitcher enum End

// League enum
enum
{
    ID_LEAGUE_LEAGUENAME     = wxID_HIGHEST + 400,   // Start at 6400
    ID_LEAGUE_YEAR,
    ID_LEAGUE_BASE,
};

// Optione enum
enum
{
    ID_OPTIONS_DATADIR     = wxID_HIGHEST + 500,   // Start at 6400
    ID_OPTIONS_IMAGE,
    ID_OPTIONS_HTMLINDEX,
    ID_OPTIONS_HTMLBG,
    ID_OPTIONS_HTMLLINKCOLOR,
    ID_OPTIONS_HTMLVLINKCOLOR,
    ID_OPTIONS_HTMLBGCOLOR,
    ID_OPTIONS_HTMLTEXTCOLOR,
};
// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(BatterNotebook, wxPanel)
    //  Controls
    EVT_BUTTON(wxID_APPLY, BatterNotebook::OnApply)
    EVT_BUTTON(wxID_ADD, BatterNotebook::OnAdd)
    EVT_BUTTON(wxID_NEW, BatterNotebook::OnNew)
    EVT_BUTTON(wxID_OK, BatterNotebook::OnOK)
    EVT_BUTTON(wxID_CANCEL, BatterNotebook::OnCancel)
    EVT_COMBOBOX(ID_COMBO_BATTER, BatterNotebook::OnComboBatterSelect)
    EVT_COMBOBOX(ID_COMBO_BATTER_TEAM, BatterNotebook::OnComboTeamSelect)

    // Event for Position Panel
    EVT_SPINCTRL(ID_POSITION_BATTER_01, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_02, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_03, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_04, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_05, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_06, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_07, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_08, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_09, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_ER01, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_ER02, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_ER03, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_ER04, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_ER05, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_ER06, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_ER07, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_ER08, BatterNotebook::OnComboPositionChangeSpin)
    EVT_SPINCTRL(ID_POSITION_BATTER_ER09, BatterNotebook::OnComboPositionChangeSpin)
    // For Spin we must still check Text updates
    EVT_TEXT(ID_POSITION_BATTER_01, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_02, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_03, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_04, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_05, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_06, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_07, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_08, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_09, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_ER01, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_ER02, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_ER03, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_ER04, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_ER05, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_ER06, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_ER07, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_ER08, BatterNotebook::OnComboPositionChangeText)
    EVT_TEXT(ID_POSITION_BATTER_ER09, BatterNotebook::OnComboPositionChangeText)

    // Events for Chance Panel
    EVT_TEXT(ID_CHANCE_BATTER_BASIC, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_RIGHT, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_LEFT, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_B1B, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_B2B, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_B3B, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_BHR, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_BW, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_BDP, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_L1B, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_L2B, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_L3B, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_LHR, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_LW, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_LDP, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_R1B, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_R2B, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_R3B, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_RHR, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_RW, BatterNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_BATTER_RDP, BatterNotebook::OnComboChanceChange)

    // Events for Stats Panel
    EVT_SPINCTRL(ID_STATS_BATTER_AB, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_2B, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_3B, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_HR, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_HITS, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_SACRICIFE, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_SB, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_CS, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_W, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_K, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_HBP, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_GAMES, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_RBI, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_RUNS, BatterNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_BATTER_RE, BatterNotebook::OnComboStatsChangeSpin)
    // For Spin we must still check Text updates
    EVT_TEXT(ID_STATS_BATTER_AB, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_2B, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_3B, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_HR, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_HITS, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_SACRICIFE, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_SB, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_CS, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_W, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_K, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_HBP, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_GAMES, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_RBI, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_RUNS, BatterNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_BATTER_RE, BatterNotebook::OnComboStatsChangeText)

    // Events for Info Panel
    EVT_COMBOBOX(ID_INFO_BATTER_BATTERHITS, BatterNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_BATTER_POWERLEFT, BatterNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_BATTER_POWERRIGHT, BatterNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_BATTER_OUTFIELDARM, BatterNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_BATTER_CATCHERARM, BatterNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_BATTER_PASSBALL, BatterNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_BATTER_STEALING, BatterNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_BATTER_BUNTING, BatterNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_BATTER_HITRUN, BatterNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_BATTER_RUNNING, BatterNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_BATTER_TRATING, BatterNotebook::OnComboInfoChange)

//    EVT_TEXT(ID_COMBO_BATTER, BatterNotebook::OnComboBatterSelect)
//    EVT_TEXT_ENTER(ID_COMBO_BATTER, BatterNotebook::OnComboBatterSelect)
//    EVT_NOTEBOOK_PAGE_CHANGING(ID_NOTEBOOK_BATTER, BatterNotebook::OnPageChanging)
    EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK_BATTER, BatterNotebook::OnPageChanged)
END_EVENT_TABLE()

BatterNotebook::BatterNotebook( wxDialog* frame, int x, int y, int w, int h )
       : wxPanel( frame, wxID_ANY, wxPoint(x, y), wxSize(w, h) )
{
    // Initialize all of the Notebook variables
    Initialization( );
	// Create a Notebook which will have four pages
	// x, y, w, and h are used to create the notebook the same
	// size as the dialog.
	CreateNotebook( x, y, w, h);

	// Set all of the defaults for spin, text and combos controls
	DefaultPanelValues( );
}

BatterNotebook::~BatterNotebook( )
{
}

void BatterNotebook::Initialization( )
{
    int rc;

    // Clear all change flags
    m_bSetValueFlagBatter = FALSE;
    m_bChangePositionFlag = FALSE;
    m_bChangeChanceFlag = FALSE;
    m_bChangeStatsFlag = FALSE;
    m_bChangeInfoFlag = FALSE;

    m_pNotebook = NULL;
    m_panel_01 = NULL;
    m_panel_02 = NULL;
    m_panel_03 = NULL;
    m_panel_04 = NULL;

    m_combo_batter_01 = NULL;
    m_combo_batter_02 = NULL;
    m_combo_batter_03 = NULL;
    m_combo_batter_04 = NULL;
    m_text_team_01 = NULL;
    m_text_team_02 = NULL;
    m_text_team_03 = NULL;
    m_text_team_04 = NULL;

    m_spinPosition1 = NULL;
    m_spinPosition2 = NULL;
    m_spinPosition3 = NULL;
    m_spinPosition4 = NULL;
    m_spinPosition5 = NULL;
    m_spinPosition6 = NULL;
    m_spinPosition7 = NULL;
    m_spinPosition8 = NULL;
    m_spinPosition9 = NULL;
    m_spinPositionER1 = NULL;
    m_spinPositionER2 = NULL;
    m_spinPositionER3 = NULL;
    m_spinPositionER4 = NULL;
    m_spinPositionER5 = NULL;
    m_spinPositionER6 = NULL;
    m_spinPositionER7 = NULL;
    m_spinPositionER8 = NULL;
    m_spinPositionER9 = NULL;

    m_textChanceBasic = NULL;
    m_textChanceLeft = NULL;
    m_textChanceRight = NULL;
    m_textChanceB1B = NULL;
    m_textChanceB2B = NULL;
    m_textChanceB3B = NULL;
    m_textChanceBHR = NULL;
    m_textChanceBW = NULL;
    m_textChanceBDP = NULL;
    m_textChanceL1B = NULL;
    m_textChanceL2B = NULL;
    m_textChanceL3B = NULL;
    m_textChanceLHR = NULL;
    m_textChanceLW = NULL;
    m_textChanceLDP = NULL;
    m_textChanceR1B = NULL;
    m_textChanceR2B = NULL;
    m_textChanceR3B = NULL;
    m_textChanceRHR = NULL;
    m_textChanceRW = NULL;
    m_textChanceRDP = NULL;

    m_spinStats2B = NULL;
    m_spinStats3B = NULL;
    m_spinStatsHR = NULL;
    m_spinStatsAB = NULL;
    m_spinStatsHITS = NULL;
    m_spinStatsRBI = NULL;
    m_spinStatsRE = NULL;
    m_spinStatsRUNS = NULL;
    m_spinStatsSACRICIFE = NULL;
    m_spinStatsSB = NULL;
    m_spinStatsCS = NULL;
    m_spinStatsK = NULL;
    m_spinStatsW = NULL;
    m_spinStatsHBP = NULL;
    m_spinStatsGAMES = NULL;
    m_textStatsAVG = NULL;
    m_textStatsSLG = NULL;
    m_textStatsOBP = NULL;

    m_combo_BatterHits = NULL;
    m_combo_PowerLeft = NULL;
    m_combo_PowerRight = NULL;
    m_combo_OutfieldArm = NULL;
    m_combo_CatcherArm = NULL;
    m_combo_Passball = NULL;
    m_combo_Stealing = NULL;
    m_combo_Bunting = NULL;
    m_combo_HitRun = NULL;
    m_combo_Running = NULL;
    m_combo_TRating = NULL;

    m_arrayLRS.Add("L");
    m_arrayLRS.Add("R");
    m_arrayLRS.Add("S");
    m_arrayPower.Add("N");
    m_arrayPower.Add("W");
    m_arrayOutfieldArm.Add("-6");
    m_arrayOutfieldArm.Add("-5");
    m_arrayOutfieldArm.Add("-4");
    m_arrayOutfieldArm.Add("-3");
    m_arrayOutfieldArm.Add("-2");
    m_arrayOutfieldArm.Add("-1");
    m_arrayOutfieldArm.Add("0");
    m_arrayOutfieldArm.Add("1");
    m_arrayOutfieldArm.Add("2");
    m_arrayOutfieldArm.Add("3");
    m_arrayOutfieldArm.Add("4");
    m_arrayOutfieldArm.Add("5");
    m_arrayCatcherArm.Add("-4");
    m_arrayCatcherArm.Add("-3");
    m_arrayCatcherArm.Add("-2");
    m_arrayCatcherArm.Add("-1");
    m_arrayCatcherArm.Add("0");
    m_arrayCatcherArm.Add("1");
    m_arrayCatcherArm.Add("2");
    m_arrayCatcherArm.Add("3");
    m_arrayCatcherArm.Add("4");
    m_arrayPassball.Add("0");
    m_arrayPassball.Add("1");
    m_arrayPassball.Add("2");
    m_arrayPassball.Add("3");
    m_arrayPassball.Add("4");
    m_arrayPassball.Add("5");
    m_arrayPassball.Add("6");
    m_arrayPassball.Add("7");
    m_arrayPassball.Add("8");
    m_arrayPassball.Add("9");
    m_arrayPassball.Add("10");
    m_arrayPassball.Add("11");
    m_arrayPassball.Add("12");
    m_arrayPassball.Add("13");
    m_arrayPassball.Add("14");
    m_arrayPassball.Add("15");
    m_arrayPassball.Add("16");
    m_arrayPassball.Add("17");
    m_arrayPassball.Add("18");
    m_arrayPassball.Add("19");
    m_arrayPassball.Add("20");
    m_arrayStealing.Add("AAA");
    m_arrayStealing.Add("AA");
    m_arrayStealing.Add("A");
    m_arrayStealing.Add("B");
    m_arrayStealing.Add("C");
    m_arrayStealing.Add("D");
    m_arrayStealing.Add("E");
    m_arrayBunting.Add("A");
    m_arrayBunting.Add("B");
    m_arrayBunting.Add("C");
    m_arrayBunting.Add("D");
    m_arrayHitRun.Add("A");
    m_arrayHitRun.Add("B");
    m_arrayHitRun.Add("C");
    m_arrayHitRun.Add("D");
    m_arrayRunning.Add("5");
    m_arrayRunning.Add("6");
    m_arrayRunning.Add("7");
    m_arrayRunning.Add("8");
    m_arrayRunning.Add("9");
    m_arrayRunning.Add("10");
    m_arrayRunning.Add("11");
    m_arrayRunning.Add("12");
    m_arrayRunning.Add("13");
    m_arrayRunning.Add("14");
    m_arrayRunning.Add("15");
    m_arrayRunning.Add("16");
    m_arrayRunning.Add("17");
    m_arrayRunning.Add("18");
    m_arrayRunning.Add("19");
    m_arrayTRating.Add("0");
    m_arrayTRating.Add("1");
    m_arrayTRating.Add("2");
    m_arrayTRating.Add("3");
    m_arrayTRating.Add("4");
    m_arrayTRating.Add("5");
    m_arrayTRating.Add("6");
    m_arrayTRating.Add("7");
    m_arrayTRating.Add("8");
    m_arrayTRating.Add("9");
    m_arrayTRating.Add("10");
    m_arrayTRating.Add("11");
    m_arrayTRating.Add("12");
    m_arrayTRating.Add("13");
    m_arrayTRating.Add("14");
    m_arrayTRating.Add("15");
    m_arrayTRating.Add("16");
    m_arrayTRating.Add("17");
    m_arrayTRating.Add("18");
    m_arrayTRating.Add("19");
    m_arrayTRating.Add("20");
}

// Used to create the Batter Notebook and its four panels
void BatterNotebook::CreateNotebook(int x, int y, int w, int h)
{
    // Create a Notebook which will have four pages
    m_pNotebook = new wxNotebook(this, ID_NOTEBOOK_BATTER, wxPoint(x, y),
                       wxSize(w, h));

    // Add Position Notebook page
    m_panel_01 = new wxPanel(m_pNotebook);
    m_panel_01->SetAutoLayout( true );

    sizer_01 = new wxBoxSizer( wxVERTICAL );

    sizer_01->Add( BuildTopCombos( m_panel_01, 1 ),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND, 0, nullptr );

    sizer_01->Add( new wxStaticText(m_panel_01, wxID_ANY,
            _T("Position and Error Ratings")),
            0, wxBOTTOM|wxALIGN_CENTER, 15);

    wxGridSizer* m_hGridSizer01 = new wxGridSizer( 5, 8, 0, 0 );

    // Add text to spin button
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("P"),
                    wxDefaultPosition, wxSize(15,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPosition1 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_01,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPosition1, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("ER1"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPositionER1 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_ER01,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPositionER1, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("3B"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPosition5 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_05,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPosition5, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("ER5"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPositionER5 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_ER05,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPositionER5, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("C"),
                    wxDefaultPosition, wxSize(15,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPosition2 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_02,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPosition2, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("ER2"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPositionER2 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_ER02,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPositionER2, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("LF"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPosition7 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_07,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPosition7, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("ER7"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPositionER7 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_ER07,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPositionER7, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("1B"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPosition3 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_03,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPosition3, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("ER3"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPositionER3 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_ER03,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPositionER3, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("CF"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPosition8 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_08,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPosition8, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("ER8"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPositionER8 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_ER08,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPositionER8, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("2B"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPosition4 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_04,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPosition4, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("ER4"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPositionER4 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_ER04,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPositionER4, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("RF"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPosition9 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_09,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPosition9, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("ER9"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPositionER9 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_ER09,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPositionER9, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("SS"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPosition6 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_06,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPosition6, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY, _T("ER6"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinPositionER6 = new wxSpinCtrl( m_panel_01, ID_POSITION_BATTER_ER06,
                    wxEmptyString, wxDefaultPosition, wxSize(50,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer01->Add( m_spinPositionER6, 0, wxALIGN_LEFT|wxLEFT, 0 );

    sizer_01->Add (m_hGridSizer01, 1 );
//    sizer_01->Add (m_hGridSizer01, 1, wxEXPAND);

    sizer_01->Add ( BuildControlButtons(m_panel_01, 1),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND);

    m_panel_01->SetSizer( sizer_01 );

    m_pNotebook->AddPage(m_panel_01, _T("Position"), false, -1);

    // Add Chance Notebook page
    m_panel_02 = new wxPanel(m_pNotebook);

    sizer_02 = new wxBoxSizer( wxVERTICAL );

    sizer_02->Add ( BuildTopCombos( m_panel_02, 2 ),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND );

    sizer_02->Add( new wxStaticText(m_panel_02, wxID_ANY,
            _T("Chance")),
            0, wxBOTTOM|wxALIGN_CENTER, 15);

    wxBoxSizer* sizer_02_HB = new wxBoxSizer( wxHORIZONTAL );

    sizer_02_HB->Add( 1, 1 );
//    sizer_02_HB->Add( 1, 1, wxEXPAND );
    sizer_02_HB->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("Basic"),
                    wxDefaultPosition, wxSize(40,-1), 0), 0, wxALIGN_CENTER_VERTICAL, 0 );
//                    wxDefaultPosition, wxSize(35,-1), 0), 0, wxALIGN_RIGHT, 0 );
    m_textChanceBasic = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_BASIC, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    sizer_02_HB->Add( m_textChanceBasic, 0, wxALIGN_LEFT, 0 );
    sizer_02_HB->Add( 1, 1 );
//    sizer_02_HB->Add( 1, 1, wxEXPAND );
    sizer_02->Add ( sizer_02_HB, 0, wxALIGN_CENTER, 0 );
//    sizer_02->Add ( sizer_02_HB, 0, wxALIGN_CENTER | wxEXPAND, 0 );

    wxGridSizer* m_hGridSizerB02 = new wxGridSizer( 2, 6, 0, 0 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("1B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceB1B = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_B1B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceB1B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("HR"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceBHR = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_BHR, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceBHR, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("2B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceB2B = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_B2B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceB2B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("W"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceBW = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_BW, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceBW, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("3B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceB3B = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_B3B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceB3B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("DP"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceBDP = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_BDP, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceBDP, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    sizer_02->Add( m_hGridSizerB02, 0, wxALIGN_CENTER );
//    sizer_02->Add( m_hGridSizerB02, 0, wxALIGN_CENTER | wxEXPAND );

    // Add a space after Basic
    sizer_02->Add ( 1, 20, wxSTRETCH_NOT );

    wxBoxSizer* sizer_02_HLR = new wxBoxSizer( wxHORIZONTAL );
    sizer_02_HLR->Add( 1, 1 );
//    sizer_02_HLR->Add( 1, 1, wxEXPAND );
    sizer_02_HLR->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("Left"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0, wxALIGN_CENTER_VERTICAL, 0 );
//                    wxDefaultPosition, wxSize(30,-1), 0), 0, wxALIGN_RIGHT, 0 );
    m_textChanceLeft = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_LEFT, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    sizer_02_HLR->Add( m_textChanceLeft, 0, wxALIGN_LEFT, 0 );
    sizer_02_HLR->Add( 1, 1, wxEXPAND );
    sizer_02_HLR->Add( new wxStaticText(m_panel_02, wxID_ANY, _T(" Right"),
                    wxDefaultPosition, wxSize(40,-1), 0), 0, wxALIGN_CENTER_VERTICAL, 0 );
//                    wxDefaultPosition, wxSize(35,-1), 0), 0, wxALIGN_RIGHT, 0 );
    m_textChanceRight = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_RIGHT, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    sizer_02_HLR->Add( m_textChanceRight, 0, wxALIGN_LEFT, 0 );
    sizer_02_HLR->Add( 1, 1, wxEXPAND );

    sizer_02->Add ( sizer_02_HLR, 0, wxALIGN_CENTER, 0 );
//    sizer_02->Add ( sizer_02_HLR, 0, wxALIGN_CENTER | wxEXPAND, 0 );

    // Add a space after Left Right
    sizer_02->Add ( 1, 10, wxSTRETCH_NOT );

    wxGridSizer* sizer_02_HH = new wxGridSizer( 1, 2, 0, 0 );

    wxGridSizer* m_hGridSizerL02 = new wxGridSizer( 3, 4, 0, 0 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("1B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceL1B = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_L1B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceL1B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("HR"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceLHR = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_LHR, "",
                    wxDefaultPosition, wxSize(50,-1), 0);
    m_hGridSizerL02->Add( m_textChanceLHR, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("2B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceL2B = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_L2B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceL2B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("W"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceLW = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_LW, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceLW, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("3B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceL3B = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_L3B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceL3B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("DP"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceLDP = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_LDP, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceLDP, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    sizer_02_HH->Add( m_hGridSizerL02, 0, wxALIGN_CENTER );
//    sizer_02_HH->Add( m_hGridSizerL02, 0, wxALIGN_CENTER | wxEXPAND );

    wxGridSizer* m_hGridSizerR02 = new wxGridSizer( 3, 4, 0, 0 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("1B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceR1B = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_R1B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceR1B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("HR"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceRHR = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_RHR, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceRHR, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("2B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceR2B = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_R2B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceR2B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("W"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceRW = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_RW, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceRW, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("3B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceR3B = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_R3B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceR3B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("DP"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceRDP = new wxTextCtrl( m_panel_02, ID_CHANCE_BATTER_RDP, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceRDP, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    sizer_02_HH->Add( m_hGridSizerR02, 0, wxALIGN_CENTER );
//    sizer_02_HH->Add( m_hGridSizerR02, 0, wxALIGN_CENTER | wxEXPAND );

    sizer_02->Add ( sizer_02_HH, 0, wxALIGN_CENTER );
//    sizer_02->Add ( sizer_02_HH, 0, wxALIGN_CENTER | wxEXPAND );

    // Add a space to expand the panel
    sizer_02->Add ( 1, 10, wxEXPAND );

    sizer_02->Add ( BuildControlButtons(m_panel_02, 2),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND);

    m_panel_02->SetSizer( sizer_02 );
    m_pNotebook->AddPage(m_panel_02, _T("Chance"), false, -1);

    // Add Stats Notebook page
    m_panel_03 = new wxPanel( m_pNotebook );

    sizer_03 = new wxBoxSizer( wxVERTICAL );

    sizer_03->Add ( BuildTopCombos( m_panel_03, 3 ),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND );

    sizer_03->Add( new wxStaticText(m_panel_03, wxID_ANY,
            _T("Statistics")),
            0, wxBOTTOM|wxALIGN_CENTER, 15);

    wxGridSizer* m_hGridSizer03 = new wxGridSizer( 6, 6, 0, 0 );

    // Add text to spin button
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("AB"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsAB = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_AB,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsAB, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("2B"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStats2B = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_2B,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
//    m_hGridSizer03->Add( m_spinStats2B, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    m_hGridSizer03->Add( m_spinStats2B, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("W"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsW = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_W,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsW, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("Runs"),
                    wxDefaultPosition, wxSize(40,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsRUNS = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_RUNS,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsRUNS, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("3B"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStats3B = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_3B,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStats3B, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("K"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsK = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_K,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsK, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("Hits"),
                    wxDefaultPosition, wxSize(35,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsHITS = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_HITS,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsHITS, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("HR"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsHR = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_HR,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsHR, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("RE"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsRE = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_RE,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsRE, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("RBI"),
                    wxDefaultPosition, wxSize(35,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsRBI = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_RBI,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsRBI, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("SF"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsSACRICIFE = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_SACRICIFE,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsSACRICIFE, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("CS"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsCS = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_CS,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsCS, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("Games"),
                    wxDefaultPosition, wxSize(50,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsGAMES = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_GAMES,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsGAMES, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("HBP"),
                    wxDefaultPosition, wxSize(35,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsHBP = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_HBP,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsHBP, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("SB"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsSB = new wxSpinCtrl( m_panel_03, ID_STATS_BATTER_SB,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsSB, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("AVG"),
                    wxDefaultPosition, wxSize(35,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_textStatsAVG = new wxTextCtrl(m_panel_03, ID_STATS_BATTER_AVG, _T(""),
                    wxDefaultPosition, wxSize(60,-1) );
    m_hGridSizer03->Add( m_textStatsAVG, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("SLG"),
                    wxDefaultPosition, wxSize(35,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_textStatsSLG = new wxTextCtrl(m_panel_03, ID_STATS_BATTER_SLG, _T(""),
                    wxDefaultPosition, wxSize(60,-1) );
    m_hGridSizer03->Add( m_textStatsSLG, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("OB%"),
                    wxDefaultPosition, wxSize(35,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_textStatsOBP = new wxTextCtrl(m_panel_03, ID_STATS_BATTER_OBP, _T(""),
                    wxDefaultPosition, wxSize(60,-1) );
    m_hGridSizer03->Add( m_textStatsOBP, 0, wxALIGN_LEFT|wxLEFT, 0 );

    sizer_03->Add (m_hGridSizer03, 1, wxEXPAND);

    sizer_03->Add (BuildControlButtons(m_panel_03, 3),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND);

    m_panel_03->SetSizer( sizer_03 );

    m_pNotebook->AddPage(m_panel_03, _T("Stats"), false, -1);

    // Add Info Notebook page
    m_panel_04 = new wxPanel(m_pNotebook);

    sizer_04 = new wxBoxSizer( wxVERTICAL );

    sizer_04->Add ( BuildTopCombos( m_panel_04, 4 ),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND );

    sizer_04->Add( new wxStaticText(m_panel_04, wxID_ANY,
            _T("Information")),
            0, wxBOTTOM|wxALIGN_CENTER, 15);

    wxGridSizer* m_hGridSizer00 = new wxGridSizer( 1, 4, 0, 0 );

    m_hGridSizer00->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("First Name"), wxDefaultPosition, wxSize(80,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_textFirstName = new wxTextCtrl( m_panel_04, ID_INFO_BATTER_FIRSTNAME, "",
                    wxDefaultPosition, wxSize(120,-1), 0);
//                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizer00->Add( m_textFirstName, 0, wxALIGN_LEFT, 0 );

    m_hGridSizer00->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Last Name"), wxDefaultPosition, wxSize(75,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_textLastName = new wxTextCtrl( m_panel_04, ID_INFO_BATTER_LASTNAME, "",
                    wxDefaultPosition, wxSize(120,-1), 0);
//                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizer00->Add( m_textLastName, 0, wxALIGN_LEFT, 0 );

    wxGridSizer* m_hGridSizer04 = new wxGridSizer( 4, 6, 0, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Batter Hits"), wxDefaultPosition, wxSize(75,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_BatterHits = new wxComboBox(m_panel_04, ID_INFO_BATTER_BATTERHITS,
//                    _T("R"), wxDefaultPosition, wxDefaultSize, m_arrayLRS,
                    _T("R"), wxDefaultPosition, wxSize(70,-1), m_arrayLRS,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_BatterHits,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Power Left"), wxDefaultPosition, wxSize(75,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_PowerLeft = new wxComboBox(m_panel_04, ID_INFO_BATTER_POWERLEFT,
                    _T("N"), wxDefaultPosition, wxSize(70,-1), m_arrayPower,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_PowerLeft,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Power Right"), wxDefaultPosition, wxSize(85,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_PowerRight = new wxComboBox(m_panel_04, ID_INFO_BATTER_POWERRIGHT,
                    _T("N"), wxDefaultPosition, wxSize(70,-1), m_arrayPower,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_PowerRight,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Outfielder Arm"), wxDefaultPosition, wxSize(75,-1), 0),
                    0, wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_OutfieldArm = new wxComboBox(m_panel_04, ID_INFO_BATTER_OUTFIELDARM,
                    _T("0"), wxDefaultPosition, wxSize(70,-1),
                    m_arrayOutfieldArm, wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_OutfieldArm,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Catcher Arm"), wxDefaultPosition, wxSize(75,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_CatcherArm = new wxComboBox(m_panel_04, ID_INFO_BATTER_CATCHERARM,
                    _T("0"), wxDefaultPosition, wxSize(70,-1),
                    m_arrayCatcherArm, wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_CatcherArm,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Passball"), wxDefaultPosition, wxSize(75,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_Passball = new wxComboBox(m_panel_04, ID_INFO_BATTER_PASSBALL,
                    _T("0"), wxDefaultPosition, wxSize(70,-1),
                    m_arrayPassball, wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_Passball,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Stealing"), wxDefaultPosition, wxSize(75,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_Stealing = new wxComboBox(m_panel_04, ID_INFO_BATTER_STEALING,
                    _T("B"), wxDefaultPosition, wxSize(70,-1), m_arrayStealing,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_Stealing,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Bunting"), wxDefaultPosition, wxSize(75,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_Bunting = new wxComboBox(m_panel_04, ID_INFO_BATTER_BUNTING, _T("A"),
                    wxDefaultPosition, wxSize(70,-1), m_arrayBunting,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_Bunting,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Hit and Run"), wxDefaultPosition, wxSize(80,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_HitRun = new wxComboBox(m_panel_04, ID_INFO_BATTER_HITRUN, _T("A"),
                    wxDefaultPosition, wxSize(70,-1), m_arrayHitRun,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_HitRun,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("Running"), wxDefaultPosition, wxSize(75,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_Running = new wxComboBox(m_panel_04, ID_INFO_BATTER_RUNNING, _T("10"),
                    wxDefaultPosition, wxSize(70,-1), m_arrayRunning,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_Running,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer04->Add( new wxStaticText(m_panel_04, wxID_ANY,
                    _T("T Rating"), wxDefaultPosition, wxSize(75,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_TRating = new wxComboBox(m_panel_04, ID_INFO_BATTER_TRATING, _T("0"),
                    wxDefaultPosition, wxSize(70,-1), m_arrayTRating,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer04->Add( m_combo_TRating,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    sizer_04->Add ( m_hGridSizer00 );

    sizer_04->AddSpacer(20);

    sizer_04->Add ( m_hGridSizer04, 1, wxEXPAND );

    sizer_04->Add ( BuildControlButtons(m_panel_04, 4),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND);

    m_panel_04->SetSizer( sizer_04 );
//    m_pNotebook->AddPage(m_panel_04, _T("Info"), false, -1);
    m_pNotebook->InsertPage(0, m_panel_04, _T("Info"), true, -1);
    // Set page to be displayed to Stats which is tab 3 on a 0 to 3 basis
    m_pNotebook->SetSelection(3);
}

void BatterNotebook::DefaultPanelValues( )
{
    // Set flag to show this is setting the initial valuse
    m_bSetValueFlagBatter = TRUE;

    // Set correct range values for each control
    m_spinPosition1->SetRange(0,8);
    m_spinPosition2->SetRange(0,5);
    m_spinPosition3->SetRange(0,5);
    m_spinPosition4->SetRange(0,5);
    m_spinPosition5->SetRange(0,5);
    m_spinPosition6->SetRange(0,5);
    m_spinPosition7->SetRange(0,5);
    m_spinPosition8->SetRange(0,5);
    m_spinPosition9->SetRange(0,5);
    m_spinPositionER1->SetRange(0,51);
    m_spinPositionER2->SetRange(0,16);
    m_spinPositionER3->SetRange(0,30);
    m_spinPositionER4->SetRange(0,71);
    m_spinPositionER5->SetRange(0,65);
    m_spinPositionER6->SetRange(0,88);
    m_spinPositionER7->SetRange(0,25);
    m_spinPositionER8->SetRange(0,25);
    m_spinPositionER9->SetRange(0,25);

    // Set correct range values for each control
    m_spinStats2B->SetRange(0,999);
    m_spinStats3B->SetRange(0,999);
    m_spinStatsHR->SetRange(0,999);
    m_spinStatsAB->SetRange(0,999);
    m_spinStatsHITS->SetRange(0,999);
    m_spinStatsRBI->SetRange(0,500);
    m_spinStatsRE->SetRange(0,100);
    m_spinStatsRUNS->SetRange(0,999);
    m_spinStatsSACRICIFE->SetRange(0,100);
    m_spinStatsSB->SetRange(0,100);
    m_spinStatsCS->SetRange(0,300);
    m_spinStatsK->SetRange(0,300);
    m_spinStatsW->SetRange(0,300);
    m_spinStatsHBP->SetRange(0,300);
    m_spinStatsGAMES->SetRange(0,500);

    // Initialize Position Notebook Page to 0
    m_spinPosition1->SetValue(0);
    m_spinPosition2->SetValue(0);
    m_spinPosition3->SetValue(0);
    m_spinPosition4->SetValue(0);
    m_spinPosition5->SetValue(0);
    m_spinPosition6->SetValue(0);
    m_spinPosition7->SetValue(0);
    m_spinPosition8->SetValue(0);
    m_spinPosition9->SetValue(0);
    m_spinPositionER1->SetValue(0);
    m_spinPositionER2->SetValue(0);
    m_spinPositionER3->SetValue(0);
    m_spinPositionER4->SetValue(0);
    m_spinPositionER5->SetValue(0);
    m_spinPositionER6->SetValue(0);
    m_spinPositionER7->SetValue(0);
    m_spinPositionER8->SetValue(0);
    m_spinPositionER9->SetValue(0);

    // Initialize Stats Notebook Page to 0
    m_spinStats2B->SetValue(0);
    m_spinStats3B->SetValue(0);
    m_spinStatsHR->SetValue(0);
    m_spinStatsAB->SetValue(0);
    m_spinStatsHITS->SetValue(0);
    m_spinStatsRBI->SetValue(0);
    m_spinStatsRE->SetValue(0);
    m_spinStatsRUNS->SetValue(0);
    m_spinStatsSACRICIFE->SetValue(0);
    m_spinStatsSB->SetValue(0);
    m_spinStatsCS->SetValue(0);
    m_spinStatsK->SetValue(0);
    m_spinStatsW->SetValue(0);
    m_spinStatsHBP->SetValue(0);
    m_spinStatsGAMES->SetValue(0);
    m_textStatsAVG->SetValue( "0" );
    m_textStatsSLG->SetValue( "0" );
    m_textStatsOBP->SetValue( "0" );

    // Initialize Chance Notebook Page to 0
    m_textChanceBasic->SetValue( "0.0" );
    m_textChanceLeft->SetValue( "0.0" );
    m_textChanceRight->SetValue( "0.0" );
    m_textChanceB1B->SetValue( "0.0" );
    m_textChanceB2B->SetValue( "0.0" );
    m_textChanceB3B->SetValue( "0.0" );
    m_textChanceBHR->SetValue( "0.0" );
    m_textChanceBW->SetValue( "0.0" );
    m_textChanceBDP->SetValue( "0.0" );
    m_textChanceL1B->SetValue( "0.0" );
    m_textChanceL2B->SetValue( "0.0" );
    m_textChanceL3B->SetValue( "0.0" );
    m_textChanceLHR->SetValue( "0.0" );
    m_textChanceLW->SetValue( "0.0" );
    m_textChanceLDP->SetValue( "0.0" );
    m_textChanceR1B->SetValue( "0.0" );
    m_textChanceR2B->SetValue( "0.0" );
    m_textChanceR3B->SetValue( "0.0" );
    m_textChanceRHR->SetValue( "0.0" );
    m_textChanceRW->SetValue( "0.0" );
    m_textChanceRDP->SetValue( "0.0" );
    // End Chance Notebook Page

    // Initialize Info Notebook Page to 0 or defaults
    m_combo_BatterHits->SetValue( "R" );
    m_combo_PowerLeft->SetValue( "N" );
    m_combo_PowerRight->SetValue( "N" );
    m_combo_OutfieldArm->SetValue( "0" );
    m_combo_CatcherArm->SetValue( "0" );
    m_combo_Passball->SetValue( "0" );
    m_combo_Stealing->SetValue( "B" );
    m_combo_Bunting->SetValue( "A" );
    m_combo_HitRun->SetValue( "A" );
    m_combo_Running->SetValue( "10" );
    m_combo_TRating->SetValue( "0" );
    // End Info Notebook Page

    // Clear the flag since we are done setting initial values
    m_bSetValueFlagBatter = FALSE;

}

wxBoxSizer* BatterNotebook::BuildControlButtons( wxWindow* panel, int page )
{
    // Create the control buttons (Apply, New, Add, OK, and Cancel)

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonPane = new wxBoxSizer (wxHORIZONTAL);
    switch ( page)
    {
		case 1:
			m_applyButton1 = new wxButton (panel, wxID_APPLY);
			//  By adding a space with wxExpand (non zero),
			//    the buttons will be positioned correctly
			buttonPane->Add( 1, 1, wxEXPAND );
			buttonPane->Add (m_applyButton1, 0, wxALIGN_CENTER);
			break;
		case 2:
			m_applyButton2 = new wxButton (panel, wxID_APPLY);
			buttonPane->Add( 1, 1, wxEXPAND );
			buttonPane->Add (m_applyButton2, 0, wxALIGN_CENTER);
			break;
		case 3:
			m_applyButton3 = new wxButton (panel, wxID_APPLY);
			buttonPane->Add( 1, 1, wxEXPAND );
			buttonPane->Add (m_applyButton3, 0, wxALIGN_CENTER);
			break;
		case 4:
			m_applyButton4 = new wxButton (panel, wxID_APPLY);
			buttonPane->Add( 1, 1, wxEXPAND );
			buttonPane->Add (m_applyButton4, 0, wxALIGN_CENTER);
			break;
   }
//    m_applyButton = new wxButton (panel, wxID_APPLY);
//    //  By adding a space with wxExpand (non zero),
//    //    the buttons will be positioned correctly
//    buttonPane->Add( 1, 1, wxEXPAND );
//    buttonPane->Add (m_applyButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );
    wxButton *newButton = new wxButton (panel, wxID_NEW);
    buttonPane->Add (newButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );
    switch ( page)
    {
		case 1:
			m_addButton1 = new wxButton (panel, wxID_ADD);
			buttonPane->Add (m_addButton1, 0, wxALIGN_CENTER);
			m_addButton1->Enable(false);
			break;
		case 2:
			m_addButton2 = new wxButton (panel, wxID_ADD);
			buttonPane->Add (m_addButton2, 0, wxALIGN_CENTER);
			m_addButton2->Enable(false);
			break;
		case 3:
			m_addButton3 = new wxButton (panel, wxID_ADD);
			buttonPane->Add (m_addButton3, 0, wxALIGN_CENTER);
			m_addButton3->Enable(false);
			break;
		case 4:
			m_addButton4 = new wxButton (panel, wxID_ADD);
			buttonPane->Add (m_addButton4, 0, wxALIGN_CENTER);
			m_addButton4->Enable(false);
			break;
   }
//    m_addButton = new wxButton (panel, wxID_ADD);
//    buttonPane->Add (m_addButton, 0, wxALIGN_CENTER);
//    m_addButton->Enable(false);
    buttonPane->Add( 1, 1, wxEXPAND );
    wxButton *okButton = new wxButton (panel, wxID_OK);
    okButton->SetDefault();
    buttonPane->Add (okButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );
    wxButton *cancelButton = new wxButton (panel, wxID_CANCEL);
    buttonPane->Add (cancelButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );

    sizer->Add (new wxStaticLine(panel, -1), 0, wxEXPAND | wxALL, 10);
    sizer->Add (buttonPane, 0, wxALIGN_CENTER );
//    sizer->Add (buttonPane, 0, wxALIGN_CENTER | wxEXPAND);
    // Add a space under the buttons
    sizer->Add( 10, 10, wxSTRETCH_NOT );

    return (sizer);
}

// Value for int combobox is established on call to BuildTopCombos
wxBoxSizer* BatterNotebook::BuildTopCombos( wxWindow* panel, int combobox )
{
    wxBoxSizer* sizer;
    wxBoxSizer* csizerB;
//    wxBoxSizer* csizerT;
    wxStaticText* statTextT;
    wxStaticText* statTextB;

    sizer = new wxBoxSizer( wxVERTICAL );

    m_sizerBatter = new wxBoxSizer ( wxHORIZONTAL );

    statTextT = new wxStaticText(panel, wxID_ANY, _T("Team Name"),
        wxDefaultPosition, wxSize(80,-1), 0);

    CreateTeamComboBox(panel, combobox);

    m_sizerBatter->Add ( 10, 1, wxSTRETCH_NOT );
    m_sizerBatter->Add ( statTextT, wxALIGN_LEFT );
    m_sizerBatter->Add ( 10, 1, wxSTRETCH_NOT );

    switch ( combobox)
    {
           case 1:
                m_sizerBatter->Add ( m_text_team_01, wxALIGN_CENTER );
                break;
           case 2:
                m_sizerBatter->Add ( m_text_team_02, wxALIGN_CENTER );
                break;
           case 3:
                m_sizerBatter->Add ( m_text_team_03, wxALIGN_CENTER );
                break;
           case 4:
                m_sizerBatter->Add ( m_text_team_04, wxALIGN_CENTER );
                break;
   }
    m_sizerBatter->Add ( 1, 1, wxEXPAND );

    sizer->Add ( 10, 10, wxSTRETCH_NOT );
    sizer->Add ( m_sizerBatter );

    csizerB = new wxBoxSizer ( wxHORIZONTAL );

    statTextB = new wxStaticText(panel, wxID_ANY, _T("Batter Name"),
        wxDefaultPosition, wxSize(80,-1), 0);

    CreateBatterComboBox(panel, combobox);

    csizerB->Add ( 10, 1, wxSTRETCH_NOT );
    csizerB->Add ( statTextB, wxALIGN_LEFT );
    csizerB->Add ( 10, 1, wxSTRETCH_NOT );
    switch ( combobox)
    {
           case 1:
                csizerB->Add ( m_combo_batter_01, wxALIGN_CENTER );
                break;
           case 2:
                csizerB->Add ( m_combo_batter_02, wxALIGN_CENTER );
                break;
           case 3:
                csizerB->Add ( m_combo_batter_03, wxALIGN_CENTER );
                break;
           case 4:
                csizerB->Add ( m_combo_batter_04, wxALIGN_CENTER );
                break;
   }
    csizerB->Add ( 1, 1, wxEXPAND );

    sizer->Add ( 10, 10, wxSTRETCH_NOT );
    sizer->Add ( csizerB );
    sizer->Add (new wxStaticLine(panel, -1), 0, wxEXPAND | wxALL, 10);

    m_sizerBatterSave = m_sizerBatter;

    return (sizer);
}

void BatterNotebook::OnApply(wxCommandEvent& event)
{
    wxMessageBox(_T("Apply Button Pressed"),
                 _T("Apply"), wxOK|wxICON_INFORMATION );
    //Save the data
    GetNotebookData();

    MakeBatterUpdate();

    m_bChangeInfoFlag = FALSE;
    m_bChangeStatsFlag = FALSE;
    m_bChangeChanceFlag = FALSE;
    m_bChangePositionFlag = FALSE;

	m_spinStatsW->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStats2B->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStats3B->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsHR->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsAB->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsHITS->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsRBI->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsRE->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsRUNS->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsSACRICIFE->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsSB->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsCS->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsK->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsW->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsHBP->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsGAMES->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK

    // Now update the current Stat panel in the notebook
    // Create a wxCommandEvent
	wxCommandEvent comboEvent(wxEVT_COMBOBOX, ID_COMBO_BATTER);

	// Set the selected item
    int mySel = m_pNotebook->GetSelection();
    switch( mySel )
    {
        case -1:
            break;
        case 0:   // Info Notebook Page
            comboEvent.SetInt( m_combo_batter_04->GetSelection() );
            break;
        case 1:   // Position Notebook Page
            comboEvent.SetInt( m_combo_batter_01->GetSelection() );
            break;
        case 2:   // Chance Notebook Page
            comboEvent.SetInt( m_combo_batter_02->GetSelection() );
            break;
        case 3:   // Stats Notebook Page
            comboEvent.SetInt( m_combo_batter_03->GetSelection() );
            break;
        default:  // Should never get here
            wxMessageBox(_T("Something is wrong with the Batter Notebook"),
                _T("Error"), wxOK|wxICON_INFORMATION );
            break;
    }

    // Post the event to the combobox
    wxPostEvent(m_combo_batter_03, comboEvent);
}

void BatterNotebook::OnAdd(wxCommandEvent& event)
{
	MyAddEvent();
}

void BatterNotebook::MyAddEvent()
{
    // Need to check for base team. Only allow inserts in base teams
    if ( wxGetApp().pDBRoutines->structTeamData.BaseTeam == TRUE )
    {
        // Need to update data, do insert and then rebbuild player names
        GetNotebookData();
        wxGetApp().pDBRoutines->DBInsertBatterData(wxGetApp().pDBRoutines->m_intTeamID);
        // The following updates the array but not the combobox
        wxGetApp().pDBRoutines->DBGetBatterStatsID(wxGetApp().pDBRoutines->m_intTeamID);
        //m_combo_pitcher_01. //  ( wxGetApp().pDBRoutines->m_arrayPitcherFullNames );
        UpdateBatterNames();

		m_bChangeInfoFlag = FALSE;
		m_bChangeStatsFlag = FALSE;
		m_bChangeChanceFlag = FALSE;
		m_bChangePositionFlag = FALSE;
//		event.Skip(TRUE);
    }
    else
    {
        wxMessageBox(_T("Attempt to insert non-Base team!"),
                 _T("Insert Failed"), wxOK|wxICON_INFORMATION );
    }

    //Set text to BLACK
	m_spinStatsW->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStats2B->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStats3B->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsHR->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsAB->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsHITS->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsRBI->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsRE->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsRUNS->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsSACRICIFE->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsSB->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsCS->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsK->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsW->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsHBP->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsGAMES->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK

	m_addButton1->Enable(false);
	m_addButton2->Enable(false);
	m_addButton3->Enable(false);
	m_addButton4->Enable(false);
	m_applyButton1->Enable(true);
	m_applyButton2->Enable(true);
	m_applyButton3->Enable(true);
	m_applyButton4->Enable(true);

}

void BatterNotebook::OnNew(wxCommandEvent& event)
{
//    wxMessageBox(_T("New Button Pressed"),
//                 _T("New"), wxOK|wxICON_INFORMATION );

	m_addButton1->Enable(true);
	m_addButton2->Enable(true);
	m_addButton3->Enable(true);
	m_addButton4->Enable(true);
	m_applyButton1->Enable(false);
	m_applyButton2->Enable(false);
	m_applyButton3->Enable(false);
	m_applyButton4->Enable(false);

	m_textFirstName->SetValue("");
	m_textLastName->SetValue("");
    wxGetApp().pDBRoutines->structBatterData.FirstName = "";
    wxGetApp().pDBRoutines->structBatterData.LastName = "";

    wxGetApp().pDBRoutines->m_arrayBatterFullNames.Clear();
    UpdateBatterNames();

    // Initialize Position Notebook Page to 0
    m_spinPosition1->SetValue(0);
    m_spinPosition2->SetValue(0);
    m_spinPosition3->SetValue(0);
    m_spinPosition4->SetValue(0);
    m_spinPosition5->SetValue(0);
    m_spinPosition6->SetValue(0);
    m_spinPosition7->SetValue(0);
    m_spinPosition8->SetValue(0);
    m_spinPosition9->SetValue(0);
    m_spinPositionER1->SetValue(0);
    m_spinPositionER2->SetValue(0);
    m_spinPositionER3->SetValue(0);
    m_spinPositionER4->SetValue(0);
    m_spinPositionER5->SetValue(0);
    m_spinPositionER6->SetValue(0);
    m_spinPositionER7->SetValue(0);
    m_spinPositionER8->SetValue(0);
    m_spinPositionER9->SetValue(0);
    wxGetApp().pDBRoutines->structBatterData.Pitcher = 0;
    wxGetApp().pDBRoutines->structBatterData.Catcher = 0;
    wxGetApp().pDBRoutines->structBatterData.FirstBase = 0;
    wxGetApp().pDBRoutines->structBatterData.SecondBase = 0;
    wxGetApp().pDBRoutines->structBatterData.Shortstop = 0;
    wxGetApp().pDBRoutines->structBatterData.ThirdBase = 0;
    wxGetApp().pDBRoutines->structBatterData.LeftField = 0;
    wxGetApp().pDBRoutines->structBatterData.CenterField = 0;
    wxGetApp().pDBRoutines->structBatterData.RightField = 0;
    wxGetApp().pDBRoutines->structBatterData.ER1 = 0;
    wxGetApp().pDBRoutines->structBatterData.ER2 = 0;
    wxGetApp().pDBRoutines->structBatterData.ER3 = 0;
    wxGetApp().pDBRoutines->structBatterData.ER4 = 0;
    wxGetApp().pDBRoutines->structBatterData.ER5 = 0;
    wxGetApp().pDBRoutines->structBatterData.ER6 = 0;
    wxGetApp().pDBRoutines->structBatterData.ER7 = 0;
    wxGetApp().pDBRoutines->structBatterData.ER8 = 0;
    wxGetApp().pDBRoutines->structBatterData.ER9 = 0;

    // Initialize Stats Notebook Page to 0
    m_spinStats2B->SetValue(0);
    m_spinStats3B->SetValue(0);
    m_spinStatsHR->SetValue(0);
    m_spinStatsAB->SetValue(0);
    m_spinStatsHITS->SetValue(0);
    m_spinStatsRBI->SetValue(0);
    m_spinStatsRE->SetValue(0);
    m_spinStatsRUNS->SetValue(0);
    m_spinStatsSACRICIFE->SetValue(0);
    m_spinStatsSB->SetValue(0);
    m_spinStatsCS->SetValue(0);
    m_spinStatsK->SetValue(0);
    m_spinStatsW->SetValue(0);
    m_spinStatsHBP->SetValue(0);
    m_spinStatsGAMES->SetValue(0);
    m_textStatsAVG->SetValue( "0" );
    m_textStatsSLG->SetValue( "0" );
    m_textStatsOBP->SetValue( "0" );
    wxGetApp().pDBRoutines->structBatterStats.AB = 0;
    wxGetApp().pDBRoutines->structBatterStats.Runs = 0;
    wxGetApp().pDBRoutines->structBatterStats.Hits = 0;
    wxGetApp().pDBRoutines->structBatterStats.RBI = 0;
    wxGetApp().pDBRoutines->structBatterStats.Doubles = 0;
    wxGetApp().pDBRoutines->structBatterStats.Triples = 0;
    wxGetApp().pDBRoutines->structBatterStats.HomeRuns = 0;
    wxGetApp().pDBRoutines->structBatterStats.Walk = 0;
    wxGetApp().pDBRoutines->structBatterStats.Strikeout = 0;
    wxGetApp().pDBRoutines->structBatterStats.ReachedOnError = 0;
    wxGetApp().pDBRoutines->structBatterStats.Sacrifice = 0;
    wxGetApp().pDBRoutines->structBatterStats.StollenBase = 0;
    wxGetApp().pDBRoutines->structBatterStats.CS = 0;
    wxGetApp().pDBRoutines->structBatterStats.Games = 0;
    wxGetApp().pDBRoutines->structBatterStats.HBP = 0;
    wxGetApp().pDBRoutines->structBatterStats.AVG = 0;
    wxGetApp().pDBRoutines->structBatterStats.SLG = 0;
    wxGetApp().pDBRoutines->structBatterStats.OBP = 0;
    wxGetApp().pDBRoutines->structBatterStats.BatterID = 0;
    wxGetApp().pDBRoutines->structBatterStats.BatterStatsID = 0;
    // End Stats Notebook Page

    // Initialize Chance Notebook Page to 0
    m_textChanceBasic->SetValue( "0.0" );
    m_textChanceLeft->SetValue( "0.0" );
    m_textChanceRight->SetValue( "0.0" );
    m_textChanceB1B->SetValue( "0.0" );
    m_textChanceB2B->SetValue( "0.0" );
    m_textChanceB3B->SetValue( "0.0" );
    m_textChanceBHR->SetValue( "0.0" );
    m_textChanceBW->SetValue( "0.0" );
    m_textChanceBDP->SetValue( "0.0" );
    m_textChanceL1B->SetValue( "0.0" );
    m_textChanceL2B->SetValue( "0.0" );
    m_textChanceL3B->SetValue( "0.0" );
    m_textChanceLHR->SetValue( "0.0" );
    m_textChanceLW->SetValue( "0.0" );
    m_textChanceLDP->SetValue( "0.0" );
    m_textChanceR1B->SetValue( "0.0" );
    m_textChanceR2B->SetValue( "0.0" );
    m_textChanceR3B->SetValue( "0.0" );
    m_textChanceRHR->SetValue( "0.0" );
    m_textChanceRW->SetValue( "0.0" );
    m_textChanceRDP->SetValue( "0.0" );
    wxGetApp().pDBRoutines->structBatterData.OBChanceBasic = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceLeft = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceRight = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRun = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceTriple = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceDouble = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceSingle = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceWalk = 0;
    wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlay = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRunRight = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceTripleRight = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceDoubleRight = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceSingleRight = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceWalkRight = 0;
    wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlayRight = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRunLeft = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceTripleLeft = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceDoubleLeft = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceSingleLeft = 0;
    wxGetApp().pDBRoutines->structBatterData.OBChanceWalkLeft = 0;
    wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlayLeft = 0;
    // End Chance Notebook Page

    // Initialize Info Notebook Page to 0 or defaults
    m_combo_BatterHits->SetValue( "R" );
    m_combo_PowerLeft->SetValue( "N" );
    m_combo_PowerRight->SetValue( "N" );
    m_combo_OutfieldArm->SetValue( "0" );
    m_combo_CatcherArm->SetValue( "0" );
    m_combo_Passball->SetValue( "0" );
    m_combo_Stealing->SetValue( "B" );
    m_combo_Bunting->SetValue( "A" );
    m_combo_HitRun->SetValue( "A" );
    m_combo_Running->SetValue( "10" );
    m_combo_TRating->SetValue( "0" );
    wxGetApp().pDBRoutines->structBatterData.BatterHits = 0;
    wxGetApp().pDBRoutines->structBatterData.PowerRight = 0;
    wxGetApp().pDBRoutines->structBatterData.PowerLeft = 0;
    wxGetApp().pDBRoutines->structBatterData.CatchArm = 0;
    wxGetApp().pDBRoutines->structBatterData.OutArm = 0;
    wxGetApp().pDBRoutines->structBatterData.Pass = 0;
    wxGetApp().pDBRoutines->structBatterData.Stealing = 0;
    wxGetApp().pDBRoutines->structBatterData.Bunting = 0;
    wxGetApp().pDBRoutines->structBatterData.HitRun = 0;
    wxGetApp().pDBRoutines->structBatterData.Running = 0;
    wxGetApp().pDBRoutines->structBatterData.TRate = 0;
    wxGetApp().pDBRoutines->structBatterData.BatterID = 0;
    // Set page to be displayed to Info which is tab 0 on a 0 to 4 basis
    m_pNotebook->SetSelection(0);
    // End Info Notebook Page
}

void BatterNotebook::OnOK(wxCommandEvent& event)
{
	int answer;

    // If one of the change flags is TRUE, Ask if updates should be dropped.
    if ( m_bChangeInfoFlag || m_bChangeStatsFlag ||
         m_bChangeChanceFlag || m_bChangePositionFlag )
    {
        answer = wxMessageBox(
			(_T("       A field has changed.\n")
			 _T("Press Yes to save or No to discard")
			),
			_T("Field Changed"), wxYES_NO|wxICON_QUESTION );
        if ( answer == wxYES)
        {
			if ( m_addButton1->IsEnabled() )
			{
				answer = wxMessageBox(
					(_T("You were Adding a player!\n")
					 _T("Press Yes to add or No to discard")
					),
					_T("Field Changed"), wxYES_NO|wxICON_QUESTION );
				if ( answer == wxYES)
				{
					// Then still Add player
					MyAddEvent();
				}
				// Disable Add button
				m_addButton1->Enable(false);
				m_addButton2->Enable(false);
				m_addButton3->Enable(false);
				m_addButton4->Enable(false);
				m_applyButton1->Enable(true);
				m_applyButton2->Enable(true);
				m_applyButton3->Enable(true);
				m_applyButton4->Enable(true);
			}
			else{
				//Save the data
				GetNotebookData();
				MakeBatterUpdate();
			}
        }
    }

	m_bChangeInfoFlag = FALSE;
	m_bChangeStatsFlag = FALSE;
	m_bChangeChanceFlag = FALSE;
	m_bChangePositionFlag = FALSE;

    // Continue and process this event normally.
    event.Skip(TRUE);
}

void BatterNotebook::OnCancel(wxCommandEvent& event)
{
    wxMessageBox(_T("Cancel Button Pressed"),
                 _T("Cancel"), wxOK|wxICON_INFORMATION );
//    EndModal(wxID_CANCEL);
    event.Skip(TRUE);
}

void BatterNotebook::GetNotebookData()
{
    BatterStruct my_BatterStruct;
    int currentnotebookpage;
    wxString strBatterName;
    int origselection;

    // Retrieve Position Notebook Page - 18 entries
    wxGetApp().pDBRoutines->structBatterData.Pitcher = m_spinPosition1->GetValue();
    wxGetApp().pDBRoutines->structBatterData.Catcher = m_spinPosition2->GetValue();
    wxGetApp().pDBRoutines->structBatterData.FirstBase = m_spinPosition3->GetValue();
    wxGetApp().pDBRoutines->structBatterData.SecondBase = m_spinPosition4->GetValue();
    wxGetApp().pDBRoutines->structBatterData.ThirdBase = m_spinPosition5->GetValue();
    wxGetApp().pDBRoutines->structBatterData.Shortstop = m_spinPosition6->GetValue();
    wxGetApp().pDBRoutines->structBatterData.LeftField = m_spinPosition7->GetValue();
    wxGetApp().pDBRoutines->structBatterData.CenterField = m_spinPosition8->GetValue();
    wxGetApp().pDBRoutines->structBatterData.RightField = m_spinPosition9->GetValue();
    wxGetApp().pDBRoutines->structBatterData.ER1 = m_spinPositionER1->GetValue();
    wxGetApp().pDBRoutines->structBatterData.ER2 = m_spinPositionER2->GetValue();
    wxGetApp().pDBRoutines->structBatterData.ER3 = m_spinPositionER3->GetValue();
    wxGetApp().pDBRoutines->structBatterData.ER4 = m_spinPositionER4->GetValue();
    wxGetApp().pDBRoutines->structBatterData.ER5 = m_spinPositionER5->GetValue();
    wxGetApp().pDBRoutines->structBatterData.ER6 = m_spinPositionER6->GetValue();
    wxGetApp().pDBRoutines->structBatterData.ER7 = m_spinPositionER7->GetValue();
    wxGetApp().pDBRoutines->structBatterData.ER8 = m_spinPositionER8->GetValue();
    wxGetApp().pDBRoutines->structBatterData.ER9 = m_spinPositionER9->GetValue();
    // End Position Notebook Page

    // Retrieve Chance Notebook Page - 21 entries
    wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlay = atof( m_textChanceBDP->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlayLeft = atof( m_textChanceLDP->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlayRight = atof( m_textChanceRDP->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceBasic = atof( m_textChanceBasic->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceDouble = atof( m_textChanceB2B->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceDoubleLeft = atof( m_textChanceL2B->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceDoubleRight = atof( m_textChanceR2B->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRun = atof( m_textChanceBHR->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRunLeft = atof( m_textChanceLHR->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRunRight = atof( m_textChanceRHR->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceLeft = atof( m_textChanceLeft->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceRight = atof( m_textChanceRight->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceSingle = atof( m_textChanceB1B->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceSingleLeft = atof( m_textChanceL1B->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceSingleRight = atof( m_textChanceR1B->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceTriple = atof( m_textChanceB3B->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceTripleLeft = atof( m_textChanceL3B->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceTripleRight = atof( m_textChanceR3B->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceWalk = atof( m_textChanceBW->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceWalkLeft = atof( m_textChanceLW->GetValue() );
    wxGetApp().pDBRoutines->structBatterData.OBChanceWalkRight = atof( m_textChanceRW->GetValue() );
    // End Chance Notebook Page

    // Retrieve Stats Notebook Page - 15 entries
    wxGetApp().pDBRoutines->structBatterStats.Hits = m_spinStatsHITS->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.AB = m_spinStatsAB->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.Doubles = m_spinStats2B->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.Triples = m_spinStats3B->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.CS = m_spinStatsCS->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.Games = m_spinStatsGAMES->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.HBP = m_spinStatsHBP->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.HomeRuns = m_spinStatsHR->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.Strikeout = m_spinStatsK->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.RBI = m_spinStatsRBI->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.ReachedOnError = m_spinStatsRE->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.Runs = m_spinStatsRUNS->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.Sacrifice = m_spinStatsSACRICIFE->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.StollenBase = m_spinStatsSB->GetValue();
    wxGetApp().pDBRoutines->structBatterStats.Walk = m_spinStatsW->GetValue();

    if ( wxGetApp().pDBRoutines->structBatterStats.AB == 0 )
    {
		wxGetApp().pDBRoutines->structBatterStats.AVG = 0.0f;
		wxGetApp().pDBRoutines->structBatterStats.OBP = 0.0f;
		wxGetApp().pDBRoutines->structBatterStats.SLG = 0.0f;
    }
    else
    {
		wxGetApp().pDBRoutines->structBatterStats.AVG =
			(float)wxGetApp().pDBRoutines->structBatterStats.Hits / wxGetApp().pDBRoutines->structBatterStats.AB;
		wxGetApp().pDBRoutines->structBatterStats.SLG = (float)
            (
                (
                    wxGetApp().pDBRoutines->structBatterStats.Hits -
                    (
                        wxGetApp().pDBRoutines->structBatterStats.Doubles +
                        wxGetApp().pDBRoutines->structBatterStats.Triples +
                        wxGetApp().pDBRoutines->structBatterStats.HomeRuns
                    )
                ) +
                (2 * wxGetApp().pDBRoutines->structBatterStats.Doubles) +
                (3 * wxGetApp().pDBRoutines->structBatterStats.Triples) +
                (4 * wxGetApp().pDBRoutines->structBatterStats.HomeRuns)
            ) /
            (wxGetApp().pDBRoutines->structBatterStats.AB);

		wxGetApp().pDBRoutines->structBatterStats.OBP =
			(float)(wxGetApp().pDBRoutines->structBatterStats.Hits +
			wxGetApp().pDBRoutines->structBatterStats.Walk +
 			wxGetApp().pDBRoutines->structBatterStats.HBP) /
			(wxGetApp().pDBRoutines->structBatterStats.AB +
				wxGetApp().pDBRoutines->structBatterStats.Walk +
				wxGetApp().pDBRoutines->structBatterStats.HBP +
				wxGetApp().pDBRoutines->structBatterStats.Sacrifice);
	}
    // End Stats Notebook Page

    // Retrieve Info Notebook Page - 13 entries
    // The next three entries save the selection, not the value
    wxGetApp().pDBRoutines->structBatterData.FirstName = m_textFirstName->GetValue();
    wxGetApp().pDBRoutines->structBatterData.LastName = m_textLastName->GetValue();
    wxGetApp().pDBRoutines->structBatterData.Pass = m_combo_Passball->GetSelection();
    // Running is 5-19 so selection is off by 5
//    wxGetApp().pDBRoutines->structBatterData.Running = atoi(m_combo_Running->GetValue());
//    wxGetApp().pDBRoutines->structBatterData.Running = m_combo_Running->GetSelection();	// 5 to 19
    wxGetApp().pDBRoutines->structBatterData.Running = atoi(m_combo_Running->GetValue());	// 5 to 19
    wxGetApp().pDBRoutines->structBatterData.TRate = m_combo_TRating->GetSelection();

    wxGetApp().pDBRoutines->structBatterData.Stealing = m_combo_Stealing->GetSelection();
    wxGetApp().pDBRoutines->structBatterData.Bunting = m_combo_Bunting->GetSelection();
    wxGetApp().pDBRoutines->structBatterData.HitRun = m_combo_HitRun->GetSelection();
    wxGetApp().pDBRoutines->structBatterData.BatterHits = m_combo_BatterHits->GetSelection();
    wxGetApp().pDBRoutines->structBatterData.PowerLeft = m_combo_PowerLeft->GetSelection();
    wxGetApp().pDBRoutines->structBatterData.PowerRight = m_combo_PowerRight->GetSelection();
    // OutArm is -6 to 5 so selection is off by 6. Value of 6 is actually 0
//    wxGetApp().pDBRoutines->structBatterData.OutArm = m_combo_OutfieldArm->GetSelection();
    wxGetApp().pDBRoutines->structBatterData.OutArm = atoi(m_combo_OutfieldArm->GetValue());
    //CatchArm is -4 ot 4 so selection is off by 4. Value of 4 is actually 0
//    wxGetApp().pDBRoutines->structBatterData.CatchArm = m_combo_CatcherArm->GetSelection();
    wxGetApp().pDBRoutines->structBatterData.CatchArm = atoi(m_combo_CatcherArm->GetValue());

    // End Info Notebook Page

//    // Notebook page is zero based
//    currentnotebookpage = m_pNotebook->GetSelection();
//    switch ( currentnotebookpage )
//    {
//        case -1:
//            break;
//        case 0:   // Position Notebook Page
//            strBatterName = m_combo_batter_01->GetValue();
//            break;
//        case 1:   // Stats Notebook Page
//            strBatterName = m_combo_batter_02->GetValue();
//            break;
//        case 2:   // Chance Notebook Page
//            strBatterName = m_combo_batter_03->GetValue();
//            break;
//        case 3:   // Info Notebook Page
//            strBatterName = m_combo_batter_04->GetValue();
//            break;
//        default:  // Should never get here
//            wxMessageBox(_T("Something is wrong with the Batter Notebook - GetNoteBookData"),
//                _T("Error"), wxOK|wxICON_INFORMATION );
//            break;
//    }
//    my_BatterStruct.m_PlayerName = strBatterName;
//    // origselection can be used to calculate displacement into file
//    origselection = m_arrayBattersOrig.Index( strBatterName );
//    if ( origselection != wxNOT_FOUND )
//    {
//        my_BatterStruct.UpdateBatter( m_strLeagueDirName + m_strTeamFileNameBatter,
//            origselection );
//    }
//    else
//    {
//        wxMessageBox(_T("Batter Name not Found"),
//            _T("Error"), wxOK|wxICON_INFORMATION );
//    }
}

void BatterNotebook::OnComboBatterSelect(wxCommandEvent& event)
{
    // GetSelection will return the zero based index from the combobox
    int iBatterSelection;
    int origselection;
	float fBattingAverage, fSLG, fOnBasePercentage;
    wxString strBatterName;
	char buf[10];

    iBatterSelection = event.GetSelection();
    strBatterName = event.GetString();
    // origselection can be used to calculate displacement into file
    origselection = m_arrayBattersOrig.Index( strBatterName );

    // Since a batter has been selected, we now need to retrieve
    // the batter information and populate the dialog.
//    my_BatterStruct.GetBatter( m_strLeagueDirName + m_strTeamFileNameBatter,
//        origselection );
    wxGetApp().pDBRoutines->DBGetBatterData(wxGetApp().pDBRoutines->m_arrayBatterStatsIDs[iBatterSelection]);
    wxGetApp().pDBRoutines->m_intBatterStatsID = wxGetApp().pDBRoutines->m_arrayBatterStatsIDs[iBatterSelection];

    // Set flag to show this is setting the initial value
    m_bSetValueFlagBatter = TRUE;

    // Populate Position Notebook Page - 18 entries
    m_spinPosition1->SetValue( wxGetApp().pDBRoutines->structBatterData.Pitcher );
    m_spinPosition2->SetValue( wxGetApp().pDBRoutines->structBatterData.Catcher );
    m_spinPosition3->SetValue( wxGetApp().pDBRoutines->structBatterData.FirstBase );
    m_spinPosition4->SetValue( wxGetApp().pDBRoutines->structBatterData.SecondBase );
    m_spinPosition5->SetValue( wxGetApp().pDBRoutines->structBatterData.ThirdBase );
    m_spinPosition6->SetValue( wxGetApp().pDBRoutines->structBatterData.Shortstop );
    m_spinPosition7->SetValue( wxGetApp().pDBRoutines->structBatterData.LeftField );
    m_spinPosition8->SetValue( wxGetApp().pDBRoutines->structBatterData.CenterField );
    m_spinPosition9->SetValue( wxGetApp().pDBRoutines->structBatterData.RightField );
    m_spinPositionER1->SetValue( wxGetApp().pDBRoutines->structBatterData.ER1 );
    m_spinPositionER2->SetValue( wxGetApp().pDBRoutines->structBatterData.ER2 );
    m_spinPositionER3->SetValue( wxGetApp().pDBRoutines->structBatterData.ER3 );
    m_spinPositionER4->SetValue( wxGetApp().pDBRoutines->structBatterData.ER4 );
    m_spinPositionER5->SetValue( wxGetApp().pDBRoutines->structBatterData.ER5 );
    m_spinPositionER6->SetValue( wxGetApp().pDBRoutines->structBatterData.ER6 );
    m_spinPositionER7->SetValue( wxGetApp().pDBRoutines->structBatterData.ER7 );
    m_spinPositionER8->SetValue( wxGetApp().pDBRoutines->structBatterData.ER8 );
    m_spinPositionER9->SetValue( wxGetApp().pDBRoutines->structBatterData.ER9 );
    // End Position Notebook Page

    // Populate Chance Notebook Page - 21 entries
    // These SetValues will trigger an Event
//    m_textChanceBDP->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlay )));
    m_textChanceBasic->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceBasic )));
    m_textChanceLeft->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceLeft )));
    m_textChanceRight->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceRight )));
    m_textChanceB1B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceSingle )));
    m_textChanceB2B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceDouble )));
    m_textChanceB3B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceTriple )));
    m_textChanceBHR->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRun )));
    m_textChanceBW->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceWalk )));
    m_textChanceBDP->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlay )));
    m_textChanceL1B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceSingleLeft )));
    m_textChanceL2B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceDoubleLeft )));
    m_textChanceL3B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceTripleLeft )));
    m_textChanceLHR->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRunLeft )));
    m_textChanceLW->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceWalkLeft )));
    m_textChanceLDP->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlayLeft )));
    m_textChanceR1B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceSingleRight )));
    m_textChanceR2B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceDoubleRight )));
    m_textChanceR3B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceTripleRight )));
    m_textChanceRHR->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceHomeRunRight )));
    m_textChanceRW->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.OBChanceWalkRight )));
    m_textChanceRDP->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structBatterData.ChanceDoublePlayRight )));
    // End Chance Notebook Page

    // Populate Stats Notebook Page - 16 entries
    //Set text to BLACK
	m_spinStatsW->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStats2B->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStats3B->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsHR->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsAB->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsHITS->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsRBI->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsRE->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsRUNS->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsSACRICIFE->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsSB->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsCS->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsK->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsW->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsHBP->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK
	m_spinStatsGAMES->SetForegroundColour(wxColour(wxT("BLACK"))); // BLACK

    m_spinStatsHITS->SetValue( wxGetApp().pDBRoutines->structBatterStats.Hits );
    m_spinStatsAB->SetValue( wxGetApp().pDBRoutines->structBatterStats.AB );
    m_spinStats2B->SetValue( wxGetApp().pDBRoutines->structBatterStats.Doubles );
    m_spinStats3B->SetValue( wxGetApp().pDBRoutines->structBatterStats.Triples );
    m_spinStatsCS->SetValue( wxGetApp().pDBRoutines->structBatterStats.CS );
    m_spinStatsGAMES->SetValue( wxGetApp().pDBRoutines->structBatterStats.Games );
    m_spinStatsHBP->SetValue( wxGetApp().pDBRoutines->structBatterStats.HBP );
    m_spinStatsHR->SetValue( wxGetApp().pDBRoutines->structBatterStats.HomeRuns );
    m_spinStatsK->SetValue( wxGetApp().pDBRoutines->structBatterStats.Strikeout );
    m_spinStatsRBI->SetValue( wxGetApp().pDBRoutines->structBatterStats.RBI );
    m_spinStatsRE->SetValue( wxGetApp().pDBRoutines->structBatterStats.ReachedOnError );
    m_spinStatsRUNS->SetValue( wxGetApp().pDBRoutines->structBatterStats.Runs );
    m_spinStatsSACRICIFE->SetValue( wxGetApp().pDBRoutines->structBatterStats.Sacrifice );
    m_spinStatsSB->SetValue( wxGetApp().pDBRoutines->structBatterStats.StollenBase );
    m_spinStatsW->SetValue( wxGetApp().pDBRoutines->structBatterStats.Walk );

    m_textStatsAVG->SetValue( wxString::Format(wxT("%1.3f"), (wxGetApp().pDBRoutines->structBatterStats.AVG )));
    m_textStatsOBP->SetValue( wxString::Format(wxT("%1.3f"), (wxGetApp().pDBRoutines->structBatterStats.OBP )));
    m_textStatsSLG->SetValue( wxString::Format(wxT("%1.3f"), (wxGetApp().pDBRoutines->structBatterStats.SLG )));

//    if ( wxGetApp().pDBRoutines->structBatterStats.AB == 0 )
//    {
//		fBattingAverage = 0.0f;
//		fSLG = 0.0f;
//		fOnBasePercentage = 0.0f;
//    }
//    else
//    {
//		fBattingAverage = (float)wxGetApp().pDBRoutines->structBatterStats.Hits / wxGetApp().pDBRoutines->structBatterStats.AB;
//		fSLG = (float)
//            (
//                (
//                    wxGetApp().pDBRoutines->structBatterStats.Hits -
//                    (
//                        wxGetApp().pDBRoutines->structBatterStats.Doubles +
//                        wxGetApp().pDBRoutines->structBatterStats.Triples +
//                        wxGetApp().pDBRoutines->structBatterStats.HomeRuns
//                    )
//                ) +
//                (2 * wxGetApp().pDBRoutines->structBatterStats.Doubles) +
//                (3 * wxGetApp().pDBRoutines->structBatterStats.Triples) +
//                (4 * wxGetApp().pDBRoutines->structBatterStats.HomeRuns)
//            ) /
//            (wxGetApp().pDBRoutines->structBatterStats.AB);
//		fOnBasePercentage = (float)(wxGetApp().pDBRoutines->structBatterStats.Hits + wxGetApp().pDBRoutines->structBatterStats.Walk +
//			wxGetApp().pDBRoutines->structBatterStats.ReachedOnError + wxGetApp().pDBRoutines->structBatterStats.Sacrifice +
//			wxGetApp().pDBRoutines->structBatterStats.StollenBase) / (wxGetApp().pDBRoutines->structBatterStats.AB +
//			wxGetApp().pDBRoutines->structBatterStats.Walk +
//			wxGetApp().pDBRoutines->structBatterStats.ReachedOnError + wxGetApp().pDBRoutines->structBatterStats.Sacrifice +
//			wxGetApp().pDBRoutines->structBatterStats.StollenBase);
//    }
//    sprintf(buf,"%1.3f",fBattingAverage);
//    m_textStatsAVG->SetValue( buf );
//    sprintf(buf,"%1.3f",fOnBasePercentage);
//    m_textStatsOBP->SetValue( buf );
//    sprintf(buf,"%1.3f",fSLG);
//    m_textStatsSLG->SetValue( buf );
    // End Stats Notebook Page

    // Populate Info Notebook Page - 11 entries
    m_textFirstName->SetValue( wxString::Format(wxT("%s"), (wxGetApp().pDBRoutines->structBatterData.FirstName )));
    m_textLastName->SetValue( wxString::Format(wxT("%s"), (wxGetApp().pDBRoutines->structBatterData.LastName )));
    m_combo_BatterHits->SetSelection(wxGetApp().pDBRoutines->structBatterData.BatterHits);
    m_combo_PowerLeft->SetSelection(wxGetApp().pDBRoutines->structBatterData.PowerLeft);
    m_combo_PowerRight->SetSelection(wxGetApp().pDBRoutines->structBatterData.PowerRight);
//    m_combo_OutfieldArm->SetSelection(wxGetApp().pDBRoutines->structBatterData.OutArm);
//wxString stringnumber = wxString::Format(wxT("%d"), (int)number_to_convert);
    m_combo_OutfieldArm->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structBatterData.OutArm));
//    m_combo_CatcherArm->SetSelection(wxGetApp().pDBRoutines->structBatterData.CatchArm);
    m_combo_CatcherArm->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structBatterData.CatchArm));
    m_combo_Passball->SetSelection(wxGetApp().pDBRoutines->structBatterData.Pass);
    m_combo_Stealing->SetSelection(wxGetApp().pDBRoutines->structBatterData.Stealing);
    m_combo_Bunting->SetSelection(wxGetApp().pDBRoutines->structBatterData.Bunting);
    m_combo_HitRun->SetSelection(wxGetApp().pDBRoutines->structBatterData.HitRun);
//    m_combo_Running->SetSelection(wxGetApp().pDBRoutines->structBatterData.Running);
    m_combo_Running->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structBatterData.Running));
    m_combo_TRating->SetSelection(wxGetApp().pDBRoutines->structBatterData.TRate);
    // End Info Notebook Page

    // Clear the flag since we are done setting initial values
    m_bSetValueFlagBatter = FALSE;

//    wxMessageDialog dialog2(NULL, strBatterName + _T("\n")+
//               m_arrayBattersOrig[origselection],
//               _T("Got string"),
//               wxOK|wxICON_INFORMATION );
//    dialog2.ShowModal();
}

void BatterNotebook::OnComboTeamSelect(wxCommandEvent& event)
{
}

// This SpinEvent is not really needed because we are checking
// the field change by usinf the Text Event.
void BatterNotebook::OnComboPositionChangeSpin(wxSpinEvent& event)
{
    m_bChangePositionFlag = TRUE;
}

void BatterNotebook::OnComboPositionChangeText(wxCommandEvent& event)
{
    // Unfortunatly, this event is triggered even with a SetValue.
    // m_bSetValueFlag is set to TRUE during initialization when
    // the SetValues are issued and then cleared to FALSE.
    if ( !m_bSetValueFlagBatter )
    {
        m_bChangePositionFlag = TRUE;
//        wxMessageBox(_T("Position Text Changed"), _T("Position Text"), wxOK);
    }
}

void BatterNotebook::OnComboChanceChange(wxCommandEvent& event)
{
    // Unfortunatly, this event is triggered even with a SetValue.
    // m_bSetValueFlag is set to TRUE during initialization when
    // the SetValues are issued and then cleared to FALSE.
    if ( !m_bSetValueFlagBatter )
    {
        m_bChangeChanceFlag = TRUE;
    }
}

// This SpinEvent is not really needed because we are checking
// the field change by using the Text Event.
void BatterNotebook::OnComboStatsChangeSpin(wxSpinEvent& event)
{
    m_bChangeStatsFlag = TRUE;
}

void BatterNotebook::OnComboStatsChangeText(wxCommandEvent& event)
{
    // Unfortunatly, this event is triggered even with a SetValue.
    // m_bSetValueFlag is set to TRUE during initialization when
    // the SetValues are issued and then cleared to FALSE.
    if ( !m_bSetValueFlagBatter )
    {
        m_bChangeStatsFlag = TRUE;
    }

	switch (event.GetId()) {
		case ID_STATS_BATTER_2B:
			m_spinStats2B->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_3B:
			m_spinStats3B->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_HR:
			m_spinStatsHR->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_AB:
			m_spinStatsAB->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_HITS:
			m_spinStatsHITS->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_RBI:
			m_spinStatsRBI->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_RE:
			m_spinStatsRE->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_RUNS:
			m_spinStatsRUNS->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_SACRICIFE:
			m_spinStatsSACRICIFE->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_SB:
			m_spinStatsSB->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_CS:
			m_spinStatsCS->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_K:
			m_spinStatsK->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_W:
			m_spinStatsW->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_HBP:
			m_spinStatsHBP->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_BATTER_GAMES:
			m_spinStatsGAMES->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		default:
			break;
    }
}

void BatterNotebook::OnComboInfoChange(wxCommandEvent& event)
{
    // Unfortunatly, this event is triggered even with a SetValue.
    // m_bSetValueFlag is set to TRUE during initialization when
    // the SetValues are issued and then cleared to FALSE.
    if ( !m_bSetValueFlagBatter )
    {
        m_bChangeInfoFlag = TRUE;
    }
}

void BatterNotebook::OnPageChanged(wxNotebookEvent& event)
{
    int oldPage;

    // Test for -1 on GetOldSelection from notebook page. A value
    // of -1 means that no page has been selected and potentially
    // the notebook is still in creation stage.

    // Set the batter the same for all pages as the page being displayed
     oldPage = event.GetOldSelection();
     switch ( oldPage )
     {
         case -1:
             break;
         case 0:   // Position Notebook Page
             m_combo_batter_01->SetSelection(m_combo_batter_04->GetSelection());
             m_combo_batter_02->SetSelection(m_combo_batter_04->GetSelection());
             m_combo_batter_03->SetSelection(m_combo_batter_04->GetSelection());

//             m_combo_team_02->SetSelection(m_combo_team_01->GetSelection());
//             m_combo_team_03->SetSelection(m_combo_team_01->GetSelection());
//             m_combo_team_04->SetSelection(m_combo_team_01->GetSelection());
             break;
         case 1:   // Chance Notebook Page
             m_combo_batter_02->SetSelection(m_combo_batter_01->GetSelection());
             m_combo_batter_03->SetSelection(m_combo_batter_01->GetSelection());
             m_combo_batter_04->SetSelection(m_combo_batter_01->GetSelection());

//             m_combo_team_01->SetSelection(m_combo_team_02->GetSelection());
//             m_combo_team_03->SetSelection(m_combo_team_02->GetSelection());
//             m_combo_team_04->SetSelection(m_combo_team_02->GetSelection());
             break;
         case 2:   // Stats Notebook Page
             m_combo_batter_01->SetSelection(m_combo_batter_02->GetSelection());
             m_combo_batter_03->SetSelection(m_combo_batter_02->GetSelection());
             m_combo_batter_04->SetSelection(m_combo_batter_02->GetSelection());

//             m_combo_team_01->SetSelection(m_combo_team_03->GetSelection());
//             m_combo_team_02->SetSelection(m_combo_team_03->GetSelection());
//             m_combo_team_04->SetSelection(m_combo_team_03->GetSelection());
             break;
         case 3:   // Info Notebook Page
             m_combo_batter_01->SetSelection(m_combo_batter_03->GetSelection());
             m_combo_batter_02->SetSelection(m_combo_batter_03->GetSelection());
             m_combo_batter_04->SetSelection(m_combo_batter_03->GetSelection());

//             m_combo_team_01->SetSelection(m_combo_team_04->GetSelection());
//             m_combo_team_02->SetSelection(m_combo_team_04->GetSelection());
//             m_combo_team_03->SetSelection(m_combo_team_04->GetSelection());
             break;
         default:  // Should never get here
             wxMessageBox(_T("Something is wrong with the Batter Notebook"),
                 _T("Error"), wxOK|wxICON_INFORMATION );
             break;
     }
}

// Value for int combobox is established on call to BuildTopCombos and then
// passed to CreateBatterComboBox from BuildTopCombos
void BatterNotebook::CreateBatterComboBox( wxWindow* parent, int combobox )
{

    switch ( combobox )
    {
        case 1:
            m_combo_batter_01 = new wxComboBox( parent, ID_COMBO_BATTER, _T(""),
                              wxDefaultPosition, wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayBatterFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER  );
            break;
        case 2:
            m_combo_batter_02 = new wxComboBox( parent, ID_COMBO_BATTER, _T(""),
                              wxDefaultPosition, wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayBatterFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER  );
            break;
        case 3:
            m_combo_batter_03 = new wxComboBox( parent, ID_COMBO_BATTER, _T(""),
                              wxDefaultPosition, wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayBatterFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER  );
            break;
        case 4:
            m_combo_batter_04 = new wxComboBox( parent, ID_COMBO_BATTER, _T(""),
                              wxDefaultPosition, wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayBatterFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER  );
            break;
        default:
            break;
    }
}

// Value for "int combobox" is established on call to BuildTopCombos and then
// passed to CreateTeamComboBox from BuildTopCombos
void BatterNotebook::CreateTeamComboBox( wxWindow* parent, int combobox )
{
    switch ( combobox )
    {
        case 1:
            m_text_team_01 = new wxTextCtrl( parent, ID_COMBO_BATTER_TEAM,
                            wxGetApp().pDBRoutines->m_strTeamName,
                            wxDefaultPosition, wxSize(200,-1),
                            wxTE_READONLY );
//            m_combo_team_01 = new wxComboBox( parent, ID_COMBO_BATTER_TEAM, _T(""),
//                              wxDefaultPosition, wxSize(200,-1),
//                              wxGetApp().pDBRoutines->m_strTeamName,
//                              wxCB_READONLY | wxTE_PROCESS_ENTER );
            break;
        case 2:
            m_text_team_02 = new wxTextCtrl( parent, ID_COMBO_BATTER_TEAM,
                            wxGetApp().pDBRoutines->m_strTeamName,
                            wxDefaultPosition, wxSize(200,-1),
                            wxTE_READONLY );
            break;
        case 3:
            m_text_team_03 = new wxTextCtrl( parent, ID_COMBO_BATTER_TEAM,
                            wxGetApp().pDBRoutines->m_strTeamName,
                            wxDefaultPosition, wxSize(200,-1),
                            wxTE_READONLY );
            break;
        case 4:
            m_text_team_04 = new wxTextCtrl( parent, ID_COMBO_BATTER_TEAM,
                            wxGetApp().pDBRoutines->m_strTeamName,
                            wxDefaultPosition, wxSize(200,-1),
                            wxTE_READONLY );
            break;
        default:
            break;
    }
}

void BatterNotebook::UpdateBatterNames()
{
    // This is kind of working
    // I am forcing the positioning and when
    // the replace is replaced it ends at the end of the tab order

    int selItem  = m_combo_batter_01->GetSelection();
//    wxComboBox* newCb = new wxComboBox(this, wxID_ANY, wxEmptyString,
//                                wxDefaultPosition, wxDefaultSize,
//                                items,
//                                flags);
//    wxComboBox* newCb = new wxComboBox( this, ID_COMBO_PITCHER, _T(""),
//                              wxPoint(100,55), wxSize(200,-1),
    wxComboBox* newCb01 = new wxComboBox( m_panel_01, wxID_ANY, wxEmptyString,
                              wxPoint(100,55), wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayBatterFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER );
    wxComboBox* newCb02 = new wxComboBox( m_panel_02, wxID_ANY, wxEmptyString,
                              wxPoint(100,55), wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayBatterFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER );
    wxComboBox* newCb03 = new wxComboBox( m_panel_03, wxID_ANY, wxEmptyString,
                              wxPoint(100,55), wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayBatterFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER );
    wxComboBox* newCb04 = new wxComboBox( m_panel_04, wxID_ANY, wxEmptyString,
                              wxPoint(100,55), wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayBatterFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER );
    if ( selItem != wxNOT_FOUND )
    {
        newCb01->SetSelection(selItem);
        newCb02->SetSelection(selItem);
        newCb03->SetSelection(selItem);
        newCb04->SetSelection(selItem);
    }

    m_sizerBatterSave->Replace(m_combo_batter_01, newCb01);
    m_sizerBatterSave->Layout();
    m_sizerBatterSave->Replace(m_combo_batter_02, newCb02);
    m_sizerBatterSave->Layout();
    m_sizerBatterSave->Replace(m_combo_batter_03, newCb03);
    m_sizerBatterSave->Layout();
    m_sizerBatterSave->Replace(m_combo_batter_04, newCb04);
    m_sizerBatterSave->Layout();

    delete m_combo_batter_01;
    delete m_combo_batter_02;
    delete m_combo_batter_03;
    delete m_combo_batter_04;
    m_combo_batter_01 = newCb01;
    m_combo_batter_01->SetId(ID_COMBO_BATTER);
    m_combo_batter_02 = newCb02;
    m_combo_batter_02->SetId(ID_COMBO_BATTER);
    m_combo_batter_03 = newCb03;
    m_combo_batter_03->SetId(ID_COMBO_BATTER);
    m_combo_batter_04 = newCb04;
    m_combo_batter_04->SetId(ID_COMBO_BATTER);
}

void BatterNotebook::MakeBatterUpdate()
{
//    wxMessageBox(_T("MakePitcherUpdate Button Pressed"),
//                 _T("MakePitcherUpdate"), wxOK|wxICON_INFORMATION );

    if ( m_bChangeStatsFlag )
    {
        wxGetApp().pDBRoutines->DBUpdateBatterStats( wxGetApp().pDBRoutines->m_intBatterStatsID  );
    }

    // Should only update PitcherData for Base team when Info and Chance data changes
    if ( m_bChangeInfoFlag || m_bChangeChanceFlag || m_bChangePositionFlag )
    {
        if (wxGetApp().pDBRoutines->structTeamData.BaseTeam == TRUE)
        {
            wxGetApp().pDBRoutines->DBUpdateBatterData( wxGetApp().pDBRoutines->structBatterStats.BatterID );
        }
        else
        {
            // Might eventually ask if its OK to update non base entry
            wxMessageBox(wxT("Team is not a Base Team!"), wxT("Team"), wxOK);
        }
    }

    m_bChangeInfoFlag = FALSE;
    m_bChangeStatsFlag = FALSE;
    m_bChangeChanceFlag = FALSE;
    m_bChangePositionFlag = FALSE;
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

//BEGIN_EVENT_TABLE(BatterDialog, wxPanel)
//    // Controls
////    EVT_BUTTON(wxID_OK, BatterDialog::OnOK)
//END_EVENT_TABLE()

BatterDialog::BatterDialog (wxWindow* parent, long style)
    : wxDialog(parent, -1, "Batter Add/Edit",
                    wxPoint(10,10), wxSize(500,625),
//                    wxPoint(10,10), wxSize(500,550),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    int dialogClientWidth, dialogClientHeight;
    int myBatterDialogReturnCode;
    int rc;

    CenterOnParent(); // Centers on the parent window

	rc = GetTeamNamesArray();
    if (rc == false)
    {
		return;
	}

    //  Now create a Notebook in the client area of the batter dialog
    GetClientSize(&dialogClientWidth, &dialogClientHeight);
    m_pBatterNotebook = new BatterNotebook( this, 0, 0,
                            dialogClientWidth, dialogClientHeight );

    myBatterDialogReturnCode = ShowModal();
//    switch ( myBatterDialogReturnCode )
//    {
//        // OK
//        case wxID_OK:
//             break;
//        // Cancel
//        case wxID_CANCEL:
//             break;
//        // Add
//        case wxID_ADD:
//             break;
//        // Apply
//        case wxID_APPLY:
//             break;
//        // New
//        case wxID_NEW:
//             break;
//        default:
//             break;
//    }
}

BatterDialog::~BatterDialog( )
{
    Destroy();
}

// Get an array of Team names and save it in a member variable
// Display a selection of Leagues then create an array of teams
bool BatterDialog::GetTeamNamesArray()
{
	int rc;
	// Check is DB is open, if not, open one
	wxGetApp().pDBRoutines->DBIsDBOpen();

    rc = wxGetApp().pDBRoutines->DBGetTeamNamesArray();

    if (rc == false)
    {
		return false;
	}
	else
	{
		return true;
	}
}

BEGIN_EVENT_TABLE(PitcherNotebook, wxPanel)
    // Controls
    EVT_BUTTON(wxID_APPLY, PitcherNotebook::OnApply)
    EVT_BUTTON(wxID_ADD, PitcherNotebook::OnAdd)
    EVT_BUTTON(wxID_NEW, PitcherNotebook::OnNew)
    EVT_BUTTON(wxID_OK, PitcherNotebook::OnOK)
    EVT_BUTTON(wxID_CANCEL, PitcherNotebook::OnCancel)
    EVT_COMBOBOX(ID_COMBO_PITCHER, PitcherNotebook::OnComboPitcherSelect)
    EVT_COMBOBOX(ID_COMBO_PITCHER_TEAM, PitcherNotebook::OnComboTeamSelect)

    EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK_PITCHER, PitcherNotebook::OnPageChanged)


    // Events for Info Panel
    EVT_COMBOBOX(ID_INFO_PITCHER_STARTER, PitcherNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_PITCHER_RELIEF, PitcherNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_PITCHER_BALK, PitcherNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_PITCHER_P, PitcherNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_PITCHER_HITTINGCARD, PitcherNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_PITCHER_ERP, PitcherNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_PITCHER_WP, PitcherNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_PITCHER_THROWS, PitcherNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_PITCHER_BUNTING, PitcherNotebook::OnComboInfoChange)
    EVT_COMBOBOX(ID_INFO_PITCHER_HOLD, PitcherNotebook::OnComboInfoChange)

    // Events for Chance Panel
    EVT_TEXT(ID_CHANCE_PITCHER_BASIC, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_RIGHT, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_LEFT, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_B1B, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_B2B, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_B3B, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_BHR, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_BW, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_BDP, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_L1B, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_L2B, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_L3B, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_LHR, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_LW, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_LDP, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_R1B, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_R2B, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_R3B, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_RHR, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_RW, PitcherNotebook::OnComboChanceChange)
    EVT_TEXT(ID_CHANCE_PITCHER_RDP, PitcherNotebook::OnComboChanceChange)

    // Events for Stats Panel
    EVT_SPINCTRL(ID_STATS_PITCHER_WINS, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_LOSS, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_STARTS, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_SAVE, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_HITS, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_GAMES, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_CGAMES, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_K, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_W, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_HR, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_ER, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_ERA, PitcherNotebook::OnComboStatsChangeSpin)
    EVT_SPINCTRL(ID_STATS_PITCHER_WHIP, PitcherNotebook::OnComboStatsChangeSpin)
    // For Spin we must still check Text updates
    EVT_TEXT(ID_STATS_PITCHER_WINS, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_LOSS, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_STARTS, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_SAVE, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_IP, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_HITS, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_GAMES, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_CGAMES, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_K, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_W, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_HR, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_ER, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_ERA, PitcherNotebook::OnComboStatsChangeText)
    EVT_TEXT(ID_STATS_PITCHER_WHIP, PitcherNotebook::OnComboStatsChangeText)

END_EVENT_TABLE()

PitcherNotebook::PitcherNotebook( wxDialog* frame, int x, int y, int w, int h )
       : wxPanel( frame, wxID_ANY, wxPoint(x, y), wxSize(w, h) )
{
    // Initialize all of the Notebook variables
    Initialization( );

    // Create a Notebook which will have four pages
    // x, y, w, and h are used to create the notebook the same
    // size as the dialog.
    CreateNotebook( x, y, w, h);

    // Set all of the defaults for spin, text and combos controls
    DefaultPanelValues( );
}

PitcherNotebook::~PitcherNotebook( )
{
}

// Used to create the Batter Notebook and its three panels
void PitcherNotebook::CreateNotebook(int x, int y, int w, int h)
{
    // Create a Notebook which will have three pages
    m_pNotebook = new wxNotebook(this, ID_NOTEBOOK_PITCHER, wxPoint(x, y),
                       wxSize(w, h));

    // Add Info Notebook page
    m_panel_01 = new wxPanel(m_pNotebook);
    m_panel_01->SetAutoLayout( true );

    sizer_01 = new wxBoxSizer( wxVERTICAL );

    sizer_01->Add ( BuildTopCombos( m_panel_01, 1 ),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND );

    sizer_01->Add( new wxStaticText(m_panel_01, wxID_ANY,
            _T("Information")),
            0, wxBOTTOM|wxALIGN_CENTER, 15);

    wxGridSizer* m_hGridSizer00 = new wxGridSizer( 1, 4, 0, 0 );

    wxGridSizer* m_hGridSizer01 = new wxGridSizer( 4, 6, 0, 0 );

    m_hGridSizer00->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("First Name"), wxDefaultPosition, wxSize(80,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_textFirstName = new wxTextCtrl( m_panel_01, ID_INFO_PITCHER_FIRSTNAME, "",
                    wxDefaultPosition, wxSize(120,-1), 0);
//                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizer00->Add( m_textFirstName, 0, wxALIGN_LEFT, 0 );

    m_hGridSizer00->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("Last Name"), wxDefaultPosition, wxSize(75,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_textLastName = new wxTextCtrl( m_panel_01, ID_INFO_PITCHER_LASTNAME, "",
                    wxDefaultPosition, wxSize(120,-1), 0);
//                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizer00->Add( m_textLastName, 0, wxALIGN_LEFT, 0 );

    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("Starter"), wxDefaultPosition, wxSize(50,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_InfoStarter = new wxComboBox(m_panel_01, ID_INFO_PITCHER_STARTER,
                    _T("1"), wxDefaultPosition, wxDefaultSize,
//                    _T("1"), wxDefaultPosition, wxSize(50,-1),
                    m_array_InfoStarter, wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer01->Add( m_combo_InfoStarter,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("Relief"), wxDefaultPosition, wxSize(45,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_InfoRelief = new wxComboBox(m_panel_01, ID_INFO_PITCHER_RELIEF,
                    _T("1"), wxDefaultPosition, wxDefaultSize,
//                    _T("1"), wxDefaultPosition, wxSize(50,-1),
                    m_array_InfoRelief, wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer01->Add( m_combo_InfoRelief,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("Balk"), wxDefaultPosition, wxSize(35,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_InfoBalk = new wxComboBox(m_panel_01, ID_INFO_PITCHER_BALK,
                    _T("0"), wxDefaultPosition, wxDefaultSize, m_array_InfoBalk,
//                    _T("0"), wxDefaultPosition, wxSize(50,-1), m_array_InfoBalk,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer01->Add( m_combo_InfoBalk,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("P"), wxDefaultPosition, wxSize(15,-1), 0),
                    0, wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_InfoP = new wxComboBox(m_panel_01, ID_INFO_PITCHER_P,
                    _T("0"), wxDefaultPosition, wxDefaultSize,
//                    _T("0"), wxDefaultPosition, wxSize(50,-1),
                    m_array_InfoP, wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer01->Add( m_combo_InfoP,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("ERP"), wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_InfoERP = new wxComboBox(m_panel_01, ID_INFO_PITCHER_ERP,
                    _T("0"), wxDefaultPosition, wxDefaultSize,
//                    _T("0"), wxDefaultPosition, wxSize(50,-1),
                    m_array_InfoERP, wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer01->Add( m_combo_InfoERP,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("WP"), wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_InfoWP = new wxComboBox(m_panel_01, ID_INFO_PITCHER_WP,
                    _T("0"), wxDefaultPosition, wxDefaultSize,
//                    _T("0"), wxDefaultPosition, wxSize(50,-1),
                    m_array_InfoWP, wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer01->Add( m_combo_InfoWP,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("Throws"), wxDefaultPosition, wxSize(50,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_InfoThrows = new wxComboBox(m_panel_01, ID_INFO_PITCHER_THROWS,
                    _T("R"), wxDefaultPosition, wxDefaultSize, m_array_InfoThrows,
//                    _T("R"), wxDefaultPosition, wxSize(50,-1), m_array_InfoThrows,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer01->Add( m_combo_InfoThrows,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("Bunting"), wxDefaultPosition, wxSize(55,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_InfoBunting = new wxComboBox(m_panel_01, ID_INFO_PITCHER_BUNTING,
                    _T("A"), wxDefaultPosition, wxDefaultSize,
//                    _T("A"), wxDefaultPosition, wxSize(50,-1),
                    m_array_InfoBunting, wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer01->Add( m_combo_InfoBunting,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("Hold"), wxDefaultPosition, wxSize(35,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_InfoHold = new wxComboBox(m_panel_01, ID_INFO_PITCHER_HOLD, _T("0"),
                    wxDefaultPosition, wxDefaultSize, m_array_InfoHold,
//                    wxDefaultPosition, wxSize(50,-1), m_array_InfoHold,
                    wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer01->Add( m_combo_InfoHold,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    m_hGridSizer01->Add( new wxStaticText(m_panel_01, wxID_ANY,
                    _T("Card"), wxDefaultPosition, wxSize(35,-1), 0),
                    0, wxALIGN_RIGHT|wxRIGHT, 0 );
    m_combo_InfoHittingCard = new wxComboBox(m_panel_01, ID_INFO_PITCHER_HITTINGCARD,
                    _T("0"), wxDefaultPosition, wxDefaultSize,
//                    _T("0"), wxDefaultPosition, wxSize(50,-1),
                    m_array_InfoHittingCard, wxCB_READONLY | wxTE_PROCESS_ENTER);
    m_hGridSizer01->Add( m_combo_InfoHittingCard,
                    0, wxALIGN_LEFT|wxLEFT, 0 );

    sizer_01->Add ( m_hGridSizer00 );
    // Add a space
//    sizer_01->AddStretchSpacer(1);
    sizer_01->AddSpacer(20);
    sizer_01->Add ( m_hGridSizer01, 1, wxEXPAND );

    sizer_01->Add ( BuildControlButtons(m_panel_01, 1),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND);

    m_panel_01->SetSizer( sizer_01 );
    m_pNotebook->AddPage(m_panel_01, _T("Info"), false, -1);

    // Add Chance Notebook page
    m_panel_02 = new wxPanel(m_pNotebook);

    sizer_02 = new wxBoxSizer( wxVERTICAL );

    sizer_02->Add ( BuildTopCombos( m_panel_02, 2 ),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND );

    sizer_02->Add( new wxStaticText(m_panel_02, wxID_ANY,
            _T("Chance")),
            0, wxBOTTOM|wxALIGN_CENTER, 15);

    wxBoxSizer* sizer_02_HB = new wxBoxSizer( wxHORIZONTAL );

    sizer_02_HB->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("Basic"),
                    wxDefaultPosition, wxSize(40,-1), 0), 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_textChanceBasic = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_BASIC, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    sizer_02_HB->Add( m_textChanceBasic, 0, wxALIGN_LEFT, 0 );
    // Add 10 pixel space between Basic and Left
    sizer_02_HB->AddSpacer(10);
    sizer_02_HB->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("Left"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_textChanceLeft = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_LEFT, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    sizer_02_HB->Add( m_textChanceLeft, 0, wxALIGN_LEFT, 0 );
    // Add 10 pixel space between Left and Right
    sizer_02_HB->AddSpacer(10);
    sizer_02_HB->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("Right"),
                    wxDefaultPosition, wxSize(40,-1), 0), 0, wxALIGN_CENTER_VERTICAL, 0 );
    m_textChanceRight = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_RIGHT, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    sizer_02_HB->Add( m_textChanceRight, 0, wxALIGN_LEFT, 0 );

    sizer_02->Add ( sizer_02_HB, 0, wxALIGN_CENTER, 0 );

    // Add a space after Basic
    sizer_02->AddSpacer(10);

    wxGridSizer* m_hGridSizerB02 = new wxGridSizer( 2, 6, 0, 0 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("1B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceB1B = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_B1B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceB1B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("HR"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceBHR = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_BHR, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceBHR, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("2B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceB2B = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_B2B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceB2B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("W"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceBW = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_BW, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceBW, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("3B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceB3B = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_B3B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceB3B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerB02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("DP"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceBDP = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_BDP, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerB02->Add( m_textChanceBDP, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    sizer_02->Add( m_hGridSizerB02, 0, wxALIGN_CENTER );

    // Add a space after Left Right
    sizer_02->AddSpacer(10);

    wxGridSizer* sizer_02_HH = new wxGridSizer( 1, 2, 0, 0 );

    wxGridSizer* m_hGridSizerL02 = new wxGridSizer( 3, 4, 0, 0 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("1B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceL1B = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_L1B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceL1B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("HR"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceLHR = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_LHR, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceLHR, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("2B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceL2B = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_L2B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceL2B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("W"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceLW = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_LW, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceLW, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("3B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceL3B = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_L3B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceL3B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerL02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("DP"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceLDP = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_LDP, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerL02->Add( m_textChanceLDP, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    sizer_02_HH->Add( m_hGridSizerL02, 0, wxALIGN_CENTER );
//    sizer_02_HH->Add( m_hGridSizerL02, 0, wxALIGN_CENTER | wxEXPAND );

    wxGridSizer* m_hGridSizerR02 = new wxGridSizer( 3, 4, 0, 0 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("1B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceR1B = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_R1B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceR1B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("HR"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceRHR = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_RHR, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceRHR, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("2B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceR2B = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_R2B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceR2B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("W"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceRW = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_RW, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceRW, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("3B"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceR3B = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_R3B, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceR3B, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    m_hGridSizerR02->Add( new wxStaticText(m_panel_02, wxID_ANY, _T("DP"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxBOTTOM, 10 );
    m_textChanceRDP = new wxTextCtrl( m_panel_02, ID_CHANCE_PITCHER_RDP, "",
                    wxDefaultPosition, wxSize(55,-1), 0);
    m_hGridSizerR02->Add( m_textChanceRDP, 0, wxALIGN_LEFT|wxBOTTOM, 10 );
    sizer_02_HH->Add( m_hGridSizerR02, 0, wxALIGN_CENTER );
//    sizer_02_HH->Add( m_hGridSizerR02, 0, wxALIGN_CENTER | wxEXPAND );

    sizer_02->Add ( sizer_02_HH, 0, wxALIGN_CENTER );
//    sizer_02->Add ( sizer_02_HH, 0, wxALIGN_CENTER | wxEXPAND );

    // Add a space to expand the panel
    sizer_02->Add ( 1, 10, wxEXPAND );

    sizer_02->Add ( BuildControlButtons(m_panel_02, 2),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND);

    m_panel_02->SetSizer( sizer_02 );
    m_pNotebook->AddPage(m_panel_02, _T("Chance"), false, -1);

    // Add Stats Notebook page
    m_panel_03 = new wxPanel( m_pNotebook );

    sizer_03 = new wxBoxSizer( wxVERTICAL );

    sizer_03->Add ( BuildTopCombos( m_panel_03, 3 ),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND );

    sizer_03->Add( new wxStaticText(m_panel_03, wxID_ANY,
            _T("Statistics")),
            0, wxBOTTOM|wxALIGN_CENTER, 15);

    wxGridSizer* m_hGridSizer03 = new wxGridSizer( 6, 6, 0, 0 );

    // Add text to spin button
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("Wins"),
                    wxDefaultPosition, wxSize(40,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsWins = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_WINS,
                    wxEmptyString, wxDefaultPosition, wxSize(55,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsWins, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("Loss"),
                    wxDefaultPosition, wxSize(35,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsLoss = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_LOSS,
                    wxEmptyString, wxDefaultPosition, wxSize(55,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsLoss, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("Starts"),
                    wxDefaultPosition, wxSize(45,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsStarts = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_STARTS,
                    wxEmptyString, wxDefaultPosition, wxSize(55,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsStarts, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("Save"),
                    wxDefaultPosition, wxSize(35,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsSave = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_SAVE,
                    wxEmptyString, wxDefaultPosition, wxSize(55,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsSave, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("IP"),
                    wxDefaultPosition, wxSize(15,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_textStatsIP = new wxTextCtrl( m_panel_03, ID_STATS_PITCHER_IP, "",
                    wxDefaultPosition, wxSize(65,-1), 0);
    m_hGridSizer03->Add( m_textStatsIP, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("Hits"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsHits = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_HITS,
                    wxEmptyString, wxDefaultPosition, wxSize(60,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsHits, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("Games"),
                    wxDefaultPosition, wxSize(50,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsGames = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_GAMES,
                    wxEmptyString, wxDefaultPosition, wxSize(55,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsGames, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("CGames"),
                    wxDefaultPosition, wxSize(60,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsCGames = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_CGAMES,
                    wxEmptyString, wxDefaultPosition, wxSize(55,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsCGames, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("W"),
                    wxDefaultPosition, wxSize(20,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsW = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_W,
                    wxEmptyString, wxDefaultPosition, wxSize(55,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsW, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("K"),
                    wxDefaultPosition, wxSize(15,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsK = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_K,
                    wxEmptyString, wxDefaultPosition, wxSize(55,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsK, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("HR"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsHR = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_HR,
                    wxEmptyString, wxDefaultPosition, wxSize(55,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsHR, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("ER"),
                    wxDefaultPosition, wxSize(25,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_spinStatsER = new wxSpinCtrl( m_panel_03, ID_STATS_PITCHER_ER,
                    wxEmptyString, wxDefaultPosition, wxSize(55,-1),
                    wxSP_ARROW_KEYS, 0, 10, 0 );
    m_hGridSizer03->Add( m_spinStatsER, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("ERA"),
                    wxDefaultPosition, wxSize(30,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_textStatsERA = new wxTextCtrl(m_panel_03, ID_STATS_PITCHER_ERA, _T(""),
                    wxDefaultPosition, wxSize(55,-1) );
    m_hGridSizer03->Add( m_textStatsERA, 0, wxALIGN_LEFT|wxLEFT, 0 );
    m_hGridSizer03->Add( new wxStaticText(m_panel_03, wxID_ANY, _T("WHIP%"),
                    wxDefaultPosition, wxSize(55,-1), 0), 0,
                    wxALIGN_RIGHT|wxRIGHT, 0 );
    m_textStatsWHIP = new wxTextCtrl(m_panel_03, ID_STATS_PITCHER_WHIP, _T(""),
                    wxDefaultPosition, wxSize(55,-1) );
    m_hGridSizer03->Add( m_textStatsWHIP, 0, wxALIGN_LEFT|wxLEFT, 0 );

    sizer_03->Add (m_hGridSizer03, 1, wxEXPAND);

    sizer_03->Add (BuildControlButtons(m_panel_03, 3),
                    0, wxALIGN_CENTER );
//                    0, wxALIGN_CENTER | wxEXPAND);

    m_panel_03->SetSizer( sizer_03 );

    m_pNotebook->AddPage(m_panel_03, _T("Stats"), false, -1);
    // Set page to be displayed to Stats which is tab 2 on a 0 to 2 basis
    m_pNotebook->SetSelection(2);

}
void PitcherNotebook::DefaultPanelValues( )
{
    // Set flag to show this is setting the initial valuse
    m_bSetValueFlagPitcher = TRUE;

    // Set correct range values for each control
    m_spinStatsWins->SetRange(0,300);
    m_spinStatsLoss->SetRange(0,300);
    m_spinStatsStarts->SetRange(0,300);
    m_spinStatsSave->SetRange(0,300);
    m_spinStatsHits->SetRange(0,999);
    m_spinStatsGames->SetRange(0,500);
    m_spinStatsCGames->SetRange(0,500);
    m_spinStatsW->SetRange(0,300);
    m_spinStatsK->SetRange(0,300);
    m_spinStatsHR->SetRange(0,300);
    m_spinStatsER->SetRange(0,300);

    // Initialize Stats Notebook Page - 12 entries
    m_spinStatsWins->SetValue(0);
    m_spinStatsLoss->SetValue(0);
    m_spinStatsStarts->SetValue(0);
    m_spinStatsSave->SetValue(0);
    m_spinStatsHits->SetValue(0);
    m_spinStatsGames->SetValue(0);
    m_spinStatsCGames->SetValue(0);
    m_spinStatsW->SetValue(0);
    m_spinStatsK->SetValue(0);
    m_spinStatsHR->SetValue(0);
    m_spinStatsER->SetValue(0);
    m_textStatsIP->SetValue( "0" );
   	m_textStatsIP->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
    m_textStatsERA->SetValue( "0" );
    m_textStatsWHIP->SetValue( "0" );
    // End Stats Notebook Page

    // Initialize Chance Notebook Page - 21 entries
    m_textChanceBasic->SetValue( "0.0" );
    m_textChanceLeft->SetValue( "0.0" );
    m_textChanceRight->SetValue( "0.0" );
    m_textChanceB1B->SetValue( "0.0" );
    m_textChanceB2B->SetValue( "0.0" );
    m_textChanceB3B->SetValue( "0.0" );
    m_textChanceBHR->SetValue( "0.0" );
    m_textChanceBW->SetValue( "0.0" );
    m_textChanceBDP->SetValue( "0.0" );
    m_textChanceL1B->SetValue( "0.0" );
    m_textChanceL2B->SetValue( "0.0" );
    m_textChanceL3B->SetValue( "0.0" );
    m_textChanceLHR->SetValue( "0.0" );
    m_textChanceLW->SetValue( "0.0" );
    m_textChanceLDP->SetValue( "0.0" );
    m_textChanceR1B->SetValue( "0.0" );
    m_textChanceR2B->SetValue( "0.0" );
    m_textChanceR3B->SetValue( "0.0" );
    m_textChanceRHR->SetValue( "0.0" );
    m_textChanceRW->SetValue( "0.0" );
    m_textChanceRDP->SetValue( "0.0" );
    // End Chance Notebook Page

    // Initialize Info Notebook Page - 9 entries
    m_combo_InfoStarter->SetValue( "0" );
    m_combo_InfoRelief->SetValue( "0" );
    m_combo_InfoBalk->SetValue( "0" );
    m_combo_InfoP->SetValue( "0" );
    m_combo_InfoHittingCard->SetValue( "0" );
    m_combo_InfoERP->SetValue( "0" );
    m_combo_InfoWP->SetValue( "0" );
    m_combo_InfoThrows->SetValue( "R" );
    m_combo_InfoBunting->SetValue( "A" );
    m_combo_InfoHold->SetValue( "0" );
    // End Info Notebook Page

    // Clear the flag since we are done setting initial values
    m_bSetValueFlagPitcher = FALSE;
}

void PitcherNotebook::Initialization( )
{
//    int i;

    // Clear all change flags
    m_bSetValueFlagPitcher = FALSE;
    m_bChangeChanceFlag = FALSE;
    m_bChangeStatsFlag = FALSE;
    m_bChangeInfoFlag = FALSE;

    m_pNotebook = NULL;
    m_panel_01 = NULL;
    m_panel_02 = NULL;
    m_panel_03 = NULL;

    m_combo_pitcher_01 = NULL;
    m_combo_pitcher_02 = NULL;
    m_combo_pitcher_03 = NULL;
    m_text_team_01 = NULL;
    m_text_team_02 = NULL;
    m_text_team_03 = NULL;

    sizer_01 = NULL;
    sizer_02 = NULL;
    sizer_03 = NULL;

    m_combo_InfoStarter = NULL;
    m_combo_InfoRelief = NULL;
    m_combo_InfoBalk = NULL;
    m_combo_InfoP = NULL;
    m_combo_InfoHittingCard = NULL;
    m_combo_InfoERP = NULL;
    m_combo_InfoWP = NULL;
    m_combo_InfoThrows = NULL;
    m_combo_InfoBunting = NULL;
    m_combo_InfoHold = NULL;

    // Text controls for Chance Page
    m_textChanceBasic = NULL;
    m_textChanceLeft = NULL;
    m_textChanceRight = NULL;
    m_textChanceB1B = NULL;
    m_textChanceB2B = NULL;
    m_textChanceB3B = NULL;
    m_textChanceBHR = NULL;
    m_textChanceBW = NULL;
    m_textChanceBDP = NULL;
    m_textChanceL1B = NULL;
    m_textChanceL2B = NULL;
    m_textChanceL3B = NULL;
    m_textChanceLHR = NULL;
    m_textChanceLW = NULL;
    m_textChanceLDP = NULL;
    m_textChanceR1B = NULL;
    m_textChanceR2B = NULL;
    m_textChanceR3B = NULL;
    m_textChanceRHR = NULL;
    m_textChanceRW = NULL;
    m_textChanceRDP = NULL;
    // Spin controls for Statistics Page
    m_spinStatsWins = NULL;
    m_spinStatsLoss = NULL;
    m_spinStatsStarts = NULL;
    m_spinStatsSave = NULL;
    m_textStatsIP = NULL;
    m_spinStatsHits = NULL;
    m_spinStatsGames = NULL;
    m_spinStatsCGames = NULL;
    m_spinStatsW = NULL;
    m_spinStatsK = NULL;
    m_spinStatsHR = NULL;
    m_spinStatsER = NULL;
    m_textStatsERA = NULL;
    m_textStatsWHIP = NULL;

    // Set correct range values for each control
    m_array_InfoThrows.Add("L");
    m_array_InfoThrows.Add("R");
    m_array_InfoStarter.Add("0");
    m_array_InfoStarter.Add("1");
    m_array_InfoStarter.Add("2");
    m_array_InfoStarter.Add("3");
    m_array_InfoStarter.Add("4");
    m_array_InfoStarter.Add("5");
    m_array_InfoStarter.Add("6");
    m_array_InfoStarter.Add("7");
    m_array_InfoStarter.Add("8");
    m_array_InfoStarter.Add("9");
    m_array_InfoRelief.Add("0");
    m_array_InfoRelief.Add("1");
    m_array_InfoRelief.Add("2");
    m_array_InfoRelief.Add("3");
    m_array_InfoRelief.Add("4");
    m_array_InfoRelief.Add("5");
    m_array_InfoBalk.Add("0");
    m_array_InfoBalk.Add("1");
    m_array_InfoBalk.Add("2");
    m_array_InfoBalk.Add("3");
    m_array_InfoBalk.Add("4");
    m_array_InfoBalk.Add("5");
    m_array_InfoBalk.Add("6");
    m_array_InfoBalk.Add("7");
    m_array_InfoBalk.Add("8");
    m_array_InfoBalk.Add("9");
    m_array_InfoBalk.Add("10");
    m_array_InfoBalk.Add("11");
    m_array_InfoBalk.Add("12");
    m_array_InfoBalk.Add("13");
    m_array_InfoBalk.Add("14");
    m_array_InfoBalk.Add("15");
    m_array_InfoBalk.Add("16");
    m_array_InfoBalk.Add("17");
    m_array_InfoBalk.Add("18");
    m_array_InfoBalk.Add("19");
    m_array_InfoBalk.Add("20");
    m_array_InfoP.Add("0");
    m_array_InfoP.Add("1");
    m_array_InfoP.Add("2");
    m_array_InfoP.Add("3");
    m_array_InfoP.Add("4");
    m_array_InfoP.Add("5");
    m_array_InfoHittingCard.Add("0");
    m_array_InfoHittingCard.Add("1");
    m_array_InfoHittingCard.Add("2");
    m_array_InfoHittingCard.Add("3");
    m_array_InfoHittingCard.Add("4");
    m_array_InfoHittingCard.Add("5");
    m_array_InfoHittingCard.Add("6");
    m_array_InfoHittingCard.Add("7");
    m_array_InfoHittingCard.Add("8");
    m_array_InfoERP.Add("0");
    m_array_InfoERP.Add("1");
    m_array_InfoERP.Add("2");
    m_array_InfoERP.Add("3");
    m_array_InfoERP.Add("4");
    m_array_InfoERP.Add("5");
    m_array_InfoERP.Add("6");
    m_array_InfoERP.Add("7");
    m_array_InfoERP.Add("8");
    m_array_InfoERP.Add("9");
    m_array_InfoERP.Add("10");
    m_array_InfoERP.Add("11");
    m_array_InfoERP.Add("12");
    m_array_InfoERP.Add("13");
    m_array_InfoERP.Add("14");
    m_array_InfoERP.Add("15");
    m_array_InfoERP.Add("16");
    m_array_InfoERP.Add("17");
    m_array_InfoERP.Add("18");
    m_array_InfoERP.Add("19");
    m_array_InfoERP.Add("20");
    m_array_InfoERP.Add("21");
    m_array_InfoERP.Add("22");
    m_array_InfoERP.Add("23");
    m_array_InfoERP.Add("24");
    m_array_InfoERP.Add("25");
    m_array_InfoERP.Add("26");
    m_array_InfoERP.Add("27");
    m_array_InfoERP.Add("28");
    m_array_InfoERP.Add("29");
    m_array_InfoERP.Add("30");
    m_array_InfoERP.Add("31");
    m_array_InfoERP.Add("32");
    m_array_InfoERP.Add("33");
    m_array_InfoERP.Add("34");
    m_array_InfoERP.Add("35");
    m_array_InfoERP.Add("36");
    m_array_InfoERP.Add("37");
    m_array_InfoERP.Add("38");
    m_array_InfoERP.Add("39");
    m_array_InfoERP.Add("40");
    m_array_InfoERP.Add("41");
    m_array_InfoERP.Add("42");
    m_array_InfoERP.Add("43");
    m_array_InfoERP.Add("44");
    m_array_InfoERP.Add("45");
    m_array_InfoERP.Add("46");
    m_array_InfoERP.Add("47");
    m_array_InfoERP.Add("48");
    m_array_InfoERP.Add("49");
    m_array_InfoERP.Add("50");
    m_array_InfoERP.Add("51");
    m_array_InfoWP.Add("0");
    m_array_InfoWP.Add("1");
    m_array_InfoWP.Add("2");
    m_array_InfoWP.Add("3");
    m_array_InfoWP.Add("4");
    m_array_InfoWP.Add("5");
    m_array_InfoWP.Add("6");
    m_array_InfoWP.Add("7");
    m_array_InfoWP.Add("8");
    m_array_InfoWP.Add("9");
    m_array_InfoWP.Add("10");
    m_array_InfoWP.Add("11");
    m_array_InfoWP.Add("12");
    m_array_InfoWP.Add("13");
    m_array_InfoWP.Add("14");
    m_array_InfoWP.Add("15");
    m_array_InfoWP.Add("16");
    m_array_InfoWP.Add("17");
    m_array_InfoWP.Add("18");
    m_array_InfoWP.Add("19");
    m_array_InfoWP.Add("20");
    m_array_InfoBunting.Add("A");
    m_array_InfoBunting.Add("B");
    m_array_InfoBunting.Add("C");
    m_array_InfoBunting.Add("D");
    m_array_InfoBunting.Add("E");
    m_array_InfoHold.Add("-6");
    m_array_InfoHold.Add("-5");
    m_array_InfoHold.Add("-4");
    m_array_InfoHold.Add("-3");
    m_array_InfoHold.Add("-2");
    m_array_InfoHold.Add("-1");
    m_array_InfoHold.Add("0");
    m_array_InfoHold.Add("1");
    m_array_InfoHold.Add("2");
    m_array_InfoHold.Add("3");
    m_array_InfoHold.Add("4");
    m_array_InfoHold.Add("5");
    m_array_InfoHold.Add("6");
    m_array_InfoHold.Add("7");
    m_array_InfoHold.Add("8");
    m_array_InfoHold.Add("9");
}

wxBoxSizer* PitcherNotebook::BuildControlButtons( wxWindow* panel, int page )
{
    // Create the control buttons (Apply, New, Add, OK, and Cancel)

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonPane = new wxBoxSizer (wxHORIZONTAL);
    switch (page)
    {
            case 1:
				m_applyButton1 = new wxButton (panel, wxID_APPLY);
				//  By adding a space with wxExpand (non zero),
				//    the butons will be positioned correctly
				buttonPane->Add( 1, 1, wxEXPAND );
				buttonPane->Add (m_applyButton1, 0, wxALIGN_CENTER);
                break;
           case 2:
				m_applyButton2 = new wxButton (panel, wxID_APPLY);
				buttonPane->Add( 1, 1, wxEXPAND );
				buttonPane->Add (m_applyButton2, 0, wxALIGN_CENTER);
                break;
           case 3:
				m_applyButton3 = new wxButton (panel, wxID_APPLY);
				buttonPane->Add( 1, 1, wxEXPAND );
				buttonPane->Add (m_applyButton3, 0, wxALIGN_CENTER);
                break;
   }
//    m_applyButton = new wxButton (panel, wxID_APPLY);
//    //  By adding a space with wxExpand (non zero),
//    //    the butons will be positioned correctly
//    buttonPane->Add( 1, 1, wxEXPAND );
//    buttonPane->Add (m_applyButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );
    wxButton *newButton = new wxButton (panel, wxID_NEW);
    buttonPane->Add (newButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );
    switch (page)
    {
		case 1:
				m_addButton1 = new wxButton (panel, wxID_ADD);
				buttonPane->Add (m_addButton1, 0, wxALIGN_CENTER);
				m_addButton1->Enable(false);
                break;
		case 2:
				m_addButton2 = new wxButton (panel, wxID_ADD);
				buttonPane->Add (m_addButton2, 0, wxALIGN_CENTER);
				m_addButton2->Enable(false);
                break;
		case 3:
				m_addButton3 = new wxButton (panel, wxID_ADD);
				buttonPane->Add (m_addButton3, 0, wxALIGN_CENTER);
				m_addButton3->Enable(false);
                break;
   }
//    m_addButton = new wxButton (panel, wxID_ADD);
//    buttonPane->Add (m_addButton, 0, wxALIGN_CENTER);
//    m_addButton->Enable(false);
    buttonPane->Add( 1, 1, wxEXPAND );
    wxButton *okButton = new wxButton (panel, wxID_OK);
    okButton->SetDefault();
    buttonPane->Add (okButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );
    wxButton *cancelButton = new wxButton (panel, wxID_CANCEL);
    buttonPane->Add (cancelButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );

    sizer->Add (new wxStaticLine(panel, -1), 0, wxEXPAND | wxALL, 10);
    sizer->Add (buttonPane, 0, wxALIGN_CENTER );
//    sizer->Add (buttonPane, 0, wxALIGN_CENTER | wxEXPAND);
    // Add a space under the buttons
    sizer->Add( 10, 10, wxSTRETCH_NOT );

    return (sizer);
}

// Value for int combobox is established on call to BuildTopCombos
wxBoxSizer* PitcherNotebook::BuildTopCombos( wxWindow* panel, int combobox )
{
//    wxBoxSizer* sizer;
//    wxBoxSizer* csizerB;
    wxBoxSizer* csizerT;
    wxStaticText* statTextT;
    wxStaticText* statTextP;

    m_sizer = new wxBoxSizer( wxVERTICAL );

    csizerT = new wxBoxSizer ( wxHORIZONTAL );

    statTextT = new wxStaticText(panel, wxID_ANY, _T("Team Name"),
        wxDefaultPosition, wxSize(80,-1), 0);

    CreateTeamComboBox(panel, combobox);

    csizerT->Add ( 10, 1, wxSTRETCH_NOT );
    csizerT->Add ( statTextT, wxALIGN_LEFT );
    csizerT->Add ( 10, 1, wxSTRETCH_NOT );

    switch ( combobox)
    {
           case 1:
                csizerT->Add ( m_text_team_01, wxALIGN_CENTER );
//                csizerT->Add ( m_combo_team_01, wxALIGN_CENTER );
                break;
           case 2:
                csizerT->Add ( m_text_team_02, wxALIGN_CENTER );
                break;
           case 3:
                csizerT->Add ( m_text_team_03, wxALIGN_CENTER );
                break;
   }
    csizerT->Add ( 1, 1, wxEXPAND );

    m_sizer->Add ( 10, 10, wxSTRETCH_NOT );
    m_sizer->Add ( csizerT );

    m_sizerPitcher = new wxBoxSizer ( wxHORIZONTAL );

    statTextP = new wxStaticText(panel, wxID_ANY, _T("Pitcher Name"),
        wxDefaultPosition, wxSize(80,-1), 0);

    CreatePitcherComboBox(panel, combobox);

    m_sizerPitcher->Add ( 10, 1, wxSTRETCH_NOT );
    m_sizerPitcher->Add ( statTextP, wxALIGN_LEFT );
    m_sizerPitcher->Add ( 10, 1, wxSTRETCH_NOT );
    switch ( combobox)
    {
           case 1:
                m_sizerPitcher->Add ( m_combo_pitcher_01, wxALIGN_CENTER );
                break;
           case 2:
                m_sizerPitcher->Add ( m_combo_pitcher_02, wxALIGN_CENTER );
                break;
           case 3:
                m_sizerPitcher->Add ( m_combo_pitcher_03, wxALIGN_CENTER );
                break;
   }
    m_sizerPitcher->Add ( 1, 1, wxEXPAND );
//    m_sizerPitcherSave = m_sizerPitcher;

    m_sizer->Add ( 10, 10, wxSTRETCH_NOT );
    m_sizer->Add ( m_sizerPitcher );
    m_sizer->Add (new wxStaticLine(panel, -1), 0, wxEXPAND | wxALL, 10);
//    m_sizerPitcherSave = m_sizer;
    m_sizerPitcherSave = m_sizerPitcher;

    return (m_sizer);
}

// Value for int combobox is established on call to BuildTopCombos and then
// passed to CreateBatterComboBox from BuildTopCombos
void PitcherNotebook::CreatePitcherComboBox( wxWindow* parent, int combobox )
{

    switch ( combobox )
    {
        case 1:
            m_combo_pitcher_01 = new wxComboBox( parent, ID_COMBO_PITCHER, _T(""),
                              wxDefaultPosition, wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayPitcherFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER  );
            break;
        case 2:
            m_combo_pitcher_02 = new wxComboBox( parent, ID_COMBO_PITCHER, _T(""),
                              wxDefaultPosition, wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayPitcherFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER  );
            break;
        case 3:
            m_combo_pitcher_03 = new wxComboBox( parent, ID_COMBO_PITCHER, _T(""),
                              wxDefaultPosition, wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayPitcherFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER  );
            break;
        default:
            break;
    }
}

// Value for "int combobox" is established on call to BuildTopCombos and then
// passed to CreateTeamComboBox from BuildTopCombos
void PitcherNotebook::CreateTeamComboBox( wxWindow* parent, int combobox )
{
    switch ( combobox )
    {
        case 1:
            m_text_team_01 = new wxTextCtrl( parent, ID_COMBO_PITCHER_TEAM,
                            wxGetApp().pDBRoutines->m_strTeamName,
                            wxDefaultPosition, wxSize(200,-1),
                            wxTE_READONLY );
//            m_combo_team_01 = new wxComboBox( parent, ID_COMBO_PITCHER_TEAM, _T(""),
//                              wxDefaultPosition, wxSize(200,-1),
//                              wxGetApp().pDBRoutines->m_arrayTeamNames,
//                              wxCB_READONLY | wxTE_PROCESS_ENTER );
            break;
        case 2:
            m_text_team_02 = new wxTextCtrl( parent, ID_COMBO_PITCHER_TEAM,
                            wxGetApp().pDBRoutines->m_strTeamName,
                            wxDefaultPosition, wxSize(200,-1),
                            wxTE_READONLY );
            break;
        case 3:
            m_text_team_03 = new wxTextCtrl( parent, ID_COMBO_PITCHER_TEAM,
                            wxGetApp().pDBRoutines->m_strTeamName,
                            wxDefaultPosition, wxSize(200,-1),
                            wxTE_READONLY );
            break;
        default:
            break;
    }
}

void PitcherNotebook::OnComboChanceChange(wxCommandEvent& event)
{
    // Unfortunatly, this event is triggered even with a SetValue.
    // m_bSetValueFlag is set to TRUE during initialization when
    // the SetValues are issued and then cleared to FALSE.
    if ( !m_bSetValueFlagPitcher )
    {
        m_bChangeChanceFlag = TRUE;
    }
}

// This SpinEvent is not really needed because we are checking
// the field change by using the Text Event.
void PitcherNotebook::OnComboStatsChangeSpin(wxSpinEvent& event)
{
    m_bChangeStatsFlag = TRUE;
}

void PitcherNotebook::OnComboStatsChangeText(wxCommandEvent& event)
{
    // Unfortunatly, this event is triggered even with a SetValue.
    // m_bSetValueFlag is set to TRUE during initialization when
    // the SetValues are issued and then cleared to FALSE.
    if ( !m_bSetValueFlagPitcher )
    {
        m_bChangeStatsFlag = TRUE;
    }

	switch (event.GetId()) {
		case ID_STATS_PITCHER_WINS:
			m_spinStatsWins->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_LOSS:
			m_spinStatsLoss->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_STARTS:
			m_spinStatsStarts->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_SAVE:
			m_spinStatsSave->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_IP:
			m_textStatsIP->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_HITS:
			m_spinStatsHits->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_GAMES:
			m_spinStatsGames->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_CGAMES:
			m_spinStatsCGames->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_W:
			m_spinStatsW->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_K:
			m_spinStatsK->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_HR:
			m_spinStatsHR->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		case ID_STATS_PITCHER_ER:
			m_spinStatsER->SetForegroundColour(wxColour(wxT("RED"))); // Red
			break;
		default:
			break;
    }
}

void PitcherNotebook::OnComboInfoChange(wxCommandEvent& event)
{
    // Unfortunatly, this event is triggered even with a SetValue.
    // m_bSetValueFlag is set to TRUE during initialization when
    // the SetValues are issued and then cleared to FALSE.
    if ( !m_bSetValueFlagPitcher )
    {
        m_bChangeInfoFlag = TRUE;
    }
}

void PitcherNotebook::OnPageChanged(wxNotebookEvent& event)
{
    int oldPage;

    // Test for -1 on GetOldSelection from notebook page. A value
    // of -1 means that no page has been selected and potentially
    // the notebook is still in creation stage.

    // Set the batter the same for all pages as the page being displayed
     oldPage = event.GetOldSelection();
     switch ( oldPage )
     {
         case -1:
             break;
         case 0:   // Info Notebook Page
             m_combo_pitcher_02->SetSelection(m_combo_pitcher_01->GetSelection());
             m_combo_pitcher_03->SetSelection(m_combo_pitcher_01->GetSelection());

//             m_combo_team_02->SetSelection(m_text_team_01->GetSelection());
//             m_combo_team_02->SetSelection(m_combo_team_01->GetSelection());
//             m_combo_team_03->SetSelection(m_combo_team_01->GetSelection());
             break;
         case 1:   // Chance Notebook Page
             m_combo_pitcher_01->SetSelection(m_combo_pitcher_02->GetSelection());
             m_combo_pitcher_03->SetSelection(m_combo_pitcher_02->GetSelection());

//             m_text_team_01->SetSelection(m_combo_team_02->GetSelection());
//             m_combo_team_01->SetSelection(m_combo_team_02->GetSelection());
//             m_combo_team_03->SetSelection(m_combo_team_02->GetSelection());
             break;
         case 2:   // Stats Notebook Page
             m_combo_pitcher_01->SetSelection(m_combo_pitcher_03->GetSelection());
             m_combo_pitcher_02->SetSelection(m_combo_pitcher_03->GetSelection());

//             m_text_team_01->SetSelection(m_combo_team_03->GetSelection());
//             m_combo_team_01->SetSelection(m_combo_team_03->GetSelection());
//             m_combo_team_02->SetSelection(m_combo_team_03->GetSelection());
             break;
         default:  // Should never get here
             wxMessageBox(_T("Something is wrong with the Pitcher Notebook"),
                 _T("Error"), wxOK|wxICON_INFORMATION );
             break;
     }
}

void PitcherNotebook::OnApply(wxCommandEvent& event)
{
    wxMessageBox(_T("Apply Button Pressed"),
                 _T("Apply"), wxOK|wxICON_INFORMATION );
    //Save the data
    GetNotebookData();

    MakePitcherUpdate();

    m_bChangeInfoFlag = FALSE;
    m_bChangeStatsFlag = FALSE;
    m_bChangeChanceFlag = FALSE;

	m_spinStatsWins->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsLoss->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsStarts->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsSave->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_textStatsIP->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsHits->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsGames->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsCGames->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsW->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsK->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsHR->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsER->SetForegroundColour(wxColour(wxT("BLACK"))); // Black

    // Now update the current Stat panel in the notebook
    // Create a wxCommandEvent
	wxCommandEvent comboEvent(wxEVT_COMBOBOX, ID_COMBO_PITCHER);

	// Set the selected item
    int mySel = m_pNotebook->GetSelection();
    switch( mySel )
    {
        case -1:
            break;
        case 0:   // Info Notebook Page
            comboEvent.SetInt( m_combo_pitcher_01->GetSelection() );
            break;
        case 1:   // Chance Notebook Page
            comboEvent.SetInt( m_combo_pitcher_02->GetSelection() );
            break;
        case 2:   // Stats Notebook Page
            comboEvent.SetInt( m_combo_pitcher_03->GetSelection() );
            break;
        default:  // Should never get here
            wxMessageBox(_T("Something is wrong with the Batter Notebook"),
                _T("Error"), wxOK|wxICON_INFORMATION );
            break;
    }

    // Post the event to the combobox
    wxPostEvent(m_combo_pitcher_03, comboEvent);
}

void PitcherNotebook::OnOK(wxCommandEvent& event)
{
	int answer;

    // If one of the change flags is TRUE, Ask if updates should be dropped.
    if ( m_bChangeInfoFlag || m_bChangeStatsFlag || m_bChangeChanceFlag )
    {
        answer = wxMessageBox(
                (_T("       A field has changed.\n")
                 _T("Press Yes to save or No to discard")
                ),
                _T("Field Changed"), wxYES_NO|wxICON_QUESTION );
        if ( answer == wxYES)
        {
			if ( m_addButton1->IsEnabled() )
			{
				answer = wxMessageBox(
					(_T("You were Adding a player!\n")
					 _T("Press Yes to add or No to discard")
					),
					_T("Field Changed"), wxYES_NO|wxICON_QUESTION );
				if ( answer == wxYES)
				{
					// Then still Add player
					MyAddEvent();
				}
				// Disable Add button
				m_addButton1->Enable(false);
				m_addButton2->Enable(false);
				m_addButton3->Enable(false);
				m_applyButton1->Enable(true);
				m_applyButton2->Enable(true);
				m_applyButton3->Enable(true);
			}
			else
			{
				//Save the data
				GetNotebookData();
				MakePitcherUpdate();
			}
        }
    }

	m_bChangeInfoFlag = FALSE;
	m_bChangeStatsFlag = FALSE;
	m_bChangeChanceFlag = FALSE;

    // Continue and process this event normally.
    event.Skip(TRUE);
}

void PitcherNotebook::OnCancel(wxCommandEvent& event)
{
    wxMessageBox(_T("Cancel Button Pressed"),
                 _T("Cancel"), wxOK|wxICON_INFORMATION );
    // Continue and process this event normally.
    event.Skip(TRUE);
}

// OnAdd means to clear all fields
void PitcherNotebook::OnAdd(wxCommandEvent& event)
{
	MyAddEvent();
}

// OnAdd means to clear all fields
void PitcherNotebook::MyAddEvent()
{
//    wxMessageBox(_T("Add Button Pressed"),
//                 _T("Add"), wxOK|wxICON_INFORMATION );

    // Need to check for base team. Only allow inserts in base teams
    if ( wxGetApp().pDBRoutines->structTeamData.BaseTeam == TRUE )
    {
        // Need to update data, do insert and then rebbuild player names
        GetNotebookData();
        wxGetApp().pDBRoutines->DBInsertPitcherData(wxGetApp().pDBRoutines->m_intTeamID);
        // The following updates the array but not the combobox
        wxGetApp().pDBRoutines->DBGetPitcherStatsID(wxGetApp().pDBRoutines->m_intTeamID);
        //m_combo_pitcher_01. //  ( wxGetApp().pDBRoutines->m_arrayPitcherFullNames );
        UpdatePitcherNames();

		m_bChangeInfoFlag = FALSE;
		m_bChangeStatsFlag = FALSE;
		m_bChangeChanceFlag = FALSE;
    }
    else
    {
        wxMessageBox(_T("Attempt to insert non-Base team!"),
                 _T("Insert Failed"), wxOK|wxICON_INFORMATION );
    }

	m_spinStatsWins->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsLoss->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsStarts->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsSave->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_textStatsIP->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsHits->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsGames->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsCGames->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsW->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsK->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsHR->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsER->SetForegroundColour(wxColour(wxT("BLACK"))); // Black

	m_addButton1->Enable(false);
	m_addButton2->Enable(false);
	m_addButton3->Enable(false);
	m_applyButton1->Enable(true);
	m_applyButton2->Enable(true);
	m_applyButton3->Enable(true);
}

void PitcherNotebook::UpdatePitcherNames()
{
    // This is kind of working
    // I am forcing the positioning and when
    // the replace is replaced it ends at the end of the tab order

    int selItem  = m_combo_pitcher_01->GetSelection();
//    wxComboBox* newCb = new wxComboBox(this, wxID_ANY, wxEmptyString,
//                                wxDefaultPosition, wxDefaultSize,
//                                items,
//                                flags);
//    wxComboBox* newCb = new wxComboBox( this, ID_COMBO_PITCHER, _T(""),
//                              wxPoint(100,55), wxSize(200,-1),
    wxComboBox* newCb01 = new wxComboBox( m_panel_01, wxID_ANY, wxEmptyString,
                              wxPoint(100,55), wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayPitcherFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER );
    wxComboBox* newCb02 = new wxComboBox( m_panel_02, wxID_ANY, wxEmptyString,
                              wxPoint(100,55), wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayPitcherFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER );
    wxComboBox* newCb03 = new wxComboBox( m_panel_03, wxID_ANY, wxEmptyString,
                              wxPoint(100,55), wxSize(200,-1),
                              wxGetApp().pDBRoutines->m_arrayPitcherFullNames,
                              wxCB_READONLY | wxTE_PROCESS_ENTER );
    if ( selItem != wxNOT_FOUND )
    {
        newCb01->SetSelection(selItem);
        newCb02->SetSelection(selItem);
        newCb03->SetSelection(selItem);
    }

    m_sizerPitcherSave->Replace(m_combo_pitcher_01, newCb01);
    m_sizerPitcherSave->Layout();
    m_sizerPitcherSave->Replace(m_combo_pitcher_02, newCb02);
    m_sizerPitcherSave->Layout();
    m_sizerPitcherSave->Replace(m_combo_pitcher_03, newCb03);
    m_sizerPitcherSave->Layout();

    delete m_combo_pitcher_01;
    delete m_combo_pitcher_02;
    delete m_combo_pitcher_03;
    m_combo_pitcher_01 = newCb01;
    m_combo_pitcher_01->SetId(ID_COMBO_PITCHER);
    m_combo_pitcher_02 = newCb02;
    m_combo_pitcher_02->SetId(ID_COMBO_PITCHER);
    m_combo_pitcher_03 = newCb03;
    m_combo_pitcher_03->SetId(ID_COMBO_PITCHER);
}

void PitcherNotebook::OnNew(wxCommandEvent& event)
{
//    wxMessageBox(_T("New Button Pressed"),
//                 _T("New"), wxOK|wxICON_INFORMATION );

	m_addButton1->Enable(true);
	m_addButton2->Enable(true);
	m_addButton3->Enable(true);
	m_applyButton1->Enable(false);
	m_applyButton2->Enable(false);
	m_applyButton3->Enable(false);

    // Initialize Name
    m_textFirstName->SetValue("");
    m_textLastName->SetValue("");
    wxGetApp().pDBRoutines->structPitcherData.FirstName = "";
    wxGetApp().pDBRoutines->structPitcherData.LastName = "";
    wxGetApp().pDBRoutines->structPitcherData.PitcherID = 0;

    wxGetApp().pDBRoutines->m_arrayPitcherFullNames.Clear();
    UpdatePitcherNames();

    // Initialize Stats Notebook Page - 14 entries
    m_spinStatsWins->SetValue(0);
    m_spinStatsLoss->SetValue(0);
    m_spinStatsStarts->SetValue(0);
    m_spinStatsSave->SetValue(0);
    m_spinStatsHits->SetValue(0);
    m_spinStatsGames->SetValue(0);
    m_spinStatsCGames->SetValue(0);
    m_spinStatsW->SetValue(0);
    m_spinStatsK->SetValue(0);
    m_spinStatsHR->SetValue(0);
    m_spinStatsER->SetValue(0);
    m_textStatsIP->SetValue( "0" );
    m_textStatsERA->SetValue( "0" );
    m_textStatsWHIP->SetValue( "0" );
    wxGetApp().pDBRoutines->structPitcherStats.Wins = 0;
    wxGetApp().pDBRoutines->structPitcherStats.Loss = 0;
    wxGetApp().pDBRoutines->structPitcherStats.Starts = 0;
    wxGetApp().pDBRoutines->structPitcherStats.Saves = 0;
    wxGetApp().pDBRoutines->structPitcherStats.InningsPitched = 0;
   	m_textStatsIP->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
    wxGetApp().pDBRoutines->structPitcherStats.ER = 0;
    wxGetApp().pDBRoutines->structPitcherStats.Hits = 0;
    wxGetApp().pDBRoutines->structPitcherStats.Walks = 0;
    wxGetApp().pDBRoutines->structPitcherStats.Strikeouts = 0;
    wxGetApp().pDBRoutines->structPitcherStats.HomeRuns = 0;
    wxGetApp().pDBRoutines->structPitcherStats.Games = 0;
    wxGetApp().pDBRoutines->structPitcherStats.CompleteGames = 0;
    wxGetApp().pDBRoutines->structPitcherStats.ERA = 0;
    wxGetApp().pDBRoutines->structPitcherStats.WHIP = 0;

    wxGetApp().pDBRoutines->structPitcherStats.PitcherID = 0;
    wxGetApp().pDBRoutines->structPitcherStats.PitcherStatsID = 0;
    // End Stats Notebook Page

    // Initialize Chance Notebook Page - 21 entries
    m_textChanceBasic->SetValue( "0.0" );
    m_textChanceLeft->SetValue( "0.0" );
    m_textChanceRight->SetValue( "0.0" );
    m_textChanceB1B->SetValue( "0.0" );
    m_textChanceB2B->SetValue( "0.0" );
    m_textChanceB3B->SetValue( "0.0" );
    m_textChanceBHR->SetValue( "0.0" );
    m_textChanceBW->SetValue( "0.0" );
    m_textChanceBDP->SetValue( "0.0" );
    m_textChanceL1B->SetValue( "0.0" );
    m_textChanceL2B->SetValue( "0.0" );
    m_textChanceL3B->SetValue( "0.0" );
    m_textChanceLHR->SetValue( "0.0" );
    m_textChanceLW->SetValue( "0.0" );
    m_textChanceLDP->SetValue( "0.0" );
    m_textChanceR1B->SetValue( "0.0" );
    m_textChanceR2B->SetValue( "0.0" );
    m_textChanceR3B->SetValue( "0.0" );
    m_textChanceRHR->SetValue( "0.0" );
    m_textChanceRW->SetValue( "0.0" );
    m_textChanceRDP->SetValue( "0.0" );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceBasic = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceLeft = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceRight = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceSingle = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceDouble = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceTriple = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRun = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceWalk = 0;
    wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlay = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceSingleRight = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceDoubleRight = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceTripleRight = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRunRight = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceWalkRight = 0;
    wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlayRight = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceSingleLeft = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceDoubleLeft = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceTripleLeft = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRunLeft = 0;
    wxGetApp().pDBRoutines->structPitcherData.OBChanceWalkLeft = 0;
    wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlayLeft = 0;
    // End Chance Notebook Page

    // Initialize Info Notebook Page - 9 entries
    m_combo_InfoStarter->SetValue( "0" );
    m_combo_InfoRelief->SetValue( "0" );
    m_combo_InfoBalk->SetValue( "0" );
    m_combo_InfoP->SetValue( "0" );
    m_combo_InfoERP->SetValue( "0" );
    m_combo_InfoWP->SetValue( "0" );
    m_combo_InfoThrows->SetValue( "R" );
    m_combo_InfoBunting->SetValue( "A" );
    m_combo_InfoHold->SetValue( "0" );
    wxGetApp().pDBRoutines->structPitcherData.FirstName = "";
    wxGetApp().pDBRoutines->structPitcherData.LastName = "";
    wxGetApp().pDBRoutines->structPitcherData.Balk = 0;
    wxGetApp().pDBRoutines->structPitcherData.ER1 = 0;
    wxGetApp().pDBRoutines->structPitcherData.Pitcher = 0;
    wxGetApp().pDBRoutines->structPitcherData.HittingCard = 0;
    wxGetApp().pDBRoutines->structPitcherData.Relief = 1;
    wxGetApp().pDBRoutines->structPitcherData.Starter = 1;
    wxGetApp().pDBRoutines->structPitcherData.WP = 0;
    wxGetApp().pDBRoutines->structPitcherData.Bunting = 1;
    wxGetApp().pDBRoutines->structPitcherData.Hold = 0;
    wxGetApp().pDBRoutines->structPitcherData.Throws = 1;
    // Set page to be displayed to Info which is tab 0 on a 0 to 3 basis
    m_pNotebook->SetSelection(0);

    // End Info Notebook Page
}

void PitcherNotebook::GetNotebookData()
{
	double fIP;

    // Retrieve Info Notebook Page - 10 entries
    wxGetApp().pDBRoutines->structPitcherData.FirstName = m_textFirstName->GetValue();
    wxGetApp().pDBRoutines->structPitcherData.LastName = m_textLastName->GetValue();
    wxGetApp().pDBRoutines->structPitcherData.Balk = atoi(m_combo_InfoBalk->GetValue());
    wxGetApp().pDBRoutines->structPitcherData.ER1 = atoi(m_combo_InfoERP->GetValue());
    wxGetApp().pDBRoutines->structPitcherData.Pitcher = atoi(m_combo_InfoP->GetValue());
    wxGetApp().pDBRoutines->structPitcherData.HittingCard = atoi(m_combo_InfoHittingCard->GetValue());
    wxGetApp().pDBRoutines->structPitcherData.Relief = atoi(m_combo_InfoRelief->GetValue());
    wxGetApp().pDBRoutines->structPitcherData.Starter = atoi(m_combo_InfoStarter->GetValue());
    wxGetApp().pDBRoutines->structPitcherData.WP = atoi(m_combo_InfoWP->GetValue());
    // The next three entries save the selection, not the value
    wxGetApp().pDBRoutines->structPitcherData.Bunting = m_combo_InfoBunting->GetSelection();
//    wxGetApp().pDBRoutines->structPitcherData.Hold = m_combo_InfoHold->GetSelection();
    wxGetApp().pDBRoutines->structPitcherData.Hold = atoi(m_combo_InfoHold->GetValue());
    wxGetApp().pDBRoutines->structPitcherData.Throws = m_combo_InfoThrows->GetSelection();
    // End Info Notebook Page

    // Retrieve Chance Notebook Page - 21 entries
//    m_textChanceBDP->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlay )));
    wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlay = atof( m_textChanceBDP->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlayLeft = atof( m_textChanceLDP->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlayRight = atof( m_textChanceRDP->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceBasic = atof( m_textChanceBasic->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceDouble = atof( m_textChanceB2B->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceDoubleLeft = atof( m_textChanceL2B->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceDoubleRight = atof( m_textChanceR2B->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRun = atof( m_textChanceBHR->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRunLeft = atof( m_textChanceLHR->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRunRight = atof( m_textChanceRHR->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceLeft = atof( m_textChanceLeft->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceRight = atof( m_textChanceRight->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceSingle = atof( m_textChanceB1B->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceSingleLeft = atof( m_textChanceL1B->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceSingleRight = atof( m_textChanceR1B->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceTriple = atof( m_textChanceB3B->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceTripleLeft = atof( m_textChanceL3B->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceTripleRight = atof( m_textChanceR3B->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceWalk = atof( m_textChanceBW->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceWalkLeft = atof( m_textChanceLW->GetValue() );
    wxGetApp().pDBRoutines->structPitcherData.OBChanceWalkRight = atof( m_textChanceRW->GetValue() );
    // End Chance Notebook Page

    // Retrieve Stats Notebook Page - 14 entries
    // ERA and WHIP are calculated and therefor are not saved
    wxGetApp().pDBRoutines->structPitcherStats.CompleteGames = m_spinStatsCGames->GetValue();
    wxGetApp().pDBRoutines->structPitcherStats.ER = m_spinStatsER->GetValue();
    wxGetApp().pDBRoutines->structPitcherStats.Games = m_spinStatsGames->GetValue();
    wxGetApp().pDBRoutines->structPitcherStats.Hits = m_spinStatsHits->GetValue();
    wxGetApp().pDBRoutines->structPitcherStats.HomeRuns = m_spinStatsHR->GetValue();
    wxGetApp().pDBRoutines->structPitcherStats.InningsPitched = atof( m_textStatsIP->GetValue() );
    // The following clears out the field.
    // The write will take only the first 7 positions of the variable.
//    my_PitcherStruct.m_IP.Append('\0',7);
    wxGetApp().pDBRoutines->structPitcherStats.Strikeouts = m_spinStatsK->GetValue();
    wxGetApp().pDBRoutines->structPitcherStats.Loss = m_spinStatsLoss->GetValue();
    wxGetApp().pDBRoutines->structPitcherStats.Saves = m_spinStatsSave->GetValue();
    wxGetApp().pDBRoutines->structPitcherStats.Starts = m_spinStatsStarts->GetValue();
    wxGetApp().pDBRoutines->structPitcherStats.Walks = m_spinStatsW->GetValue();
    wxGetApp().pDBRoutines->structPitcherStats.Wins = m_spinStatsWins->GetValue();

	fIP = atof( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherStats.InningsPitched )));

	if (fIP == 0)
	{
		wxGetApp().pDBRoutines->structPitcherStats.ERA = 0.0f;
		wxGetApp().pDBRoutines->structPitcherStats.WHIP = 0.0f;
	}
	else
	{
		wxGetApp().pDBRoutines->structPitcherStats.ERA =
			(double)( wxGetApp().pDBRoutines->structPitcherStats.ER *9 ) / fIP;
		wxGetApp().pDBRoutines->structPitcherStats.WHIP =
			(double)( wxGetApp().pDBRoutines->structPitcherStats.Hits + wxGetApp().pDBRoutines->structPitcherStats.Walks ) / fIP;
	}
    // End Stats Notebook Page
//
//    wxString Foobar;
//    Foobar.Printf( wxT("Basic = %f"), wxGetApp().pDBRoutines->structPitcherData.OBChanceBasic);
//    wxMessageBox(Foobar);
//
    // Notebook page is zero based
//    currentnotebookpage = m_pNotebook->GetSelection();
//    switch ( currentnotebookpage )
//    {
//        case -1:
//            break;
//        case 0:   // Info Notebook Page
//            strPitcherName = m_combo_pitcher_01->GetValue();
//            break;
//        case 1:   // Chance Notebook Page
//            strPitcherName = m_combo_pitcher_02->GetValue();
//            break;
//        case 2:   // Stats Notebook Page
//            strPitcherName = m_combo_pitcher_03->GetValue();
//            break;
//        default:  // Should never get here
//            wxMessageBox(_T("Something is wrong with the Pitcher Notebook - GetNoteBookData"),
//                _T("Error"), wxOK|wxICON_INFORMATION );
//            break;
//    }
//    my_PitcherStruct.m_PitcherName = strPitcherName;
//    // origselection can be used to calculate displacement into file
//    origselection = m_arrayPitchersOrig.Index( strPitcherName );
//    if ( origselection != wxNOT_FOUND )
//    {
//        my_PitcherStruct.UpdatePitcher( m_strLeagueDirName + m_strTeamFileNamePitcher,
//            origselection );
//    }
//    else
//    {
//        wxMessageBox(_T("Pitcher Name not Found"),
//            _T("Error"), wxOK|wxICON_INFORMATION );
//    }
}


void PitcherNotebook::OnComboPitcherSelect(wxCommandEvent& event)
{
    int iPitcherSelection;

    iPitcherSelection = event.GetSelection();
    // Since a pitcher has been selected, we now need to retrieve
    // the pitcher information and populate the dialog.
    wxGetApp().pDBRoutines->DBGetPitcherData(wxGetApp().pDBRoutines->m_arrayPitcherStatsIDs[iPitcherSelection]);
    wxGetApp().pDBRoutines->m_intPitcherStatsID = wxGetApp().pDBRoutines->m_arrayPitcherStatsIDs[iPitcherSelection];
//    wxGetApp().pDBRoutines->DBGetPitcherData(iPitcherSelection);

    // Set flag to show this is setting the initial value
    m_bSetValueFlagPitcher = TRUE;

    // Populate Info Notebook Page - 9 entries
    m_textFirstName->SetValue( wxString::Format(wxT("%s"), (wxGetApp().pDBRoutines->structPitcherData.FirstName )));
    m_textLastName->SetValue( wxString::Format(wxT("%s"), (wxGetApp().pDBRoutines->structPitcherData.LastName )));
    m_combo_InfoBalk->SetSelection( wxGetApp().pDBRoutines->structPitcherData.Balk );
    m_combo_InfoBunting->SetSelection( wxGetApp().pDBRoutines->structPitcherData.Bunting );
    m_combo_InfoERP->SetSelection( wxGetApp().pDBRoutines->structPitcherData.ER1 );
    m_combo_InfoHold->SetSelection( wxGetApp().pDBRoutines->structPitcherData.Hold + 6 );
    m_combo_InfoP->SetSelection( wxGetApp().pDBRoutines->structPitcherData.Pitcher );
    m_combo_InfoHittingCard->SetSelection( wxGetApp().pDBRoutines->structPitcherData.HittingCard );
    m_combo_InfoRelief->SetSelection( wxGetApp().pDBRoutines->structPitcherData.Relief );
    m_combo_InfoStarter->SetSelection( wxGetApp().pDBRoutines->structPitcherData.Starter );
    m_combo_InfoThrows->SetSelection( wxGetApp().pDBRoutines->structPitcherData.Throws);
    m_combo_InfoWP->SetSelection( wxGetApp().pDBRoutines->structPitcherData.WP );
    // End Info Notebook Page

    // Populate Chance Notebook Page - 21 entries
    m_textChanceBDP->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlay )));
    m_textChanceLDP->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlayLeft )));
    m_textChanceRDP->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.ChanceDoublePlayRight )));
    m_textChanceBasic->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceBasic )));
    m_textChanceB2B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceDouble )));
    m_textChanceL2B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceDoubleLeft )));
    m_textChanceR2B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceDoubleRight )));
    m_textChanceBHR->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRun )));
    m_textChanceLHR->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRunLeft )));
    m_textChanceRHR->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceHomeRunRight )));
    m_textChanceLeft->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceLeft )));
    m_textChanceRight->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceRight )));
    m_textChanceB1B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceSingle )));
    m_textChanceL1B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceSingleLeft )));
    m_textChanceR1B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceSingleRight )));
    m_textChanceB3B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceTriple )));
    m_textChanceL3B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceTripleLeft )));
    m_textChanceR3B->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceTripleRight )));
    m_textChanceBW->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceWalk )));
    m_textChanceLW->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceWalkLeft )));
    m_textChanceRW->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherData.OBChanceWalkRight )));
    // End Chance Notebook Page

    // Populate Stats Notebook Page - 14 entries

	m_spinStatsWins->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsLoss->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsStarts->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsSave->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_textStatsIP->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsHits->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsGames->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsCGames->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsW->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsK->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsHR->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
	m_spinStatsER->SetForegroundColour(wxColour(wxT("BLACK"))); // Black

    m_spinStatsCGames->SetValue( wxGetApp().pDBRoutines->structPitcherStats.CompleteGames );
    m_spinStatsER->SetValue( wxGetApp().pDBRoutines->structPitcherStats.ER );
    m_spinStatsGames->SetValue( wxGetApp().pDBRoutines->structPitcherStats.Games );
    m_spinStatsHits->SetValue( wxGetApp().pDBRoutines->structPitcherStats.Hits );
    m_spinStatsHR->SetValue( wxGetApp().pDBRoutines->structPitcherStats.HomeRuns );
    m_textStatsIP->SetValue( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherStats.InningsPitched )));;
	m_textStatsIP->SetForegroundColour(wxColour(wxT("BLACK"))); // Black
    m_spinStatsK->SetValue( wxGetApp().pDBRoutines->structPitcherStats.Strikeouts );
    m_spinStatsLoss->SetValue( wxGetApp().pDBRoutines->structPitcherStats.Loss );
    m_spinStatsSave->SetValue( wxGetApp().pDBRoutines->structPitcherStats.Saves );
    m_spinStatsStarts->SetValue( wxGetApp().pDBRoutines->structPitcherStats.Starts );
    m_spinStatsW->SetValue( wxGetApp().pDBRoutines->structPitcherStats.Walks );
    m_spinStatsWins->SetValue( wxGetApp().pDBRoutines->structPitcherStats.Wins );

    m_textStatsERA->SetValue( wxString::Format(wxT("%1.2f"), (wxGetApp().pDBRoutines->structPitcherStats.ERA )));
    m_textStatsWHIP->SetValue( wxString::Format(wxT("%1.2f"), (wxGetApp().pDBRoutines->structPitcherStats.WHIP )));

//	fIP = atof( wxString::Format(wxT("%2.2f"), (wxGetApp().pDBRoutines->structPitcherStats.InningsPitched )));
//
//	if (fIP == 0)
//	{
//		m_textStatsERA->SetValue( "0" );
//		m_textStatsWHIP->SetValue( "0" );
//	}
//	else
//	{
//		fERA = (double)( wxGetApp().pDBRoutines->structPitcherStats.ER *9 ) / fIP;
//		sprintf(buf,"%1.2f",fERA);
//        m_textStatsERA->SetValue( buf );
//		fWHIP = (double)( wxGetApp().pDBRoutines->structPitcherStats.Hits + wxGetApp().pDBRoutines->structPitcherStats.Walks ) / fIP;
//		sprintf(buf,"%1.2f",fWHIP);
//        m_textStatsWHIP->SetValue( buf );
//	}
    // End Stats Notebook Page

    // Clear the flag since we are done setting initial values
    m_bSetValueFlagPitcher = FALSE;
}

void PitcherNotebook::OnComboTeamSelect(wxCommandEvent& event)
{
}

void PitcherNotebook::MakePitcherUpdate()
{
//    wxMessageBox(_T("MakePitcherUpdate Button Pressed"),
//                 _T("MakePitcherUpdate"), wxOK|wxICON_INFORMATION );

    if ( m_bChangeStatsFlag )
    {
        wxGetApp().pDBRoutines->DBUpdatePitcherStats( wxGetApp().pDBRoutines->m_intPitcherStatsID  );
    }

    // Should only update PitcherData for Base team when Info and Chance data changes
    if ( m_bChangeInfoFlag || m_bChangeChanceFlag )
    {
        if (wxGetApp().pDBRoutines->structTeamData.BaseTeam == TRUE)
        {
            wxGetApp().pDBRoutines->DBUpdatePitcherData( wxGetApp().pDBRoutines->structPitcherStats.PitcherID );
        }
        else
        {
            // Might eventually ask if its OK to update non base entry
            wxMessageBox(wxT("Team is not a Base Team!"), wxT("Team"), wxOK);
        }
    }

    m_bChangeInfoFlag = FALSE;
    m_bChangeStatsFlag = FALSE;
    m_bChangeChanceFlag = FALSE;
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

//BEGIN_EVENT_TABLE(PitcherDialog, wxPanel)
//    // Controls
////    EVT_BUTTON(wxID_OK, PitcherDialog::OnOK)
//END_EVENT_TABLE()

PitcherDialog::PitcherDialog (wxWindow* parent, long style)
    : wxDialog(parent, -1, "Pitcher Add/Edit",
                    wxPoint(10,10), wxSize(500,625),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    int dialogClientWidth, dialogClientHeight;
    int myPitcherDialogReturnCode;
    int rc;

    CenterOnParent(); // Centers on the parent window

	rc = GetTeamNamesArray();
    if (rc == false)
    {
		return;
	}
    //  Create a dialog that will be used to edit batter data
    wxPanel *m_pPitcherNotebook;

    //  Now create a Notebook in the client area of the batter dialog
    GetClientSize(&dialogClientWidth, &dialogClientHeight);
    m_pPitcherNotebook = new PitcherNotebook( this, 0, 0,
                            dialogClientWidth, dialogClientHeight );

    myPitcherDialogReturnCode = ShowModal();
}

PitcherDialog::~PitcherDialog( )
{
    Destroy();
}

// Get an array of Team names and save it in a member variable
// Display a selection of Leagues then create an array of teams
bool PitcherDialog::GetTeamNamesArray()
{
	int rc;
	// Check is DB is open, if not, open one
	wxGetApp().pDBRoutines->DBIsDBOpen();

    rc = wxGetApp().pDBRoutines->DBGetTeamNamesArray();

    if (rc == false)
    {
		return false;
	}
	else
	{
		return true;
	}
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(LeagueDialogAdd, wxPanel)
    // Controls
    EVT_BUTTON(wxID_ADD, LeagueDialogAdd::OnADD)
    EVT_BUTTON(wxID_CANCEL, LeagueDialogAdd::OnCancel)
    EVT_CLOSE(LeagueDialogAdd::OnClose)
END_EVENT_TABLE()

LeagueDialogAdd::LeagueDialogAdd (wxWindow* parent, long style)
    : wxDialog(parent, -1, "League Add",
                    wxPoint(10,10), wxSize(400,225),
//                    wxPoint(10,10), wxSize(500,550),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	int myRC;

    CenterOnParent(); // Centers on the parent window

	LeagueDialogAddCreate();

    myRC = ShowModal();

    if ( myRC == wxID_CANCEL)
    {
		wxMessageBox(_T("wxID_CANCEL"),
                 _T("wxID_CANCEL"), wxOK|wxICON_INFORMATION );
    }
    if ( myRC == wxID_ADD )
    {
//		wxMessageBox(_T("wxID_ADD"),
//                 _T("wxID_ADD"), wxOK|wxICON_INFORMATION );
		ConferenceDialog( this );
	}
}

LeagueDialogAdd::~LeagueDialogAdd( )
{
    Destroy();
}

void LeagueDialogAdd::LeagueDialogAddCreate()
{
//  Create a Panel that will be used to display and edit the Leagues
    m_pLeaguePanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxTAB_TRAVERSAL, "League Panel");

    wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *pItem01 = new wxBoxSizer( wxVERTICAL );

// Data section
    wxBoxSizer *pItemSizer01 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizer02 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizer03 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer01->Add(new wxStaticText(m_pLeaguePanel, wxID_ANY,
        _("League Name:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextLeagueName = new wxTextCtrl(m_pLeaguePanel, ID_LEAGUE_LEAGUENAME,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizer01->Add( m_pTextLeagueName, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizer02->Add(new wxStaticText(m_pLeaguePanel, wxID_ANY,
        _("League Year:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextLeagueYear = new wxTextCtrl(m_pLeaguePanel, ID_LEAGUE_YEAR,
                    _T(""), wxDefaultPosition, wxSize(60,-1) );
    pItemSizer02->Add( m_pTextLeagueYear, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizer03->Add(new wxStaticText(m_pLeaguePanel, wxID_ANY,
        _("Base League:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pCheckBoxLeagueBase = new wxCheckBox(m_pLeaguePanel, ID_LEAGUE_BASE,
		_T(""), wxDefaultPosition, wxDefaultSize );
    pItemSizer03->Add( m_pCheckBoxLeagueBase, 0, wxALIGN_LEFT|wxLEFT, 0 );

// Add each line item to the panel
	pItem01->AddSpacer(10);
    pItem01->Add( pItemSizer01, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer02, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer03, 0, wxALIGN_LEFT|wxLEFT, 0 );

// Create and place Control buttons
    pItem01->Add ( BuildControlButtons(m_pLeaguePanel),
                    0, wxALIGN_CENTER );

    pTopSizer->Add( pItem01, 1, wxGROW|wxALL, 5 );
    pTopSizer->AddSpacer(5);

    m_pLeaguePanel->SetSizer(pTopSizer);
    pTopSizer->Fit(m_pLeaguePanel);

}

wxBoxSizer* LeagueDialogAdd::BuildControlButtons( wxWindow* panel )
{
    // Create the control buttons (Apply, Add, and Cancel)

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonPane = new wxBoxSizer (wxHORIZONTAL);

    m_pAddButton = new wxButton (panel, wxID_ADD);
    m_pAddButton->SetDefault();
    buttonPane->Add (m_pAddButton, 0, wxALIGN_CENTER);

    buttonPane->AddSpacer( 10 );

    m_pCancelButton = new wxButton (panel, wxID_CANCEL);
    buttonPane->Add (m_pCancelButton, 0, wxALIGN_CENTER);

    sizer->Add (new wxStaticLine(panel, -1, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
    sizer->Add (buttonPane, 0, wxALIGN_CENTER );

    // Add a space under the buttons
    sizer->AddSpacer( 10 );

    return (sizer);
}

void LeagueDialogAdd::OnADD(wxCommandEvent& event)
{
	int leagueID;
//    wxMessageBox(_T("ADD Button Pressed"),
//                 _T("ADD"), wxOK|wxICON_INFORMATION );
// Retrieve the League variables and place in the League Structure
	wxGetApp().pDBRoutines->structLeagueData.LeagueName = m_pTextLeagueName->GetValue();
	wxGetApp().pDBRoutines->structLeagueData.LeagueYear = atoi(m_pTextLeagueYear->GetValue());
	// If checked base = 1, not checked base = 0
	wxGetApp().pDBRoutines->structLeagueData.BaseLeague = m_pCheckBoxLeagueBase->IsChecked();

	// Set defaults of 0 for number of conferences and divisions
	wxGetApp().pDBRoutines->structLeagueData.NumberOfConferences = 0;
	wxGetApp().pDBRoutines->structLeagueData.NumberOfDivisions = 0;

	// The following is commented to allow simple testing
	leagueID = wxGetApp().pDBRoutines->DBInsertLeague();

    // Continue and process this event normally.
    EndModal(wxID_ADD); // End with a RC of ADD
}

void LeagueDialogAdd::OnCancel(wxCommandEvent& event)
{
//    wxMessageBox(_T("Cancel Button Pressed"),
//                 _T("Cancel"), wxOK|wxICON_INFORMATION );
    // If the change flag is TRUE, Ask if updates should be dropped.
//    if ( m_bChangeOptionsFlag )
//    {
//        int answer = wxMessageBox(
//                (_T("       A field has changed.\n")
//                 _T("Press Yes to save or No to discard")
//                ),
//                _T("Field Changed"), wxYES_NO|wxICON_QUESTION );
//        if ( answer == wxYES)
//        {
//            //Save the data
//            m_bChangeOptionsFlag = FALSE;
//            OnApply(event);
//        }
//    }
//    else
//    {
//    }
    // Continue and process this event normally.
//    event.Skip(TRUE);
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

// The 'X' in the top right was clicked
void LeagueDialogAdd::OnClose(wxCloseEvent& event)
{
    EndModal(wxID_CLOSE); // End with a RC of CLOSE
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ConferenceDialog, wxPanel)
    // Controls
    EVT_BUTTON(wxID_ADD, ConferenceDialog::OnADD)
    EVT_BUTTON(wxID_CANCEL, ConferenceDialog::OnCancel)
    EVT_CLOSE(ConferenceDialog::OnClose)
END_EVENT_TABLE()

ConferenceDialog::ConferenceDialog (wxWindow* parent, long style)
    : wxDialog(parent, -1, "Conferences",
                    wxPoint(10,10), wxSize(400,430),
//                    wxPoint(10,10), wxSize(500,550),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    CenterOnParent(); // Centers on the parent window

	ConferenceDialogCreate();

	m_pTextLeagueName->SetValue( wxGetApp().pDBRoutines->structLeagueData.LeagueName );
	m_pTextLeagueYear->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structLeagueData.LeagueYear) );

    int myRC = ShowModal();

}


ConferenceDialog::~ConferenceDialog( )
{
    Destroy();
}

void ConferenceDialog::ConferenceDialogCreate( )
{
//  Create a Panel that will be used to display and edit the Leagues
    m_pConferencePanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxTAB_TRAVERSAL, "Conference Panel");

    wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *pItem01 = new wxBoxSizer( wxVERTICAL );

// Data section
    wxBoxSizer *pItemSizer01 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizer02 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerConf01 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerConf02 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerConf03 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerConf04 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerConf05 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerConf06 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer01->Add(new wxStaticText(m_pConferencePanel, wxID_ANY,
        _("League Name:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextLeagueName = new wxTextCtrl(m_pConferencePanel, ID_LEAGUE_LEAGUENAME,
                    _T(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
    pItemSizer01->Add( m_pTextLeagueName, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizer02->Add(new wxStaticText(m_pConferencePanel, wxID_ANY,
        _("League Year:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextLeagueYear = new wxTextCtrl(m_pConferencePanel, ID_LEAGUE_YEAR,
                    _T(""), wxDefaultPosition, wxSize(60,-1), wxTE_READONLY );
    pItemSizer02->Add( m_pTextLeagueYear, 0, wxALIGN_LEFT|wxLEFT, 0 );

	// Conference section
    pItemSizerConf01->Add(new wxStaticText(m_pConferencePanel, wxID_ANY,
        _("Conference 1:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextConference01 = new wxTextCtrl(m_pConferencePanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerConf01->Add( m_pTextConference01, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizerConf02->Add(new wxStaticText(m_pConferencePanel, wxID_ANY,
        _("Conference 2:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextConference02 = new wxTextCtrl(m_pConferencePanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerConf02->Add( m_pTextConference02, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizerConf03->Add(new wxStaticText(m_pConferencePanel, wxID_ANY,
        _("Conference 3:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextConference03 = new wxTextCtrl(m_pConferencePanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerConf03->Add( m_pTextConference03, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizerConf04->Add(new wxStaticText(m_pConferencePanel, wxID_ANY,
        _("Conference 4:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextConference04 = new wxTextCtrl(m_pConferencePanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerConf04->Add( m_pTextConference04, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizerConf05->Add(new wxStaticText(m_pConferencePanel, wxID_ANY,
        _("Conference 5:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextConference05 = new wxTextCtrl(m_pConferencePanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerConf05->Add( m_pTextConference05, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizerConf06->Add(new wxStaticText(m_pConferencePanel, wxID_ANY,
        _("Conference 6:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextConference06 = new wxTextCtrl(m_pConferencePanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerConf06->Add( m_pTextConference06, 0, wxALIGN_LEFT|wxLEFT, 0 );

// Add each line item to the panel
	pItem01->AddSpacer(10);
    pItem01->Add( pItemSizer01, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer02, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add (new wxStaticLine(m_pConferencePanel, wxID_ANY, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
//    pItem01->AddSpacer(20);
    pItem01->Add( pItemSizerConf01, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizerConf02, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizerConf03, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizerConf04, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizerConf05, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizerConf06, 0, wxALIGN_LEFT|wxLEFT, 0 );

	// Create and place Control buttons
    pItem01->Add ( BuildControlButtons(m_pConferencePanel),
                    0, wxALIGN_CENTER );

    pTopSizer->Add( pItem01, 1, wxGROW|wxALL, 5 );
    pTopSizer->AddSpacer(5);

    m_pConferencePanel->SetSizer(pTopSizer);
    pTopSizer->Fit(m_pConferencePanel);
//    pTopSizer->SetSizeHints(this);
}

wxBoxSizer* ConferenceDialog::BuildControlButtons( wxWindow* panel )
{
    // Create the control buttons (Apply, Add, and Cancel)

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonPane = new wxBoxSizer (wxHORIZONTAL);

    m_pAddButton = new wxButton (panel, wxID_ADD);
    m_pAddButton->SetDefault();
    buttonPane->Add (m_pAddButton, 0, wxALIGN_CENTER);

    buttonPane->AddSpacer( 10 );

    m_pCancelButton = new wxButton (panel, wxID_CANCEL);
    buttonPane->Add (m_pCancelButton, 0, wxALIGN_CENTER);

    sizer->Add (new wxStaticLine(panel, -1, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
    sizer->Add (buttonPane, 0, wxALIGN_CENTER );

    // Add a space under the buttons
    sizer->AddSpacer( 10 );

    return (sizer);
}

void ConferenceDialog::OnADD(wxCommandEvent& event)
{
	wxArrayString arrayConference;
	int i_conference;
	int i_division;

//    wxMessageBox(_T("ADD Button Pressed"),
//                 _T("ADD"), wxOK|wxICON_INFORMATION );
// Retrieve the Conference variables and place in the Conference Structure

	arrayConference.Clear();

	if (!m_pTextConference01->IsEmpty()) arrayConference.Add(m_pTextConference01->GetValue());
	if (!m_pTextConference02->IsEmpty()) arrayConference.Add(m_pTextConference02->GetValue());
	if (!m_pTextConference03->IsEmpty()) arrayConference.Add(m_pTextConference03->GetValue());
	if (!m_pTextConference04->IsEmpty()) arrayConference.Add(m_pTextConference04->GetValue());
	if (!m_pTextConference05->IsEmpty()) arrayConference.Add(m_pTextConference05->GetValue());
	if (!m_pTextConference06->IsEmpty()) arrayConference.Add(m_pTextConference06->GetValue());

	// Conference dialog forces the entry of one conference.
	if (!arrayConference.GetCount()) arrayConference.Add("DEFAULT");

	wxGetApp().pDBRoutines->structLeagueData.NumberOfConferences = arrayConference.GetCount();

	for (i_conference = 0; i_conference < wxGetApp().pDBRoutines->structLeagueData.NumberOfConferences; i_conference++)
	{
		wxGetApp().pDBRoutines->structConferenceData.ConferenceName = arrayConference[i_conference];
		wxGetApp().pDBRoutines->structConferenceData.LeagueID = wxGetApp().pDBRoutines->structLeagueData.LeagueID;
		wxGetApp().pDBRoutines->structConferenceData.BaseConference = wxGetApp().pDBRoutines->structLeagueData.BaseLeague;

		// Insert the new Conference, The new conferenceID will be in the structConferenceData.ConferenceID field
		wxGetApp().pDBRoutines->DBInsertConference();

		DivisionDialog( this );
	}

	wxGetApp().pDBRoutines->DBUpdateLeague( wxGetApp().pDBRoutines->structLeagueData.LeagueID );

    // Continue and process this event normally.
    EndModal(wxID_ADD); // End with a RC of ADD
}

void ConferenceDialog::OnCancel(wxCommandEvent& event)
{
//    wxMessageBox(_T("Cancel Button Pressed"),
//                 _T("Cancel"), wxOK|wxICON_INFORMATION );
    // If the change flag is TRUE, Ask if updates should be dropped.
//    if ( m_bChangeOptionsFlag )
//    {
//        int answer = wxMessageBox(
//                (_T("       A field has changed.\n")
//                 _T("Press Yes to save or No to discard")
//                ),
//                _T("Field Changed"), wxYES_NO|wxICON_QUESTION );
//        if ( answer == wxYES)
//        {
//            //Save the data
//            m_bChangeOptionsFlag = FALSE;
//            OnApply(event);
//        }
//    }
//    else
//    {
//    }
    // Continue and process this event normally.
//    event.Skip(TRUE);
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

void ConferenceDialog::OnClose(wxCloseEvent& event)
{
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(DivisionDialog, wxPanel)
    // Controls
    EVT_BUTTON(wxID_ADD, DivisionDialog::OnADD)
    EVT_BUTTON(wxID_CANCEL, DivisionDialog::OnCancel)
    EVT_CLOSE(DivisionDialog::OnClose)
END_EVENT_TABLE()

DivisionDialog::DivisionDialog (wxWindow* parent, long style)
    : wxDialog(parent, -1, "Divisions",
                    wxPoint(10,10), wxSize(400,430),
//                    wxPoint(10,10), wxSize(500,550),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    CenterOnParent(); // Centers on the parent window

	DivisionDialogCreate();

	m_pTextLeagueName->SetValue( wxGetApp().pDBRoutines->structLeagueData.LeagueName );
	m_pTextLeagueYear->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structLeagueData.LeagueYear) );

    int myRC = ShowModal();

}


DivisionDialog::~DivisionDialog( )
{
    Destroy();
}

void DivisionDialog::DivisionDialogCreate( )
{
//  Create a Panel that will be used to display and edit the Leagues
    m_pDivisionPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxTAB_TRAVERSAL, "Division Panel");

    wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *pItem01 = new wxBoxSizer( wxVERTICAL );

// Data section
    wxBoxSizer *pItemSizer01 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizer02 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerDivision01 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerDivision02 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerDivision03 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerDivision04 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerDivision05 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizerDivision06 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer01->Add(new wxStaticText(m_pDivisionPanel, wxID_ANY,
        _("League Name:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextLeagueName = new wxTextCtrl(m_pDivisionPanel, ID_LEAGUE_LEAGUENAME,
                    _T(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
    pItemSizer01->Add( m_pTextLeagueName, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizer02->Add(new wxStaticText(m_pDivisionPanel, wxID_ANY,
        _("League Year:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextLeagueYear = new wxTextCtrl(m_pDivisionPanel, ID_LEAGUE_YEAR,
                    _T(""), wxDefaultPosition, wxSize(60,-1), wxTE_READONLY );
    pItemSizer02->Add( m_pTextLeagueYear, 0, wxALIGN_LEFT|wxLEFT, 0 );

	// Conference section
    pItemSizerDivision01->Add(new wxStaticText(m_pDivisionPanel, wxID_ANY,
        _("Division 1:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextDivision01 = new wxTextCtrl(m_pDivisionPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerDivision01->Add( m_pTextDivision01, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizerDivision02->Add(new wxStaticText(m_pDivisionPanel, wxID_ANY,
        _("Division 2:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextDivision02 = new wxTextCtrl(m_pDivisionPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerDivision02->Add( m_pTextDivision02, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizerDivision03->Add(new wxStaticText(m_pDivisionPanel, wxID_ANY,
        _("Division 3:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextDivision03 = new wxTextCtrl(m_pDivisionPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerDivision03->Add( m_pTextDivision03, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizerDivision04->Add(new wxStaticText(m_pDivisionPanel, wxID_ANY,
        _("Division 4:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextDivision04 = new wxTextCtrl(m_pDivisionPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerDivision04->Add( m_pTextDivision04, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizerDivision05->Add(new wxStaticText(m_pDivisionPanel, wxID_ANY,
        _("Division 5:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextDivision05 = new wxTextCtrl(m_pDivisionPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerDivision05->Add( m_pTextDivision05, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizerDivision06->Add(new wxStaticText(m_pDivisionPanel, wxID_ANY,
        _("Division 6:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextDivision06 = new wxTextCtrl(m_pDivisionPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizerDivision06->Add( m_pTextDivision06, 0, wxALIGN_LEFT|wxLEFT, 0 );

// Add each line item to the panel
	pItem01->AddSpacer(10);
    pItem01->Add( pItemSizer01, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer02, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add (new wxStaticLine(m_pDivisionPanel, wxID_ANY, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
//    pItem01->AddSpacer(20);
    pItem01->Add( pItemSizerDivision01, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizerDivision02, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizerDivision03, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizerDivision04, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizerDivision05, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizerDivision06, 0, wxALIGN_LEFT|wxLEFT, 0 );

	// Create and place Control buttons
    pItem01->Add ( BuildControlButtons(m_pDivisionPanel),
                    0, wxALIGN_CENTER );

    pTopSizer->Add( pItem01, 1, wxGROW|wxALL, 5 );
    pTopSizer->AddSpacer(5);

    m_pDivisionPanel->SetSizer(pTopSizer);
    pTopSizer->Fit(m_pDivisionPanel);
//    pTopSizer->SetSizeHints(this);
}

wxBoxSizer* DivisionDialog::BuildControlButtons( wxWindow* panel )
{
    // Create the control buttons (Apply, Add, and Cancel)

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonPane = new wxBoxSizer (wxHORIZONTAL);

    m_pAddButton = new wxButton (panel, wxID_ADD);
    m_pAddButton->SetDefault();
    buttonPane->Add (m_pAddButton, 0, wxALIGN_CENTER);

    buttonPane->AddSpacer( 10 );

    m_pCancelButton = new wxButton (panel, wxID_CANCEL);
    buttonPane->Add (m_pCancelButton, 0, wxALIGN_CENTER);

    sizer->Add (new wxStaticLine(panel, -1, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
    sizer->Add (buttonPane, 0, wxALIGN_CENTER );

    // Add a space under the buttons
    sizer->AddSpacer( 10 );

    return (sizer);
}

void DivisionDialog::OnADD(wxCommandEvent& event)
{
	wxArrayString arrayDivision;
	//int i_conference;
	int i_division;
	int numberofdivisions;

//    wxMessageBox(_T("ADD Button Pressed"),
//                 _T("ADD"), wxOK|wxICON_INFORMATION );
// Retrieve the Conference variables and place in the Conference Structure

	arrayDivision.Clear();

	if (!m_pTextDivision01->IsEmpty()) arrayDivision.Add(m_pTextDivision01->GetValue());
	if (!m_pTextDivision02->IsEmpty()) arrayDivision.Add(m_pTextDivision02->GetValue());
	if (!m_pTextDivision03->IsEmpty()) arrayDivision.Add(m_pTextDivision03->GetValue());
	if (!m_pTextDivision04->IsEmpty()) arrayDivision.Add(m_pTextDivision04->GetValue());
	if (!m_pTextDivision05->IsEmpty()) arrayDivision.Add(m_pTextDivision05->GetValue());
	if (!m_pTextDivision06->IsEmpty()) arrayDivision.Add(m_pTextDivision06->GetValue());

//	// Division dialog forces the entry of one division.
//	if (!arrayDivision.GetCount()) arrayDivision.Add("DEFAULT");

	wxGetApp().pDBRoutines->structLeagueData.NumberOfDivisions =
		arrayDivision.GetCount() + wxGetApp().pDBRoutines->structLeagueData.NumberOfDivisions;

	numberofdivisions = arrayDivision.GetCount();
	for (i_division = 0; i_division < numberofdivisions; i_division++)
	{
		wxGetApp().pDBRoutines->structDivisionData.DivisionName = arrayDivision[i_division];
		wxGetApp().pDBRoutines->structDivisionData.LeagueID = wxGetApp().pDBRoutines->structLeagueData.LeagueID;
		wxGetApp().pDBRoutines->structDivisionData.ConferenceID = wxGetApp().pDBRoutines->structConferenceData.ConferenceID;
		wxGetApp().pDBRoutines->structDivisionData.BaseDivisions = wxGetApp().pDBRoutines->structLeagueData.BaseLeague;

		// Insert the new Conference, The new conferenceID will be in the structConferenceData.ConferenceID field
		wxGetApp().pDBRoutines->DBInsertDivision();
	}

    // Continue and process this event normally.
    EndModal(wxID_ADD); // End with a RC of ADD
}

void DivisionDialog::OnCancel(wxCommandEvent& event)
{
//    wxMessageBox(_T("Cancel Button Pressed"),
//                 _T("Cancel"), wxOK|wxICON_INFORMATION );
    // If the change flag is TRUE, Ask if updates should be dropped.
//    if ( m_bChangeOptionsFlag )
//    {
//        int answer = wxMessageBox(
//                (_T("       A field has changed.\n")
//                 _T("Press Yes to save or No to discard")
//                ),
//                _T("Field Changed"), wxYES_NO|wxICON_QUESTION );
//        if ( answer == wxYES)
//        {
//            //Save the data
//            m_bChangeOptionsFlag = FALSE;
//            OnApply(event);
//        }
//    }
//    else
//    {
//    }
    // Continue and process this event normally.
//    event.Skip(TRUE);
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

void DivisionDialog::OnClose(wxCloseEvent& event)
{
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TeamDialogAdd, wxPanel)
    // Controls
    EVT_BUTTON(wxID_ADD, TeamDialogAdd::OnADD)
    EVT_BUTTON(wxID_CANCEL, TeamDialogAdd::OnCancel)
    EVT_CLOSE(TeamDialogAdd::OnClose)
END_EVENT_TABLE()

TeamDialogAdd::TeamDialogAdd (wxWindow* parent, long style)
    : wxDialog(parent, -1, "Team",
                    wxPoint(10,10), wxSize(400,540),
//                    wxPoint(10,10), wxSize(500,550),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    CenterOnParent(); // Centers on the parent window

	// Need a way to check for a cancel
	TeamDialogCreate();
//
//	m_pTextLeagueName->SetValue( wxGetApp().pDBRoutines->structLeagueData.LeagueName );
//	m_pTextLeagueYear->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structLeagueData.LeagueYear) );

	m_pTextLeagueName->SetValue( wxGetApp().pDBRoutines->structLeagueData.LeagueName );
	m_pTextLeagueYear->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structLeagueData.LeagueYear) );
	m_pTextConferenceName->SetValue( wxGetApp().pDBRoutines->m_strConferenceName );
	m_pTextDivisionName->SetValue( wxGetApp().pDBRoutines->m_strDivisionName );
	m_pTextTotalWins->SetValue("0");
	m_pTextTotalLosses->SetValue("0");
	m_pTextHomeWins->SetValue("0");
	m_pTextHomeLosses->SetValue("0");
	m_pTextAwayWins->SetValue("0");
	m_pTextAwayLosses->SetValue("0");
	m_pCheckBoxTeamBase->SetValue(wxGetApp().pDBRoutines->structLeagueData.BaseLeague);

    int myRC = ShowModal();

}

TeamDialogAdd::~TeamDialogAdd( )
{
    Destroy();
}

void TeamDialogAdd::TeamDialogCreate( )
{
//  Create a Panel that will be used to display and edit the Leagues
    m_pTeamPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxTAB_TRAVERSAL, "Team Panel");

    wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *pItem01 = new wxBoxSizer( wxVERTICAL );

// Data section
    wxBoxSizer *pItemSizer01 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizer02 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizer03 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizer04 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemTeamName = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemTeamNameShort = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemBallpark = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemTotalWinsLoss = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemHomeWinsLoss = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemAwayWinsLoss = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemTeamYearBase = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer01->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("League Name:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextLeagueName = new wxTextCtrl(m_pTeamPanel, ID_LEAGUE_LEAGUENAME,
                    _T(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
    pItemSizer01->Add( m_pTextLeagueName, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizer02->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("League Year:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextLeagueYear = new wxTextCtrl(m_pTeamPanel, ID_LEAGUE_YEAR,
                    _T(""), wxDefaultPosition, wxSize(60,-1), wxTE_READONLY );
    pItemSizer02->Add( m_pTextLeagueYear, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizer03->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Conference Name:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextConferenceName = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
    pItemSizer03->Add( m_pTextConferenceName, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizer04->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Division Name:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextDivisionName = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
    pItemSizer04->Add( m_pTextDivisionName, 0, wxALIGN_LEFT|wxLEFT, 0 );

	// Team section

    pItemTeamName->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Team Name:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextTeamName = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemTeamName->Add( m_pTextTeamName, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemTeamNameShort->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Team Name Short:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextTeamNameShort = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemTeamNameShort->Add( m_pTextTeamNameShort, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemBallpark->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Ballpark:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextBallpark = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemBallpark->Add( m_pTextBallpark, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemTotalWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Total Wins:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextTotalWins = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemTotalWinsLoss->Add( m_pTextTotalWins, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemTotalWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Total Losses:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextTotalLosses = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemTotalWinsLoss->Add( m_pTextTotalLosses, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemHomeWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Home Wins:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextHomeWins = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemHomeWinsLoss->Add( m_pTextHomeWins, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemHomeWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Home Losses:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextHomeLosses = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemHomeWinsLoss->Add( m_pTextHomeLosses, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemAwayWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Away Wins:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextAwayWins = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemAwayWinsLoss->Add( m_pTextAwayWins, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemAwayWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Away Losses:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextAwayLosses = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemAwayWinsLoss->Add( m_pTextAwayLosses, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemTeamYearBase->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Team Year:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextTeamYear = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemTeamYearBase->Add( m_pTextTeamYear, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemTeamYearBase->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Base Team:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pCheckBoxTeamBase = new wxCheckBox(m_pTeamPanel, ID_LEAGUE_BASE,
		_T(""), wxDefaultPosition, wxDefaultSize );
    pItemTeamYearBase->Add( m_pCheckBoxTeamBase, 0, wxALIGN_LEFT|wxLEFT, 0 );

// Add each line item to the panel
	pItem01->AddSpacer(10);
    pItem01->Add( pItemSizer01, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer02, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer03, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer04, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add (new wxStaticLine(m_pTeamPanel, wxID_ANY, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
    pItem01->Add( pItemTeamName, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemTeamNameShort, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemBallpark, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemTotalWinsLoss, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemHomeWinsLoss, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemAwayWinsLoss, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemTeamYearBase, 0, wxALIGN_LEFT|wxLEFT, 0 );
//    pItem01->AddSpacer(20);

	// Create and place Control buttons
    pItem01->Add ( BuildControlButtons(m_pTeamPanel),
                    0, wxALIGN_CENTER );

    pTopSizer->Add( pItem01, 1, wxGROW|wxALL, 5 );
    pTopSizer->AddSpacer(5);

    m_pTeamPanel->SetSizer(pTopSizer);
    pTopSizer->Fit(m_pTeamPanel);
//    pTopSizer->SetSizeHints(this);
}

wxBoxSizer* TeamDialogAdd::BuildControlButtons( wxWindow* panel )
{
    // Create the control buttons (Apply, Add, and Cancel)

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonPane = new wxBoxSizer (wxHORIZONTAL);

    m_pAddButton = new wxButton (panel, wxID_ADD);
    m_pAddButton->SetDefault();
    buttonPane->Add (m_pAddButton, 0, wxALIGN_CENTER);

    buttonPane->AddSpacer( 10 );

    m_pCancelButton = new wxButton (panel, wxID_CANCEL);
    buttonPane->Add (m_pCancelButton, 0, wxALIGN_CENTER);

    sizer->Add (new wxStaticLine(panel, -1, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
    sizer->Add (buttonPane, 0, wxALIGN_CENTER );

    // Add a space under the buttons
    sizer->AddSpacer( 10 );

    return (sizer);
}

void TeamDialogAdd::OnADD(wxCommandEvent& event)
{
    wxGetApp().pDBRoutines->structTeamData.TeamName = m_pTextTeamName->GetValue();
    wxGetApp().pDBRoutines->structTeamData.TeamNameShort = m_pTextTeamNameShort->GetValue();
    wxGetApp().pDBRoutines->structTeamData.BallparkName = m_pTextBallpark->GetValue();
    wxGetApp().pDBRoutines->structTeamData.TotalWins = atoi(m_pTextTotalWins->GetValue());
    wxGetApp().pDBRoutines->structTeamData.TotalLosses = atoi(m_pTextTotalLosses->GetValue());
    wxGetApp().pDBRoutines->structTeamData.HomeWins = atoi(m_pTextHomeWins->GetValue());
    wxGetApp().pDBRoutines->structTeamData.HomeLosses = atoi(m_pTextHomeLosses->GetValue());
    wxGetApp().pDBRoutines->structTeamData.AwayWins = atoi(m_pTextAwayWins->GetValue());
    wxGetApp().pDBRoutines->structTeamData.AwayLosses = atoi(m_pTextAwayLosses->GetValue());
    wxGetApp().pDBRoutines->structTeamData.LeagueID = wxGetApp().pDBRoutines->structLeagueData.LeagueID;
    wxGetApp().pDBRoutines->structTeamData.ConferenceID = wxGetApp().pDBRoutines->m_intConferenceID;
    wxGetApp().pDBRoutines->structTeamData.DivisionID = wxGetApp().pDBRoutines->m_intDivisionID;
    wxGetApp().pDBRoutines->structTeamData.TeamYear = atoi( m_pTextTeamYear->GetValue() );
    wxGetApp().pDBRoutines->structTeamData.BaseTeam = m_pCheckBoxTeamBase->IsChecked();

    wxGetApp().pDBRoutines->DBInsertTeam();

    // Continue and process this event normally.
    EndModal(wxID_ADD); // End with a RC of ADD
}

void TeamDialogAdd::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

void TeamDialogAdd::OnClose(wxCloseEvent& event)
{
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TeamDialogApply, wxPanel)
    // Controls
    EVT_BUTTON(wxID_APPLY, TeamDialogApply::OnApply)
    EVT_BUTTON(wxID_CANCEL, TeamDialogApply::OnCancel)
    EVT_CLOSE(TeamDialogApply::OnClose)
END_EVENT_TABLE()

TeamDialogApply::TeamDialogApply (wxWindow* parent, long style)
    : wxDialog(parent, -1, "Team",
                    wxPoint(10,10), wxSize(400,540),
//                    wxPoint(10,10), wxSize(500,550),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    CenterOnParent(); // Centers on the parent window

	TeamDialogCreate();
//
//	m_pTextLeagueName->SetValue( wxGetApp().pDBRoutines->structLeagueData.LeagueName );
//	m_pTextLeagueYear->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structLeagueData.LeagueYear) );

	m_pTextLeagueName->SetValue( wxGetApp().pDBRoutines->structLeagueData.LeagueName );
	m_pTextLeagueYear->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structLeagueData.LeagueYear) );
	m_pTextConferenceName->SetValue( wxGetApp().pDBRoutines->m_strConferenceName );
	m_pTextDivisionName->SetValue( wxGetApp().pDBRoutines->m_strDivisionName );
	m_pTextTeamName->SetValue( wxGetApp().pDBRoutines->structTeamData.TeamName );
	m_pTextTeamNameShort->SetValue( wxGetApp().pDBRoutines->structTeamData.TeamNameShort );
	m_pTextBallpark->SetValue( wxGetApp().pDBRoutines->structTeamData.BallparkName );
	m_pTextTotalWins->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structTeamData.TotalWins) );
	m_pTextTotalLosses->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structTeamData.TotalLosses) );
	m_pTextHomeWins->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structTeamData.HomeWins) );
	m_pTextHomeLosses->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structTeamData.HomeLosses) );
	m_pTextAwayWins->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structTeamData.AwayWins) );
	m_pTextAwayLosses->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structTeamData.AwayLosses) );
	m_pTextTeamYear->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structTeamData.TeamYear) );
	m_pCheckBoxTeamBase->SetValue( wxGetApp().pDBRoutines->structTeamData.BaseTeam );

    int myRC = ShowModal();

}

TeamDialogApply::~TeamDialogApply( )
{
    Destroy();
}

void TeamDialogApply::TeamDialogCreate( )
{
//  Create a Panel that will be used to display and edit the Leagues
    m_pTeamPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxTAB_TRAVERSAL, "Team Panel");

    wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *pItem01 = new wxBoxSizer( wxVERTICAL );

// Data section
    wxBoxSizer *pItemSizer01 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizer02 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizer03 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemSizer04 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemTeamName = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemTeamNameShort = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemBallpark = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemTotalWinsLoss = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemHomeWinsLoss = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemAwayWinsLoss = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItemTeamYearBase = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer01->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("League Name:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextLeagueName = new wxTextCtrl(m_pTeamPanel, ID_LEAGUE_LEAGUENAME,
                    _T(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
    pItemSizer01->Add( m_pTextLeagueName, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizer02->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("League Year:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextLeagueYear = new wxTextCtrl(m_pTeamPanel, ID_LEAGUE_YEAR,
                    _T(""), wxDefaultPosition, wxSize(60,-1), wxTE_READONLY );
    pItemSizer02->Add( m_pTextLeagueYear, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizer03->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Conference Name:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextConferenceName = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
    pItemSizer03->Add( m_pTextConferenceName, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemSizer04->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Division Name:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextDivisionName = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1), wxTE_READONLY );
    pItemSizer04->Add( m_pTextDivisionName, 0, wxALIGN_LEFT|wxLEFT, 0 );

	// Team section

    pItemTeamName->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Team Name:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextTeamName = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemTeamName->Add( m_pTextTeamName, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemTeamNameShort->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Team Name Short:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextTeamNameShort = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemTeamNameShort->Add( m_pTextTeamNameShort, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemBallpark->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Ballpark:"), wxDefaultPosition, wxSize(125,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextBallpark = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemBallpark->Add( m_pTextBallpark, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemTotalWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Total Wins:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextTotalWins = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemTotalWinsLoss->Add( m_pTextTotalWins, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemTotalWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Total Losses:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextTotalLosses = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemTotalWinsLoss->Add( m_pTextTotalLosses, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemHomeWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Home Wins:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextHomeWins = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemHomeWinsLoss->Add( m_pTextHomeWins, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemHomeWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Home Losses:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextHomeLosses = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemHomeWinsLoss->Add( m_pTextHomeLosses, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemAwayWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Away Wins:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextAwayWins = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemAwayWinsLoss->Add( m_pTextAwayWins, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemAwayWinsLoss->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Away Losses:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextAwayLosses = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemAwayWinsLoss->Add( m_pTextAwayLosses, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemTeamYearBase->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Team Year:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextTeamYear = new wxTextCtrl(m_pTeamPanel, wxID_ANY,
                    _T(""), wxDefaultPosition, wxSize(55,-1) );
    pItemTeamYearBase->Add( m_pTextTeamYear, 0, wxALIGN_LEFT|wxLEFT, 0 );

    pItemTeamYearBase->Add(new wxStaticText(m_pTeamPanel, wxID_ANY,
        _("Base Team:"), wxDefaultPosition, wxSize(100,-1)), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pCheckBoxTeamBase = new wxCheckBox(m_pTeamPanel, ID_LEAGUE_BASE,
		_T(""), wxDefaultPosition, wxDefaultSize );
    pItemTeamYearBase->Add( m_pCheckBoxTeamBase, 0, wxALIGN_LEFT|wxLEFT, 0 );

// Add each line item to the panel
	pItem01->AddSpacer(10);
    pItem01->Add( pItemSizer01, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer02, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer03, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer04, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add (new wxStaticLine(m_pTeamPanel, wxID_ANY, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
    pItem01->Add( pItemTeamName, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemTeamNameShort, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemBallpark, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemTotalWinsLoss, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemHomeWinsLoss, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemAwayWinsLoss, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemTeamYearBase, 0, wxALIGN_LEFT|wxLEFT, 0 );
//    pItem01->AddSpacer(20);

	// Create and place Control buttons
    pItem01->Add ( BuildControlButtons(m_pTeamPanel),
                    0, wxALIGN_CENTER );

    pTopSizer->Add( pItem01, 1, wxGROW|wxALL, 5 );
    pTopSizer->AddSpacer(5);

    m_pTeamPanel->SetSizer(pTopSizer);
    pTopSizer->Fit(m_pTeamPanel);
//    pTopSizer->SetSizeHints(this);
}

wxBoxSizer* TeamDialogApply::BuildControlButtons( wxWindow* panel )
{
    // Create the control buttons (Apply, Add, and Cancel)

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonPane = new wxBoxSizer (wxHORIZONTAL);

    m_pApplyButton = new wxButton (panel, wxID_APPLY);
    m_pApplyButton->SetDefault();
    buttonPane->Add (m_pApplyButton, 0, wxALIGN_CENTER);

    buttonPane->AddSpacer( 10 );

    m_pCancelButton = new wxButton (panel, wxID_CANCEL);
    buttonPane->Add (m_pCancelButton, 0, wxALIGN_CENTER);

    sizer->Add (new wxStaticLine(panel, -1, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
    sizer->Add (buttonPane, 0, wxALIGN_CENTER );

    // Add a space under the buttons
    sizer->AddSpacer( 10 );

    return (sizer);
}

void TeamDialogApply::OnApply(wxCommandEvent& event)
{
    wxGetApp().pDBRoutines->structTeamData.TeamName = m_pTextTeamName->GetValue();
    wxGetApp().pDBRoutines->structTeamData.TeamNameShort = m_pTextTeamNameShort->GetValue();
    wxGetApp().pDBRoutines->structTeamData.BallparkName = m_pTextBallpark->GetValue();
    wxGetApp().pDBRoutines->structTeamData.TotalWins = atoi(m_pTextTotalWins->GetValue());
    wxGetApp().pDBRoutines->structTeamData.TotalLosses = atoi(m_pTextTotalLosses->GetValue());
    wxGetApp().pDBRoutines->structTeamData.HomeWins = atoi(m_pTextHomeWins->GetValue());
    wxGetApp().pDBRoutines->structTeamData.HomeLosses = atoi(m_pTextHomeLosses->GetValue());
    wxGetApp().pDBRoutines->structTeamData.AwayWins = atoi(m_pTextAwayWins->GetValue());
    wxGetApp().pDBRoutines->structTeamData.AwayLosses = atoi(m_pTextAwayLosses->GetValue());
    wxGetApp().pDBRoutines->structTeamData.LeagueID = wxGetApp().pDBRoutines->structLeagueData.LeagueID;
    wxGetApp().pDBRoutines->structTeamData.ConferenceID = wxGetApp().pDBRoutines->m_intConferenceID;
    wxGetApp().pDBRoutines->structTeamData.DivisionID = wxGetApp().pDBRoutines->m_intDivisionID;
    wxGetApp().pDBRoutines->structTeamData.TeamYear = atoi( m_pTextTeamYear->GetValue() );
    wxGetApp().pDBRoutines->structTeamData.BaseTeam = m_pCheckBoxTeamBase->IsChecked();

    wxGetApp().pDBRoutines->DBUpdateTeam( wxGetApp().pDBRoutines->structTeamData.TeamID );

    // Continue and process this event normally.
    EndModal(wxID_APPLY); // End with a RC of ADD
}

void TeamDialogApply::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

void TeamDialogApply::OnClose(wxCloseEvent& event)
{
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(TeamDialogCreate, wxPanel)
    // Controls
    EVT_BUTTON(wxID_APPLY, TeamDialogCreate::OnApply)
    EVT_BUTTON(wxID_CANCEL, TeamDialogCreate::OnCancel)
    EVT_CLOSE(TeamDialogCreate::OnClose)
    EVT_TREELIST_ITEM_CHECKED(wxID_ANY, TeamDialogCreate::OnItemChecked)
END_EVENT_TABLE()

TeamDialogCreate::TeamDialogCreate (wxWindow* parent, long style)
    : wxDialog(parent, -1, "Team",
                    wxPoint(10,10), wxSize(800,540),
//                    wxPoint(10,10), wxSize(500,550),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	int DBRC;

    CenterOnParent(); // Centers on the parent window

	DBRC = wxGetApp().pDBRoutines->DBIsDBOpen();

	TeamDialogCreateDLG();
//
//	m_pTextLeagueName->SetValue( wxGetApp().pDBRoutines->structLeagueData.LeagueName );
//	m_pTextLeagueYear->SetValue( wxString::Format(wxT("%i"), wxGetApp().pDBRoutines->structLeagueData.LeagueYear) );

    int myRC = ShowModal();
}

TeamDialogCreate::~TeamDialogCreate( )
{
    Destroy();
}

void TeamDialogCreate::TeamDialogCreateDLG( )
{
//  Create a Panel that will be used to display and edit the Leagues
    m_pTeamPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxTAB_TRAVERSAL, "Team Panel");

    wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *pHorizontal = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *pItem01 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *pItem02 = new wxBoxSizer( wxVERTICAL );

// Data section

    m_isFlat = false;

    // Construct the image list with the standard images.
    InitImageList();

    // Create and layout child controls.
//    m_treelist = CreateTreeListCtrl(wxTL_DEFAULT_STYLE);
    m_treelist = CreateTreeListCtrl( wxTL_DEFAULT_STYLE | wxTL_CHECKBOX | wxTL_MULTIPLE );
    m_treelistOut = CreateTreeListCtrlOut( wxTL_DEFAULT_STYLE | wxTL_CHECKBOX );

	// Create and place Control buttons
    pItem01->Add ( BuildControlButtons(m_pTeamPanel),
                    0, wxALIGN_CENTER );

    pHorizontal->Add( m_treelist, 1, wxGROW|wxALL, 5 );
    pHorizontal->Add( m_treelistOut, 1, wxGROW|wxALL, 5 );
    pTopSizer->Add( pHorizontal, wxSizerFlags(2).Expand() );
    pTopSizer->Add( pItem01, 1, wxGROW|wxALL, 5 );
    pTopSizer->AddSpacer(5);

    m_pTeamPanel->SetSizer(pTopSizer);
    pTopSizer->Fit(m_pTeamPanel);
//    pTopSizer->SetSizeHints(this);
}

wxBoxSizer* TeamDialogCreate::BuildControlButtons( wxWindow* panel )
{
    // Create the control buttons (Apply, Add, and Cancel)

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonPane = new wxBoxSizer (wxHORIZONTAL);

    m_pApplyButton = new wxButton (panel, wxID_APPLY);
    m_pApplyButton->SetDefault();
    buttonPane->Add (m_pApplyButton, 0, wxALIGN_CENTER);

    buttonPane->AddSpacer( 10 );

    m_pCancelButton = new wxButton (panel, wxID_CANCEL);
    buttonPane->Add (m_pCancelButton, 0, wxALIGN_CENTER);

    sizer->Add (new wxStaticLine(panel, -1, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
    sizer->Add (buttonPane, 0, wxALIGN_CENTER );

    // Add a space under the buttons
    sizer->AddSpacer( 10 );

    return (sizer);
}

void TeamDialogCreate::InitImageList()
{
    wxSize iconSize = wxArtProvider::GetSizeHint(wxART_LIST, this);
    if ( iconSize == wxDefaultSize )
        iconSize = FromDIP(wxSize(16, 16));

    m_imageList = new wxImageList(iconSize.x, iconSize.y);

    // The order should be the same as for the enum elements.
    static const wxString icons[] =
    {
        wxART_NORMAL_FILE,
        wxART_FOLDER,
        wxART_FOLDER_OPEN
    };

    for ( unsigned n = 0; n < WXSIZEOF(icons); n++ )
    {
        m_imageList->Add
                     (
                        wxArtProvider::GetIcon(icons[n], wxART_LIST, iconSize)
                     );
    }
}

wxTreeListCtrl* TeamDialogCreate::CreateTreeListCtrl(long style)
{
    wxTreeListCtrl* const tree = new wxTreeListCtrl(m_pTeamPanel, wxID_ANY,
                                  wxDefaultPosition, wxSize( 390, 420 ),
                                  style);
	int myCountLeague;
	int myCountConference;
	int myCountDivision;
	int myCountTeams;
	int myCountBatters;
	int myCountPitchers;
//	wxString myLeagueName;
	wxTreeListItem myItemLeague;
	wxTreeListItem myItemConference;
	wxTreeListItem myItemDivision;
	wxTreeListItem myItemTeam;
	wxTreeListItem myItemBatters;
	wxTreeListItem myItemPitchers;

//    tree->SetImageList(m_imageList);

    tree->AppendColumn("Base Leagues",
                       wxCOL_WIDTH_AUTOSIZE,
                       wxALIGN_LEFT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);

	wxGetApp().pDBRoutines->DBGetBaseLeagues( TRUE );
//	wxString myLeague = "Base 1965";
	myCountLeague = wxGetApp().pDBRoutines->m_arrayLeagueNames.Count();

    wxTreeListItem root = tree->GetRootItem();

	// Loop through all Base Leagues, their conferences,
	// there divisions, teams and then players.
	for ( int iL = 0; iL < myCountLeague; iL++ )
	{
		myItemLeague = tree->AppendItem( root, wxGetApp().pDBRoutines->m_arrayLeagueNames[iL], -1, -1 );
		wxGetApp().pDBRoutines->DBGetConferenceList( wxGetApp().pDBRoutines->m_arrayLeagueIDs[iL] );
		myCountConference = wxGetApp().pDBRoutines->m_arrayConferenceNames.Count();
		for ( int iC = 0; iC < myCountConference; iC++ )
		{
			myItemConference = tree->AppendItem( myItemLeague, wxGetApp().pDBRoutines->m_arrayConferenceNames[iC], -1, -1 );
			wxGetApp().pDBRoutines->DBGetDivisionList( wxGetApp().pDBRoutines->m_arrayConferenceIDs[iC] );
			myCountDivision = wxGetApp().pDBRoutines->m_arrayDivisionNames.Count();
			for ( int iD = 0; iD < myCountDivision; iD++ )
			{
				myItemDivision = tree->AppendItem( myItemConference, wxGetApp().pDBRoutines->m_arrayDivisionNames[iD], -1, -1 );
				wxGetApp().pDBRoutines->DBGetTeamList(
					wxGetApp().pDBRoutines->m_arrayLeagueIDs[iL],
					wxGetApp().pDBRoutines->m_arrayConferenceIDs[iC],
					wxGetApp().pDBRoutines->m_arrayDivisionIDs[iD]
					);
				myCountTeams = wxGetApp().pDBRoutines->m_arrayTeamNames.Count();
				for ( int iT = 0; iT < myCountTeams; iT++ )
				{
					myItemTeam = tree->AppendItem( myItemDivision, wxGetApp().pDBRoutines->m_arrayTeamNames[iT], -1, -1 );
					myItemBatters = tree->AppendItem( myItemTeam, "Batters", -1, -1 );
					wxGetApp().pDBRoutines->DBGetBatterStatsID( wxGetApp().pDBRoutines->m_arrayTeamIDs[iT] );
					myCountBatters = wxGetApp().pDBRoutines->m_arrayBatterLastFirst.Count();
					// It is possible that a team does not have any batters
					if ( myCountBatters |= 0 )
					{
						for ( int iB = 0; iB < myCountBatters; iB++ )
						{
							tree->AppendItem( myItemBatters, wxGetApp().pDBRoutines->m_arrayBatterLastFirst[iB], -1, -1 );
						}
					}
					myItemPitchers = tree->AppendItem( myItemTeam, "Pitchers", -1, -1 );
					wxGetApp().pDBRoutines->DBGetPitcherStatsID( wxGetApp().pDBRoutines->m_arrayTeamIDs[iT] );
					myCountPitchers = wxGetApp().pDBRoutines->m_arrayPitcherLastFirst.Count();
					// It is possible that a team does not have any pitchers
					if ( myCountPitchers |= 0 )
					{
						for ( int iP = 0; iP < myCountPitchers; iP++ )
						{
							tree->AppendItem( myItemPitchers, wxGetApp().pDBRoutines->m_arrayPitcherLastFirst[iP], -1, -1 );
						}
					}
				}
			}
		}
	}

//	wxTreeListItem myItemLeague = tree->AppendItem( root, myLeagueName, -1, -1 );
//		wxTreeListItem myItemConference = tree->AppendItem( myItemLeague, "BaseAL1965", -1, -1 );
//			wxTreeListItem myItemDivision = tree->AppendItem( myItemConference, "DEFAULT", -1, -1 );
//				wxTreeListItem myItemTeam = tree->AppendItem( myItemDivision, "Cleveland Indians", -1, -1 );
//					wxTreeListItem myItemBatters = tree->AppendItem( myItemTeam, "Batters", -1, -1 );
//					wxTreeListItem myItemPitchers = tree->AppendItem( myItemTeam, "Pitchers", -1, -1 );
//						tree->AppendItem( myItemBatters, "Fred Whitfield", -1, -1 );
//						tree->AppendItem( myItemBatters, "Pedro Gonzalez", -1, -1 );
//						tree->AppendItem( myItemBatters, "Joe Azcue", -1, -1 );
//						tree->AppendItem( myItemBatters, "Chuck Hinton", -1, -1 );
//						tree->AppendItem( myItemBatters, "Max Alvis", -1, -1 );
//						tree->AppendItem( myItemBatters, "Larry Brown", -1, -1 );
//						tree->AppendItem( myItemBatters, "Rocky Colavito", -1, -1 );
//						tree->AppendItem( myItemBatters, "Vic Davalillo", -1, -1 );
//						tree->AppendItem( myItemBatters, "Phil Roof", -1, -1 );
//						tree->AppendItem( myItemBatters, "Dick Howser", -1, -1 );
//						tree->AppendItem( myItemBatters, "Chico Salmon", -1, -1 );
//						tree->AppendItem( myItemBatters, "Leon Wagner", -1, -1 );
//						tree->AppendItem( myItemBatters, "Lee Stange", -1, -1 );
//						tree->AppendItem( myItemBatters, "Sam McDowell", -1, -1 );
//						tree->AppendItem( myItemBatters, "Luis Tiant", -1, -1 );
//						tree->AppendItem( myItemBatters, "Sonny Siebert", -1, -1 );
//						tree->AppendItem( myItemBatters, "Ralph Terry", -1, -1 );
//						tree->AppendItem( myItemBatters, "Gary Bell", -1, -1 );
//						tree->AppendItem( myItemBatters, "Jack Kralick", -1, -1 );
//						tree->AppendItem( myItemBatters, "Don McMahon", -1, -1 );
//						tree->AppendItem( myItemPitchers, "Lee Stange", -1, -1 );
//						tree->AppendItem( myItemPitchers, "Sam McDowell", -1, -1 );
//						tree->AppendItem( myItemPitchers, "Luis Tiant", -1, -1 );
//						tree->AppendItem( myItemPitchers, "Sonny Siebert", -1, -1 );
//						tree->AppendItem( myItemPitchers, "Ralph Terry", -1, -1 );
//						tree->AppendItem( myItemPitchers, "Gary Bell", -1, -1 );
//						tree->AppendItem( myItemPitchers, "Jack Kralick", -1, -1 );
//						tree->AppendItem( myItemPitchers, "Don McMahon", -1, -1 );

    return tree;
}

wxTreeListCtrl* TeamDialogCreate::CreateTreeListCtrlOut(long style)
{
    wxTreeListCtrl* const tree = new wxTreeListCtrl(m_pTeamPanel, wxID_ANY,
                                  wxDefaultPosition, wxSize( 390, 420 ),
                                  style);
	int myCountLeague;
	int myCountConference;
	int myCountDivision;
	int myCountTeams;
	int myCountBatters;
	int myCountPitchers;
	wxTreeListItem myItemLeague;
	wxTreeListItem myItemConference;
	wxTreeListItem myItemDivision;
	wxTreeListItem myItemTeam;
	wxTreeListItem myItemBatters;
	wxTreeListItem myItemPitchers;

//    tree->SetImageList(m_imageList);

    tree->AppendColumn("Target Leagues",
                       wxCOL_WIDTH_AUTOSIZE,
                       wxALIGN_LEFT,
                       wxCOL_RESIZABLE | wxCOL_SORTABLE);

	// Get Non base teams
	wxGetApp().pDBRoutines->DBGetBaseLeagues( FALSE );
	myCountLeague = wxGetApp().pDBRoutines->m_arrayLeagueNames.Count();

    wxTreeListItem root = tree->GetRootItem();

	// Loop through all Leagues, their conferences,
	// there divisions and teams
	for ( int iL = 0; iL < myCountLeague; iL++ )
	{
		myItemLeague = tree->AppendItem( root, wxGetApp().pDBRoutines->m_arrayLeagueNames[iL], -1, -1 );
		wxGetApp().pDBRoutines->DBGetConferenceList( wxGetApp().pDBRoutines->m_arrayLeagueIDs[iL] );
		myCountConference = wxGetApp().pDBRoutines->m_arrayConferenceNames.Count();
		for ( int iC = 0; iC < myCountConference; iC++ )
		{
			myItemConference = tree->AppendItem( myItemLeague, wxGetApp().pDBRoutines->m_arrayConferenceNames[iC], -1, -1 );
			wxGetApp().pDBRoutines->DBGetDivisionList( wxGetApp().pDBRoutines->m_arrayConferenceIDs[iC] );
			myCountDivision = wxGetApp().pDBRoutines->m_arrayDivisionNames.Count();
			for ( int iD = 0; iD < myCountDivision; iD++ )
			{
				myItemDivision = tree->AppendItem( myItemConference, wxGetApp().pDBRoutines->m_arrayDivisionNames[iD], -1, -1 );
				wxGetApp().pDBRoutines->DBGetTeamList(
					wxGetApp().pDBRoutines->m_arrayLeagueIDs[iL],
					wxGetApp().pDBRoutines->m_arrayConferenceIDs[iC],
					wxGetApp().pDBRoutines->m_arrayDivisionIDs[iD]
					);
				myCountTeams = wxGetApp().pDBRoutines->m_arrayTeamNames.Count();
				for ( int iT = 0; iT < myCountTeams; iT++ )
				{
					myItemTeam = tree->AppendItem( myItemDivision, wxGetApp().pDBRoutines->m_arrayTeamNames[iT], -1, -1 );
				}
			}
		}
	}
    return tree;
}

void TeamDialogCreate::OnApply(wxCommandEvent& event)
{
	int numSelected;
	numSelected = 0;
	int numSelectedOut;
	numSelectedOut = 0;
	int numIsSelected;
	numIsSelected = 0;
	int mynumber;
	wxString MsgBuffer;
    wxTreeListItems selections;
    wxString strPlayerLastFirst;
    wxString strPlayerType;
    wxString strTeam;
    wxString strDivision;
    wxString strConference;
    wxString strLeague;
    int leagueIDIn;
    int conferenceIDIn;
    int divisionIDIn;
    int teamIDIn;
    int leagueIDOut;
    int conferenceIDOut;
    int divisionIDOut;
    int teamIDOut;
	int mycomma;
	int mycount;
	int myBatterID;
	int myPitcherID;
	int noDestTeamSelected = false;
    wxTreeListItem itemPlayer;
    wxTreeListItem itemTeam;

	// Get the destination Team
	for ( wxTreeListItem item = m_treelistOut->GetFirstItem();
			item.IsOk();
			item = m_treelistOut->GetNextItem(item) )
			{
				if ( m_treelistOut->GetCheckedState(item) == wxCHK_CHECKED )
				{
					// This check will determin if we are selecting a team
					// or if something like a division was selected.
					// We can only select a team for this to work.
					itemTeam = m_treelistOut->GetFirstChild( item );
					if ( itemTeam.IsOk() )
					{
						wxMessageBox("The selected item is not a Team!");
						return;
					}

					noDestTeamSelected = true;
					strTeam = m_treelistOut->GetItemText( item );
					strDivision = m_treelistOut->GetItemText( m_treelistOut->GetItemParent( item ) );
					strConference = m_treelistOut->GetItemText( m_treelistOut->GetItemParent( m_treelistOut->GetItemParent( item ) ) );
					strLeague = m_treelistOut->GetItemText( m_treelistOut->GetItemParent( m_treelistOut->GetItemParent( m_treelistOut->GetItemParent( item ) ) ) );

					leagueIDOut = wxGetApp().pDBRoutines->DBGetLeagueID( strLeague );
					conferenceIDOut = wxGetApp().pDBRoutines->DBGetConferenceID( strConference, leagueIDOut );
					if ( strDivision == "DEFAULT" )
					{
						divisionIDOut = 1;
					}
					else
					{
						divisionIDOut = wxGetApp().pDBRoutines->DBGetDivisionID( strDivision, leagueIDOut );
					}
					teamIDOut = wxGetApp().pDBRoutines->DBGetTeamID( strTeam, leagueIDOut, conferenceIDOut, divisionIDOut );
				}
			}
			if ( noDestTeamSelected == false )
			{
				wxMessageBox("No destination Team was selected!");
				return;
			}

	for ( wxTreeListItem item = m_treelist->GetFirstItem();
			item.IsOk();
			item = m_treelist->GetNextItem(item) )
			{
				if ( m_treelist->GetCheckedState(item) == wxCHK_CHECKED )
				{
					// This check will determin if we are selecting a player
					// or if something like a division was selected.
					// We can only select a Player for this to work.
					itemPlayer = m_treelist->GetFirstChild( item );
					if ( itemPlayer.IsOk() )
					{
						wxMessageBox("The selected item is not a Player!");
						return;
					}

					strTeam = m_treelist->GetItemText( m_treelist->GetItemParent( m_treelist->GetItemParent( item ) ) );
					strDivision = m_treelist->GetItemText( m_treelist->GetItemParent( m_treelist->GetItemParent( m_treelist->GetItemParent( item ) ) ) );
					strConference = m_treelist->GetItemText( m_treelist->GetItemParent( m_treelist->GetItemParent( m_treelist->GetItemParent( m_treelist->GetItemParent( item ) ) ) ) );
					strLeague = m_treelist->GetItemText( m_treelist->GetItemParent( m_treelist->GetItemParent( m_treelist->GetItemParent( m_treelist->GetItemParent( m_treelist->GetItemParent( item ) ) ) ) ) );

					leagueIDIn = wxGetApp().pDBRoutines->DBGetLeagueID( strLeague );
					conferenceIDIn = wxGetApp().pDBRoutines->DBGetConferenceID( strConference, leagueIDIn );
					if ( strDivision == "DEFAULT" )
					{
						divisionIDIn = 1;
					}
					else
					{
						divisionIDIn = wxGetApp().pDBRoutines->DBGetDivisionID( strDivision, leagueIDIn );
					}
					teamIDIn = wxGetApp().pDBRoutines->DBGetTeamID( strTeam, leagueIDIn, conferenceIDIn, divisionIDIn );
//					MsgBuffer.Printf( wxT("The input League is: %s\nThe input Team is: %s\nThe input Division is: %s\nThe input Conference is: %s\n"), strLeague, strTeam, strDivision, strConference );
//					wxMessageBox(MsgBuffer);
//					MsgBuffer.Printf( wxT("The input TeamID is: %i"), teamIDIn );
//					wxMessageBox(MsgBuffer);

					// Retrieve the player name in Last, First format
					strPlayerLastFirst = m_treelist->GetItemText( item );
					mycomma = strPlayerLastFirst.Find(", ");
					mycount = strPlayerLastFirst.size();
					wxString myLast = strPlayerLastFirst.substr(0, mycomma);
					wxString myFirst = strPlayerLastFirst.substr(mycomma+2, mycount);

					// Decide if player is a batter or a pitcher
					// Field will have either Batters ot Pitchers
					strPlayerType = m_treelist->GetItemText( m_treelist->GetItemParent( item ) );
					if ( strPlayerType == "Batters")
					{
						// Get BatterID using teamIDIn, myFirst, myLast
//						MsgBuffer.Printf( wxT("The TeamID is: %i\nFirstName is: %S\nLastNAme is:%s"), teamIDIn, myFirst, myLast );
//						wxMessageBox(MsgBuffer);
						myBatterID = wxGetApp().pDBRoutines->DBGetBatterID( teamIDIn, myFirst, myLast );
						// Clear the BatterStats
						wxGetApp().pDBRoutines->DBClearBatterStats();
						// Create new Batter stats using teamIDOut and myBatterID
//						MsgBuffer.Printf( wxT("The Selected TeamOutID is: %i\nThe Selected BatterID is: %i"), teamIDOut, myBatterID );
//						wxMessageBox(MsgBuffer);
						wxGetApp().pDBRoutines->DBInsertBatterStats( myBatterID, teamIDOut );
//						wxMessageBox("The selected item is a Batter!");
//						MsgBuffer.Printf( wxT("The Selected BatterID is: %i"), myBatterID );
//						wxMessageBox(MsgBuffer);
					}
					else
					{
						// Get PitcherID using teamIDIn, myFirst, myLast
						myPitcherID = wxGetApp().pDBRoutines->DBGetPitcherID( teamIDIn, myFirst, myLast );
						// Clear the PitcherStats
						wxGetApp().pDBRoutines->DBClearPitcherStats();
						// Create new PitcherStats using teamIDOut and myPitcherID
						wxGetApp().pDBRoutines->DBInsertPitcherStats( myPitcherID, teamIDOut );
//						wxMessageBox("The selected item is a Pitcher!");
//						MsgBuffer.Printf( wxT("The Selected PitcherID is: %i"), myPitcherID );
//						wxMessageBox(MsgBuffer);
					}
				}	// End if ifChecked
			}
    // Continue and process this event normally.
    EndModal(wxID_APPLY); // End with a RC of ADD
}

wxString TeamDialogCreate::DumpItem(wxTreeListItem item) const
{
    return item.IsOk() ? m_treelist->GetItemText(item) : wxString("NONE");
}

void TeamDialogCreate::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

void TeamDialogCreate::OnClose(wxCloseEvent& event)
{
    EndModal(wxID_CANCEL); // End with a RC of CANCEL
}

void TeamDialogCreate::OnItemChecked(wxTreeListEvent& event)
{
    wxTreeListItem item = event.GetItem();

    if ( m_treelist->GetCheckedState(item) == wxCHK_CHECKED )
    {
		m_treelist->Select( item );
//		wxLogMessage("Item Checked and Selected: %s", DumpItem( item ));
    }
    else
    {
		m_treelist->Unselect( item );
//		wxLogMessage("Item UnChecked and UnSelected: %s", DumpItem( item ));
    }

//    wxLogMessage("Item \"%s\" toggled, now %s (was %s)",
//                 DumpItem(item),
//                 CheckedStateString(m_treelist->GetCheckedState(item)),
//                 CheckedStateString(event.GetOldCheckedState()));
}

/* static */
const char* TeamDialogCreate::CheckedStateString(wxCheckBoxState state)
{
    switch ( state )
    {
        case wxCHK_UNCHECKED:
            return "unchecked";

        case wxCHK_UNDETERMINED:
            return "undetermined";

        case wxCHK_CHECKED:
            return "checked";
    }

    return "invalid";
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(CheckEntriesDialog, wxDialog)
    // Controls
//    EVT_BUTTON(wxID_APPLY, CheckEntriesDialog::OnApply)
//    EVT_BUTTON(wxID_OK, CheckEntriesDialog::OnOK)
//    EVT_BUTTON(wxID_CANCEL, CheckEntriesDialog::OnCancel)
//
//    EVT_TEXT(ID_CHECKENTRIES_DATADIR, CheckEntriesDialog::OnCheckEntriesChangeText)
//    EVT_TEXT(ID_CHECKENTRIES_HTMLINDEX, CheckEntriesDialog::OnCheckEntriesChangeText)
//    EVT_TEXT(ID_CHECKENTRIES_HTMLBG, CheckEntriesDialog::OnCheckEntriesChangeText)
//    EVT_TEXT(ID_CHECKENTRIES_HTMLLINKCOLOR, CheckEntriesDialog::OnCheckEntriesChangeText)
//    EVT_TEXT(ID_CHECKENTRIESS_HTMLVLINKCOLOR, CheckEntriesDialog::OnCheckEntriesChangeText)
//    EVT_TEXT(ID_CHECKENTRIES_HTMLBGCOLOR, CheckEntriesDialog::OnCheckEntriesChangeText)
//    EVT_TEXT(ID_CHECKENTRIES_HTMLTEXTCOLOR, CheckEntriesDialog::OnCheckEntriesChangeText)

END_EVENT_TABLE()

CheckEntriesDialog::CheckEntriesDialog (wxWindow* parent, long style)
    : wxDialog(parent, -1, "Options",
                    wxPoint(10,10), wxSize(500,550),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    CenterOnParent(); // Centers on the parent window

    CheckEntriesDialogCreate();
    CheckEntriesDialogInitialize();
    ShowModal();
}

CheckEntriesDialog::~CheckEntriesDialog( )
{
    Destroy();
}

void CheckEntriesDialog::CheckEntriesDialogCreate ()
{
//  Create a dialog that will be used to display and edit the options

    m_pCheckEntriesPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxTAB_TRAVERSAL, "Check Entries Panel");

    wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *pItem01 = new wxBoxSizer( wxVERTICAL );
//    wxBoxSizer *pItem02 = new wxBoxSizer( wxVERTICAL );

// Heading section
    wxBoxSizer *pItemSizer01 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer01->Add(new wxStaticText(m_pCheckEntriesPanel, wxID_ANY,
        _("Check Entries:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

// Data section
    // Hits
    wxBoxSizer *pItemSizer02 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer02->Add(new wxStaticText(m_pCheckEntriesPanel, wxID_ANY,
        _("Batter Hits:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    m_strBattersHits = new wxTextCtrl(m_pCheckEntriesPanel, wxID_ANY, wxEmptyString);
    pItemSizer02->Add(m_strBattersHits, 0, wxALL, 5);
//    m_intBattersHits->SetValue("100");
    pItemSizer02->Add(new wxStaticText(m_pCheckEntriesPanel, wxID_ANY,
        _("Pitcher Hits:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    m_strPitchersHits = new wxTextCtrl(m_pCheckEntriesPanel, wxID_ANY, wxEmptyString);
    pItemSizer02->Add(m_strPitchersHits, 0, wxALL, 5);

    // Walks
    wxBoxSizer *pItemSizer03 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer03->Add(new wxStaticText(m_pCheckEntriesPanel, wxID_ANY,
        _("Batter Walks:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    m_strBattersWalks = new wxTextCtrl(m_pCheckEntriesPanel, wxID_ANY, wxEmptyString);
    pItemSizer03->Add(m_strBattersWalks, 0, wxALL, 5);
//    m_intBattersHits->SetValue("100");
    pItemSizer03->Add(new wxStaticText(m_pCheckEntriesPanel, wxID_ANY,
        _("Pitcher Walks:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    m_strPitchersWalks = new wxTextCtrl(m_pCheckEntriesPanel, wxID_ANY, wxEmptyString);
    pItemSizer03->Add(m_strPitchersWalks, 0, wxALL, 5);

    // StrikeOuts
    wxBoxSizer *pItemSizer04 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer04->Add(new wxStaticText(m_pCheckEntriesPanel, wxID_ANY,
        _("Batter StrikeOuts:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    m_strBattersStrikeOuts = new wxTextCtrl(m_pCheckEntriesPanel, wxID_ANY, wxEmptyString);
    pItemSizer04->Add(m_strBattersStrikeOuts, 0, wxALL, 5);
//    m_intBattersHits->SetValue("100");
    pItemSizer04->Add(new wxStaticText(m_pCheckEntriesPanel, wxID_ANY,
        _("Pitcher StrikeOuts:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    m_strPitchersStrikeOuts = new wxTextCtrl(m_pCheckEntriesPanel, wxID_ANY, wxEmptyString);
    pItemSizer04->Add(m_strPitchersStrikeOuts, 0, wxALL, 5);

    // HomeRuns
    wxBoxSizer *pItemSizer05 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer05->Add(new wxStaticText(m_pCheckEntriesPanel, wxID_ANY,
        _("Batter HomeRuns:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    m_strBattersHRs = new wxTextCtrl(m_pCheckEntriesPanel, wxID_ANY, wxEmptyString);
    pItemSizer05->Add(m_strBattersHRs, 0, wxALL, 5);
//    m_intBattersHits->SetValue("100");
    pItemSizer05->Add(new wxStaticText(m_pCheckEntriesPanel, wxID_ANY,
        _("Pitcher HomeRuns:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    m_strPitchersHRs = new wxTextCtrl(m_pCheckEntriesPanel, wxID_ANY, wxEmptyString);
    pItemSizer05->Add(m_strPitchersHRs, 0, wxALL, 5);


// Add each line item to the panel
    pItem01->Add( pItemSizer01, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer02, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer03, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer04, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer05, 0, wxALIGN_LEFT|wxLEFT, 0 );

// Create and place Control buttons
    pItem01->Add ( BuildControlButtons(m_pCheckEntriesPanel),
                    0, wxALIGN_CENTER);

    pTopSizer->Add( pItem01, 1, wxGROW|wxALL, 5 );
    pTopSizer->AddSpacer(5);

    m_pCheckEntriesPanel->SetSizer(pTopSizer);
    pTopSizer->Fit(m_pCheckEntriesPanel);
}

void CheckEntriesDialog::CheckEntriesDialogInitialize ()
{
//    wxGetApp().pDBRoutines->DBCheckEntries( LeagueID, ConferenceID, DivisionID );
	int LeagueID;
	int ConferenceID;
	int DivisionID;
//	int TeamID;

	// Check is DB is open, if not, open one
	wxGetApp().pDBRoutines->DBIsDBOpen();

	// DBGetLeague will prompt for the league
	LeagueID = wxGetApp().pDBRoutines->DBGetLeague();
	if (LeagueID != wxID_CANCEL)
	{
		// This will populate the structLeagueData
		wxGetApp().pDBRoutines->DBGetLeague( LeagueID );

		// Get Conference, check to see if valid, then
		// get Division and check to see if its valid
		ConferenceID = wxGetApp().pDBRoutines->DBGetConferenceID( LeagueID );
		if (ConferenceID != wxID_CANCEL)
		{
			DivisionID = wxGetApp().pDBRoutines->DBGetDivisionID( ConferenceID );
			if (DivisionID != wxID_CANCEL)
			{
			// Get Teams based on LeagueID, ConferenceID and Division ID.
			// Then sum hits, HRs, Walks, Strikeouts for all playersStats in selections.

			// Process League data
				wxGetApp().pDBRoutines->DBCheckEntries( LeagueID, ConferenceID, DivisionID );

			}
		}
		m_strBattersHits->SetValue( wxString::Format(wxT("%3i"), (wxGetApp().pDBRoutines->m_totalBHits )));
		m_strPitchersHits->SetValue( wxString::Format(wxT("%3i"), (wxGetApp().pDBRoutines->m_totalPHits )));
		m_strBattersWalks->SetValue( wxString::Format(wxT("%3i"), (wxGetApp().pDBRoutines->m_totalBWalks )));
		m_strPitchersWalks->SetValue( wxString::Format(wxT("%3i"), (wxGetApp().pDBRoutines->m_totalPWalks )));
		m_strBattersStrikeOuts->SetValue( wxString::Format(wxT("%3i"), (wxGetApp().pDBRoutines->m_totalBStrikeouts )));
		m_strPitchersStrikeOuts->SetValue( wxString::Format(wxT("%3i"), (wxGetApp().pDBRoutines->m_totalPStrikeouts )));
		m_strBattersHRs->SetValue( wxString::Format(wxT("%3i"), (wxGetApp().pDBRoutines->m_totalBHomeRuns )));
		m_strPitchersHRs->SetValue( wxString::Format(wxT("%3i"), (wxGetApp().pDBRoutines->m_totalPHomeRuns )));
	}
}

wxBoxSizer* CheckEntriesDialog::BuildControlButtons( wxWindow* panel )
{
    // Create the control buttons ( OK )

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonPane = new wxBoxSizer (wxHORIZONTAL);
    //  By adding a space with wxExpand (non zero),
    //    the butons will be positioned correctly

    wxButton *okButton = new wxButton (panel, wxID_OK);
    okButton->SetDefault();
    buttonPane->Add (okButton, 0, wxALIGN_CENTER);
//    buttonPane->Add( 1, 1, wxEXPAND );

    sizer->Add (new wxStaticLine(panel, -1, wxDefaultPosition, wxSize(350,-1)), 0, wxEXPAND | wxALL, 10);
    sizer->Add (buttonPane, 0, wxALIGN_CENTER );

    return (sizer);
}

// ---------------------------------------------------------------------------
// event tables
// ---------------------------------------------------------------------------

BEGIN_EVENT_TABLE(OptionsDialog, wxDialog)
    // Controls
    EVT_BUTTON(wxID_APPLY, OptionsDialog::OnApply)
    EVT_BUTTON(wxID_OK, OptionsDialog::OnOK)
    EVT_BUTTON(wxID_CANCEL, OptionsDialog::OnCancel)

    EVT_TEXT(ID_OPTIONS_DATADIR, OptionsDialog::OnOptionsChangeText)
    EVT_TEXT(ID_OPTIONS_HTMLINDEX, OptionsDialog::OnOptionsChangeText)
    EVT_TEXT(ID_OPTIONS_HTMLBG, OptionsDialog::OnOptionsChangeText)
    EVT_TEXT(ID_OPTIONS_HTMLLINKCOLOR, OptionsDialog::OnOptionsChangeText)
    EVT_TEXT(ID_OPTIONS_HTMLVLINKCOLOR, OptionsDialog::OnOptionsChangeText)
    EVT_TEXT(ID_OPTIONS_HTMLBGCOLOR, OptionsDialog::OnOptionsChangeText)
    EVT_TEXT(ID_OPTIONS_HTMLTEXTCOLOR, OptionsDialog::OnOptionsChangeText)

END_EVENT_TABLE()

OptionsDialog::OptionsDialog (wxWindow* parent, long style)
    : wxDialog(parent, -1, "Options",
                    wxPoint(10,10), wxSize(500,550),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    CenterOnParent(); // Centers on the parent window

    OptionsDialogCreate();
    OptionsDialogInitialize();
}

OptionsDialog::~OptionsDialog( )
{
    Destroy();
}

void OptionsDialog::OptionsDialogCreate ()
{
//  Create a dialog that will be used to display and edit the options

    m_pOptionsPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxTAB_TRAVERSAL, "Options Panel");

    wxBoxSizer *pTopSizer = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *pItem01 = new wxBoxSizer( wxVERTICAL );

// Data Directory section
    wxBoxSizer *pItemSizer01 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer01->Add(new wxStaticText(m_pOptionsPanel, wxID_ANY,
        _("Data Dir:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextDataDir = new wxTextCtrl(m_pOptionsPanel, ID_OPTIONS_DATADIR,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizer01->Add( m_pTextDataDir, 0, wxALIGN_LEFT|wxLEFT, 0 );

// HTML Index section
    wxBoxSizer *pItemSizer02 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer02->Add(new wxStaticText(m_pOptionsPanel, wxID_ANY,
        _("HTML Index:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextHTMLIndex = new wxTextCtrl(m_pOptionsPanel, ID_OPTIONS_HTMLINDEX,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizer02->Add( m_pTextHTMLIndex, 0, wxALIGN_LEFT|wxLEFT, 0 );

// HTML BackGround section
    wxBoxSizer *pItemSizer03 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer03->Add(new wxStaticText(m_pOptionsPanel, wxID_ANY,
        _("HTML Background Image:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextHTMLBG = new wxTextCtrl(m_pOptionsPanel, ID_OPTIONS_HTMLBG,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizer03->Add( m_pTextHTMLBG, 0, wxALIGN_LEFT|wxLEFT, 0 );

// HTML LinkColor section
    wxBoxSizer *pItemSizer04 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer04->Add(new wxStaticText(m_pOptionsPanel, wxID_ANY,
        _("HTML Link Color:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextHTMLLinkColor = new wxTextCtrl(m_pOptionsPanel, ID_OPTIONS_HTMLLINKCOLOR,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizer04->Add( m_pTextHTMLLinkColor, 0, wxALIGN_LEFT|wxLEFT, 0 );

// HTML VLinkColor section
    wxBoxSizer *pItemSizer05 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer05->Add(new wxStaticText(m_pOptionsPanel, wxID_ANY,
        _("HTML VLink Color:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextHTMLVLinkColor = new wxTextCtrl(m_pOptionsPanel, ID_OPTIONS_HTMLVLINKCOLOR,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizer05->Add( m_pTextHTMLVLinkColor, 0, wxALIGN_LEFT|wxLEFT, 0 );

// HTML Background Color section
    wxBoxSizer *pItemSizer06 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer06->Add(new wxStaticText(m_pOptionsPanel, wxID_ANY,
        _("HTML Background Color:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextHTMLBGColor = new wxTextCtrl(m_pOptionsPanel, ID_OPTIONS_HTMLBGCOLOR,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizer06->Add( m_pTextHTMLBGColor, 0, wxALIGN_LEFT|wxLEFT, 0 );

// HTML Text Color section
    wxBoxSizer *pItemSizer07 = new wxBoxSizer( wxHORIZONTAL );

    pItemSizer07->Add(new wxStaticText(m_pOptionsPanel, wxID_ANY,
        _("HTML Background Color:")), 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    m_pTextHTMLTextColor = new wxTextCtrl(m_pOptionsPanel, ID_OPTIONS_HTMLBGCOLOR,
                    _T(""), wxDefaultPosition, wxSize(250,-1) );
    pItemSizer07->Add( m_pTextHTMLTextColor, 0, wxALIGN_LEFT|wxLEFT, 0 );

// Add each line item to the panel
    pItem01->Add( pItemSizer01, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer02, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer03, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer04, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer05, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer06, 0, wxALIGN_LEFT|wxLEFT, 0 );
    pItem01->Add( pItemSizer07, 0, wxALIGN_LEFT|wxLEFT, 0 );

// Create and place Control buttons
    pItem01->Add ( BuildControlButtons(m_pOptionsPanel),
                    0, wxALIGN_CENTER | wxEXPAND);

    pTopSizer->Add( pItem01, 1, wxGROW|wxALIGN_CENTRE|wxALL, 5 );
    pTopSizer->AddSpacer(5);

    m_pOptionsPanel->SetSizer(pTopSizer);
    pTopSizer->Fit(m_pOptionsPanel);
}

void OptionsDialog::OptionsDialogInitialize ()
{
// Set flag to show that we are setting the initial values
    m_bSetValueFlagOptions = TRUE;

// Clear the Change Options Flag
    m_bChangeOptionsFlag = FALSE;

// Retrieve the global options variables and place in the panel
    m_pTextDataDir->SetValue(wxGetApp().g_DataDir);
    m_pTextHTMLIndex->SetValue(wxGetApp().g_HTML_Index);
    m_pTextHTMLBG->SetValue(wxGetApp().g_HTML_BG);
    m_pTextHTMLLinkColor->SetValue(wxGetApp().g_HTML_LinkColor);
    m_pTextHTMLVLinkColor->SetValue(wxGetApp().g_HTML_VLinkColor);
    m_pTextHTMLBGColor->SetValue(wxGetApp().g_HTML_BGColor);
    m_pTextHTMLTextColor->SetValue(wxGetApp().g_HTML_TextColor);

// Clear the flag since we are done setting initial values
    m_bSetValueFlagOptions = FALSE;
}

wxBoxSizer* OptionsDialog::BuildControlButtons( wxWindow* panel )
{
    // Create the control buttons (Apply, New, Add, OK, and Cancel)

    wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *buttonPane = new wxBoxSizer (wxHORIZONTAL);
    //  By adding a space with wxExpand (non zero),
    //    the butons will be positioned correctly
    wxButton *okButton = new wxButton (panel, wxID_OK);
    okButton->SetDefault();
    buttonPane->Add (okButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );

    wxButton *applyButton = new wxButton (panel, wxID_APPLY);
    buttonPane->Add (applyButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );

    wxButton *cancelButton = new wxButton (panel, wxID_CANCEL);
    buttonPane->Add (cancelButton, 0, wxALIGN_CENTER);
    buttonPane->Add( 1, 1, wxEXPAND );

    sizer->Add (new wxStaticLine(panel, -1), 0, wxEXPAND | wxALL, 10);
    sizer->Add (buttonPane, 0, wxALIGN_CENTER | wxEXPAND);
    // Add a space under the buttons
    sizer->Add( 10, 10, wxSTRETCH_NOT );

    return (sizer);
}

void OptionsDialog::OnApply(wxCommandEvent& event)
{
//    wxMessageBox(_T("Apply Button Pressed"),
//                 _T("Apply"), wxOK|wxICON_INFORMATION );
// Save the data

// Retrieve the global options variables and place in the global area
    wxGetApp().g_DataDir = m_pTextDataDir->GetValue();
    wxGetApp().g_HTML_Index = m_pTextHTMLIndex->GetValue();
    wxGetApp().g_HTML_BG = m_pTextHTMLBG->GetValue();
    wxGetApp().g_HTML_LinkColor = m_pTextHTMLLinkColor->GetValue();
    wxGetApp().g_HTML_VLinkColor = m_pTextHTMLVLinkColor->GetValue();
    wxGetApp().g_HTML_BGColor = m_pTextHTMLBGColor->GetValue();
    wxGetApp().g_HTML_TextColor = m_pTextHTMLTextColor->GetValue();

    m_bChangeOptionsFlag = FALSE;

    EndModal(wxID_APPLY); // End with a RC of APPLY
}

void OptionsDialog::OnOK(wxCommandEvent& event)
{
//    wxMessageBox(_T("OK Button Pressed"),
//                 _T("OK"), wxOK|wxICON_INFORMATION );
    // If the change flag is TRUE, Ask if updates should be dropped.
    if ( m_bChangeOptionsFlag )
    {
        int answer = wxMessageBox(
                (_T("       A field has changed.\n")
                 _T("Press Yes to save or No to discard")
                ),
                _T("Field Changed"), wxYES_NO|wxICON_QUESTION );
        if ( answer == wxYES)
        {
            //Save the data
            m_bChangeOptionsFlag = FALSE;
            OnApply(event);
        }
    }
    else
    {
    }
    // Continue and process this event normally.
    event.Skip(TRUE);
}

void OptionsDialog::OnCancel(wxCommandEvent& event)
{
//    wxMessageBox(_T("Cancel Button Pressed"),
//                 _T("Cancel"), wxOK|wxICON_INFORMATION );
    // Continue and process this event normally.
    event.Skip(TRUE);
}

void OptionsDialog::OnOptionsChangeText(wxCommandEvent& event)
{
    // Unfortunatly, this event is triggered even with a SetValue.
    // m_bSetValueFlag is set to TRUE during initialization when
    // the SetValues are issued and then cleared to FALSE.
    if ( !m_bSetValueFlagOptions )
    {
//        wxMessageBox(_T("Data Changed on Options Panel"),
//            _T("Data"), wxOK|wxICON_INFORMATION );
        m_bChangeOptionsFlag = TRUE;
    }
}
