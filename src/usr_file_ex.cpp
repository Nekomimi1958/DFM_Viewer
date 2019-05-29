//----------------------------------------------------------------------//
// File Operation (Support path of 260 characters or more)				//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <winioctl.h>
#include <memory>
#include "usr_str.h"
#include "usr_file_ex.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
UnicodeString ExePath;

//---------------------------------------------------------------------------
UnicodeString cv_ex_filename(UnicodeString fnam)
{
	if (fnam.Length()>=MAX_PATH) {
		if (StartsStr("\\\\", fnam)) fnam.Insert("?\\UNC\\", 3); else fnam.Insert("\\\\?\\", 1);
	}
	return fnam;
}

//---------------------------------------------------------------------------
UnicodeString cv_env_var(UnicodeString s)
{
	for (;;) {
		if (!contains_s(s, _T('%'))) break;
		UnicodeString estr = get_tkn_m(s, '%', '%');
		if (!ContainsText(s, "%" + estr + "%")) break;
		UnicodeString eval = GetEnvironmentVariable(estr);  if (eval.IsEmpty()) break;
		s = ReplaceText(s, "%" + estr + "%", eval);
	}

	return s;
}

//---------------------------------------------------------------------------
UnicodeString cv_env_str(UnicodeString s)
{
	s = ReplaceText(s, "%ExePath%", ExePath);
	s = cv_env_var(s);
	return s;
}

//---------------------------------------------------------------------------
UnicodeString get_actual_path(UnicodeString pnam)
{
	pnam = cv_env_str(pnam);

	if (contains_s(pnam, _T('$'))) {
		pnam = REPLACE_TS(pnam, "$$", "\f");	//$$ を一時的に \f に変えておいて
		pnam = ReplaceStr(pnam, "$X", ExcludeTrailingPathDelimiter(ExePath));
		pnam = ReplaceStr(pnam, "$D", ExtractFileDrive(ExePath));
		pnam = REPLACE_TS(pnam, "\f", "$");		//$$(\f) を $ に変換
	}
	return pnam;
}

//---------------------------------------------------------------------------
UnicodeString get_actual_name(UnicodeString fnam)
{
	fnam = cv_env_str(fnam);

	if (!fnam.IsEmpty() && fnam.Pos('\\')==0) {
		bool chk_ext = get_extension(fnam).IsEmpty();
		TStringDynArray plst = split_strings_semicolon(GetEnvironmentVariable("PATH"));
		TStringDynArray xlst = split_strings_semicolon(GetEnvironmentVariable("PATHEXT"));
		UnicodeString rnam;
		for (int i=0; i<plst.Length && rnam.IsEmpty(); i++) {
			UnicodeString pnam = IncludeTrailingPathDelimiter(plst[i]);
			if (chk_ext) {
				for (int j=0; j<xlst.Length && rnam.IsEmpty(); j++) {
					UnicodeString anam = pnam + fnam + xlst[j];
					if (file_exists(anam)) rnam = anam;
				}
			}
			else if (file_exists(pnam + fnam))
				rnam = pnam + fnam;
		}
		if (!rnam.IsEmpty()) fnam = rnam;
	}

	return fnam;
}

//---------------------------------------------------------------------------
UnicodeString exclude_env_path(UnicodeString fnam)
{
	if (fnam.IsEmpty() || ExtractFileDrive(fnam).IsEmpty()) return fnam;

	UnicodeString fext = get_extension(fnam);
	if (fext.IsEmpty()) return fnam;

	TStringDynArray xlst = split_strings_semicolon(GetEnvironmentVariable("PATHEXT"));
	bool ok = false;
	for (int i=0; i<xlst.Length && !ok; i++) ok = SameText(fext, xlst[i]);

	if (ok) {
		TStringDynArray plst = split_strings_semicolon(GetEnvironmentVariable("PATH"));
		UnicodeString pnam = ExtractFilePath(fnam);
		for (int i=0; i<plst.Length; i++) {
			if (SameText(IncludeTrailingPathDelimiter(plst[i]), pnam)) {
				fnam = ChangeFileExt(ExtractFileName(fnam), EmptyStr);
				break;
			}
		}
	}

	return fnam;
}

//---------------------------------------------------------------------------
UnicodeString to_relative_name(UnicodeString fnam)
{
	remove_top_text(fnam, ExePath);
	return fnam;
}

//---------------------------------------------------------------------------
UnicodeString rel_to_absdir(
	UnicodeString fnam,		//変換対象ファイル名
	UnicodeString rnam)		//基準ディレクトリ名 (default = EmptyStr/ ExePath);
{
	if (fnam.IsEmpty() || !ExtractFileDrive(fnam).IsEmpty()) return fnam;

	if (rnam.IsEmpty()) rnam = ExePath;

	for (;;) {
		if (remove_top_text(fnam, _T(".\\"))) continue;
		if (!remove_top_text(fnam, _T("..\\"))) break;
		rnam = get_parent_path(rnam);
	}

	return contains_PathDlmtr(rnam)? rnam + fnam : EmptyStr;
}

//---------------------------------------------------------------------------
bool is_same_file(UnicodeString fnam1, UnicodeString fnam2,
	UnicodeString rnam)		//基準ファイル名 (default = EmptyStr/ ExePath);
{
	fnam1 = rel_to_absdir(exclude_quot(fnam1), rnam);
	fnam2 = rel_to_absdir(exclude_quot(fnam2), rnam);
	return SameText(fnam1, fnam2);
}

//---------------------------------------------------------------------------
UnicodeString get_root_name(UnicodeString pnam)
{
	UnicodeString rstr = pnam;
	if (remove_top_text(rstr, _T("\\\\")))
		rstr = "\\\\" + get_tkn(rstr, '\\');
	else
		rstr = ExtractFileDrive(rstr);
	return IncludeTrailingPathDelimiter(rstr);
}

//---------------------------------------------------------------------------
UnicodeString get_case_name(UnicodeString fnam)
{
	UnicodeString drv_nam = IncludeTrailingPathDelimiter(ExtractFileDrive(fnam));
	UnicodeString ret_str = drv_nam;
	UnicodeString pnam;
	TStringDynArray plst = split_path(fnam);
	for (int i=0; i<plst.Length-1; i++) {
		pnam += plst[i] + "\\";
		if (USAME_TS(pnam, "\\") || USAME_TS(pnam, "\\\\")) continue;
		UnicodeString sea_str = pnam + plst[i + 1];
		if (SameText(IncludeTrailingPathDelimiter(sea_str), drv_nam)) continue;
		if (sea_str.Length()>=MAX_PATH) sea_str.Insert("\\\\?\\", 1);
		TSearchRec sr;
		if (FindFirst(sea_str, faAnyFile, sr)==0) {
			ret_str += sr.Name;
			if (sr.Attr & faDirectory) ret_str = IncludeTrailingPathDelimiter(ret_str);
			FindClose(sr);
		}
		else {
			//見つからなかったらそのまま返す
			ret_str = fnam;
			break;
		}
	}
	return ret_str;
}

//---------------------------------------------------------------------------
bool is_root_dir(UnicodeString dnam)
{
	if (remove_top_s(dnam, _T("\\\\")))
		return !contains_PathDlmtr(ExcludeTrailingPathDelimiter(get_tkn_r(dnam, '\\')));
	else
		return !contains_PathDlmtr(ExcludeTrailingPathDelimiter(dnam));
}
//---------------------------------------------------------------------------
bool is_root_unc(UnicodeString dnam)
{
	if (remove_top_s(dnam, _T("\\\\")))
		return !contains_PathDlmtr(ExcludeTrailingPathDelimiter(get_tkn_r(dnam, '\\')));
	else
		return false;
}

//---------------------------------------------------------------------------
UnicodeString exclede_delimiter_if_root(UnicodeString dnam)
{
	if (!dnam.IsEmpty()) {
		dnam = is_root_dir(dnam) ? ExcludeTrailingPathDelimiter(dnam)
								 : IncludeTrailingPathDelimiter(dnam);
	}
	return dnam;
}

//---------------------------------------------------------------------------
UnicodeString get_drive_str(UnicodeString dnam)
{
	return IncludeTrailingPathDelimiter(ExtractFileDrive(dnam)).UpperCase();
}

//---------------------------------------------------------------------------
bool drive_exists(UnicodeString drv)
{
	drv = get_tkn(drv, ':');	if (drv.IsEmpty())	 return false;
	int d_n = drv[1] - _T('A');	if (d_n<0 || d_n>25) return false;
	DWORD d_flag = 0x00000001 << d_n;
	DWORD d_bit = ::GetLogicalDrives();	//利用可能なドライブをビットマスク形式で取得
	return (d_bit & d_flag);
}

//---------------------------------------------------------------------------
UINT get_drive_type(UnicodeString dnam)
{
	return ::GetDriveType(get_drive_str(dnam).c_str());
}

//---------------------------------------------------------------------------
UnicodeString get_volume_info(UnicodeString dnam,
	UnicodeString *fsys)	//[o] ファイルシステム
{
	UnicodeString ret_str;
	_TCHAR vol_nam[MAX_PATH];
	_TCHAR fil_sys[MAX_PATH];
	DWORD VolSerialNo, MaxCompLen, Flags;
	if (::GetVolumeInformation(get_drive_str(dnam).c_str(),
		vol_nam, sizeof(vol_nam), &VolSerialNo, &MaxCompLen, &Flags, fil_sys, sizeof(fil_sys)))
	{
		ret_str = vol_nam;
		if (fsys) *fsys = fil_sys;
	}
	return ret_str;
}

//---------------------------------------------------------------------------
int get_ClusterSize(UnicodeString dnam)
{
	DWORD SecPerCl, BytePerSec, FreeCl, TotalCl;
	if (::GetDiskFreeSpace(get_drive_str(dnam).c_str(), &SecPerCl, &BytePerSec, &FreeCl, &TotalCl))
		return (BytePerSec * SecPerCl);
	else
		return 0;
}

//---------------------------------------------------------------------------
UnicodeString get_base_name(UnicodeString fnam)
{
	fnam = ExtractFileName(fnam);
	if (fnam.IsEmpty()) return EmptyStr;
	return StartsStr('.', fnam)? "." + ChangeFileExt(exclude_top(fnam), EmptyStr) : ChangeFileExt(fnam, EmptyStr);
}
//---------------------------------------------------------------------------
UnicodeString get_extension(UnicodeString fnam)
{
	fnam = ExtractFileName(fnam);
	if (fnam.IsEmpty()) return EmptyStr;
	return ExtractFileExt(StartsStr('.', fnam)? exclude_top(fnam) : fnam);
}
//---------------------------------------------------------------------------
UnicodeString get_extension_if_file(UnicodeString fnam)
{
	return dir_exists(fnam)? EmptyStr : get_extension(fnam);
}

//---------------------------------------------------------------------------
bool test_FileExt(UnicodeString fext, UnicodeString list)
{
	if (list.IsEmpty() || fext.IsEmpty() || USAME_TS(fext, ".")) return false;
	if (USAME_TS(list, "*") || USAME_TS(list, ".*")) return true;
	if (!StartsStr(".", fext)) fext.Insert(".", 1);
	if (!EndsStr('.', fext))   fext.UCAT_T(".");
	if (!StartsStr(".", list)) list.Insert(".", 1);
	if (!EndsStr('.', list))   list.UCAT_T(".");
	return ContainsText(list, fext);
}
//---------------------------------------------------------------------------
bool test_FileExt(UnicodeString fext, const _TCHAR *list)
{
	return test_FileExt(fext, UnicodeString(list));
}
//---------------------------------------------------------------------------
bool test_FileExtSize(UnicodeString fext, UnicodeString list, __int64 size)
{
	bool ret = false;
	TStringDynArray xlst = SplitString(list, ".");
	UnicodeString xstr;
	for (int i=0; i<xlst.Length; i++) {
		if (test_FileExt(fext, get_tkn(xlst[i], ":"))) xstr =xlst[i];
	}
	if (!xstr.IsEmpty()) {
		__int64 sz = get_tkn_r(xstr, ":").ToIntDef(0) * 1048576ul;	//MB
		ret = (size>=sz);
	}

	return ret;
}

//---------------------------------------------------------------------------
UnicodeString to_path_name(UnicodeString dnam)
{
	return (!dnam.IsEmpty()? IncludeTrailingPathDelimiter(dnam) : EmptyStr);
}

//---------------------------------------------------------------------------
UnicodeString get_dir_name(UnicodeString dnam)
{
	return ExtractFileName(ExcludeTrailingPathDelimiter(dnam));
}

//---------------------------------------------------------------------------
UnicodeString get_parent_path(UnicodeString dnam)
{
	return ExtractFilePath(ExcludeTrailingPathDelimiter(dnam));
}

//---------------------------------------------------------------------------
TStringDynArray split_path(
	UnicodeString pnam,
	UnicodeString dlmt)		//区切り	(default = "\\")
{
	pnam = ExcludeTrailingPathDelimiter(pnam);
	UnicodeString top_s = remove_top_s(pnam, dlmt + dlmt)? dlmt + dlmt : EmptyStr;

	TStringDynArray ret_array = SplitString(pnam, dlmt);
	if (!top_s.IsEmpty() && ret_array.Length>0) ret_array[0] = top_s + ret_array[0];

	return ret_array;
}

//---------------------------------------------------------------------------
bool is_computer_name(UnicodeString pnam)
{
	if (!remove_top_s(pnam, _T("\\\\"))) return false;
	return (ExcludeTrailingPathDelimiter(pnam).Pos('\\')==0);
}

//---------------------------------------------------------------------------
bool is_dir_accessible(UnicodeString dnam)
{
	if (!is_drive_accessible(dnam)) return false;
	if (is_root_dir(dnam)) return true;

	if (!contains_PathDlmtr(ExcludeTrailingPathDelimiter(dnam))) dnam = to_path_name(dnam);
	if (dnam.IsEmpty()) return false;

	if (dnam.Length()>=248) {
		if (StartsStr("\\\\", dnam)) dnam.Insert("?\\UNC\\", 3); else dnam.Insert("\\\\?\\", 1);
	}
	TSearchRec sr;
	if (FindFirst(UAPP_T(dnam, "*.*"), faAnyFile, sr)==0) {
		FindClose(sr);
		return true;
	}
	else
		return false;
}

//---------------------------------------------------------------------------
bool is_drive_accessible(UnicodeString dnam)
{
	if (dnam.IsEmpty()) return false;
	UnicodeString dstr = get_drive_str(dnam);
	if (dstr.IsEmpty()) return false;

	UINT old_mode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
	DWORD MaxCompLen, Flags;
	bool res = (::GetVolumeInformation(dstr.c_str(), NULL, 0, NULL, &MaxCompLen, &Flags, NULL, 0)!=0);
	::SetErrorMode(old_mode);

	return res;
}
//---------------------------------------------------------------------------
bool is_drive_protected(UnicodeString dnam)
{
	bool ret = false;
	dnam = "\\\\.\\" + ExtractFileDrive(dnam);
	HANDLE hDevice = ::CreateFile(dnam.c_str(),
		GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (hDevice!=INVALID_HANDLE_VALUE) {
		DWORD dwWritten;
		if (::DeviceIoControl(hDevice, IOCTL_DISK_IS_WRITABLE, NULL, 0, NULL, 0, &dwWritten, NULL)==FALSE &&
			GetLastError()==ERROR_WRITE_PROTECT)
				ret = true;
		::CloseHandle(hDevice);
	}
	return ret;
}

//---------------------------------------------------------------------------
int file_GetAttr(UnicodeString fnam)
{
	if (fnam.Length()>=MAX_PATH) {
		if (StartsStr("\\\\", fnam)) fnam.Insert("?\\UNC\\", 3); else fnam.Insert("\\\\?\\", 1);
	}
	return ::GetFileAttributes(fnam.c_str());
}

//---------------------------------------------------------------------------
bool file_SetAttr(UnicodeString fnam, int attr)
{
	try {
		return (FileSetAttr(cv_ex_filename(fnam).c_str(), attr)==0);
	}
	catch (...) {
		return false;
	}
}
//---------------------------------------------------------------------------
bool dir_CopyAttr(UnicodeString src_nam, UnicodeString dst_nam, bool remove_ro)
{
	int attr = file_GetAttr(src_nam);	if (attr==faInvalid) return false;
	if (remove_ro) attr &= ~faReadOnly;
	if ((attr & (faReadOnly|faHidden|faSysFile)) == 0) return true;
	return file_SetAttr(dst_nam, attr);
}

//---------------------------------------------------------------------------
bool set_FileWritable(UnicodeString fnam)
{
	bool ret = true;
	if (file_exists(fnam)) {
		int atr = file_GetAttr(fnam);
		if (atr!=faInvalid && (atr & faReadOnly)) {
			atr &= ~faReadOnly;
			ret = file_SetAttr(fnam, atr);
		}
	}
	return ret;
}

//---------------------------------------------------------------------------
bool is_SymLink(UnicodeString fnam)
{
	fnam = cv_ex_filename(ExcludeTrailingPathDelimiter(fnam));
	DWORD attr = ::GetFileAttributes(fnam.c_str());
	return (attr!=0xffffffff && (attr & FILE_ATTRIBUTE_REPARSE_POINT));
}

//---------------------------------------------------------------------------
int get_HardLinkCount(UnicodeString fnam)
{
	int cnt = 0;

	HANDLE hFile = ::CreateFile(cv_ex_filename(fnam).c_str(),
		0, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile!=INVALID_HANDLE_VALUE) {
		BY_HANDLE_FILE_INFORMATION fi;
		if (::GetFileInformationByHandle(hFile, &fi)) cnt = fi.nNumberOfLinks;
		::CloseHandle(hFile);
	}

    return cnt;
}


//---------------------------------------------------------------------------
bool file_exists(UnicodeString fnam)
{
	if (fnam.IsEmpty()) return false;
	return (file_GetAttr(fnam) != faInvalid);
}
//---------------------------------------------------------------------------
bool file_exists_x(UnicodeString fnam)
{
	if (fnam.IsEmpty()) return false;
	int attr = file_GetAttr(fnam);

	return (attr!=faInvalid && (attr & faDirectory)==0);
}

//---------------------------------------------------------------------------
bool dir_exists(UnicodeString dnam)
{
	if (dnam.IsEmpty()) return false;
	return ::PathIsDirectory(cv_ex_filename(dnam).c_str());
}

//---------------------------------------------------------------------------
TDateTime utc_to_DateTime(FILETIME *utc)
{
	FILETIME f_tm;
	SYSTEMTIME st;
	::FileTimeToLocalFileTime(utc, &f_tm);
	::FileTimeToSystemTime(&f_tm, &st);
	return SystemTimeToDateTime(st);
}

//---------------------------------------------------------------------------
TDateTime get_file_age(UnicodeString fnam)
{
	TDateTime ft = 0;
	HANDLE hFile = ::CreateFile(cv_ex_filename(fnam).c_str(),
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hFile!=INVALID_HANDLE_VALUE) {
		FILETIME c_tm, a_tm, w_tm, f_tm;
		if (::GetFileTime(hFile, &c_tm, &a_tm, &w_tm)) ft = utc_to_DateTime(&w_tm);
		::CloseHandle(hFile);
	}
	return ft;
}

//---------------------------------------------------------------------------
bool set_file_age(UnicodeString fnam, TDateTime dt, bool force)
{
	bool res = false;

	int atr = file_GetAttr(fnam);
	bool chg_ro = force && atr!=faInvalid && (atr & faReadOnly);
	if (chg_ro && !set_FileWritable(fnam)) chg_ro = false;

	UnicodeString l_fnam = cv_ex_filename(fnam);
	HANDLE hFile = ::CreateFile(l_fnam.c_str(),
		GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hFile!=INVALID_HANDLE_VALUE) {
		SYSTEMTIME s_tm;
		FILETIME l_tm, f_tm;
		DateTimeToSystemTime(dt, s_tm);
		if (::SystemTimeToFileTime(&s_tm, &l_tm) && ::LocalFileTimeToFileTime(&l_tm, &f_tm)) {
			res = ::SetFileTime(hFile, NULL, NULL, &f_tm);
		}
		::CloseHandle(hFile);
	}

	if (chg_ro && atr!=-1) file_SetAttr(fnam, atr);

	return res;
}

//---------------------------------------------------------------------------
__int64 get_file_size(UnicodeString fnam)
{
	__int64 size = 0;
	HANDLE hFile = ::CreateFile(cv_ex_filename(fnam).c_str(),
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile!=INVALID_HANDLE_VALUE) {
		DWORD size_hi;
		DWORD size_lo = ::GetFileSize(hFile, &size_hi);
		size = size_hi;
		size = (size << 32) + size_lo;
		::CloseHandle(hFile);
	}
	return size;
}

//---------------------------------------------------------------------------
UnicodeString chk_cre_dir(UnicodeString dnam)
{
	if (!dir_exists(dnam)) dnam = create_Dir(dnam)? IncludeTrailingPathDelimiter(dnam) : EmptyStr;
	return dnam;
}

//---------------------------------------------------------------------------
bool is_EmptyDir(UnicodeString dnam, bool no_file)
{
	bool e_flag = true;
	dnam = IncludeTrailingPathDelimiter(dnam);
	TSearchRec sr;
	if (FindFirst(cv_ex_filename(UAPP_T(dnam, "*")), faAnyFile, sr)==0) {
		do {
			if (ContainsStr("..", sr.Name)) continue;
			if (no_file && (sr.Attr & faDirectory))
				e_flag = is_EmptyDir(dnam + sr.Name, no_file);
			else
				e_flag = false;
		} while(FindNext(sr)==0 && e_flag);
		FindClose(sr);
	}
	return e_flag;
}

//---------------------------------------------------------------------------
int get_available_drive_list(TStringList *lst)
{
	lst->Clear();
	DWORD d_bit = ::GetLogicalDrives();
	DWORD d_flag = 0x00000001;
	UINT old_mode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
	for (int d_n=_T('A'); d_n <= _T('Z'); d_n++,d_flag<<=1) {
		if ((d_bit & d_flag)==0) continue;
		UnicodeString drvnam; drvnam.sprintf(_T("%c:\\"), d_n);
		if (is_drive_accessible(drvnam)) lst->Add(drvnam);
	}
	::SetErrorMode(old_mode);
	return lst->Count;
}

//---------------------------------------------------------------------------
UnicodeString get_file_attr_str(int atr)
{
	UnicodeString atr_str = "____";
	if (atr!=faInvalid) {
		if (atr & faReadOnly)	atr_str[1] = 'R';
		if (atr & faHidden)		atr_str[2] = 'H';
		if (atr & faSysFile)	atr_str[3] = 'S';
		if (atr & faArchive)	atr_str[4] = 'A';
	}
	return atr_str;
}

//---------------------------------------------------------------------------
void get_files(
	UnicodeString pnam,	//Directory name
	const _TCHAR *mask,	//Mask
	TStrings *lst,		//[o]
	bool subSW)			//include subdirectorys
{
	if (!dir_exists(pnam)) return;

	pnam = IncludeTrailingPathDelimiter(pnam);

	UnicodeString mask_u = def_if_empty(mask, _T("*.*"));

	TSearchRec sr;
	if (subSW) {
		if (FindFirst(cv_ex_filename(UAPP_T(pnam, "*")), faAnyFile, sr)==0) {
			do {
				if ((sr.Attr & faDirectory)==0) continue;
				if (ContainsStr("..", sr.Name)) continue;
				get_all_files_ex(pnam + sr.Name, mask_u, lst, subSW);
			} while(FindNext(sr)==0);
			FindClose(sr);
		}
	}

	if (FindFirst(cv_ex_filename(pnam + mask_u), faAnyFile, sr)==0) {
		do {
			if (sr.Attr & faDirectory) continue;
			if (lst->IndexOf(sr.Name)==-1) lst->Add(pnam + sr.Name);
		} while(FindNext(sr)==0);
		FindClose(sr);
	}
}

//---------------------------------------------------------------------------
int get_all_files_ex(
	UnicodeString pnam,		//Directory name
	UnicodeString mask,		//Mask
	TStrings *lst,			//[o] Relust list
	bool sub_sw,			//include subdirectorys		(default = fals)
	int sub_n,				//depth of subdirectory		(default = 99)
	UnicodeString skip_dir,	//skip directory			(default = EmptyStr)
	bool inc_hide,			//include hidden attribute	(default = true)
	bool inc_sys)			//include system attribute	(default = true)
{
	int fcnt = 0;
	if (!dir_exists(pnam)) return fcnt;

	pnam = IncludeTrailingPathDelimiter(pnam);
	if (mask.IsEmpty()) mask = "*.*";

	TSearchRec sr;
	if (sub_sw && sub_n>0) {
		TStringDynArray skip_dir_lst = split_strings_semicolon(skip_dir);
		if (FindFirst(cv_ex_filename(UAPP_T(pnam, "*")), faAnyFile, sr)==0) {
			do {
				if ((sr.Attr & faDirectory)==0) continue;
				if (!inc_hide && (sr.Attr & faHidden))  continue;
				if (!inc_sys  && (sr.Attr & faSysFile)) continue;
		
				UnicodeString dnam = sr.Name;
				if ((sr.Attr & faDirectory) && !ContainsStr("..", dnam)) {
					bool skip = false;
					for (int i=0; i<skip_dir_lst.Length; i++) {
						UnicodeString snam = skip_dir_lst[i];
						if (str_match(skip_dir_lst[i], dnam)) { skip = true; break; }
					}

					if (!skip) {
						int scnt = get_all_files_ex(pnam + dnam, mask, lst, sub_sw, sub_n - 1, skip_dir,
													inc_hide, inc_sys);
						fcnt += scnt;
					}
				}
			} while(FindNext(sr)==0);
			FindClose(sr);
		}
	}

	if (FindFirst(cv_ex_filename(pnam + mask), faAnyFile, sr)==0) {
		do {
			if (sr.Attr & faDirectory) continue;
			if (!inc_hide && (sr.Attr & faHidden))  continue;
			if (!inc_sys  && (sr.Attr & faSysFile)) continue;
			lst->Add(pnam + sr.Name);
			fcnt++;
		} while(FindNext(sr)==0);
		FindClose(sr);
	}

	return fcnt;
}

//---------------------------------------------------------------------------
void get_dirs(UnicodeString pnam, TStrings *lst)
{
	if (pnam.IsEmpty() || !dir_exists(pnam)) return;

	pnam = IncludeTrailingPathDelimiter(pnam);
	TSearchRec sr;
	if (FindFirst(cv_ex_filename(UAPP_T(pnam, "*")), faAnyFile, sr)==0) {
		do {
			if ((sr.Attr & faDirectory)==0 || ContainsStr("..", sr.Name)) continue;
			lst->Add(pnam + sr.Name);
			get_dirs(pnam + sr.Name, lst);
		} while(FindNext(sr)==0);
		FindClose(sr);
	}
}

//---------------------------------------------------------------------------
bool create_EmptyFile(UnicodeString fnam)
{
	HANDLE hFile = ::CreateFile(cv_ex_filename(fnam).c_str(),
		GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==INVALID_HANDLE_VALUE) return false;
	::CloseHandle(hFile);
	return true;
}

//---------------------------------------------------------------------------
bool create_Dir(UnicodeString dnam)
{
	if (dnam.Length()>=248) {
		if (StartsStr("\\\\", dnam)) dnam.Insert("?\\UNC\\", 3); else dnam.Insert("\\\\?\\", 1);
	}
	return ::CreateDirectory(dnam.c_str(), NULL);
}
//---------------------------------------------------------------------------
bool create_ForceDirs(UnicodeString dnam)
{
	TStringDynArray plst = split_path(dnam);
	UnicodeString rnam = get_root_name(dnam);
	UnicodeString pnam;
	for (int i=0; i<plst.Length; i++) {
		pnam += UAPP_T(plst[i], "\\");
		if (USAME_TS(pnam, "\\") || USAME_TS(pnam, "\\\\") || SameStr(pnam, rnam)) continue;
		if (dir_exists(pnam)) continue;
		if (!create_Dir(pnam)) return false;
	}
	return true;
}

//---------------------------------------------------------------------------
bool delete_Dir(UnicodeString dnam)
{
	return ::RemoveDirectory(cv_ex_filename(dnam).c_str());
}
//---------------------------------------------------------------------------
bool delete_Dirs(UnicodeString pnam)
{
	pnam = to_path_name(pnam);
	if (!dir_exists(pnam)) return false;

	TSearchRec sr;
	if (FindFirst(cv_ex_filename(UAPP_T(pnam, "*")), faAnyFile, sr)==0) {
		bool ok = true;
		do {
			if ((sr.Attr & faDirectory)==0 || ContainsStr("..", sr.Name)) continue;
			ok = delete_Dirs(pnam + sr.Name);
		} while(FindNext(sr)==0 && ok);
		FindClose(sr);
	}

	if (!set_FileWritable(pnam)) return false;
	return delete_Dir(pnam);
}

//---------------------------------------------------------------------------
bool move_File(UnicodeString old_nam, UnicodeString new_nam)
{
	return ::MoveFileEx(cv_ex_filename(old_nam).c_str(), cv_ex_filename(new_nam).c_str(),
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
}
//---------------------------------------------------------------------------
bool copy_File(UnicodeString src_nam, UnicodeString dst_nam)
{
	return ::CopyFile(cv_ex_filename(src_nam).c_str(), cv_ex_filename(dst_nam).c_str(), FALSE);
}

//---------------------------------------------------------------------------
bool rename_File(UnicodeString old_nam, UnicodeString new_nam)
{
	return ::MoveFile(cv_ex_filename(old_nam).c_str(), cv_ex_filename(new_nam).c_str());
}

//---------------------------------------------------------------------------
bool rename_Path(UnicodeString old_nam, UnicodeString new_nam)
{
	if (!SameText(get_root_name(old_nam),  get_root_name(new_nam))) return false;

	TStringDynArray o_plst = split_path(old_nam);
	TStringDynArray n_plst = split_path(new_nam);
	if (o_plst.Length!=n_plst.Length) return false;

	UnicodeString cur_dir = GetCurrentDir();

	bool ok = true;
	UnicodeString o_pnam, n_pnam;
	for (int i=0; i<o_plst.Length && ok; i++) {
		o_pnam += UAPP_T(o_plst[i], "\\");
		n_pnam += UAPP_T(n_plst[i], "\\");
		if (USAME_TS(o_pnam, "\\") || USAME_TS(o_pnam, "\\\\") || USAME_TS(n_pnam, "\\") || USAME_TS(n_pnam, "\\\\")) continue;
		if (!SameText(o_plst[i], n_plst[i])) {
			SetCurrentDir(get_parent_path(o_pnam));
			ok = rename_File(o_pnam, n_pnam);
		}
		o_pnam = n_pnam;
	}

	SetCurrentDir(cur_dir);

	return ok;
}

//---------------------------------------------------------------------------
UnicodeString get_ReparsePointTarget(
	UnicodeString pnam,
	unsigned int &tag)	//[o] IO_REPARSE_TAG_MOUNT_POINT = Junction
						//	  IO_REPARSE_TAG_SYMLINK	 = Symbolic Link
						//	  0 = なし
{
	tag = 0;

	HANDLE hDir = ::CreateFile(pnam.c_str(), GENERIC_READ,
						FILE_SHARE_READ, NULL, OPEN_EXISTING,
						FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hDir==INVALID_HANDLE_VALUE) return EmptyStr;

	std::unique_ptr<BYTE[]> dat_buf(new BYTE[MAXIMUM_REPARSE_DATA_BUFFER_SIZE]);
	::ZeroMemory(dat_buf.get(), MAXIMUM_REPARSE_DATA_BUFFER_SIZE);

	UnicodeString rnam;
	DWORD dat_sz;
	if (::DeviceIoControl(hDir, FSCTL_GET_REPARSE_POINT,
		NULL, 0, dat_buf.get(), MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dat_sz, NULL))
	{
		REPARSE_DATA_BUFFER *rp_buf = (_REPARSE_DATA_BUFFER*)dat_buf.get();
		if (IsReparseTagMicrosoft(rp_buf->ReparseTag)) {
			tag = rp_buf->ReparseTag;
			switch (tag) {
			case IO_REPARSE_TAG_MOUNT_POINT:	//ジャンクション
				rnam = (WCHAR*)rp_buf->MountPointReparseBuffer.PathBuffer
						+ rp_buf->MountPointReparseBuffer.SubstituteNameOffset/sizeof(WCHAR);
				break;
			case IO_REPARSE_TAG_SYMLINK:		//シンボリックリンク
				rnam = (WCHAR*)rp_buf->SymbolicLinkReparseBuffer.PathBuffer
						+ rp_buf->SymbolicLinkReparseBuffer.SubstituteNameOffset/sizeof(WCHAR);
				break;
			}
			remove_top_s(rnam, _T("\\??\\"));
		}
	}

	::CloseHandle(hDir);

	return rnam;
}

//---------------------------------------------------------------------------
bool cre_Junction(UnicodeString pnam, UnicodeString target)
{
	if (!::CreateDirectory(pnam.c_str(), NULL)) return false;

	int buf_size = MAX_PATH*sizeof(WCHAR) + sizeof(REPARSE_DATA_BUFFER);
	std::unique_ptr<BYTE[]> dat_buf(new BYTE[buf_size]);
	::ZeroMemory(dat_buf.get(), buf_size);
	REPARSE_DATA_BUFFER* reparse = (REPARSE_DATA_BUFFER*)dat_buf.get();

	target.Insert("\\??\\", 1);
	int path_len = target.Length() + 1;
	reparse->ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
	reparse->ReparseDataLength = (path_len + 2) * sizeof(WCHAR) + 6;
	reparse->MountPointReparseBuffer.SubstituteNameLength = (path_len-1) * sizeof(WCHAR);
	wcscpy((wchar_t *)reparse->MountPointReparseBuffer.PathBuffer, target.c_str());
	reparse->MountPointReparseBuffer.PrintNameOffset = path_len * sizeof(WCHAR);
	int data_len = reparse->ReparseDataLength + 8;

	HANDLE hDir = ::CreateFile(pnam.c_str(), GENERIC_WRITE,
						0, NULL, OPEN_EXISTING,
						FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hDir==INVALID_HANDLE_VALUE) return false;

	DWORD size;
	BOOL res = ::DeviceIoControl(hDir, FSCTL_SET_REPARSE_POINT, dat_buf.get(), data_len, NULL, 0, &size, NULL);
	::CloseHandle(hDir);

	if (res)
		return true;
	else {
		::RemoveDirectory(pnam.c_str());
		return false;
	}
}

//---------------------------------------------------------------------------
