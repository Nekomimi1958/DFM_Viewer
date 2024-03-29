//----------------------------------------------------------------------//
// Dark Mode															//
//----------------------------------------------------------------------//
#include "usr_dark.h"

//---------------------------------------------------------------------------
//API
HMODULE hUxTheme = NULL;
FUNC_ShouldAppsUseDarkMode	lpfShouldAppsUseDarkMode  = NULL;
FUNC_AllowDarkModeForWindow	lpfAllowDarkModeForWindow = NULL;
FUNC_AllowDarkModeForApp	lpfAllowDarkModeForApp	  = NULL;
FUNC_FlushMenuThemes		lpfFlushMenuThemes		  = NULL;

//---------------------------------------------------------------------------
bool SupportDarkMode = false;
bool IsDarkMode 	 = false;
bool AllowDarkMode   = false;

//---------------------------------------------------------------------------
bool SureCancel;
bool SureDefNo;
bool SureAdjPos;
bool MsgPosCenter = false;

//---------------------------------------------------------------------------
const TColor col_None = Graphics::clNone;

TColor col_bgOptTab = clHighlight;
TColor col_fgOptTab = clHighlightText;
TColor col_Invalid;
TColor col_Illegal;

TColor col_DkPanel = col_None;
TColor col_DkInval;
TColor col_DkIlleg;

//---------------------------------------------------------------------------
//System Colors
TColor scl_Window;
TColor scl_WindowText;
TColor scl_Highlight;
TColor scl_HighlightText;
TColor scl_BtnFace;
TColor scl_BtnText;
TColor scl_BtnShadow;
TColor scl_BtnHighlight;
TColor scl_Menu;
TColor scl_MenuText;
TColor scl_MenuSelect;
TColor scl_MenuSelect2;
TColor scl_bgMenuBar;
TColor scl_fgMenuBar;
TColor scl_htMenuBar;

//Dark Colors
TColor dcl_Window;
TColor dcl_WindowText;
TColor dcl_Highlight;
TColor dcl_Highlight2;
TColor dcl_HighlightText;
TColor dcl_BtnFace;
TColor dcl_BtnText;
TColor dcl_BtnShadow;
TColor dcl_BtnHighlight;
TColor dcl_Menu;
TColor dcl_MenuText;
TColor dcl_bgMenuBar;
TColor dcl_fgMenuBar;
TColor dcl_htMenuBar;

TBrush *MenuBrush = NULL;

//---------------------------------------------------------------------------
bool is_HighContrast()
{
	HIGHCONTRAST hc = {sizeof(HIGHCONTRAST)};
	return (::SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0)
				&& ((hc.dwFlags & HCF_HIGHCONTRASTON)!=0));
}

//---------------------------------------------------------------------------
//Initialize System Colors
//---------------------------------------------------------------------------
void InitializeSysColor()
{
	scl_Window		  = clWindow;
	scl_WindowText	  = clWindowText;
	scl_Highlight	  = clHighlight;
	scl_HighlightText = clHighlightText;
	scl_BtnFace		  = clBtnFace;
	scl_BtnText		  = clBtnText;
	scl_BtnShadow	  = clBtnShadow;
	scl_BtnHighlight  = clBtnHighlight;
	scl_Menu		  = clMenu;
	scl_MenuText	  = clMenuText;
	scl_MenuSelect	  = TColor(RGB(0x91,0xc9,0xf7));
	scl_MenuSelect2	  = TColor(RGB(0x56,0xb0,0xfa));
	scl_bgMenuBar	  = clMenu;
	scl_fgMenuBar	  = clMenuText;
	scl_htMenuBar	  = TColor(RGB(0x91,0xc9,0xf7));

	dcl_Window		  = TColor(RGB(0x20, 0x20, 0x20));
	dcl_WindowText	  = clWhite;
	dcl_Highlight	  = TColor(RGB(0x41, 0x41, 0x41));
	dcl_Highlight2	  = TColor(RGB(0x77, 0x77, 0x77));
	dcl_HighlightText = clWhite;
	dcl_BtnFace 	  = TColor(RGB(0x33, 0x33, 0x33));
	dcl_BtnText 	  = clWhite;
	dcl_BtnShadow	  = TColor(RGB(0x40, 0x40, 0x40));
	dcl_BtnHighlight  = TColor(RGB(0x80, 0x80, 0x80));
	dcl_Menu		  = TColor(RGB(0x2b, 0x2b, 0x2b));
	dcl_MenuText	  = clWhite;
	dcl_bgMenuBar	  = clBlack;
	dcl_fgMenuBar	  = clWhite;
	dcl_htMenuBar	  = clHighlight;
}

//---------------------------------------------------------------------------
void InitializeDarkMode()
{
	if (CheckWin32Version(10) && TOSVersion::Build >= 17763 && !is_HighContrast()) {
		hUxTheme = ::LoadLibrary(_T("uxtheme.dll"));
		if (hUxTheme) {
			lpfShouldAppsUseDarkMode  = (FUNC_ShouldAppsUseDarkMode)::GetProcAddress(hUxTheme, MAKEINTRESOURCEA(132));
			lpfAllowDarkModeForWindow = (FUNC_AllowDarkModeForWindow)::GetProcAddress(hUxTheme, MAKEINTRESOURCEA(133));
			lpfAllowDarkModeForApp	  = (FUNC_AllowDarkModeForApp)::GetProcAddress(hUxTheme, MAKEINTRESOURCEA(135));
			lpfFlushMenuThemes		  = (FUNC_FlushMenuThemes)::GetProcAddress(hUxTheme, MAKEINTRESOURCEA(136));
			SupportDarkMode = (lpfShouldAppsUseDarkMode && lpfAllowDarkModeForWindow
								&& lpfAllowDarkModeForApp && lpfFlushMenuThemes);
		}
	}

	InitializeSysColor();

	MenuBrush = new TBrush();
}
//---------------------------------------------------------------------------
void EndDarkMode()
{
	if (hUxTheme) ::FreeLibrary(hUxTheme);

	delete MenuBrush;
}

//---------------------------------------------------------------------------
bool ApplyDarkMode(HWND hWnd)
{
	IsDarkMode = false;
	if (SupportDarkMode && lpfShouldAppsUseDarkMode()) {
		lpfAllowDarkModeForApp(AllowDarkMode);
		lpfFlushMenuThemes();
		IsDarkMode = AllowDarkMode;

		if (!hWnd) hWnd = Application->MainForm->Handle;
		if (hWnd) {
			::SetWindowTheme(hWnd, IsDarkMode? _T("DarkMode_Explorer") : NULL, NULL);
			BOOL is_dk = IsDarkMode;
			::DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &is_dk, sizeof(is_dk));
		}
	}

	return IsDarkMode;
}

//---------------------------------------------------------------------------
TColor get_WinColor(
	bool is_inv)	//Invalid item	(default = false)
{
	return is_inv? (IsDarkMode? col_DkInval : col_Invalid) : (IsDarkMode? dcl_Window : scl_Window);
}
//---------------------------------------------------------------------------
TColor get_TextColor(
	bool hl)		//Highlight	(default = false)
{
	return hl? (IsDarkMode? dcl_HighlightText : scl_HighlightText) : (IsDarkMode? dcl_WindowText : scl_WindowText);
}
//---------------------------------------------------------------------------
TColor get_PanelColor(
	bool is_inv)	//Invalid item	(default = false)
{
	if (is_inv) return (IsDarkMode? col_DkInval : col_Invalid);
	return (IsDarkMode? ((col_DkPanel!=col_None)? col_DkPanel : dcl_BtnFace) : scl_BtnFace);
}
//---------------------------------------------------------------------------
TColor get_LabelColor()
{
	return (IsDarkMode? dcl_BtnText : scl_BtnText);
}

//---------------------------------------------------------------------------
void SetMenuBgColor(HMENU hMenu)
{
	MenuBrush->Color = IsDarkMode? dcl_Menu : scl_Menu;
	MENUINFO mi = {sizeof(MENUINFO)};
	mi.fMask	= MIM_BACKGROUND|MIM_APPLYTOSUBMENUS;
	mi.hbrBack	= MenuBrush->Handle;
	::SetMenuInfo(hMenu, &mi);
}

//---------------------------------------------------------------------------
void SetHighlight(TCanvas *cv, bool hl)
{
	if (IsDarkMode) {
		cv->Brush->Color = hl? dcl_Highlight : dcl_Window;
		cv->Font->Color  = hl? dcl_HighlightText : dcl_WindowText;
	}
	else {
		cv->Brush->Color = hl? scl_Highlight : scl_Window;
		cv->Font->Color  = hl? scl_HighlightText : scl_WindowText;
	}
}

//---------------------------------------------------------------------------
//Apply Dark Mode to Controls
//---------------------------------------------------------------------------
void SetDarkWinTheme(TWinControl *wp)
{
	if (!wp) return;
	if (wp->ClassNameIs("TStatusBar")) return;

	if (wp->ClassNameIs("TTabSheet")) {
		TTabSheet *tp = (TTabSheet *)wp;
		if (!tp->TabVisible) return;
	}

	TColor bg_panel = get_PanelColor();
	TColor fg_label = get_LabelColor();
	TColor bg_win	= get_WinColor();
	TColor fg_txt   = get_TextColor();

	double scale = Screen->PixelsPerInch / 96.0;

	if (wp->InheritsFrom(__classid(TForm))) {
		TForm *fp = (TForm*)wp;
		::SetWindowTheme(wp->Handle, IsDarkMode? _T("DarkMode_Explorer") : NULL, NULL);
		BOOL is_dk = IsDarkMode;
		::DwmSetWindowAttribute(wp->Handle, DWMWA_USE_IMMERSIVE_DARK_MODE, &is_dk, sizeof(is_dk));

		fp->Color = bg_panel;
		for (int i=0; i<fp->ControlCount; i++) {
			TControl *cp = wp->Controls[i];
			if (cp->ClassNameIs("TLabel")) {
				TLabel *lp = (TLabel *)cp;
				lp->Color		= bg_panel;
				lp->Font->Color = fg_label;
			}
			else if (cp->InheritsFrom(__classid(TWinControl))) {
				SetDarkWinTheme((TWinControl *)cp);
			}
		}
	}
	else if (wp->ClassNameIs("TPageControl")) {
		TPageControl *pp = (TPageControl *)wp;
		pp->StyleElements >> seClient;
		for (int i=0; i<pp->PageCount; i++) SetDarkWinTheme(pp->Pages[i]);
	}
	else if (wp->ClassNameIs("TPanel") || wp->ClassNameIs("TTabSheet")
		|| wp->ClassNameIs("TGroupBox") || wp->ClassNameIs("TRadioGroup")
		|| wp->ClassNameIs("TTabControl"))
	{
		if (wp->ClassNameIs("TPanel")) {
			TPanel *pp = (TPanel *)wp;
			pp->StyleElements.Clear();
			pp->Color		= bg_panel;
			pp->Font->Color = fg_label;
		}
		else if (wp->ClassNameIs("TGroupBox") || wp->ClassNameIs("TRadioGroup")) {
			AttachLabelToGroup(wp);
		}

		//RadioButton group
		if (wp->ClassNameIs("TRadioGroup")) {
			if (IsDarkMode) {
				TRadioGroup *gp = (TRadioGroup *)wp;
				for (int i=0; i<gp->Items->Count; i++) {
					if (!gp->Items->Strings[i].IsEmpty()) {
						//Replace Caption with Label
						TRadioButton *cp = gp->Buttons[i];
						TLabel *lp = new TLabel(cp->Parent);
						lp->Tag		= (18 << 16) + cp->Width - 8;
						lp->Parent	= cp->Parent;
						lp->Font->Assign(cp->Font);
						lp->Color		= bg_panel;
						lp->Font->Color = fg_label;
						lp->Caption = cp->Caption;
						cp->Caption = EmptyStr;
						lp->Left	= cp->Left + 16;
						lp->Top 	= cp->Top + (cp->Height - lp->Height)/2;
						gp->Items->Strings[i] = EmptyStr;
						cp->Caption = EmptyStr;
						lp->BringToFront();
					}
				}
			}

			for (int i=0; i<wp->ControlCount; i++) {
				TControl *cp = wp->Controls[i];
				if (cp->ClassNameIs("TLabel")) {
					TLabel *lp = (TLabel *)cp;
					lp->Color		= bg_panel;
					lp->Font->Color = fg_label;
				}
			}
		}
		//Others
		else {
			for (int i=0; i<wp->ControlCount; i++) {
				TControl *cp = wp->Controls[i];
				if (cp->ClassNameIs("TLabel")) {
					TLabel *lp = (TLabel *)cp;
					lp->Color		= bg_panel;
					lp->Font->Color = fg_label;
				}
				else if (cp->InheritsFrom(__classid(TWinControl))) {
					SetDarkWinTheme((TWinControl *)cp);
				}
			}
		}
	}
	//Single child control
	else if (wp->ClassNameIs("TEdit")) {
		TEdit *ep = (TEdit *)wp;
		ep->Color		= bg_win;
		ep->Font->Color = fg_txt;
	}
	else if (wp->ClassNameIs("TLabeledEdit")) {
		TLabeledEdit *ep = (TLabeledEdit *)wp;
		ep->Color		= bg_win;
		ep->Font->Color = fg_txt;
		ep->EditLabel->Font->Color = ep->Font->Color;
	}
	else if (wp->ClassNameIs("TMaskEdit")) {
		TMaskEdit *ep = (TMaskEdit *)wp;
		ep->Color		= bg_win;
		ep->Font->Color = fg_txt;
	}
	else if (wp->ClassNameIs("TMemo")) {
		::SetWindowTheme(wp->Handle, IsDarkMode? _T("DarkMode_Explorer") : NULL, NULL);
	}
	else if (wp->ClassNameIs("TComboBox")) {
		::SetWindowTheme(wp->Handle, IsDarkMode? _T("DarkMode_CFD") : NULL, NULL);
		TComboBox *cp = (TComboBox *)wp;
		cp->Color		= bg_win;
		cp->Font->Color = fg_txt;
	}
	else if (wp->ClassNameIs("TCheckBox")) {
		if (IsDarkMode) {
			TCheckBox *cp = (TCheckBox *)wp;
			if (!cp->Caption.IsEmpty()) {
				//Replace Caption with Label
				TLabel *lp = new TLabel(cp->Parent);
				lp->Tag		= (cp->Height << 16) + cp->Width;
				lp->Parent	= cp->Parent;
				lp->Font->Assign(cp->Font);
				lp->Color		= bg_panel;
				lp->Font->Color = fg_label;
				lp->Caption = cp->Caption;
				lp->Anchors = cp->Anchors;
				lp->Left	= cp->Left + 16 * scale;
				lp->Top 	= cp->Top;
				cp->Caption = EmptyStr;
				lp->BringToFront();
			}
		}
	}
	else if (wp->ClassNameIs("TRadioButton")) {
		if (IsDarkMode) {
			TRadioButton *cp = (TRadioButton *)wp;
			if (!cp->Caption.IsEmpty()) {
				//Replace Caption with Label
				TLabel *lp = new TLabel(cp->Parent);
				lp->Tag		= (cp->Height << 16) + cp->Width;
				lp->Parent	= cp->Parent;
				lp->Font->Assign(cp->Font);
				lp->Color		= bg_panel;
				lp->Font->Color = fg_label;
				lp->Caption = cp->Caption;
				lp->Left	= cp->Left + 16 * scale;
				lp->Top 	= cp->Top;
				cp->Caption = EmptyStr;
				lp->BringToFront();
			}
		}
	}
	else {
		::SetWindowTheme(wp->Handle, IsDarkMode? _T("DarkMode_Explorer") : NULL, NULL);
	}
}

//---------------------------------------------------------------------------
TLabel* AttachLabelToGroup(TWinControl *wp,
	UnicodeString s)	//Caption	(default = EmptyStr)
{
	TLabel *ret_p = NULL;

	UnicodeString cap_str;
	TWinControl *parent;
	int xp, yp;
	double scale = Screen->PixelsPerInch / 96.0;
	int size8 = (int)(8 * scale);

	if (wp->ClassNameIs("TGroupBox")) {
		TGroupBox *gp = (TGroupBox *)wp;
		parent = gp->Parent;
		xp = gp->Left + size8;
		yp = gp->Top - 1;
		gp->Color		= get_PanelColor();
		gp->Font->Color = get_LabelColor();
		if (s.IsEmpty()) s = gp->Caption;
		if (!gp->Caption.IsEmpty()) gp->Caption = " ";
	}
	else if (wp->ClassNameIs("TRadioGroup")) {
		TRadioGroup *gp = (TRadioGroup *)wp;
		parent = gp->Parent;
		xp = gp->Left + size8;
		yp = gp->Top - 1;
		gp->Color		= get_PanelColor();
		gp->Font->Color = get_LabelColor();
		if (s.IsEmpty()) s = gp->Caption;
		if (!gp->Caption.IsEmpty()) gp->Caption = " ";
	}
	else {
		return NULL;
	}

	if (!Trim(s).IsEmpty()) {
		ret_p		= new TLabel(parent);
		TPanel *pp	= new TPanel(parent);
		pp->Parent	= parent;
		ret_p->Font->Assign(Application->DefaultFont);
		if (scale>1.0) ret_p->Font->Size = (int)(ret_p->Font->Size/scale);
		ret_p->Font->Color = get_LabelColor();
		ret_p->Color	   = get_PanelColor();
		pp->Left		   = xp;
		pp->Top 		   = yp;
		pp->Color		   = get_PanelColor();
		pp->Caption 	   = EmptyStr;
		pp->BorderWidth    = 1;
		pp->BevelOuter	   = bvNone;
		pp->StyleElements.Clear();
		ret_p->Caption	   = s;
		ret_p->Parent	   = pp;
		pp->AutoSize	   = true;
		pp->BringToFront();
	}

	return ret_p;
}

//---------------------------------------------------------------------------
void msgbox_ERR(UnicodeString msg)
{
	if (msg.IsEmpty()) return;

	TForm *MsgDlg = CreateMessageDialog(msg, mtError, TMsgDlgButtons() << mbOK, mbOK);
	SetDarkWinTheme(MsgDlg);
	MsgDlg->ShowModal();
	delete MsgDlg;
}
//---------------------------------------------------------------------------
void msgbox_WARN(UnicodeString msg)
{
	if (msg.IsEmpty()) return;

	TForm *MsgDlg = CreateMessageDialog(msg, mtWarning, TMsgDlgButtons() << mbOK, mbOK);
	SetDarkWinTheme(MsgDlg);
	MsgDlg->ShowModal();
	delete MsgDlg;
}
//---------------------------------------------------------------------------
void msgbox_OK(UnicodeString msg, UnicodeString tit)
{
	TForm *MsgDlg = CreateMessageDialog(msg, mtConfirmation, TMsgDlgButtons() << mbOK, mbOK);
	SetDarkWinTheme(MsgDlg);
	if (!tit.IsEmpty()) MsgDlg->Caption = tit;
	MsgDlg->ShowModal();
	delete MsgDlg;
}
//---------------------------------------------------------------------------
bool msgbox_Y_N(UnicodeString msg, UnicodeString tit)
{
	TMsgDlgButtons opt = TMsgDlgButtons() << mbYes << mbNo;
	TForm *MsgDlg = CreateMessageDialog(msg, mtConfirmation, opt, SureDefNo? mbNo : mbYes);
	SetDarkWinTheme(MsgDlg);
	if (!tit.IsEmpty()) MsgDlg->Caption = tit;
	int res = MsgDlg->ShowModal();
	delete MsgDlg;
	return (res==mrYes);
}
//---------------------------------------------------------------------------
TModalResult msgbox_Y_N_C(UnicodeString msg, UnicodeString tit)
{
	TMsgDlgButtons opt = TMsgDlgButtons() << mbYes << mbNo << mbCancel;
	TForm *MsgDlg = CreateMessageDialog(msg, mtConfirmation, opt, SureDefNo? mbNo : mbYes);
	SetDarkWinTheme(MsgDlg);
	if (!tit.IsEmpty()) MsgDlg->Caption = tit;
	TModalResult res = MsgDlg->ShowModal();
	delete MsgDlg;
	return res;
}

//---------------------------------------------------------------------------
bool msgbox_Sure(UnicodeString msg, bool ask, bool center)
{
	if (!ask) return true;

	TMsgDlgButtons opt = TMsgDlgButtons() << mbYes << mbNo;
	if (SureCancel) opt << mbCancel; 
	TForm *MsgDlg = CreateMessageDialog(msg, mtConfirmation, opt, SureDefNo? mbNo : mbYes);
	SetDarkWinTheme(MsgDlg);

	MsgPosCenter = center;
	int res = MsgDlg->ShowModal();
	MsgPosCenter = false;
	delete MsgDlg;

	return (res==mrYes);
}
//---------------------------------------------------------------------------
bool msgbox_Sure(const _TCHAR *msg, bool ask, bool center)
{
	return msgbox_Sure(UnicodeString(msg), ask, center);
}

//---------------------------------------------------------------------------
void draw_SortMark(TCanvas *cv, int x, int y,
	bool is_asc,	//Ascending order
	TColor fg)		//Mark color
{
	TPoint mrk[3];
	if (is_asc) {
		mrk[0] = Point(x,	  y + 3);
		mrk[1] = Point(x + 6, y + 3);
		mrk[2] = Point(x + 3, y);
	}
	else {
		mrk[0] = Point(x,	  y);
		mrk[1] = Point(x + 6, y);
		mrk[2] = Point(x + 3, y + 3);
	}
	cv->Pen->Style   = psSolid;
	cv->Pen->Color	 = fg;
	cv->Brush->Style = bsSolid;
	cv->Brush->Color = fg;
	cv->Polygon(mrk, 2);
}

//---------------------------------------------------------------------------
void set_ButtonMark(TSpeedButton *bp,
	int id,		//Identifier		(default = UBMK_DOWN)
	TColor fg,	//Foreground color	(default = col_BtnText)
	TColor bg)	//Background color	(default = col_BtnFace)
{
	int size;
	switch (id) {
	case UBMK_HTOP: case UBMK_HEND: case UBMK_LEFT: case UBMK_RIGHT:
		size = 16;	break;
	default:
		size = 9;
	}

	Graphics::TBitmap *bmp = bp->Glyph;
	TCanvas *cv = bmp->Canvas;
	bmp->SetSize(size, size);
	bmp->Transparent = true;
	cv->Brush->Color = bg;
	cv->FillRect(TRect(0, 0, size, size));
	cv->Pen->Style = psSolid;
	cv->Pen->Color = fg;

	switch (id) {
	case UBMK_UP:
		draw_SortMark(cv, 0, 2, true,  fg);	break;
	case UBMK_DOWN:
		draw_SortMark(cv, 0, 2, false, fg);	break;

	case UBMK_BUP:
		cv->Pen->Width = 2;
		cv->MoveTo(1, 5);  cv->LineTo(4, 2);
		cv->MoveTo(7, 5);  cv->LineTo(4, 2);
		break;
	case UBMK_BDOWN:
		cv->Pen->Width = 2;
		cv->MoveTo(1, 2);  cv->LineTo(4, 5);
		cv->MoveTo(7, 2);  cv->LineTo(4, 5);
		break;

	case UBMK_VUP:
		cv->Pen->Width = 2;
		cv->MoveTo(1, 7);  cv->LineTo(4, 4);
		cv->MoveTo(7, 7);  cv->LineTo(4, 4);
		break;
	case UBMK_VDOWN:
		cv->Pen->Width = 2;
		cv->MoveTo(1, 2);  cv->LineTo(4, 5);
		cv->MoveTo(7, 2);  cv->LineTo(4, 5);
		break;

	case UBMK_VTOP:
		cv->Pen->Width = 1;
		cv->MoveTo(0, 2);  cv->LineTo(9, 2);
		cv->MoveTo(0, 3);  cv->LineTo(9, 3);
		cv->Pen->Width = 2;
		cv->MoveTo(1, 7);  cv->LineTo(4, 4);
		cv->MoveTo(7, 7);  cv->LineTo(4, 4);
		break;
	case UBMK_VEND:
		cv->Pen->Width = 1;
		cv->MoveTo(0, 7);  cv->LineTo(9, 7);
		cv->MoveTo(0, 6);  cv->LineTo(9, 6);
		cv->Pen->Width = 2;
		cv->MoveTo(1, 2);  cv->LineTo(4, 5);
		cv->MoveTo(7, 2);  cv->LineTo(4, 5);
		break;

	case UBMK_HTOP:
		cv->Pen->Width = 1;
		cv->MoveTo(4, 1);  cv->LineTo( 4, 14);
		cv->Pen->Width = 2;
		cv->MoveTo(6, 7);  cv->LineTo(11,  2);
		cv->MoveTo(6, 7);  cv->LineTo(11, 12);
		break;
	case UBMK_HEND:
		cv->Pen->Width = 1;
		cv->MoveTo(11, 1);  cv->LineTo(11, 14);
		cv->Pen->Width = 2;
		cv->MoveTo(9, 7);  cv->LineTo(4,  2);
		cv->MoveTo(9, 7);  cv->LineTo(4, 12);
		break;
	case UBMK_LEFT:
		cv->Pen->Width = 2;
		cv->MoveTo(6, 7);  cv->LineTo(11,  2);
		cv->MoveTo(6, 7);  cv->LineTo(11, 12);
		break;
	case UBMK_RIGHT:
		cv->Pen->Width = 2;
		cv->MoveTo(9, 7);  cv->LineTo(4,  2);
		cv->MoveTo(9, 7);  cv->LineTo(4, 12);
		break;
	}
}
//---------------------------------------------------------------------------
void set_BtnMarkDark(TSpeedButton *bp, int id)
{
	set_ButtonMark(bp, id, (IsDarkMode? dcl_BtnText : scl_BtnText), get_PanelColor());
}

//---------------------------------------------------------------------------
void draw_OwnerTab(TCustomTabControl *Control, int idx, const TRect rc,
	bool active,
	bool dark_sw)	//Dark Mode	(default = false)
{
	TTabControl *tp = (TTabControl*)Control;
	TCanvas *cv = tp->Canvas;
	cv->Brush->Color = active? col_bgOptTab : get_PanelColor();
	cv->FillRect(rc);

	UnicodeString titstr = tp->Tabs->Strings[idx];
	cv->Font->Color = active? col_fgOptTab : get_LabelColor();
	cv->Font->Style = active? (cv->Font->Style << fsBold) : (cv->Font->Style >> fsBold);
	TRect tt_rc = rc;
	tt_rc.Left	= rc.Left + (rc.Width() - cv->TextWidth(ReplaceStr(titstr, "&", EmptyStr)))/2;
	tt_rc.Top	= (tp->TabPosition==tpBottom)? rc.Bottom - cv->TextHeight(titstr) - 4 : rc.Top + (active? 4 : 2);
	::DrawText(cv->Handle, titstr.c_str(), -1, &tt_rc, DT_LEFT);
}

//---------------------------------------------------------------------------
void draw_MenuSeparator(TCanvas *cv, TRect rc)
{
	double scale = Screen->PixelsPerInch / 96.0;
	int yp = rc.Top + rc.Height()/2;
	int x0 = rc.Left + ::GetSystemMetrics(SM_CYMENU) + (int)(4 *scale);
	int x1 = rc.Right;
	cv->Pen->Style = psSolid;
	cv->Pen->Mode  = pmCopy;
	cv->Pen->Width = 1;
	cv->Pen->Color = TColor(IsDarkMode? RGB(0x80, 0x80, 0x80) : RGB(0xd7, 0xd7, 0xd7));
	cv->MoveTo(x0, yp);  cv->LineTo(x1, yp);
}

//---------------------------------------------------------------------------
void draw_OptColorItem(TColor col, UnicodeString s, TCanvas *cv, TRect rc, bool hl)
{
	SetHighlight(cv, hl);
	cv->FillRect(rc);

	bool brk = StartsStr("|", s);
	if (brk) s.Delete(1, 1);
	int yp = rc.Top + (rc.Height() - cv->TextHeight("Q")) / 2;
	cv->TextOut(rc.Left + 34, yp, s);

	//Color
	TRect c_rc = rc;  c_rc.Right = c_rc.Left + 30;
	cv->Brush->Color = col;
	if (cv->Brush->Color!=col_None)
		cv->FillRect(c_rc);
	else {
		cv->Brush->Color = get_PanelColor();
		cv->FillRect(c_rc);
		cv->Font->Color  = get_LabelColor();
		cv->TextOut(rc.Left + 2, yp, _T("Invalid"));
	}

	//Border
	if (brk) {
		cv->Pen->Color = IsDarkMode? dcl_BtnShadow : scl_BtnShadow;
		cv->Pen->Style = psSolid;
		cv->Pen->Width = 1;
		cv->MoveTo(rc.Left,      rc.Top);
		cv->LineTo(rc.Right + 1, rc.Top);
	}
}
//---------------------------------------------------------------------------
void draw_OptFontItem(TFont *fnt, UnicodeString s, TCanvas *cv, TRect rc, bool hl)
{
	int xp = rc.Left + 2;
	int yp = rc.Top;
	int fh = abs(cv->Font->Height);

	SetHighlight(cv, hl);
	cv->FillRect(rc);
	cv->TextOut(xp, yp, s);
	xp += fh*10;	//***

	if (fnt) {
		//Size
		UnicodeString szstr = fnt->Size;
		cv->TextOut(xp + cv->TextWidth(" 99") - cv->TextWidth(szstr), yp, szstr);
		xp += fh*2;
		//Name
		int sz = cv->Font->Size;
		cv->Font->Assign(fnt);
		cv->Font->Size	 = sz;
		SetHighlight(cv, hl);
		cv->TextOut(xp, yp, fnt->Name);
	}
}

//---------------------------------------------------------------------------
bool set_focus_RadioGroup(TRadioGroup *gp)
{
	if (!gp->Parent->Visible || !gp->Parent->Enabled || !gp->Visible || !gp->Enabled || gp->ItemIndex==-1) return false;
	gp->Buttons[gp->ItemIndex]->SetFocus();
	return true;
}
//---------------------------------------------------------------------------
bool set_focus_GroupBox(TGroupBox *gp)
{
	if (!gp->Parent->Visible || !gp->Parent->Enabled || !gp->Visible || !gp->Enabled) return false;

	TWinControl *top_wp = NULL;
	for (int i=0; i<gp->ControlCount; i++) {
		TControl *cp = gp->Controls[i];
		if (!cp->InheritsFrom(__classid(TWinControl))) continue;
		TWinControl *wp = (TWinControl *)cp;
		if (!top_wp)
			top_wp = wp;
		else if (wp->TabOrder < top_wp->TabOrder)
			top_wp = wp;
	}

	if (!top_wp || !top_wp->Visible || !top_wp->Enabled) return false;

	if (top_wp->ClassNameIs("TRadioGroup")) {
		set_focus_RadioGroup((TRadioGroup *)top_wp);
	}
	else {
		top_wp->SetFocus();
	}
	return true;
}
//---------------------------------------------------------------------------
void invert_CheckBox(TCheckBox *cp)
{
	if (cp->Parent->Visible && cp->Parent->Enabled
		&& cp->Visible && cp->Enabled && cp->Caption.IsEmpty())
	{
		cp->SetFocus();
		cp->Checked = !cp->Checked;
	}
}
//---------------------------------------------------------------------------
