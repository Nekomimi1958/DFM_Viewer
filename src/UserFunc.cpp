//----------------------------------------------------------------------//
// Other Functions														//
//																		//
//----------------------------------------------------------------------//
#include <memory>
#include <System.StrUtils.hpp>
#include "usr_str.h"
#include "UserFunc.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
const _TCHAR *NnVCtrls =
		_T("TMainMenu|TMenuItem|TPopupMenu|TActionList|TAction|TApplicationEvents|TImageList|TTimer|TTrayIcon|")
		_T("TFileOpen|TFileOpenWith|TFileSaveAs|TFileExit|")
		_T("TEditCopy|TEditCut|TEditDelete|TEditPaste|TEditSelectAll|TEditUndo|")
		_T("THelpContents|THelpContextAction|THelpOnHelp|THelpTopicSearch");

//---------------------------------------------------------------------------
// Convert #nnnn to strings
//---------------------------------------------------------------------------
UnicodeString conv_ValText(
	UnicodeString vstr,
	bool add_quot)		// (default = false)
{
	UnicodeString sbuf;

	bool is_str = false;
	while (!vstr.IsEmpty()) {
		if (remove_top_s(vstr, '\'')) {
			sbuf += split_tkn(vstr, '\'');
			is_str = true;
		}
		else if (remove_top_s(vstr, '#')) {
			UnicodeString tmpstr = split_tkn(vstr, '\'');
			if (!vstr.IsEmpty()) vstr.Insert("\'", 1);
			TStringDynArray c_lst = SplitString(tmpstr, "#");
			UnicodeString tstr;
			for (int j=0; j<c_lst.Length; j++) {
				int c = c_lst[j].ToIntDef(0);
				switch (c) {
				case 0x0d: tstr += "\\r"; break;
				case 0x0a: tstr += "\\n"; break;
				default:   tstr += wchar_t(c);
				}
			}
			sbuf += tstr;
			is_str = true;
		}
		else {
			sbuf += vstr;
			break;
		}
	}

	if (is_str && add_quot) sbuf = "\'" + sbuf + "\'";
	return sbuf;
}

//---------------------------------------------------------------------------
double GetLuminance(TColor col)
{
	int c = ColorToRGB(col);
	return ((GetRValue(c)*0.3 + GetGValue(c)*0.59 + GetBValue(c)*0.11) / 255.0);
}
//---------------------------------------------------------------------------
TColor RatioCol(TColor col, float rt)
{
	if (rt>1.0) rt = 1.0; else if (rt<0.0) rt = 0.0;
	int c = ColorToRGB(col);
	int r = GetRValue(c) * rt;
	int g = GetGValue(c) * rt;
	int b = GetBValue(c) * rt;
	return TColor(RGB(r, g, b));
}
//---------------------------------------------------------------------------
TColor SelectWorB(TColor col, float  rt)
{
	double v = GetLuminance(col);
	if (rt<1.0)
		return (v>0.5)? RatioCol(clWhite, 1.0 - rt) : RatioCol(clWhite, rt);
	else
		return (v>0.5)? clBlack : clWhite;
}
//---------------------------------------------------------------------------
TColor AdjustColor(
	TColor col,
	int adj)		//0～255
{
	int cref = ColorToRGB(col);
	int r = GetRValue(cref);
	int g = GetGValue(cref);
	int b = GetBValue(cref);

	//明→暗
	if (GetLuminance(col)>0.5) {
		r -= (adj * r / 255);  if (r<0) r = 0;
		g -= (adj * g / 255);  if (g<0) g = 0;
		b -= (adj * b / 255);  if (b<0) b = 0;
	}
	//暗→明
	else {
		r += (adj * (255 - r) / 255);  if (r>255) r = 255;
		g += (adj * (255 - g) / 255);  if (g>255) g = 255;
		b += (adj * (255 - b) / 255);  if (b>255) b = 255;
	}
	return TColor(RGB(r, g, b));
}

//---------------------------------------------------------------------------
//長方形にアルファブレンド
//---------------------------------------------------------------------------
void alpha_blend_Rect(TCanvas *cv, int x, int y, int w, int h, TColor col, int alpha)
{
	BLENDFUNCTION blend_f;
	blend_f.BlendOp				= AC_SRC_OVER;
	blend_f.BlendFlags			= 0;
	blend_f.SourceConstantAlpha = alpha;
	blend_f.AlphaFormat			= 0;

	std::unique_ptr<Graphics::TBitmap> bp_b(new Graphics::TBitmap());
	bp_b->SetSize(8, 8);
	bp_b->Canvas->Brush->Style = bsSolid;
	bp_b->Canvas->Brush->Color = col;
	bp_b->Canvas->FillRect(TRect(0, 0, 8, 8));
	::AlphaBlend(cv->Handle, x, y, w, h, bp_b->Canvas->Handle, 0, 0, 8, 8, blend_f);
}
//---------------------------------------------------------------------------
void alpha_blend_Rect(TCanvas *cv, TRect rc, TColor col, int alpha)
{
	alpha_blend_Rect(cv, rc.Left, rc.Top, rc.Width(), rc.Height(), col, alpha);
}
//---------------------------------------------------------------------------
