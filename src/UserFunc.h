//----------------------------------------------------------------------//
// Other Functions														//
//																		//
//----------------------------------------------------------------------//
#ifndef UsrFuncH
#define UsrFuncH

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

//---------------------------------------------------------------------------
extern const _TCHAR *NnVCtrls;

//---------------------------------------------------------------------------
UnicodeString conv_ValText(UnicodeString vstr, bool with_q = false);

double GetLuminance(TColor col);
TColor RatioCol(TColor col, float rt);
TColor SelectWorB(TColor col, float rt = 1.0);
TColor AdjustColor(TColor col, int adj);

bool open_by_TextEditor(UnicodeString fnam, int lno = 0);
//---------------------------------------------------------------------------
#endif
