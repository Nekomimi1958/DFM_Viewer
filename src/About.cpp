//----------------------------------------------------------------------//
// About Box															//
//----------------------------------------------------------------------//
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
	ProductName->Font->Assign(Application->DefaultFont);
	ProductName->Font->Size = 20;
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

