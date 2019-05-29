//----------------------------------------------------------------------//
// DFM Viewer															//
// 																		//
//----------------------------------------------------------------------//
#ifndef Unit1H
#define Unit1H

//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.StrUtils.hpp>
#include <System.Actions.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.AppEvnts.hpp>
#include <Vcl.ActnMan.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ActnCtrls.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ActnMenus.hpp>
#include <Vcl.ToolWin.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.FileCtrl.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Mask.hpp>
#include <Vcl.Grids.hpp>
#include <inifiles.hpp>
#include <Vcl.StdActns.hpp>

//---------------------------------------------------------------------------
#define ACTCTG_NONE	_T("(none)")
#define ACTCTG_ALL	_T("(all)")

//---------------------------------------------------------------------------
class TDfmViewerForm : public TForm
{
__published:
	TAction *CopyAction;
	TAction *CopyValAction;
	TAction *FindObjAction;
	TAction *ToParentAction;
	TActionList *ActionList1;
	TComboBox *FindComboBox;
	TComboBox *SubComboBox;
	TComboBox *ClassComboBox;
	TLabel *FilterSttLabel2;
	TLabel *ObjSttLabel;
	TLabeledEdit *FilterEdit2;
	TListBox *ObjListBox;
	TListBox *PropListBox;
	TListBox *TextListBox;
	TMenuItem *PopCopyItem;
	TMenuItem *PopCopyValItem;
	TPageControl *MainPageControl;
	TPageControl *ViewPageControl;
	TPaintBox *ViewPaintBox;
	TPanel *FindOpePanel;
	TPanel *FindPanel1;
	TPanel *ImagePanel;
	TPanel *ListPanel;
	TPanel *MainPanel;
	TPanel *ObjFindPanel;
	TPanel *ObjSubPanel;
	TPopupMenu *PopupMenu1;
	TSpeedButton *SpeedButton1;
	TSplitter *Splitter1;
	TSplitter *Splitter2;
	TStatusBar *StatusBar1;
	TStringGrid *ResultGrid;
	TTabSheet *ListSheet;
	TTabSheet *ImageSheet;
	TTabSheet *ObjectSheet;
	TTabSheet *TextSheet;
	TLabel *Label1;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Setup1;
	TMenuItem *ReloadItem;
	TMenuItem *OptionsItem;
	TLabel *Label2;
	TFileExit *FileExit1;
	TMenuItem *ExitItem;
	TMenuItem *SelectProjectDirectory1;
	TMenuItem *Sep_1;
	TAction *EditSrcAction;
	TMenuItem *N1;
	TMenuItem *Editsource1;
	TMenuItem *Edit1;
	TMenuItem *EditSrcItem;
	TMenuItem *N2;
	TMenuItem *CopyItem;
	TMenuItem *CopyValueItem;

	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ObjListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ObjListBoxClick(TObject *Sender);
	void __fastcall PropListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall ViewPaintBoxPaint(TObject *Sender);
	void __fastcall ToParentActionExecute(TObject *Sender);
	void __fastcall ToParentActionUpdate(TObject *Sender);
	void __fastcall ObjListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall ObjListBoxKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall PropListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall PropListBoxKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall FindObjActionExecute(TObject *Sender);
	void __fastcall FindObjActionUpdate(TObject *Sender);
	void __fastcall FindComboBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FindComboBoxKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall FindComboBoxChange(TObject *Sender);
	void __fastcall CopyValActionExecute(TObject *Sender);
	void __fastcall CopyActionExecute(TObject *Sender);
	void __fastcall FindComboBoxClick(TObject *Sender);
	void __fastcall OptionBtnClick(TObject *Sender);
	void __fastcall ReloadBtnClick(TObject *Sender);
	void __fastcall ViewPaintBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall TextListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect,TOwnerDrawState State);
	void __fastcall PropListBoxDblClick(TObject *Sender);
	void __fastcall ObjListBoxDblClick(TObject *Sender);
	void __fastcall TextListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall TextListBoxDblClick(TObject *Sender);
	void __fastcall SubComboBoxClick(TObject *Sender);
	void __fastcall ListPanelResize(TObject *Sender);
	void __fastcall ResultGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect, TGridDrawState State);
	void __fastcall ClassComboBoxChange(TObject *Sender);
	void __fastcall FilterEdit2Change(TObject *Sender);
	void __fastcall ClassComboBoxClick(TObject *Sender);
	void __fastcall ResultGridClick(TObject *Sender);
	void __fastcall ResultGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall SelectProjectDirectory1Click(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall EditSrcActionExecute(TObject *Sender);
	void __fastcall EditSrcActionUpdate(TObject *Sender);
	void __fastcall CopyActionUpdate(TObject *Sender);
	void __fastcall CopyValActionUpdate(TObject *Sender);

private:
	TIniFile *IniFile;
	TStringList  *DfmFileList;
	UnicodeString DfmFileDir;
	UnicodeString DfmFileName;
	UnicodeString IniFormName;
	UnicodeString RootPath;
	UnicodeString CurrPath;
	UnicodeString ActivePath;
	TStringList  *ObjItemList;
	TStringList  *ObjIndexList;

	TStringList *AllPropNameList;
	TStringList *PropNameList;
	TStringList *DefNameList;
	TStringList *ClassNameList;
	TStringList *ClassFilterList;
	UnicodeString CurObjName;

	double FrameRatio;
	int MaxCtlWidth;
	TRect MaxRect;
	int CtlFontHi;

	int MaxObjWidth;
	int MaxPrpWidth;

	bool InhClassCombo;

	void  __fastcall WmMenuChar(TMessage &msg);

	void  __fastcall ClearObjList();
	void  __fastcall InitObjList();
	void  __fastcall InitObjFile(UnicodeString fnam);
	void  __fastcall UpdateObjList(UnicodeString pnam, UnicodeString onam = EmptyStr, UnicodeString act_ctg = EmptyStr);
	void  __fastcall InitResultGrid();
	UnicodeString __fastcall GetCaptionStr(TStringList *plst);
	void  __fastcall GetOffPos(int idx, int *x, int*y);
	TRect __fastcall GetObjRect(int idx);
	void  __fastcall DrawControl(TCanvas *cv, UnicodeString onam, TColor col, int level);

	void __fastcall AddColorList(UnicodeString id, UnicodeString dsc, TColor def) 
	{
		ColorValList->AddObject(id, (TObject*)IniFile->ReadInteger("Color", id, def));
		ColorNameList->Add(id + "=" + dsc);
	}
	TColor __fastcall GetOptCol(UnicodeString cnam)
	{
		int idx = ColorValList->IndexOf(cnam);
		return (idx!=-1)? (TColor)ColorValList->Objects[idx] : clNone;
	}

	TStringList* __fastcall GetPropList(UnicodeString obj_nam)
	{
		int idx = ObjItemList->IndexOf(obj_nam);
		return (idx!=-1)? (TStringList *)ObjItemList->Objects[idx] : NULL;
	}

	UnicodeString __fastcall GetAbsObjName(UnicodeString obj)
	{
		UnicodeString rnam = IncludeTrailingPathDelimiter(get_tkn(CurrPath, "\\"));
		for (int i=0; i<ObjItemList->Count; i++) {
			UnicodeString onam = ObjItemList->Strings[i];
			if (StartsStr(rnam, onam) && SameStr(ExtractFileName(onam), obj)) return onam;
		}
		return EmptyStr;
	}
	UnicodeString __fastcall GetAbsObjName(UnicodeString frm, UnicodeString obj)
	{
		for (int i=0; i<ObjItemList->Count; i++) {
			UnicodeString onam = ObjItemList->Strings[i];
			if (SameStr(get_tkn(onam, "\\"), frm) && SameStr(ExtractFileName(onam), obj)) return onam;
		}
		return EmptyStr;
	}

	bool __fastcall GetFormIndex(UnicodeString frm_nam, int *top_idx, int *end_idx)
	{
		int idx = ObjIndexList->IndexOf(frm_nam);  if (idx==-1) return false;
		int idx_hl = (int)ObjIndexList->Objects[idx];
		*top_idx = LOWORD(idx_hl);
		*end_idx = HIWORD(idx_hl);
		return (*top_idx>=0 && *end_idx>=*top_idx);
	}

	void __fastcall SaveResultColWidth();
	void __fastcall UpdateResultList();

	bool __fastcall OpenByTextEditor(UnicodeString fnam, int lno = 0);

public:
	TStringList *ColorValList;
	TStringList *ColorNameList;

	TFont *ListFont;

	UnicodeString Editor;
	UnicodeString EditorFormat;

	__fastcall TDfmViewerForm(TComponent* Owner);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_MENUCHAR,	TMessage,	WmMenuChar)
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TDfmViewerForm *DfmViewerForm;
//---------------------------------------------------------------------------
#endif
