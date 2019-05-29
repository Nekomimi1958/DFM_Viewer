//----------------------------------------------------------------------//
// Environment															//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <System.StrUtils.hpp>

#include "usr_env.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
UnicodeString get_version_str(UnicodeString fnam)
{
	if (fnam.IsEmpty()) fnam = Application->ExeName;
	UnicodeString verstr;
	unsigned mj, mi, bl;
	if (GetProductVersion(fnam, mj, mi, bl)) verstr.sprintf(_T("%u.%u.%u.0"), mj,mi,bl);
	return verstr;
}
//---------------------------------------------------------------------------
int get_version_no()
{
	UnicodeString verstr = ReplaceStr(get_version_str(), ".", EmptyStr).SubString(1, 3);
	return verstr.ToIntDef(0);
}

//---------------------------------------------------------------------------
void load_form_pos(TForm *frm, TIniFile *ini_file, int w, int h)
{
	UnicodeString sct = "General";
	if (w>0) frm->Width  = ini_file->ReadInteger(sct, "WinWidth",	w);
	if (h>0) frm->Height = ini_file->ReadInteger(sct, "WinHeight",	h);
	frm->Left = ini_file->ReadInteger(sct, "WinLeft",	(Screen->Width - frm->Width)/2);
	frm->Top  = ini_file->ReadInteger(sct, "WinTop",	(Screen->Height - frm->Height)/2);
	if (Screen->MonitorCount==1) {
		if (frm->Left<0) frm->Left = 0;
		if (frm->Top<0)  frm->Top  = 0;
		if (frm->BoundsRect.Right>Screen->Width)   frm->Left = Screen->Width - frm->Width;
		if (frm->BoundsRect.Bottom>Screen->Height) frm->Top  = Screen->Height - frm->Height;
	}
	frm->WindowState = (TWindowState)ini_file->ReadInteger(sct, "WinState", (int)wsNormal);
}
//---------------------------------------------------------------------------
void save_form_pos(TForm *frm, TIniFile *ini_file)
{
	UnicodeString sct = "General";
	if (frm->WindowState==wsMinimized) frm->WindowState = wsNormal;
	ini_file->WriteInteger(sct, "WinState",	(int)frm->WindowState);
	if (frm->WindowState==wsMaximized) frm->WindowState = wsNormal;
	ini_file->WriteInteger(sct, "WinLeft",	frm->Left);
	ini_file->WriteInteger(sct, "WinTop",	frm->Top);
	ini_file->WriteInteger(sct, "WinWidth",	frm->Width);
	ini_file->WriteInteger(sct, "WinHeight",frm->Height);
}

//---------------------------------------------------------------------------
void load_pos_info(TForm *frm, TIniFile *ini_file, int x, int y, int w, int h)
{
	UnicodeString sct = "General";
	frm->Left = ini_file->ReadInteger(sct, frm->Name + "Left", x);
	frm->Top  = ini_file->ReadInteger(sct, frm->Name + "Top",  y);

	if (frm->BorderStyle!=bsDialog && w>0 && h>0) {
		int ww = ini_file->ReadInteger(sct, frm->Name + "Width",  w);
		if (ww<=0) ww = w;
		int wh = ini_file->ReadInteger(sct, frm->Name + "Height", h);
		if (wh<=0) wh = h;
		frm->Width	= ww;
		frm->Height = wh;
	}

	if (Screen->MonitorCount==1) {
		if (frm->Left<0) frm->Left = 0;
		if (frm->Top<0)  frm->Top  = 0;
		if (frm->BoundsRect.Right>Screen->Width)   frm->Left = Screen->Width - frm->Width;
		if (frm->BoundsRect.Bottom>Screen->Height) frm->Top  = Screen->Height - frm->Height;
	}
}
//---------------------------------------------------------------------
void load_pos_info(TForm *frm, TIniFile *ini_file)
{
	int w = frm->Constraints->MinWidth;		if (w==0) w = frm->Width;
	int h = frm->Constraints->MinHeight;	if (h==0) h = frm->Height;

	load_pos_info(frm, ini_file, (Screen->Width - w)/2, (Screen->Height - h)/2, w, h);
}

//---------------------------------------------------------------------
void save_pos_info(TForm *frm, TIniFile *ini_file)
{
	UnicodeString sct = "General";
	ini_file->WriteInteger(sct, frm->Name + "Left", frm->Left);
	ini_file->WriteInteger(sct, frm->Name + "Top",  frm->Top);

	if (frm->BorderStyle!=bsDialog) {
		ini_file->WriteInteger(sct, frm->Name + "Width",  frm->Width);
		ini_file->WriteInteger(sct, frm->Name + "Height", frm->Height);
	}
}

//---------------------------------------------------------------------------
void load_GridColWidth(TStringGrid *gp, TIniFile *ip, int cnt, ...)
{
	va_list ap;
	va_start(ap, cnt);
	for (int i=0; i<cnt; i++) {
		int arg = va_arg(ap,int);
		if (i<gp->FixedCols)
			gp->ColWidths[i] = arg;
		else if (i<gp->ColCount && arg>0 && arg<1200)
			gp->ColWidths[i] = ip->ReadInteger(gp->Name, "ColWidth" + IntToStr(i), arg);
	}
	va_end(ap);
}
//---------------------------------------------------------------------------
void load_GridColDefWidth(TStringGrid *gp, TIniFile *ip, int cnt, int wd)
{
	if (cnt==0) cnt = gp->ColCount;
	for (int i=0; i<cnt; i++) {
		if (i<gp->ColCount) gp->ColWidths[i] = ip->ReadInteger(gp->Name, "ColWidth" + IntToStr(i), wd);
	}
}

//---------------------------------------------------------------------------
void save_GridColWidth(TStringGrid *gp, TIniFile *ip)
{
	for (int i=0; i<gp->ColCount; i++)
		ip->WriteInteger(gp->Name, "ColWidth" + IntToStr(i), gp->ColWidths[i]);
}


//---------------------------------------------------------------------------
void load_ComboBoxItems(TComboBox *cp, TIniFile *ip, UnicodeString sct, int max_items)
{
	if (sct.IsEmpty()) sct = cp->Name;
	cp->Clear();

	for (int i=0; i<max_items; i++) {
		UnicodeString itm_str = ip->ReadString(sct, "Item" + IntToStr(i + 1), EmptyStr);
		if (itm_str.IsEmpty()) break;
		if (cp->Items->IndexOf(itm_str)==-1) cp->Items->Add(itm_str);
	}
}
//---------------------------------------------------------------------------
void save_ComboBoxItems(TComboBox *cp, TIniFile *ip, UnicodeString sct, int max_items)
{
	if (sct.IsEmpty()) sct = cp->Name;

	for (int i=0; i<max_items; i++) {
		UnicodeString itm_str = EmptyStr;
		if (i<cp->Items->Count) itm_str = cp->Items->Strings[i];
		ip->WriteString(sct, "Item" + IntToStr(i + 1), itm_str);
	}
}
//---------------------------------------------------------------------------
void load_tool_info(TToolBar *tp, TIniFile *ip, UnicodeString sct)
{
	if (sct.IsEmpty()) sct = tp->Name;

	if (ip->ReadBool(sct, "Floating", false)) {
		tp->ManualFloat(
			Rect(ip->ReadInteger(sct, "FltLeft",	100),
				 ip->ReadInteger(sct, "FltTop",		100),
				 ip->ReadInteger(sct, "FltRight",	200),
				 ip->ReadInteger(sct, "FltBottom",	200))
			);
	}
	else {
		tp->Top  = ip->ReadInteger(sct, "DockTop",	tp->Top);
		tp->Left = ip->ReadInteger(sct, "DockLeft",	tp->Left);
	}

	tp->Visible = ip->ReadBool(sct, "Visible", true);
}
//---------------------------------------------------------------------------
void save_tool_info(TToolBar *tp, TIniFile *ip, UnicodeString sct)
{
	if (sct.IsEmpty()) sct = tp->Name;

	ip->WriteBool(sct, "Visible", 	tp->Visible);
	ip->WriteBool(sct, "Floating",	tp->Floating);
	if (tp->Floating) {
		TControl *st = tp->HostDockSite;
		ip->WriteInteger(sct, "FltLeft",	st->BoundsRect.Left);
		ip->WriteInteger(sct, "FltTop",	st->BoundsRect.Top);
		ip->WriteInteger(sct, "FltRight",	st->BoundsRect.Left + st->ClientWidth);
		ip->WriteInteger(sct, "FltBottom",	st->BoundsRect.Top + st->ClientHeight);
	}
	else {
		ip->WriteInteger(sct, "DockTop",	tp->Top);
		ip->WriteInteger(sct, "DockLeft",	tp->Left);
	}
}

//---------------------------------------------------------------------------
TFont* load_font_inf(
	TIniFile *ip, UnicodeString sct,
	TFont *def)		//Default font (default = NULL)
{
	TFont *f = new TFont();
	f->Charset = (TFontCharset)ip->ReadInteger(sct, "FontCharset", (int)SHIFTJIS_CHARSET);
	f->Name    = ip->ReadString( sct, "FontName", (def? def->Name : EmptyStr));
	f->Size    = ip->ReadInteger(sct, "FontSize", (def? def->Size : 11));
	TFontStyles fstyle = TFontStyles();
	if (ip->ReadBool(sct, "FontBold", (def? def->Style.Contains(fsBold) : false)))
		fstyle << fsBold;
	if (ip->ReadBool(sct, "FontItalic", (def? def->Style.Contains(fsItalic) : false)))
		fstyle << fsItalic;
	f->Style = fstyle;
	return f;
}
//---------------------------------------------------------------------------
void save_font_inf(TIniFile *ip, UnicodeString sct, TFont *f)
{
	ip->WriteString( sct, "FontName",	f->Name);
	ip->WriteInteger(sct, "FontSize",	f->Size);
	ip->WriteBool(   sct, "FontBold",	f->Style.Contains(fsBold));
	ip->WriteBool(   sct, "FontItalic",	f->Style.Contains(fsItalic));
}
//---------------------------------------------------------------------------
