//----------------------------------------------------------------------//
// DFM Viewer															//
// Option Dialog														//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop

#include "usr_str.h"
#include "usr_ctrl.h"
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
	FontComboBox->Items->Text =
		"リスト\r\n";
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormShow(TObject *Sender)
{
	set_ListBoxItemHi(ColorListBox, 1.25);

	EditorEdit->Text  = DfmViewerForm->Editor;
	EditPrmEdit->Text = DfmViewerForm->EditorFormat;

	ColorListBox->Items->Assign(DfmViewerForm->ColorValList);
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ColorListBoxDrawItem(TWinControl *Control, int Index,
	TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox *)Control;
	UnicodeString s = DfmViewerForm->ColorNameList->Values[lp->Items->Strings[Index]];
	draw_ColorItem((TColor)lp->Items->Objects[Index], s, lp->Canvas, Rect, State.Contains(odSelected));
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FontComboBoxDrawItem(TWinControl *Control, int Index,
	TRect &Rect, TOwnerDrawState State)
{
	TComboBox *cp = (TComboBox*)Control;
	draw_FontItem(NULL, cp->Items->Strings[Index], cp->Canvas, Rect, State.Contains(odSelected));
}

//---------------------------------------------------------------------------
//エディタの指定
//---------------------------------------------------------------------
void __fastcall TOptionDlg::RefEditorBtnClick(TObject *Sender)
{
	OpenDialog1->Title	  = "テキストエディタ";
	OpenDialog1->Filter   = "*.exe";
	OpenDialog1->FileName = "*.exe";
	if (OpenDialog1->Execute()) EditorEdit->Text = OpenDialog1->FileName;
}

//---------------------------------------------------------------------------
//フォント
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
//配色
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefColBtnClick(TObject *Sender)
{
	TListBox *lp = ColorListBox;
	int idx = lp->ItemIndex;
	if (idx!=-1) {
		ColorDialog1->Color = (TColor)lp->Items->Objects[idx];
		if (ColorDialog1->Execute()) {
			lp->Items->Objects[idx] = (TObject *)ColorDialog1->Color;
			lp->Repaint();
		}
	}
}

//---------------------------------------------------------------------------
//確定
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OKBtnClick(TObject *Sender)
{
	DfmViewerForm->ColorValList->Assign(ColorListBox->Items);

	DfmViewerForm->Editor		= EditorEdit->Text;
	DfmViewerForm->EditorFormat = EditPrmEdit->Text;
}
//---------------------------------------------------------------------------

