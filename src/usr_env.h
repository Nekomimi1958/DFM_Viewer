//----------------------------------------------------------------------//
// Environment															//
// 	use TIniFile														//
//----------------------------------------------------------------------//
#ifndef UsrEnvH
#define UsrEnvH

#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <System.IniFiles.hpp>

//---------------------------------------------------------------------------
UnicodeString get_version_str(UnicodeString fnam = EmptyStr);
int  get_version_no();
bool get_app_inf(UnicodeString fnam, TStringList *lst);

void load_form_pos(TForm *frm, TIniFile *ini_file, int w=0, int h=0);
void save_form_pos(TForm *frm, TIniFile *ini_file);

void load_pos_info(TForm *frm, TIniFile *ini_file, int x, int y, int w=0, int h=0);
void load_pos_info(TForm *frm, TIniFile *ini_file);
void save_pos_info(TForm *frm, TIniFile *ini_file);

void load_GridColWidth(TStringGrid *gp, TIniFile *ip, int cnt, ...);
void load_GridColDefWidth(TStringGrid *gp, TIniFile *ip, int cnt = 0, int wd = 100);
void save_GridColWidth(TStringGrid *gp, TIniFile *ip);

void load_ComboBoxItems(TComboBox *cp, TIniFile *ip,
	UnicodeString sct=EmptyStr, int max_items=20);
void save_ComboBoxItems(TComboBox *cp, TIniFile *ip,
	UnicodeString sct=EmptyStr, int max_items=20);

void load_tool_info(TToolBar *tp, TIniFile *ip, UnicodeString sct=EmptyStr);
void save_tool_info(TToolBar *tp, TIniFile *ip, UnicodeString sct=EmptyStr);

TFont* load_font_inf(TIniFile *ip, UnicodeString sct, TFont *def=NULL);
void save_font_inf(TIniFile *ip, UnicodeString sct, TFont *f);

//---------------------------------------------------------------------------
#endif
