//----------------------------------------------------------------------//
// Dark Mode															//
//----------------------------------------------------------------------//
#ifndef UsrDarkH
#define UsrDarkH

//---------------------------------------------------------------------------
#include <Vcl.Buttons.hpp>
#include <Vcl.Mask.hpp>
#include <Vcl.ComCtrls.hpp>

//---------------------------------------------------------------------------
typedef BOOL (WINAPI *FUNC_ShouldAppsUseDarkMode)();
typedef BOOL (WINAPI *FUNC_AllowDarkModeForWindow)(HWND, BOOL);
typedef BOOL (WINAPI *FUNC_AllowDarkModeForApp)(BOOL);
typedef VOID (WINAPI *FUNC_FlushMenuThemes)();

#define DWMWA_USE_IMMERSIVE_DARK_MODE 19

extern  HMODULE hUxTheme;
extern  FUNC_ShouldAppsUseDarkMode	lpfShouldAppsUseDarkMode;
extern  FUNC_AllowDarkModeForWindow	lpfAllowDarkModeForWindow;
extern  FUNC_AllowDarkModeForApp	lpfAllowDarkModeForApp;
extern  FUNC_FlushMenuThemes		lpfFlushMenuThemes ;

extern bool SupportDarkMode;
extern bool IsDarkMode;
extern bool AllowDarkMode;

extern bool SureCancel;
extern bool SureDefNo;
extern bool SureAdjPos;
extern bool MsgPosCenter;

//---------------------------------------------------------------------------
extern TColor col_bgOptTab;
extern TColor col_fgOptTab;
extern TColor col_Invalid;
extern TColor col_Illegal;
extern TColor col_DkPanel;
extern TColor col_DkInval;
extern TColor col_DkIlleg;
extern const TColor col_None;

extern TColor scl_Window;
extern TColor scl_WindowText;
extern TColor scl_Highlight;
extern TColor scl_HighlightText;
extern TColor scl_BtnFace;
extern TColor scl_BtnText;
extern TColor scl_BtnShadow;
extern TColor scl_BtnHighlight;
extern TColor scl_Menu;
extern TColor scl_MenuText;
extern TColor scl_MenuSelect;
extern TColor scl_MenuSelect2;
extern TColor scl_bgMenuBar;
extern TColor scl_fgMenuBar;
extern TColor scl_htMenuBar;

extern TColor dcl_Window;
extern TColor dcl_WindowText;
extern TColor dcl_Highlight;
extern TColor dcl_Highlight2;
extern TColor dcl_HighlightText;
extern TColor dcl_BtnFace;
extern TColor dcl_BtnText;
extern TColor dcl_BtnShadow;
extern TColor dcl_BtnHighlight;
extern TColor dcl_Menu;
extern TColor dcl_MenuText;
extern TColor dcl_bgMenuBar;
extern TColor dcl_fgMenuBar;
extern TColor dcl_htMenuBar;

extern TBrush *MenuBrush;

//---------------------------------------------------------------------------
bool is_HighContrast();
void InitializeSysColor();
void InitializeDarkMode();
void EndDarkMode();
bool ApplyDarkMode(HWND hWnd = NULL);

void SetDarkWinTheme(TWinControl *wp);
TLabel* AttachLabelToGroup(TWinControl *wp, UnicodeString s = EmptyStr);

TColor get_WinColor(bool is_inv = false);
TColor get_TextColor(bool hl = false);
TColor get_PanelColor(bool is_inv = false);
TColor get_LabelColor();

void SetMenuBgColor(HMENU hMenu);
void SetHighlight(TCanvas *cv, bool hl);

void msgbox_ERR(UnicodeString msg);
void msgbox_WARN(UnicodeString msg);
void msgbox_OK(UnicodeString msg, UnicodeString tit = EmptyStr);
bool msgbox_Y_N(UnicodeString msg, UnicodeString tit = EmptyStr);
TModalResult msgbox_Y_N_C(UnicodeString msg, UnicodeString tit = EmptyStr);
bool msgbox_Sure(UnicodeString msg, bool ask = true, bool cente = false);
bool msgbox_Sure(const _TCHAR *msg, bool ask, bool center = false);

//---------------------------------------------------------------------------
#define UBMK_UP		1
#define UBMK_DOWN	2
#define UBMK_BUP	3
#define UBMK_BDOWN	4
#define UBMK_VUP	5
#define UBMK_VDOWN	6
#define UBMK_VTOP	7
#define UBMK_VEND	8
#define UBMK_HTOP	9
#define UBMK_HEND	10
#define UBMK_LEFT	11
#define UBMK_RIGHT	12

void draw_SortMark(TCanvas *cv, int x, int y, bool is_asc, TColor fg = Graphics::clNone);
void set_ButtonMark(TSpeedButton *bp, int id = UBMK_DOWN, TColor fg = scl_BtnText, TColor bg = scl_BtnFace);
void set_BtnMarkDark(TSpeedButton *bp, int id);

void draw_OwnerTab(TCustomTabControl *Control, int idx, const TRect rc, bool active, bool dark_sw = false);
void draw_MenuSeparator(TCanvas *cv, TRect rc);
void draw_OptColorItem(TColor col, UnicodeString s, TCanvas *cv, TRect rc, bool hl);
void draw_OptFontItem(TFont *fnt, UnicodeString s, TCanvas *cv, TRect rc, bool hl);

//---------------------------------------------------------------------------
bool set_focus_RadioGroup(TRadioGroup *gp);
bool set_focus_GroupBox(TGroupBox *gp);
void invert_CheckBox(TCheckBox *cp);

//---------------------------------------------------------------------------
#endif
