//----------------------------------------------------------------------//
// String Operation														//
//																		//
//----------------------------------------------------------------------//
#ifndef UsrStrH
#define UsrStrH

#include <vcl.h>
#include <tchar.h>

//---------------------------------------------------------------------------
extern const _TCHAR *null_TCHAR;

//---------------------------------------------------------------------------
#define UCAT_T(str)	cat_sprintf(_T("%s"), _T(str))
#define USET_T(str)	sprintf(_T("%s"), _T(str))

#define USAME_TI(s1,s2)	same_ut_i(s1, _T(s2))
#define USAME_TS(s1,s2)	same_ut_s(s1, _T(s2))

#define REPLACE_TI(s,o,r)	replace_i(s, _T(o), _T(r))
#define REPLACE_TS(s,o,r)	replace_s(s, _T(o), _T(r))

#define UAPP_T(s,t)		append_str(s, _T(t))

//---------------------------------------------------------------------------
int __fastcall comp_NaturalOrder(TStringList *List, int Index1, int Index2);

int __fastcall comp_AscendOrder(TStringList *List, int Index1, int Index2);
int __fastcall comp_DescendOrder(TStringList *List, int Index1, int Index2);
int __fastcall comp_ObjectsOrder(TStringList *List, int Index1, int Index2);

//---------------------------------------------------------------------------
UnicodeString get_tkn(UnicodeString s, UnicodeString sp);
UnicodeString get_tkn(UnicodeString s, const _TCHAR *sp);
UnicodeString get_tkn(UnicodeString s, WideChar sp);

UnicodeString get_tkn_r(UnicodeString s, UnicodeString sp);
UnicodeString get_tkn_r(UnicodeString s, const _TCHAR *sp);
UnicodeString get_tkn_r(UnicodeString s, WideChar sp);

UnicodeString get_tkn_m(UnicodeString s, const _TCHAR *sp1, const _TCHAR *sp2);
UnicodeString get_tkn_m(UnicodeString s, WideChar sp1, WideChar sp2);

UnicodeString get_pre_tab(UnicodeString s);
UnicodeString get_post_tab(UnicodeString s);

UnicodeString get_first_line(UnicodeString s);

UnicodeString get_in_paren(UnicodeString s);
UnicodeString split_in_paren(UnicodeString &s);
UnicodeString get_norm_str(UnicodeString s);

UnicodeString split_tkn(UnicodeString &s, UnicodeString sp);
UnicodeString split_tkn(UnicodeString &s, const _TCHAR *sp);
UnicodeString split_tkn(UnicodeString &s, WideChar sp);

UnicodeString split_pre_tab(UnicodeString &s);
UnicodeString split_dsc(UnicodeString &s);
UnicodeString split_top_ch(UnicodeString &s);
WideChar split_top_wch(UnicodeString &s);

int split_cmd_line(UnicodeString s, TStringList *lst);
UnicodeString split_file_param(UnicodeString &s);

TStringDynArray split_strings_tab(UnicodeString s);
TStringDynArray split_strings_semicolon(UnicodeString s);

bool remove_text(UnicodeString &s, UnicodeString w);
bool remove_text(UnicodeString &s, const _TCHAR *w);

bool remove_top_text(UnicodeString &s, UnicodeString w);
bool remove_top_text(UnicodeString &s, const _TCHAR *w);

bool remove_top_s(UnicodeString &s, UnicodeString w);
bool remove_top_s(UnicodeString &s, const _TCHAR *w);
bool remove_top_s(UnicodeString &s, WideChar w);

bool remove_top_AT(UnicodeString &s);
bool remove_top_Dollar(UnicodeString &s);

bool remove_end_text(UnicodeString &s, UnicodeString w);

bool remove_end_s(UnicodeString &s, UnicodeString w);
bool remove_end_s(UnicodeString &s, WideChar w);

void delete_end(UnicodeString &s);

UnicodeString exclude_top(UnicodeString s);
UnicodeString exclude_top_end(UnicodeString s);

UnicodeString replace_i(UnicodeString s, const _TCHAR *o, const _TCHAR *r);
UnicodeString replace_s(UnicodeString s, const _TCHAR *o, const _TCHAR *r);
UnicodeString replace_regex(UnicodeString s, const _TCHAR *o, const _TCHAR *r);

UnicodeString replace_str_by_list(UnicodeString s, TStringList *lst);

UnicodeString append_str(UnicodeString s, const _TCHAR *t);
void cat_str_semicolon(UnicodeString &s, UnicodeString t);

UnicodeString ins_spc_length(UnicodeString s, int len);

UnicodeString def_if_empty(UnicodeString s, UnicodeString def);
UnicodeString def_if_empty(UnicodeString s, const _TCHAR *def);

bool is_num_str(UnicodeString s);
int extract_int(UnicodeString s);
int extract_int_def(UnicodeString s, int def = 0);
UnicodeString extract_top_int_str(UnicodeString s);
TColor xRRGGBB_to_col(UnicodeString s);

int pos_i(UnicodeString wd, UnicodeString s);
int pos_i(const _TCHAR *wd, UnicodeString s);
int pos_r(UnicodeString wd, UnicodeString s);
int pos_r(const _TCHAR *wd, UnicodeString s);
int pos_r_i(UnicodeString wd, UnicodeString s);
int pos_r_q(UnicodeString wd, UnicodeString s);
int pos_r_q_colon(UnicodeString s);

bool contains_i(UnicodeString s, const _TCHAR *w);
bool contains_i(UnicodeString s, WideChar w);
bool contains_s(UnicodeString s, const _TCHAR *w);
bool contains_s(UnicodeString s, WideChar w);

bool contains_word_and_or(UnicodeString s, UnicodeString kwd, bool case_sw = false);

bool contained_wd_i(UnicodeString lst, UnicodeString wd);
bool contained_wd_i(const _TCHAR *lst, UnicodeString wd);

bool contains_wd_i(UnicodeString s, const _TCHAR *lst);

UnicodeString get_word_i_idx(const _TCHAR *lst, int idx);
int idx_of_word_i(const _TCHAR *lst, UnicodeString wd);

bool str_match(UnicodeString ptn, UnicodeString s);

bool chk_RegExPtn(UnicodeString ptn);
bool is_match_regex(UnicodeString s, const _TCHAR *ptn);
bool is_match_regex_i(UnicodeString s, const _TCHAR *ptn);
UnicodeString extract_prm_RegExPtn(UnicodeString &s);
UnicodeString ptn_match_str(UnicodeString ptn, UnicodeString s);
bool starts_ptn(UnicodeString ptn, UnicodeString s);

void get_find_wd_list(UnicodeString wd, TStringList *lst);
bool find_mlt(UnicodeString wd, UnicodeString s, bool and_sw = false, bool not_sw = false, bool case_sw = false);
bool find_mlt_str(UnicodeString wd, UnicodeString s, TStringList *lst, bool case_sw);

int get_line_count(UnicodeString s);

void add_dyn_array(TStringDynArray &lst, UnicodeString s);

TStringDynArray get_csv_array(UnicodeString src, int size, bool force_size = false);
UnicodeString get_csv_item(UnicodeString src, int idx);
UnicodeString get_tsv_item(UnicodeString src, int idx);
UnicodeString make_csv_str(UnicodeString s);
UnicodeString make_csv_str(bool sw);
UnicodeString make_csv_rec_str(TStringDynArray lst);
int indexof_csv_list(TStringList *lst, UnicodeString s, int p);
TStringDynArray record_of_csv_list(TStringList *lst, UnicodeString s, int p, int size);

UnicodeString conv_esc_char(UnicodeString s);

UnicodeString yen_to_slash(UnicodeString s);
UnicodeString slash_to_yen(UnicodeString s);

bool same_ut_i(UnicodeString s, const _TCHAR *t);
bool same_ut_s(UnicodeString s, const _TCHAR *t);

bool equal_1(UnicodeString s);
bool equal_0(UnicodeString s);
bool equal_ENTER(UnicodeString s);
bool equal_ESC(UnicodeString s);
bool equal_TAB(UnicodeString s);
bool equal_DEL(UnicodeString s);
bool equal_LEFT(UnicodeString s);
bool equal_RIGHT(UnicodeString s);
bool equal_F1(UnicodeString s);

bool is_separator(UnicodeString s);

bool starts_tchs(const _TCHAR *lst, UnicodeString s);
bool ends_tchs(const _TCHAR *lst, UnicodeString s);

bool starts_AT(UnicodeString s);
bool starts_Dollar(UnicodeString s);

bool ends_PathDlmtr(UnicodeString s);

bool contains_PathDlmtr(UnicodeString s);
bool contains_Slash(UnicodeString s);

bool is_regex_slash(UnicodeString s);

bool is_quot(UnicodeString s);
UnicodeString add_quot_if_spc(UnicodeString s);
UnicodeString exclude_quot(UnicodeString s);

int  get_ListIntVal(TStringList *lst, const _TCHAR *name, int def = 0);
bool ListVal_is_empty(TStringList *lst, const _TCHAR *name);
bool ListVal_equal_1(TStringList *lst, const _TCHAR *name);

UnicodeString __fastcall mSecToTStr(unsigned int ms, bool cs = true);
int param_to_mSec(UnicodeString prm);

bool IsIrregularFont(TFont *fnt);
int  get_CharWidth(TCanvas *cv, int n, int mgn = 0);

UnicodeString fit_StrToPanel(UnicodeString s, int p, TPanel *pp);

int str_len_half(UnicodeString s);
int str_len_unicode(UnicodeString s);
void max_len_half(int &w, UnicodeString s);

UnicodeString align_r_str(UnicodeString s, int wd, UnicodeString post_s = EmptyStr);
UnicodeString align_l_str(UnicodeString s, int wd, UnicodeString post_s = EmptyStr);

UnicodeString to_Full_or_Half(UnicodeString s, bool to_w);
UnicodeString to_FullWidth(UnicodeString s);
UnicodeString to_HalfWidth(UnicodeString s);

UnicodeString make_RuledLine(int cnt, ...);

UnicodeString get_AddrStr(__int64 adr, int w = 0);
UnicodeString get_AspectStr(int w, int h);

//---------------------------------------------------------------------------
extern const UnicodeString CodePageList;
extern const UnicodeString CodePageListX;

UnicodeString get_NameOfCodePage(int code_page);
int get_CodePageOfName(UnicodeString code_name);

int  check_UTF8(BYTE *bp, int size);
int  get_MemoryCodePage(TMemoryStream *ms, bool *has_bom = NULL);
UnicodeString get_MemoryStrins(TMemoryStream *ms);

//---------------------------------------------------------------------------
UnicodeString check_Surrogates(UnicodeString s);
UnicodeString check_EnvDepandChars(UnicodeString s);

//---------------------------------------------------------------------------
#endif
