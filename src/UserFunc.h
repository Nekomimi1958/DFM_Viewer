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
UnicodeString conv_ValText(UnicodeString vstr, bool add_quot = false);

double GetLuminance(TColor col);
TColor RatioCol(TColor col, float rt);
TColor SelectWorB(TColor col, float rt = 1.0);
TColor AdjustColor(TColor col, int adj);
void alpha_blend_Rect(TCanvas *cv, int x, int y, int w, int h, TColor col, int alpha);
void alpha_blend_Rect(TCanvas *cv, TRect rc, TColor col, int alpha);

//---------------------------------------------------------------------------
#endif
