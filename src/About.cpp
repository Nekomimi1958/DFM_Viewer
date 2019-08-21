//----------------------------------------------------------------------//
// About Box															//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <memory>
#include "usr_dark.h"
#include "About.h"

//---------------------------------------------------------------------
#pragma resource "*.dfm"
TAboutBox *AboutBox = NULL;

//---------------------------------------------------------------------
__fastcall TAboutBox::TAboutBox(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAboutBox::FormCreate(TObject *Sender)
{
	unsigned mj, mi, bl;
	int vno = GetProductVersion(Application->ExeName, mj, mi, bl)? mj*100 + mi*10 + bl : 0;
	VersionLabel->Caption = UnicodeString().sprintf(_T("V%.2f (x86)"), vno/100.0);

	ProductName->Font->Assign(Application->DefaultFont);
	ProductName->Font->Size = 20;

	Copyright->Caption  = "2019 by Nekomimi";
	SupportURL->Caption = SUPPORT_URL;
}
//---------------------------------------------------------------------------
void __fastcall TAboutBox::FormShow(TObject *Sender)
{
	SetDarkWinTheme(this);
	SupportURL->Font->Color = IsDarkMode? TColor(RGB(0x66, 0x99, 0xFF)) : clNavy;
}
//---------------------------------------------------------------------------
void __fastcall TAboutBox::SupportURLClick(TObject *Sender)
{
	::ShellExecute(NULL, _T("open"), SUPPORT_URL, NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

