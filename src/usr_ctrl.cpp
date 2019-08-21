//----------------------------------------------------------------------//
// Functions for Control												//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <memory>
#include <algorithm>
#include "usr_str.h"
#include "usr_ctrl.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
int get_FontHeight(TFont *font, int mgn)
{
	int fh = abs(font->Height) + mgn;
	HWND hWnd = Application->MainForm? Application->MainForm->Handle : NULL;
	HDC dc = ::GetDC(hWnd);
	if (dc) {
		try {
			std::unique_ptr<TCanvas> cv(new TCanvas());
			cv->Handle = dc;
			cv->Font->Assign(font);
			fh = cv->TextHeight("Q") + mgn;
		}
		__finally {
			::ReleaseDC(hWnd, dc);
		}
	}

	return fh;
}

//---------------------------------------------------------------------------
bool has_Leading(TCanvas *cv)
{
	return (cv->TextHeight("Q")>abs(cv->Font->Height));
}

//---------------------------------------------------------------------------
void set_ListBoxItemHi(TListBox *lp, double r)
{
	if (r<1.0) r = 1.0;
	lp->ItemHeight = get_FontHeight(lp->Font, abs(lp->Font->Height) * (r - 1.0) + 1);
}
//---------------------------------------------------------------------------
void set_ListBoxItemHi(TCheckListBox *lp, double r)
{
	if (r<1.0) r = 1.0;
	lp->ItemHeight = get_FontHeight(lp->Font, abs(lp->Font->Height) * (r - 1.0) + 1);
}
//---------------------------------------------------------------------------
void set_ListBoxItemHi(	TListBox *lp, TFont *font,  bool with_ico)
{
	if (font) {
		lp->Font->Assign(font);
		lp->Canvas->Font->Assign(font);
	}

	lp->ItemHeight = std::max(get_FontHeight(lp->Font, abs(lp->Font->Height) / 3.0 + 1), with_ico? 20 : 0);
}
//---------------------------------------------------------------------------
void set_ListBoxInterLn(TListBox *lp, int d)
{
	lp->ItemHeight = get_FontHeight(lp->Font, d);
}


//---------------------------------------------------------------------------
void ListBoxSetIndex(TListBox *lp, int idx)
{
	if (lp->Count==0) return;

	if (idx<0) {
		lp->ItemIndex = 0;
	}
	else if (idx==lp->ItemIndex) {
		int pn = lp->ClientHeight/lp->ItemHeight;
		if (lp->ItemIndex<lp->TopIndex)
			lp->TopIndex = std::max(lp->ItemIndex - LISTBOX_SCRMGN, 0);
		else if (lp->ItemIndex > lp->TopIndex + pn - 1)
			lp->TopIndex = std::max(lp->ItemIndex - pn + 1 + LISTBOX_SCRMGN, 0);
	}
	else {
		if (idx>=lp->Count) idx = lp->Count - 1;
		if (idx<lp->ItemIndex) {
			if (idx < lp->TopIndex+LISTBOX_SCRMGN)
				lp->TopIndex = std::max(idx - LISTBOX_SCRMGN, 0);
		}
		else {
			int pn = lp->ClientHeight/lp->ItemHeight;
			if (idx > (lp->TopIndex+pn - 1 - LISTBOX_SCRMGN))
				lp->TopIndex = std::max(idx - pn + 1 + LISTBOX_SCRMGN, 0);
		}
		lp->ItemIndex = idx;
	}
}
//---------------------------------------------------------------------------
void ListBoxCursorDown(TListBox *lp)
{
	int idx = lp->ItemIndex + 1;
	if (idx<lp->Count) ListBoxSetIndex(lp, idx);
}
//---------------------------------------------------------------------------
void ListBoxCursorDown(TListBox *lp, int n)
{
	ListBoxSetIndex(lp, std::min(lp->ItemIndex + n, lp->Count - 1));
}
//---------------------------------------------------------------------------
void ListBoxCursorUp(TListBox *lp)
{
	int idx = lp->ItemIndex - 1;
	if (idx>=0) ListBoxSetIndex(lp, idx);
}
//---------------------------------------------------------------------------
void ListBoxCursorUp(TListBox *lp, int n)
{
	ListBoxSetIndex(lp, std::max(lp->ItemIndex - n, 0));
}
//---------------------------------------------------------------------------
void ListBoxPageDown(TListBox *lp)
{
	ListBoxCursorDown(lp, lp->ClientHeight/lp->ItemHeight - 1);
}
//---------------------------------------------------------------------------
void ListBoxPageUp(TListBox *lp)
{
	ListBoxCursorUp(lp, lp->ClientHeight/lp->ItemHeight - 1);
}

//---------------------------------------------------------------------------
//Draw Cursor Line of ListBox
//---------------------------------------------------------------------------
void draw_ListCursor(TListBox *lp, TRect &Rect, int Index, TOwnerDrawState State,
	TColor col, int lw)
{
	bool is_focused = State.Contains(odFocused);
	if (Index==lp->ItemIndex && is_focused) {
		int yp = Rect.Bottom - lw;
		TCanvas *cv = lp->Canvas;
		cv->Pen->Mode  = pmCopy;
		cv->Pen->Style = psSolid;
		cv->Pen->Color = col;
		cv->Pen->Width = lw;
		cv->MoveTo(Rect.Left, yp);
		cv->LineTo(Rect.Right, yp);
	}

	if (is_focused) lp->Canvas->DrawFocusRect(Rect);
}
//---------------------------------------------------------------------------
//Draw Cursor Line of SgringGrid
//---------------------------------------------------------------------------
void draw_GridCursor(TStringGrid *gp, TRect &Rect, int ARow, TGridDrawState State,
	TColor col, int lw)
{
	if (gp->Row == ARow) {
		int yp = Rect.Bottom - lw;
		TCanvas *cv = gp->Canvas;
		cv->Pen->Mode  = pmCopy;
		cv->Pen->Style = psSolid;
		cv->Pen->Color = col;
		cv->Pen->Width = lw;
		cv->MoveTo(Rect.Left, yp);
		cv->LineTo(Rect.Right + 1, yp);
	}
}

//---------------------------------------------------------------------------
void draw_Separator(
	TCanvas *cv, TRect rc,
	TColor bg)	//îwåiêF	(default = clNone : cv->Brush->Color)
{
	int yp = rc.Top + rc.Height()/2;
	int x0 = rc.Left  + 4;
	int x1 = rc.Right - 4;

	int c = ColorToRGB((bg==clNone)? cv->Brush->Color : bg);
	double v = ((GetRValue(c)*0.3 + GetGValue(c)*0.59 + GetBValue(c)*0.11) / 255.0);

	TColor c0 = TColor((v>0.5)? RGB(0x77, 0x77, 0x77) : RGB(0x33, 0x33, 0x33));	//***
	TColor c1 = TColor((v>0.5)? RGB(0xdd, 0xdd, 0xdd) : RGB(0x99, 0x99, 0x99));	//***

	cv->Pen->Style = psSolid;
	cv->Pen->Width = 1;
	cv->Pen->Color = c0;	cv->MoveTo(x0, yp);  cv->LineTo(x1, yp++);
	cv->Pen->Color = c1;	cv->MoveTo(x0, yp);  cv->LineTo(x1, yp);
}

//---------------------------------------------------------------------------
void str_to_GridHeadder(TStringGrid *gp, UnicodeString s,
	UnicodeString sp)	//ãÊêÿÇË (default = "|")
{
	for (int i=0; i<gp->ColCount; i++) {
		UnicodeString itm = get_tkn(s, sp);
		if (itm.IsEmpty()) break;
		gp->Cells[i][0] = itm;
		s = get_tkn_r(s, sp);
	}
}
//---------------------------------------------------------------------------
void set_GridWdHdr(TStringGrid *gp, int wd, int dtmax)
{
	if (wd==0) wd = 20;
	gp->ColCount = wd + 1;
	gp->RowCount = (int)(ceil(1.0*dtmax/wd) + 1);

	for (int i=1; i<gp->ColCount; i++) gp->Cells[i][0] = IntToStr(i);
	for (int i=1; i<gp->RowCount; i++) gp->Cells[0][i] = IntToStr((i - 1)*(gp->ColCount - 1));
}

//---------------------------------------------------------------------------
void set_GridScrBar(TStringGrid *gp)
{
	int invisi_rowcnt = gp->RowCount - gp->FixedRows -gp->VisibleRowCount;
	if (invisi_rowcnt>0 && gp->VisibleRowCount>0) {
		SCROLLINFO si_v0;
		::ZeroMemory(&si_v0, sizeof(SCROLLINFO));
		si_v0.cbSize = sizeof(SCROLLINFO);
		si_v0.fMask  = SIF_RANGE | SIF_PAGE;
		::GetScrollInfo(gp->Handle, SB_VERT, &si_v0);
		SCROLLINFO si_v1 = si_v0;

		int max0 = 127;
		si_v1.nPage = max0 * gp->VisibleRowCount / invisi_rowcnt;
		if (si_v1.nPage>127) si_v1.nPage = 127;
		int thumb_sz = (gp->ClientHeight - ::GetSystemMetrics(SM_CYVSCROLL)) * si_v1.nPage / (127 + si_v1.nPage);
		int thumb_min = ::GetSystemMetrics(SM_CXVSCROLL);
		if (thumb_sz<thumb_min) {
			si_v1.nMax = 127; si_v1.nPage = 0;
		}
		else {
			si_v1.nMax = max0 + si_v1.nPage - 1;
		}

		if (si_v0.nPage!=si_v1.nPage || si_v0.nMax!=si_v1.nMax) ::SetScrollInfo(gp->Handle, SB_VERT, &si_v1, TRUE);
	}

	int invisi_colcnt = gp->ColCount - gp->FixedCols -gp->VisibleColCount;
	if (invisi_colcnt>0) {
		SCROLLINFO si_h0;
		::ZeroMemory(&si_h0, sizeof(SCROLLINFO));
		si_h0.cbSize = sizeof(SCROLLINFO);
		si_h0.fMask  = SIF_RANGE | SIF_PAGE;
		::GetScrollInfo(gp->Handle, SB_HORZ, &si_h0);
		SCROLLINFO si_h1 = si_h0;

		int max0 = 127;
		si_h1.nPage = max0 * gp->VisibleColCount / invisi_colcnt;
		if (si_h1.nPage>127) si_h1.nPage = 127;
		int thumb_sz = (gp->ClientHeight - ::GetSystemMetrics(SM_CXHSCROLL)) * si_h1.nPage / (127 + si_h1.nPage);
		int thumb_min = ::GetSystemMetrics(SM_CYHSCROLL);
		if (thumb_sz<thumb_min) {
			si_h1.nMax = 127; si_h1.nPage = 0;
		}
		else {
			si_h1.nMax = max0 + si_h1.nPage - 1;
		}

		if (si_h0.nPage!=si_h1.nPage || si_h0.nMax!=si_h1.nMax) ::SetScrollInfo(gp->Handle, SB_HORZ, &si_h1, TRUE);
	}
}

//---------------------------------------------------------------------------
int __fastcall sort_comp_Str(TStringList *List, int Index1, int Index2)
{
	UnicodeString s1 = List->Strings[Index1];
	UnicodeString s2 = List->Strings[Index2];

	if (s1.IsEmpty() && s2.IsEmpty()) return 0;
	if (s1.IsEmpty()) return  1;
	if (s2.IsEmpty()) return -1;

	return CompareStr(s1, s2);
}
//---------------------------------------------------------------------------
int __fastcall sort_comp_Str_Rev(TStringList *List, int Index1, int Index2)
{
	return -sort_comp_Str(List, Index1, Index2);
}
//---------------------------------------------------------------------------
int __fastcall sort_comp_Number(TStringList *List, int Index1, int Index2)
{
	return (List->Strings[Index1].ToIntDef(0) - List->Strings[Index2].ToIntDef(0));
}
//---------------------------------------------------------------------------
int __fastcall sort_comp_Number_Rev(TStringList *List, int Index1, int Index2)
{
	return (List->Strings[Index2].ToIntDef(0) - List->Strings[Index1].ToIntDef(0));
}

//---------------------------------------------------------------------------
void sort_GridCol(
	TStringGrid *gp,
	int sort_col,
	bool rev_sw,
	bool num_sw)
{
	std::unique_ptr<TStringList> lst(new TStringList());
	lst->Assign(gp->Cols[sort_col]);

	int fix_rows = gp->FixedRows;
	for (int i=0; i<fix_rows; i++) lst->Delete(0);
	for (int i=fix_rows; i<gp->RowCount; i++) {
		TStringList *St2 = new TStringList();
		St2->Assign(gp->Rows[i]);
		lst->Objects[i - fix_rows] = St2;
	}

	if (num_sw) {
		if (rev_sw)
			lst->CustomSort(sort_comp_Number_Rev);
		else
			lst->CustomSort(sort_comp_Number);
	}
	else {
		if (rev_sw)
			lst->CustomSort(sort_comp_Str_Rev);
		else
			lst->CustomSort(sort_comp_Str);
	}

	for (int i=fix_rows; i<gp->RowCount; i++) {
		TStringList *lp = (TStringList*)lst->Objects[i - fix_rows];
		gp->Rows[i]->Assign(lp);
		delete lp;
	}
}
//---------------------------------------------------------------------------
void sort_GridCol_Custom(
	TStringGrid *gp,
	int sort_col,
	TStringListSortCompare compfunc)
{
	std::unique_ptr<TStringList> lst(new TStringList());
	lst->Assign(gp->Cols[sort_col]);

	int fix_rows = gp->FixedRows;
	for (int i=0; i<fix_rows; i++) lst->Delete(0);
	for (int i=fix_rows; i<gp->RowCount; i++) {
		TStringList *St2 = new TStringList();
		St2->Assign(gp->Rows[i]);
		lst->Objects[i - fix_rows] = St2;
	}

	lst->CustomSort(compfunc);

	for (int i=fix_rows; i<gp->RowCount; i++) {
		TStringList *lp = (TStringList*)lst->Objects[i - fix_rows];
		gp->Rows[i]->Assign(lp);
		delete lp;
	}
}

//---------------------------------------------------------------------------
void str_to_ComboBox(TComboBox *cp, UnicodeString s,
	UnicodeString sp)	//(default = "|")
{
	for (;;) {
		UnicodeString itm = get_tkn(s, sp);
		if (itm.IsEmpty()) break;
		cp->Items->Add(itm);
		s = get_tkn_r(s, sp);
	}
}

//---------------------------------------------------------------------------
void add_ComboBox_history(TComboBox *cp, UnicodeString kwd)
{
	if (!kwd.IsEmpty()) {
		TStrings *lp = cp->Items;
		int idx = lp->IndexOf(kwd);
		if (idx!=0) {
			if (idx>0) lp->Delete(idx);
			lp->Insert(0, kwd);	//êÊì™Ç…í«â¡
		}
		cp->Text = kwd;
	}
}
//---------------------------------------------------------------------------
void del_ComboBox_history(TComboBox *cp, UnicodeString kwd)
{
	if (!kwd.IsEmpty()) {
		TStrings *lp = cp->Items;
		int idx = lp->IndexOf(kwd);
		if (idx!=-1) lp->Delete(idx);
		cp->Text = EmptyStr;
	}
}
//---------------------------------------------------------------------------
