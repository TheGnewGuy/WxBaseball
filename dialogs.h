/////////////////////////////////////////////////////////////////////////////
// Name:        dialogs.h                                                  //
// Purpose:     Baseball statistics tracking tool for Strat-O-Matic        //
// Author:      John Gnew                                                  //
// Created:     12/14/04                                                   //
// Copyright:   (c) John Gnew                                              //
// Modifications:                                                          //
//   Date       Description                                                //
// 08/31/09     Added OptionsDialog                                        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// Todo:                                                                   //
/////////////////////////////////////////////////////////////////////////////
#ifndef _DIALOGS_H_
#define _DIALOGS_H_

#ifndef _WX_NOTEBOOK_H_BASE_
    #include <wx/notebook.h>
#endif // _WX_NOTEBOOK_H_BASE_

#ifndef _WX_SPINCTRL_H_
    #include <wx/spinctrl.h>
#endif // _WX_SPINCTRL_H_

#include "wx/treelist.h"
#include "wx/artprov.h"
#include <wx/log.h>
#include "wx/regex.h"

class BatterNotebook: public wxPanel
{
public:
    ~BatterNotebook ();
    BatterNotebook( wxDialog* frame, int x, int y, int w, int h );
    void CreateBatterComboBox( wxWindow* parent, int combobox );
    void CreateTeamComboBox( wxWindow* parent, int combobox );
    void OnApply( wxCommandEvent& event );
    void OnAdd( wxCommandEvent& event );
    void OnNew( wxCommandEvent& event );
    void OnOK( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnComboBatterSelect( wxCommandEvent& event );
    void OnComboTeamSelect( wxCommandEvent& event );
    void OnPageChanged( wxNotebookEvent& event );
    void OnComboPositionChangeSpin( wxSpinEvent& event );
    void OnComboPositionChangeText( wxCommandEvent& event);
    void OnComboChanceChange( wxCommandEvent& event );
    void OnComboStatsChangeSpin( wxSpinEvent& event );
    void OnComboStatsChangeText( wxCommandEvent& event );
    void OnComboInfoChange( wxCommandEvent& event );
    void MakeBatterUpdate();
//    void OnPageChanging( wxNotebookEvent& event );
    wxComboBox* m_combo_team;

private:
    void Initialization( );
    void DefaultPanelValues( );
	// Used to create the Batter Notebook and its four panels
	void CreateNotebook(int x, int y, int w, int h);
    wxBoxSizer* BuildControlButtons( wxWindow* parent );
    wxBoxSizer* BuildTopCombos( wxWindow* parent, int combobox );
    bool GetTeamNamesArray();
    bool UpdateBatterNames();
    void GetNotebookData();

    wxBoxSizer* m_sizerBatter;
    wxSizer* m_sizerBatterSave;

    wxBoxSizer* sizer_01;
    wxBoxSizer* sizer_02;
    wxBoxSizer* sizer_03;
    wxBoxSizer* sizer_04;

    wxNotebook* m_pNotebook;
    wxPanel* m_panel_01;
    wxPanel* m_panel_02;
    wxPanel* m_panel_03;
    wxPanel* m_panel_04;

    wxComboBox* m_combo_batter_01;
    wxComboBox* m_combo_batter_02;
    wxComboBox* m_combo_batter_03;
    wxComboBox* m_combo_batter_04;
//    wxComboBox* m_combo_team_01;
//    wxComboBox* m_combo_team_02;
//    wxComboBox* m_combo_team_03;
//    wxComboBox* m_combo_team_04;
    wxTextCtrl* m_text_team_01;
    wxTextCtrl* m_text_team_02;
    wxTextCtrl* m_text_team_03;
    wxTextCtrl* m_text_team_04;

    // Spin controls for Position Notebook Page
    wxSpinCtrl* m_spinPosition1;
    wxSpinCtrl* m_spinPosition2;
    wxSpinCtrl* m_spinPosition3;
    wxSpinCtrl* m_spinPosition4;
    wxSpinCtrl* m_spinPosition5;
    wxSpinCtrl* m_spinPosition6;
    wxSpinCtrl* m_spinPosition7;
    wxSpinCtrl* m_spinPosition8;
    wxSpinCtrl* m_spinPosition9;
    wxSpinCtrl* m_spinPositionER1;
    wxSpinCtrl* m_spinPositionER2;
    wxSpinCtrl* m_spinPositionER3;
    wxSpinCtrl* m_spinPositionER4;
    wxSpinCtrl* m_spinPositionER5;
    wxSpinCtrl* m_spinPositionER6;
    wxSpinCtrl* m_spinPositionER7;
    wxSpinCtrl* m_spinPositionER8;
    wxSpinCtrl* m_spinPositionER9;

    // Text controls for Chance Page
    wxTextCtrl* m_textChanceBasic;
    wxTextCtrl* m_textChanceLeft;
    wxTextCtrl* m_textChanceRight;
    wxTextCtrl* m_textChanceB1B;
    wxTextCtrl* m_textChanceB2B;
    wxTextCtrl* m_textChanceB3B;
    wxTextCtrl* m_textChanceBHR;
    wxTextCtrl* m_textChanceBW;
    wxTextCtrl* m_textChanceBDP;
    wxTextCtrl* m_textChanceL1B;
    wxTextCtrl* m_textChanceL2B;
    wxTextCtrl* m_textChanceL3B;
    wxTextCtrl* m_textChanceLHR;
    wxTextCtrl* m_textChanceLW;
    wxTextCtrl* m_textChanceLDP;
    wxTextCtrl* m_textChanceR1B;
    wxTextCtrl* m_textChanceR2B;
    wxTextCtrl* m_textChanceR3B;
    wxTextCtrl* m_textChanceRHR;
    wxTextCtrl* m_textChanceRW;
    wxTextCtrl* m_textChanceRDP;

    // Spin controls for Statistics Page
    wxSpinCtrl* m_spinStats2B;
    wxSpinCtrl* m_spinStats3B;
    wxSpinCtrl* m_spinStatsHR;
    wxSpinCtrl* m_spinStatsAB;
    wxSpinCtrl* m_spinStatsHITS;
    wxSpinCtrl* m_spinStatsRBI;
    wxSpinCtrl* m_spinStatsRE;
    wxSpinCtrl* m_spinStatsRUNS;
    wxSpinCtrl* m_spinStatsSACRICIFE;
    wxSpinCtrl* m_spinStatsSB;
    wxSpinCtrl* m_spinStatsCS;
    wxSpinCtrl* m_spinStatsK;
    wxSpinCtrl* m_spinStatsW;
    wxSpinCtrl* m_spinStatsHBP;
    wxSpinCtrl* m_spinStatsGAMES;
    wxTextCtrl* m_textStatsAVG;
    wxTextCtrl* m_textStatsSLG;
    wxTextCtrl* m_textStatsOBP;

    // ComboBox controls for Info Page
    wxTextCtrl* m_textFirstName;
    wxTextCtrl* m_textLastName;
    wxComboBox* m_combo_BatterHits;
    wxComboBox* m_combo_PowerLeft;
    wxComboBox* m_combo_PowerRight;
    wxComboBox* m_combo_OutfieldArm;
    wxComboBox* m_combo_CatcherArm;
    wxComboBox* m_combo_Passball;
    wxComboBox* m_combo_Stealing;
    wxComboBox* m_combo_Bunting;
    wxComboBox* m_combo_HitRun;
    wxComboBox* m_combo_Running;
    wxComboBox* m_combo_TRating;

    wxArrayString m_arrayLRS;
    wxArrayString m_arrayPower;
    wxArrayString m_arrayOutfieldArm;
    wxArrayString m_arrayCatcherArm;
    wxArrayString m_arrayPassball;
    wxArrayString m_arrayStealing;
    wxArrayString m_arrayBunting;
    wxArrayString m_arrayHitRun;
    wxArrayString m_arrayRunning;
    wxArrayString m_arrayTRating;

    // General varables available to class
    wxArrayString m_arrayTeamNamesAndFiles;
    // Name of League we are using
    wxString m_strLeagueDirName;
    // Team File name that we are working with - TB123456.dat and TP123456.dat
    wxString m_strTeamFileNameBatter;
    wxString m_strTeamFileNamePitcher;
    // 40 characters for the Team name
    wxArrayString m_arrayTeamNames;
    // The 12 characters of the file name xx000001.dat
    wxArrayString m_arrayTeamFiles;
    wxArrayString m_arrayBattersOrig;
    wxArrayString m_arrayBatters;
    // Change Flags
    bool m_bSetValueFlagBatter;
    bool m_bChangePositionFlag;
    bool m_bChangeChanceFlag;
    bool m_bChangeStatsFlag;
    bool m_bChangeInfoFlag;

    DECLARE_EVENT_TABLE()
};

class BatterDialog: public wxDialog
{
public:
    ~BatterDialog ();
    BatterDialog( wxWindow* parent, long style = 0 );

private:
//    DECLARE_EVENT_TABLE()
};

class PitcherNotebook: public wxPanel
{
public:
    ~PitcherNotebook ();
    PitcherNotebook( wxDialog* frame, int x, int y, int w, int h );
    void CreatePitcherComboBox( wxWindow* parent, int combobox );
    void CreateTeamComboBox( wxWindow* parent, int combobox );
    wxComboBox* m_combo_team;
    void OnPageChanged( wxNotebookEvent& event );
    void OnApply( wxCommandEvent& event );
    void OnAdd( wxCommandEvent& event );
    void OnNew( wxCommandEvent& event );
    void OnOK( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnComboPitcherSelect( wxCommandEvent& event );
    void OnComboTeamSelect( wxCommandEvent& event );
    void OnComboChanceChange( wxCommandEvent& event );
    void OnComboStatsChangeSpin( wxSpinEvent& event );
    void OnComboStatsChangeText( wxCommandEvent& event );
    void OnComboInfoChange( wxCommandEvent& event );
    void MakePitcherUpdate();

private:
    void Initialization( );
    void DefaultPanelValues( );
	// Used to create the Batter Notebook and its four panels
	void CreateNotebook(int x, int y, int w, int h);
    bool GetTeamNamesArray();
    bool UpdatePitcherNames();
    wxBoxSizer* BuildControlButtons( wxWindow* parent );
    wxBoxSizer* BuildTopCombos( wxWindow* parent, int combobox );
    void GetNotebookData();

    wxBoxSizer* m_sizerPitcher;
    wxSizer* m_sizerPitcherSave;

    wxBoxSizer* m_sizer;

    wxBoxSizer* sizer_01;
    wxBoxSizer* sizer_02;
    wxBoxSizer* sizer_03;

    wxNotebook* m_pNotebook;
    wxPanel* m_panel_01;
    wxPanel* m_panel_02;
    wxPanel* m_panel_03;

    wxComboBox* m_combo_pitcher_01;
    wxComboBox* m_combo_pitcher_02;
    wxComboBox* m_combo_pitcher_03;
//    wxComboBox* m_combo_team_01;
//    wxComboBox* m_combo_team_02;
//    wxComboBox* m_combo_team_03;
    wxTextCtrl* m_text_team_01;
    wxTextCtrl* m_text_team_02;
    wxTextCtrl* m_text_team_03;

    wxTextCtrl* m_textFirstName;
    wxTextCtrl* m_textLastName;
    wxComboBox* m_combo_InfoStarter;
    wxComboBox* m_combo_InfoRelief;
    wxComboBox* m_combo_InfoBalk;
    wxComboBox* m_combo_InfoP;
    wxComboBox* m_combo_InfoERP;
    wxComboBox* m_combo_InfoWP;
    wxComboBox* m_combo_InfoThrows;
    wxComboBox* m_combo_InfoBunting;
    wxComboBox* m_combo_InfoHold;

    // Text controls for Chance Page
    wxTextCtrl* m_textChanceBasic;
    wxTextCtrl* m_textChanceLeft;
    wxTextCtrl* m_textChanceRight;
    wxTextCtrl* m_textChanceB1B;
    wxTextCtrl* m_textChanceB2B;
    wxTextCtrl* m_textChanceB3B;
    wxTextCtrl* m_textChanceBHR;
    wxTextCtrl* m_textChanceBW;
    wxTextCtrl* m_textChanceBDP;
    wxTextCtrl* m_textChanceL1B;
    wxTextCtrl* m_textChanceL2B;
    wxTextCtrl* m_textChanceL3B;
    wxTextCtrl* m_textChanceLHR;
    wxTextCtrl* m_textChanceLW;
    wxTextCtrl* m_textChanceLDP;
    wxTextCtrl* m_textChanceR1B;
    wxTextCtrl* m_textChanceR2B;
    wxTextCtrl* m_textChanceR3B;
    wxTextCtrl* m_textChanceRHR;
    wxTextCtrl* m_textChanceRW;
    wxTextCtrl* m_textChanceRDP;

    // Spin controls for Statistics Page
    wxSpinCtrl* m_spinStatsWins;
    wxSpinCtrl* m_spinStatsLoss;
    wxSpinCtrl* m_spinStatsStarts;
    wxSpinCtrl* m_spinStatsSave;
    wxTextCtrl* m_textStatsIP;
    wxSpinCtrl* m_spinStatsHits;
    wxSpinCtrl* m_spinStatsGames;
    wxSpinCtrl* m_spinStatsCGames;
    wxSpinCtrl* m_spinStatsW;
    wxSpinCtrl* m_spinStatsK;
    wxSpinCtrl* m_spinStatsHR;
    wxSpinCtrl* m_spinStatsER;
    wxTextCtrl* m_textStatsERA;
    wxTextCtrl* m_textStatsWHIP;

    wxArrayString m_array_InfoStarter;
    wxArrayString m_array_InfoRelief;
    wxArrayString m_array_InfoBalk;
    wxArrayString m_array_InfoP;
    wxArrayString m_array_InfoERP;
    wxArrayString m_array_InfoWP;
    wxArrayString m_array_InfoThrows;
    wxArrayString m_array_InfoBunting;
    wxArrayString m_array_InfoHold;

    // General varables available to class
    wxArrayString m_arrayTeamNames;

    // Change Flags
    bool m_bSetValueFlagPitcher;
    bool m_bChangeChanceFlag;
    bool m_bChangeStatsFlag;
    bool m_bChangeInfoFlag;

    DECLARE_EVENT_TABLE()
};

class PitcherDialog: public wxDialog
{
public:
    ~PitcherDialog ();
    PitcherDialog( wxWindow* parent, long style = 0 );
    wxPanel *m_pPitcherNotebook;

private:
//    DECLARE_EVENT_TABLE()
};

class LeagueDialogAdd: public wxDialog
{
public:
    ~LeagueDialogAdd ();
    LeagueDialogAdd( wxWindow* parent, long style = 0 );
    void LeagueDialogAddCreate();
    void OnADD( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnClose( wxCloseEvent& event );

private:
    wxBoxSizer* BuildControlButtons( wxWindow* parent );
    wxPanel *m_pLeaguePanel;
    wxTextCtrl *m_pTextLeagueName;
    wxTextCtrl *m_pTextLeagueYear;
    wxCheckBox *m_pCheckBoxLeagueBase;
    wxButton *m_pAddButton;
    wxButton *m_pApplyButton;
    wxButton *m_pCancelButton;

    DECLARE_EVENT_TABLE()
};

class ConferenceDialog: public wxDialog
{
public:
    ~ConferenceDialog ();
    ConferenceDialog( wxWindow* parent, long style = 0 );
    void ConferenceDialogCreate();
    void OnADD( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnClose( wxCloseEvent& event );

private:
    wxBoxSizer* BuildControlButtons( wxWindow* parent );
    wxPanel *m_pConferencePanel;
    wxTextCtrl *m_pTextLeagueName;
    wxTextCtrl *m_pTextLeagueYear;
    wxTextCtrl *m_pTextConference01;
    wxTextCtrl *m_pTextConference02;
    wxTextCtrl *m_pTextConference03;
    wxTextCtrl *m_pTextConference04;
    wxTextCtrl *m_pTextConference05;
    wxTextCtrl *m_pTextConference06;
    wxCheckBox *m_pCheckBoxLeagueBase;
    wxButton *m_pAddButton;
    wxButton *m_pApplyButton;
    wxButton *m_pCancelButton;

    DECLARE_EVENT_TABLE()
};

class DivisionDialog: public wxDialog
{
public:
    ~DivisionDialog ();
    DivisionDialog( wxWindow* parent, long style = 0 );
    void DivisionDialogCreate();
    void OnADD( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnClose( wxCloseEvent& event );

private:
    wxBoxSizer* BuildControlButtons( wxWindow* parent );
    wxPanel *m_pDivisionPanel;
    wxTextCtrl *m_pTextLeagueName;
    wxTextCtrl *m_pTextLeagueYear;
    wxTextCtrl *m_pTextDivision01;
    wxTextCtrl *m_pTextDivision02;
    wxTextCtrl *m_pTextDivision03;
    wxTextCtrl *m_pTextDivision04;
    wxTextCtrl *m_pTextDivision05;
    wxTextCtrl *m_pTextDivision06;
    wxCheckBox *m_pCheckBoxLeagueBase;
    wxButton *m_pAddButton;
    wxButton *m_pApplyButton;
    wxButton *m_pCancelButton;

    DECLARE_EVENT_TABLE()
};

class TeamDialogAdd: public wxDialog
{
public:
    ~TeamDialogAdd ();
    TeamDialogAdd( wxWindow* parent, long style = 0 );
    void TeamDialogCreate();
    void OnADD( wxCommandEvent& event );
    void OnUpdate( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnClose( wxCloseEvent& event );

private:
    wxBoxSizer* BuildControlButtons( wxWindow* parent );
    wxPanel *m_pTeamPanel;
    wxTextCtrl *m_pTextTeamName;
    wxTextCtrl *m_pTextTeamNameShort;
    wxTextCtrl *m_pTextTeamYear;
    wxTextCtrl *m_pTextBallpark;
    wxTextCtrl *m_pTextTotalWins;
    wxTextCtrl *m_pTextTotalLosses;
    wxTextCtrl *m_pTextHomeWins;
    wxTextCtrl *m_pTextHomeLosses;
    wxTextCtrl *m_pTextAwayWins;
    wxTextCtrl *m_pTextAwayLosses;
    wxTextCtrl *m_pTextConferenceName;
    wxTextCtrl *m_pTextDivisionName;
    wxTextCtrl *m_pTextLeagueName;
    wxTextCtrl *m_pTextLeagueYear;
    wxCheckBox *m_pCheckBoxTeamBase;
    wxButton *m_pAddButton;
    wxButton *m_pApplyButton;
    wxButton *m_pCancelButton;

    DECLARE_EVENT_TABLE()
};

class TeamDialogApply: public wxDialog
{
public:
    ~TeamDialogApply ();
    TeamDialogApply( wxWindow* parent, long style = 0 );
    void TeamDialogCreate();
    void OnApply( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnClose( wxCloseEvent& event );

private:
    wxBoxSizer* BuildControlButtons( wxWindow* parent );
    wxPanel *m_pTeamPanel;
    wxTextCtrl *m_pTextTeamName;
    wxTextCtrl *m_pTextTeamNameShort;
    wxTextCtrl *m_pTextTeamYear;
    wxTextCtrl *m_pTextBallpark;
    wxTextCtrl *m_pTextTotalWins;
    wxTextCtrl *m_pTextTotalLosses;
    wxTextCtrl *m_pTextHomeWins;
    wxTextCtrl *m_pTextHomeLosses;
    wxTextCtrl *m_pTextAwayWins;
    wxTextCtrl *m_pTextAwayLosses;
    wxTextCtrl *m_pTextConferenceName;
    wxTextCtrl *m_pTextDivisionName;
    wxTextCtrl *m_pTextLeagueName;
    wxTextCtrl *m_pTextLeagueYear;
    wxCheckBox *m_pCheckBoxTeamBase;
    wxButton *m_pAddButton;
    wxButton *m_pApplyButton;
    wxButton *m_pCancelButton;

    DECLARE_EVENT_TABLE()
};

class TeamDialogCreate: public wxDialog
{
public:
    ~TeamDialogCreate ();
    TeamDialogCreate( wxWindow* parent, long style = 0 );
    void TeamDialogCreateDLG();
    void OnApply( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnClose( wxCloseEvent& event );

private:

    // Create the image list, called once only. Should add images to it in the
    // same order as they appear in the enum above.
    void InitImageList();

    // Create the control with the given styles.
    wxTreeListCtrl* CreateTreeListCtrl(long style);
    wxTreeListCtrl* CreateTreeListCtrlOut(long style);
    void OnItemChecked(wxTreeListEvent& event);

    // Change Flags
    wxBoxSizer* BuildControlButtons( wxWindow* parent );
    wxPanel *m_pTeamPanel;
    wxButton *m_pAddButton;
    wxButton *m_pApplyButton;
    wxButton *m_pCancelButton;

    wxImageList* m_imageList;

    wxTreeListCtrl* m_treelist;
    wxTreeListCtrl* m_treelistOut;
    bool m_isFlat;

    // Helper: return the text of the item or "NONE" if the item is invalid.
    wxString DumpItem(wxTreeListItem item) const;

    // Another helper: just translate wxCheckBoxState to user-readable text.
    static const char* CheckedStateString(wxCheckBoxState state);

    enum
    {
        Icon_File,
        Icon_FolderClosed,
        Icon_FolderOpened
    };

// Tree list columns.
enum
{
    Col_Component,
    Col_Files,
    Col_Size
};

    DECLARE_EVENT_TABLE()
};

class OptionsDialog: public wxDialog
{
public:
    ~OptionsDialog ();
    OptionsDialog( wxWindow* parent, long style = 0 );
    void OptionsDialogCreate(  );
    void OptionsDialogInitialize(  );
    void OnApply( wxCommandEvent& event );
    void OnOK( wxCommandEvent& event );
    void OnCancel( wxCommandEvent& event );
    void OnOptionsChangeText( wxCommandEvent& event );

private:
    wxBoxSizer* BuildControlButtons( wxWindow* parent );

    bool m_bSetValueFlagOptions;
    bool m_bChangeOptionsFlag;
    wxPanel *m_pOptionsPanel;
    wxTextCtrl *m_pTextDataDir;
    wxTextCtrl *m_pTextHTMLIndex;
    wxTextCtrl *m_pTextHTMLBG;
    wxTextCtrl *m_pTextHTMLLinkColor;
    wxTextCtrl *m_pTextHTMLVLinkColor;
    wxTextCtrl *m_pTextHTMLBGColor;
    wxTextCtrl *m_pTextHTMLTextColor;

    DECLARE_EVENT_TABLE()
};

#endif // _DIALOGS_H_
