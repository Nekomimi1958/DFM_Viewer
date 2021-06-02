//----------------------------------------------------------------------//
// DFM Viewer															//
// Option Dialog														//
//----------------------------------------------------------------------//
#include "usr_str.h"
#include "usr_ctrl.h"
#include "usr_dark.h"
#include "Unit1.h"
#include "OptDlg.h"

//---------------------------------------------------------------------
#pragma resource "*.dfm"
TOptionDlg *OptionDlg;

//--------------------------------------------------------------------- 
__fastcall TOptionDlg::TOptionDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormCreate(TObject *Sender)
{
	FontComboBox->Items->Text = "List\r\n";
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormShow(TObject *Sender)
{
	ColorListBox->Items->Assign(DfmViewerForm->ColorValList);

	SetDarkWinTheme(this);
	set_ListBoxItemHi(ColorListBox, 1.25);
	SetDarkWinTheme(ColorListBox);
	ColorListBox->Color = get_WinColor();

	EditorEdit->Text  = DfmViewerForm->Editor;
	EditPrmEdit->Text = DfmViewerForm->EditorFormat;
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::PageControl1DrawTab(TCustomTabControl *Control, int TabIndex,
	const TRect &Rect, bool Active)
{
	draw_OwnerTab(Control, TabIndex, Rect, Active, IsDarkMode);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ColorListBoxDrawItem(TWinControl *Control, int Index,
	TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox *)Control;
	UnicodeString s = DfmViewerForm->ColorNameList->Values[lp->Items->Strings[Index]];
	draw_OptColorItem((TColor)(int)lp->Items->Objects[Index], s, lp->Canvas, Rect, State.Contains(odSelected));
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FontComboBoxDrawItem(TWinControl *Control, int Index,
	TRect &Rect, TOwnerDrawState State)
{
	TComboBox *cp = (TComboBox*)Control;
	draw_OptFontItem(NULL, cp->Items->Strings[Index], cp->Canvas, Rect, State.Contains(odSelected));
}

//---------------------------------------------------------------------------
//Editor
//---------------------------------------------------------------------
void __fastcall TOptionDlg::RefEditorBtnClick(TObject *Sender)
{
	OpenDialog1->Title	  = "Text Editor";
	OpenDialog1->Filter   = "*.exe";
	OpenDialog1->FileName = "*.exe";
	if (OpenDialog1->Execute()) EditorEdit->Text = OpenDialog1->FileName;
}

//---------------------------------------------------------------------------
//Font
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefFontBtnClick(TObject *Sender)
{
	TComboBox *cp = FontComboBox;
	int idx = cp->ItemIndex;	if (idx==-1) return;

	switch (idx) {
	case 0: FontDialog1->Font->Assign(DfmViewerForm->ListFont);		break;
	}

	if (FontDialog1->Execute()) {
		switch (idx) {
		case 0:
			DfmViewerForm->ListFont->Assign(FontDialog1->Font);
			break;
		}
		cp->Repaint();
	}
}

//---------------------------------------------------------------------------
//Color
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefColBtnClick(TObject *Sender)
{
	TListBox *lp = ColorListBox;
	int idx = lp->ItemIndex;
	if (idx!=-1) {
		ColorDialog1->Color = (TColor)(int)lp->Items->Objects[idx];
		if (ColorDialog1->Execute()) {
			lp->Items->Objects[idx] = (TObject *)ColorDialog1->Color;
			lp->Repaint();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OKBtnClick(TObject *Sender)
{
	DfmViewerForm->ColorValList->Assign(ColorListBox->Items);

	DfmViewerForm->Editor		= EditorEdit->Text;
	DfmViewerForm->EditorFormat = EditPrmEdit->Text;
}
//---------------------------------------------------------------------------

