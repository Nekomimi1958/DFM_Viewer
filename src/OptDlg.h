//----------------------------------------------------------------------//
// DFM Viewer															//
// Option Dialog														//
//----------------------------------------------------------------------//
#ifndef OptDlgH
#define OptDlgH
//----------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.SysUtils.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Graphics.hpp>

//----------------------------------------------------------------------------
class TOptionDlg : public TForm
{
__published:
	TButton *CancelBtn;
	TButton *OKBtn;
	TButton *RefColBtn;
	TButton *RefEditorBtn;
	TButton *RefFontBtn;
	TColorDialog *ColorDialog1;
	TComboBox *FontComboBox;
	TFontDialog *FontDialog1;
	TGroupBox *ColorGroupBox;
	TGroupBox *GroupBox1;
	TLabel *Label1;
	TLabeledEdit *EditorEdit;
	TLabeledEdit *EditPrmEdit;
	TListBox *ColorListBox;
	TOpenDialog *OpenDialog1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;

	void __fastcall FormShow(TObject *Sender);
	void __fastcall OKBtnClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall PageControl1DrawTab(TCustomTabControl *Control, int TabIndex, const TRect &Rect, bool Active);
	void __fastcall FontComboBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall ColorListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall RefFontBtnClick(TObject *Sender);
	void __fastcall RefColBtnClick(TObject *Sender);
	void __fastcall RefEditorBtnClick(TObject *Sender);

private:

public:
	virtual __fastcall TOptionDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TOptionDlg *OptionDlg;
//----------------------------------------------------------------------------
#endif    
