//----------------------------------------------------------------------//
// Functions for Control												//
//																		//
//----------------------------------------------------------------------//
#ifndef EtcUsrCtrlH
#define EtcUsrCtrlH

//---------------------------------------------------------------------------
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.CheckLst.hpp>

//---------------------------------------------------------------------------
int  get_FontHeight(TFont *font, int mgn = 0);
bool has_Leading(TCanvas *cv);

#define LISTBOX_SCRMGN	2

void set_ListBoxItemHi(TListBox *lp, double r);
void set_ListBoxItemHi(TCheckListBox *lp, double r);
void set_ListBoxItemHi(TListBox *lp, TFont *font = NULL, bool with_ico = false);
void set_ListBoxInterLn(TListBox *lp, int d);

void ListBoxSetIndex(TListBox *lp, int idx);
void ListBoxCursorDown(TListBox *lp);
void ListBoxCursorDown(TListBox *lp, int n);
void ListBoxCursorUp(TListBox *lp);
void ListBoxCursorUp(TListBox *lp, int n);
void ListBoxPageDown(TListBox *lp);
void ListBoxPageUp(TListBox *lp);

void draw_ListCursor(TListBox *lp, TRect &Rect, int Index, TOwnerDrawState State, TColor col, int lw);
void draw_GridCursor(TStringGrid *gp, TRect &Rect, int ARow, TGridDrawState State, TColor col, int lw);

void draw_Separator(TCanvas *cv, TRect rc, TColor bg = Graphics::clNone);

void str_to_GridHeadder(TStringGrid *gp, UnicodeString s, UnicodeString sp = "|");
void set_GridWdHdr(TStringGrid *gp, int wd, int dtmax);
void set_GridScrBar(TStringGrid *gp);

int  __fastcall sort_comp_Str(TStringList *List, int Index1, int Index2);
int  __fastcall sort_comp_Str_Rev(TStringList *List, int Index1, int Index2);
int  __fastcall sort_comp_Number(TStringList *List, int Index1, int Index2);
int  __fastcall sort_comp_Number_Rev(TStringList *List, int Index1, int Index2);
void sort_GridCol(TStringGrid *gp, int sort_col, bool rev_sw=false, bool num_sw=false);
void sort_GridCol_Custom(TStringGrid *gp, int sort_col, TStringListSortCompare compfunc);

void str_to_ComboBox(TComboBox *cp, UnicodeString s, UnicodeString sep = "|");
void add_ComboBox_history(TComboBox *cp, UnicodeString kwd);
void del_ComboBox_history(TComboBox *cp, UnicodeString kwd);

//---------------------------------------------------------------------------
#endif
