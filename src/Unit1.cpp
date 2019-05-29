//----------------------------------------------------------------------//
// DFM Viewer															//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <algorithm>
#include <memory>
#include <System.IOUtils.hpp>
#include <RegularExpressions.hpp>
#include <Vcl.FileCtrl.hpp>
#include <Vcl.Clipbrd.hpp>

#include "usr_env.h"
#include "usr_str.h"
#include "usr_key.h"
#include "usr_ctrl.h"
#include "usr_file_ex.h"
#include "UserFunc.h"
#include "OptDlg.h"
#include "About.h"
#include "Unit1.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDfmViewerForm *DfmViewerForm;

//---------------------------------------------------------------------------
//Comparison Function for Sorting (TabOrder)
//---------------------------------------------------------------------------
int __fastcall SortComp_TabOrder(TStringList *List, int Index1, int Index2)
{
	if (EndsStr("..", List->Strings[Index1])) return -1;
	if (EndsStr("..", List->Strings[Index2])) return  1;

	TStringList *sp0 = (TStringList *)List->Objects[Index1];
	TStringList *sp1 = (TStringList *)List->Objects[Index2];
	int tag0 = sp0->Values["TabOrder"].ToIntDef(999);
	int tag1 = sp1->Values["TabOrder"].ToIntDef(999);

	if (tag0==tag1) {
		UnicodeString cls0 = sp0->Values["Class"];
		UnicodeString cls1 = sp1->Values["Class"];
		bool nonv0 = contained_wd_i(NnVCtrls, cls0);
		bool nonv1 = contained_wd_i(NnVCtrls, cls1);
		if (nonv0!=nonv1) {
			if (nonv0) return  1;
			if (nonv1) return -1;
		}

		if (SameStr(cls0, cls1) && SameStr(cls0, "TMenuItem")) return (Index1 - Index2);

		return StrCmpLogicalW(sp0->Values["Name"].c_str(), sp1->Values["Name"].c_str());
	}

	return (tag0 - tag1);
}
//---------------------------------------------------------------------------
//Comparison Function for Sorting (Property Name)
//---------------------------------------------------------------------------
int __fastcall SortComp_PropName(TStringList *List, int Index1, int Index2)
{
	UnicodeString nam0 = List->Strings[Index1];
	UnicodeString nam1 = List->Strings[Index2];

	std::unique_ptr<TStringList> nlst(new TStringList());
	nlst->Text = 
		"Form\n"
		"Name\n"
		"Caption\n"
		"EditLabel.Caption\n"
		"Hint\n"
		"Left\n"
		"Top\n"
		"Width\n"
		"Height\n"
		"TabOrder\n"
		"Tag\n";

	int idx0 = nlst->IndexOf(nam0);
	int idx1 = nlst->IndexOf(nam1);

	if (idx0==-1 && idx1==-1) return StrCmpLogicalW(nam0.c_str(), nam1.c_str());

	if (idx0==-1) return 1;
	if (idx1==-1) return -1;

	return (idx0 - idx1);
}

//---------------------------------------------------------------------------
// TDfmViewerForm Class
//---------------------------------------------------------------------------
__fastcall TDfmViewerForm::TDfmViewerForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FormCreate(TObject *Sender)
{
	IniFile = new TIniFile(ChangeFileExt(Application->ExeName, ".INI"));

	DfmFileList 	= new TStringList();
	ObjItemList 	= new TStringList();
	ObjIndexList	= new TStringList();
	ClassNameList	= new TStringList();
	ClassFilterList = new TStringList();
	AllPropNameList	= new TStringList();
	PropNameList 	= new TStringList();

	DefNameList = new TStringList();
	DefNameList->Text =
		"Form\n"
		"Name\n";

	FrameRatio = 1.0;
	InhClassCombo = false;

	//Startup Option
	for (int i=1; !ParamStr(i).IsEmpty(); i++) {
		UnicodeString prmbuf = ParamStr(i).Trim();
		if (prmbuf.SubString(1,1)=="-") {
			;
		}
		else {
			if (EndsStr("\\", prmbuf)) {
				DfmFileName = EmptyStr;
				DfmFileDir	= ExcludeTrailingPathDelimiter(prmbuf);
			}
			else {
				DfmFileName = prmbuf;
				DfmFileDir	= ExtractFileDir(DfmFileName);
			}
		}
	}

	if (DfmFileDir.IsEmpty()) DfmFileDir = IncludeTrailingPathDelimiter(GetCurrentDir());
	get_files(DfmFileDir, _T("*.dfm"), DfmFileList);

	if (DfmFileList->IndexOf(DfmFileName)!=-1) {
		std::unique_ptr<TStringList> fbuf(new TStringList());
		fbuf->LoadFromFile(DfmFileName);
		for (int i=0; i<fbuf->Count; i++) {
			UnicodeString lbuf = Trim(fbuf->Strings[i]);
			if (StartsText("object", lbuf)) {
				IniFormName = get_tkn_m(lbuf, ' ', ':');
				break;
			}
		}
	}

	Caption = Application->Title + " - " + ReplaceStr(DfmFileDir, "\\", "/");
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FormShow(TObject *Sender)
{
	load_form_pos(this, IniFile, 800,600);

	UnicodeString sct = "General";
	ListPanel->Width	= IniFile->ReadInteger(sct, "ListPanelWidth",	200);
	PropListBox->Height = IniFile->ReadInteger(sct, "PropListHeight",	100);

	sct = "Option";
	Editor		 = IniFile->ReadString(sct, "Editor",		EmptyStr);
	EditorFormat = IniFile->ReadString(sct, "EditorFormat",	"$F");

	//Color
	ColorValList  = new TStringList();
	ColorNameList = new TStringList();
	AddColorList("bgList", "List Background",		clBlack);
	AddColorList("fgList", "List Text",				clWhite);
	AddColorList("bgHead", "Header Background",		clBtnFace);
	AddColorList("fgHead", "Grid Header",			clBtnText);
	AddColorList("fgNmbr", "Numeric Value",			clSkyBlue);
	AddColorList("fgText", "Strings",				clAqua);
	AddColorList("fgInvl", "Invalid Item",			clDkGray);
	AddColorList("fgNnvs", "Unvisual Item",			clLtGray);
	AddColorList("fgName", "Item Name",				clAqua);
	AddColorList("LnCurs", "Cursor Line",			clFuchsia);
	AddColorList("bgImgV", "Image Background",		clBlack);
	AddColorList("fgActv", "Active Control",		clOlive);
	AddColorList("fgPrnt", "Parent Control",		clGreen);
	AddColorList("fgSame", "Same Level Controls",	clOlive);
	AddColorList("fgChld", "Child Contols",			clSkyBlue);
	AddColorList("fgCtrl", "Control Symbol",		clSilver);
	AddColorList("fgLabl", "Label",					clWhite);

	ObjListBox->Color  = GetOptCol("bgList");
	PropListBox->Color = GetOptCol("bgList");
	TextListBox->Color = GetOptCol("bgList");
	ResultGrid->Color  = GetOptCol("bgList");
	ImagePanel->Color  = GetOptCol("bgImgV");

	//Font
	ListFont = load_font_inf(IniFile, "ObjList",	Font);
	set_ListBoxItemHi(ObjListBox,  ListFont);
	set_ListBoxItemHi(PropListBox, ListFont);
	set_ListBoxItemHi(TextListBox, ListFont);
	ResultGrid->Font->Assign(ListFont);

	ViewPaintBox->Canvas->Font->Assign(Application->DefaultFont);
	ViewPaintBox->Canvas->Font->Size = 8;
	CtlFontHi = ViewPaintBox->Canvas->TextHeight("Q");

	InitObjList();
	PropNameList->Sort();
	AllPropNameList->Assign(DefNameList);
	AllPropNameList->AddStrings(PropNameList);
	PropNameList->CustomSort(SortComp_PropName);

	for (int i=0; i<AllPropNameList->Count; i++) {
		AllPropNameList->Objects[i]
			= (TObject*)(NativeInt)IniFile->ReadInteger("GridColWidth", AllPropNameList->Strings[i], 60);
	}

	InitResultGrid();
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	save_form_pos(this, IniFile);

	save_font_inf(IniFile, "ObjList",	ListFont);

	SaveResultColWidth();
	for (int i=0; i<AllPropNameList->Count; i++) {
		IniFile->WriteInteger("GridColWidth", AllPropNameList->Strings[i],	(int)AllPropNameList->Objects[i]);
	}

	UnicodeString sct = "General";
	IniFile->WriteInteger(sct, "ListPanelWidth",	ListPanel->Width);
	IniFile->WriteInteger(sct, "PropListHeight",	PropListBox->Height);

	sct = "Option";
	IniFile->WriteString(sct,  "Editor",		Editor);
	IniFile->WriteString(sct,  "EditorFormat",	EditorFormat);

	for (int i=0; i<ColorValList->Count; i++) {
		IniFile->WriteInteger("Color", ColorValList->Strings[i], (int)ColorValList->Objects[i]);
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FormDestroy(TObject *Sender)
{
	ClearObjList();
	delete ObjItemList;
	delete DfmFileList;
	delete ColorValList;
	delete ColorNameList;
	delete ClassFilterList;
	delete AllPropNameList;
	delete PropNameList;
	delete DefNameList;
	delete ClassNameList;
	delete IniFile;
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FormResize(TObject *Sender)
{
	StatusBar1->Panels->Items[0]->Width = ClientWidth * 2/3;
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::WmMenuChar(TMessage &msg)
{
	if (msg.WParamHi==MF_POPUP)
		TForm::Dispatch(&msg);
	else
		msg.Result = (MNC_CLOSE << 16);
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ListPanelResize(TObject *Sender)
{
	ObjListBox->Repaint();
	PropListBox->Repaint();
}

//---------------------------------------------------------------------------
void  __fastcall TDfmViewerForm::InitResultGrid()
{
	TStringGrid *gp = ResultGrid;
	if (!ClassComboBox->Text.IsEmpty()) {
		gp->ColCount = PropNameList->Count + 1;
		for (int i=0; i<PropNameList->Count; i++) gp->Cells[i][0] = PropNameList->Strings[i];
	}
	else {
		gp->ColCount = DefNameList->Count + 1;
		for (int i=0; i<DefNameList->Count; i++) gp->Cells[i][0] = DefNameList->Strings[i];
	}

	for (int i=0; i<gp->ColCount; i++) {
		int idx = AllPropNameList->IndexOf(gp->Cells[i][0]);
		gp->ColWidths[i] = (idx!=-1)? (int)AllPropNameList->Objects[idx] : 60;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ClearObjList()
{
	for (int i=0; i<ObjItemList->Count; i++) {
		delete (TStringList*)ObjItemList->Objects[i];
	}
	ObjItemList->Clear();
	ObjIndexList->Clear();

	RootPath = CurrPath = EmptyStr;
	MaxRect  = Rect(0,0,0,0);

	ClassNameList->Clear();

	MaxPrpWidth = 100;
	MaxObjWidth = 100;
}

//---------------------------------------------------------------------------
//Initialize Object List
//---------------------------------------------------------------------------
void  __fastcall TDfmViewerForm::InitObjList()
{
	Screen->Cursor = crHourGlass;

	ClearObjList();
	for (int i=0; i<DfmFileList->Count; i++) InitObjFile(DfmFileList->Strings[i]);
	MaxPrpWidth += 16;

	ClassNameList->Sort();
	ClassComboBox->Items->Assign(ClassNameList);

	UpdateObjList(EmptyStr, IniFormName);

	Screen->Cursor = crDefault;
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::InitObjFile(UnicodeString fnam)
{
	try {
		std::unique_ptr<TStringList> fbuf(new TStringList());
		fbuf->LoadFromFile(fnam);
		UnicodeString cur_path;
		TStringList *p_lst = NULL;
		int  nn_idx = 0;		//Index of Unnamed Object
		int  pg_idx = 0;		//PageIndex
		bool skip1 = false;		// ( )
		bool skip2 = false;		// < >
		bool skip3 = false;		// { }
		int  top_idx = -1;
		int  end_idx = -1;

		UnicodeString tmp;
		for (int i=0; i<fbuf->Count; i++) {
			UnicodeString lbuf = Trim(fbuf->Strings[i]);
			if (SameText(get_tkn(lbuf, " "), "object")) {
				lbuf = get_tkn_r(lbuf, " ");
				p_lst = new TStringList();
				UnicodeString cls;
				if (lbuf.Pos(":")) {
					cls = get_tkn_r(lbuf, ":").Trim();
					lbuf = get_tkn(lbuf, ":");
				}
				else {
					cls = Trim(lbuf);
					lbuf = UnicodeString().sprintf(_T("<Unnamed-%u>"), ++nn_idx);
				}

				p_lst->Add("Class=" + cls);
				if (ClassNameList->IndexOf(cls)==-1) ClassNameList->Add(cls);
				p_lst->Add("Name=" + lbuf);

				if (!cur_path.IsEmpty()) cur_path += "\\";
				cur_path += lbuf;
				p_lst->Insert(0, "ObjectPath=" + cur_path);
				p_lst->Insert(0, tmp.sprintf(_T("FormFile=%s\t(%u)"), fnam.c_str(), i + 1));

				if (SameStr(cls, "TPageControl")) {
					pg_idx = 0;
				}
				else if (SameStr(cls, "TTabSheet")) {
					p_lst->Add(tmp.sprintf(_T("PageIndex=%u"), pg_idx++));
				}

				end_idx = ObjItemList->AddObject(cur_path, (TObject*)p_lst);
				if (top_idx==-1) top_idx = end_idx;
			}
			else if (!skip2 && SameText(lbuf, "end")) {
				p_lst	 = NULL;
				cur_path = ExtractFileDir(cur_path);
			}
			else if (p_lst) {
				UnicodeString vnam = Trim(get_tkn(lbuf, '='));
				UnicodeString vstr = Trim(get_tkn_r(lbuf, '='));
				if (vstr=="(") {
					skip1 = true;
				}
				else if (skip1 && EndsStr(")", lbuf)) {
					skip1 = false;
				}
				else if (vstr=="<") {
					skip2 = true;
				}
				else if (skip2 && EndsStr(">", lbuf)) {
					skip2 = false;
				}
				else if (vstr=="{") {
					skip3 = true;
				}
				else if (skip3 && EndsStr("}", lbuf)) {
					skip3 = false;
				}
				else if (skip1 || skip2 || skip3) {
					continue;
				}
				else if (!vnam.IsEmpty() && !vstr.IsEmpty()) {
					p_lst->Add(vnam + "=" + conv_ValText(vstr));
					if (PropNameList->IndexOf(vnam)==-1) PropNameList->Add(vnam);
					MaxPrpWidth = std::max(ObjListBox->Canvas->TextWidth(vnam), MaxPrpWidth);
				}
			}
		}

		if (top_idx!=-1) {
			ObjIndexList->AddObject(ObjItemList->Strings[top_idx], (TObject *)MAKELONG(top_idx, end_idx));
		}

		for (int i=top_idx; i<ObjItemList->Count; i++) {
			TStringList *plst = (TStringList *)ObjItemList->Objects[i];
			if (!contained_wd_i(NnVCtrls, plst->Values["Class"])) {
				TRect rc = Rect(0,0,0,0);
				UnicodeString w_str = plst->Values["Width"];
				if (w_str.IsEmpty()) w_str = plst->Values["ClientWidth"];
				UnicodeString h_str = plst->Values["Height"];
				if (h_str.IsEmpty()) h_str = plst->Values["ClientHeight"];
				rc.SetWidth( w_str.ToIntDef(0) * FrameRatio);
				rc.SetHeight(h_str.ToIntDef(0) * FrameRatio);
				MaxRect.Union(rc);
			}

			//Child count
			int ch_cnt = 0;
			UnicodeString onam = ObjItemList->Strings[i];
			for (int j=i+1; j<ObjItemList->Count; j++) {
				if (SameStr(onam, ExtractFileDir(ObjItemList->Strings[j]))) ch_cnt++;
			}
			plst->Insert(2,UnicodeString().sprintf(_T("ChildCount=%u"), ch_cnt));
		}
	}
	catch (...) {
		ClearObjList();
	}
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::UpdateObjList(
	UnicodeString pnam, 	//Path Name
	UnicodeString onam,		//Object Name
	UnicodeString act_ctg)	//Action.Category
{
	CurrPath = pnam;
	UnicodeString frm_nam = get_tkn(CurrPath, "\\");
	bool is_root = frm_nam.IsEmpty();
	int top_idx, end_idx;
	if (is_root) {
		top_idx = 0;
		end_idx = ObjItemList->Count - 1;
	}
	else {
		if (!GetFormIndex(get_tkn(CurrPath, "\\"), &top_idx, &end_idx)) return;
	}

	TStringList *parent_plst = GetPropList(CurrPath);
	bool is_act_lst = (parent_plst && SameStr(parent_plst->Values["Class"], "TActionList"));
	std::unique_ptr<TStringList> ctg_lst(new TStringList());
	if (is_act_lst) ctg_lst->Add(ACTCTG_ALL);

	std::unique_ptr<TStringList> o_lst(new TStringList());
	TListBox *lp = ObjListBox;
	MaxObjWidth = 100;
	int act_cnt = 0;
	for (int i=top_idx; i<=end_idx; i++) {
		TStringList  *plst = (TStringList *)ObjItemList->Objects[i];
		if (SameStr(pnam, ExtractFileDir(plst->Values["ObjectPath"]))) {
			if (is_act_lst) {
				act_cnt++;
				UnicodeString ctg = plst->Values["Category"];
				if (ctg.IsEmpty()) ctg = ACTCTG_NONE;
				if (act_ctg.IsEmpty()) {
					if (ctg_lst->IndexOf(ctg)==-1) ctg_lst->Add(ctg);
				}
				else {
					if (!SameStr(act_ctg, ACTCTG_ALL) && !SameStr(ctg, act_ctg)) continue;
				}
			}
			o_lst->AddObject(ObjItemList->Strings[i], (TObject *)plst);
			MaxObjWidth = std::max(lp->Canvas->TextWidth(plst->Values["Name"]), MaxObjWidth);
		}
	}
	MaxObjWidth += 40;

	SubComboBox->Enabled = false;
	if (is_act_lst) {
		if (act_ctg.IsEmpty()) {
			ctg_lst->Sort();
			SubComboBox->Items->Assign(ctg_lst.get());
			SubComboBox->ItemIndex = 0;
		}
		ObjSttLabel->Caption = UnicodeString().sprintf(_T("%3u/%u"), o_lst->Count, act_cnt);
		SubComboBox->Enabled = true;
	}
	else {
		SubComboBox->Clear();
		ObjSttLabel->Caption = EmptyStr;
	}

	//[..] Item
	if (!CurrPath.IsEmpty()) {
		TStringList *plst = new TStringList();
		plst->Add("Name=" + ExtractFileName(pnam));
		plst->Add("ObjectPath=" + pnam);
		plst->Add(UnicodeString().sprintf(_T("ChildCount=%u"), o_lst->Count));
		o_lst->InsertObject(0, pnam + "\\..", (TObject*)plst);
	}

	o_lst->CustomSort(SortComp_TabOrder);
	lp->Items->BeginUpdate();
		lp->Items->Assign(o_lst.get());
		if (!pnam.IsEmpty()) onam = IncludeTrailingPathDelimiter(pnam) + onam;
		lp->ItemIndex = !onam.IsEmpty()? std::max(lp->Items->IndexOf(onam), 0) : 0;
	lp->Items->EndUpdate();
	if (MainPageControl->ActivePage==ObjectSheet) lp->SetFocus();
	ObjListBoxClick(NULL);
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::SaveResultColWidth()
{
	TStringGrid *gp = ResultGrid;
	UnicodeString sct = "GridColWidth";
	for (int i=0; i<gp->ColCount; i++) {
		int idx = AllPropNameList->IndexOf(gp->Cells[i][0]);
		if (idx!=-1) AllPropNameList->Objects[idx] = (TObject*)(NativeInt)gp->ColWidths[i];
	}
}

//---------------------------------------------------------------------------
//Update Result List
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::UpdateResultList()
{
	SaveResultColWidth();

	TStringGrid *gp = ResultGrid;
	UnicodeString cls_str = ClassComboBox->Text;
	if (cls_str.IsEmpty()) {
		gp->RowCount = 2;
		for (int j=0; j<PropNameList->Count; j++) gp->Cells[j][1] = EmptyStr;
		return;
	}

	Screen->Cursor = crHourGlass;

	ClassFilterList->Clear();
	for (int i=0; i<ObjItemList->Count; i++) {
		TStringList *plst = (TStringList *)ObjItemList->Objects[i];
		UnicodeString cls = plst->Values["Class"];
		if (SameStr(cls, cls_str)) {
			ClassFilterList->AddObject(ObjItemList->Strings[i], ObjItemList->Objects[i]);
		}
	}

	//Filter
	int total_cnt = ClassFilterList->Count;
	UnicodeString kwd = FilterEdit2->Text;
	if (!kwd.IsEmpty()) {
		int i = 0;
		while (i<ClassFilterList->Count) {
			TStringList *plst = (TStringList *)ClassFilterList->Objects[i];
			if (!ContainsText(plst->Text, kwd)) ClassFilterList->Delete(i); else i++;
		}
	}
	FilterSttLabel2->Caption = UnicodeString().sprintf(_T("%3u/%3u"), ClassFilterList->Count, total_cnt);

	if (ClassFilterList->Count>0) {
		std::unique_ptr<TStringList> pbuf(new TStringList());
		for (int i=0; i<ClassFilterList->Count; i++) {
			TStringList *plst = (TStringList *)ClassFilterList->Objects[i];
			for (int j=5; j<plst->Count; j++) {	//***
				UnicodeString nam = plst->Names[j];
				if (pbuf->IndexOf(nam)==-1) pbuf->Add(nam);
			}
		}
		pbuf->Sort();
		PropNameList->Assign(DefNameList);
		PropNameList->AddStrings(pbuf.get());
		PropNameList->CustomSort(SortComp_PropName);
		InitResultGrid();

		gp->RowCount = ClassFilterList->Count + 1;
		int max_wd0 = 60;
		int max_wd1 = 60;
		for (int i=0; i<ClassFilterList->Count; i++) {
			gp->Cells[0][i + 1] = get_tkn(ClassFilterList->Strings[i], "\\");
			max_wd0 = std::max(max_wd0, gp->Canvas->TextWidth(gp->Cells[0][i + 1]));

			TStringList *plst = (TStringList *)ClassFilterList->Objects[i];
			for (int j=1; j<PropNameList->Count; j++) {
				gp->Cells[j][i + 1] = plst->Values[PropNameList->Strings[j]];
				if (j==1) max_wd1 = std::max(max_wd1, gp->Canvas->TextWidth(gp->Cells[1][i + 1]));
			}
		}
		gp->ColWidths[0] = max_wd0 + 12;
		gp->ColWidths[1] = max_wd1 + 12;
	}
	else {
		gp->RowCount = 2;
		for (int i=0; i<gp->ColCount; i++) gp->Cells[i][1] = EmptyStr;
	}

	int row = 1;
	if (!CurObjName.IsEmpty()) {
		for (int i=1; i<gp->RowCount; i++) {
			if (gp->Cells[1][i]==CurObjName) {
				row = i; break;
			}
		}
	}
	gp->Row = row;

	Screen->Cursor = crDefault;
}

//---------------------------------------------------------------------------
UnicodeString __fastcall TDfmViewerForm::GetCaptionStr(TStringList *plst)
{
	UnicodeString ret_str;
	if (plst) {
		ret_str = plst->Values["Caption"];
		if (ret_str.IsEmpty()) {
			ret_str = plst->Values["EditLabel.Caption"];
		}
		if (ret_str.IsEmpty()) {
			UnicodeString onam = GetAbsObjName(plst->Values["Action"]);
			if (!onam.IsEmpty()) {
				TStringList *pp = GetPropList(onam);
				if (pp) ret_str = pp->Values["Caption"];
			}
		}
	}
	return ret_str;
}

//---------------------------------------------------------------------------
//Draw the Object List
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ObjListBoxDrawItem(TWinControl *Control, int Index,
	TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas *cv  = lp->Canvas;
	cv->Font->Assign(lp->Font);
	cv->Brush->Color = GetOptCol("bgList");
	cv->FillRect(Rect);

	int xp = Rect.Left + 4;
	int yp = Rect.Top + 1;

	TStringList  *plst = (TStringList *)lp->Items->Objects[Index];
	if (plst) {
		UnicodeString cls = plst->Values["Class"];
		if (SameStr(cls, "TPopupMenu")) {
			cv->Brush->Color = GetOptCol("fgCtrl");
			TRect m_rc = Rect;
			m_rc.Left = xp + 4;
			m_rc.Top  = Rect.Top + (Rect.Height() - 16)/2;
			m_rc.SetWidth(8);
			m_rc.SetHeight(14);
			cv->FrameRect(m_rc);
		}
		else if (SameStr(cls, "TMenuItem")) {
			cv->Brush->Color = GetOptCol("fgCtrl");
			TRect m_rc = Rect;
			m_rc.Left = xp + 2;
			m_rc.Top  = Rect.Top + (Rect.Height() - 8)/2;
			m_rc.SetWidth(14);
			m_rc.SetHeight(8);
			cv->FrameRect(m_rc);
		}
		else {
			UnicodeString cmk;
			if		(SameStr(cls, "TCheckBox"))		cmk = _T("\u2611");
			else if (SameStr(cls, "TRadioButton"))	cmk = "Åù";
			else if (SameStr(cls, "TTimer"))		cmk = "Çs";
			else 									cmk = "Å@";
			cv->Font->Color = GetOptCol("fgCtrl");
			cv->TextOut(xp, yp, cmk);
		}

		xp += 20;
		cv->Brush->Color = GetOptCol("bgList");
		cv->Font->Color  = GetOptCol(contained_wd_i(NnVCtrls, plst->Values["Class"])? "fgNnvs" : "fgList");
		cv->TextOut(xp, yp, ExtractFileName(lp->Items->Strings[Index]));
		xp += MaxObjWidth;

		//Number of child controls
		int ch_cnt = plst->Values["ChildCount"].ToIntDef(0);
		if (ch_cnt>0) {
			cv->Font->Color = GetOptCol("fgNmbr");
			cv->TextOut(xp, yp, UnicodeString().sprintf(_T("%3u"), ch_cnt));
		}
		else {
			cv->Font->Color = GetOptCol("fgInvl");
			cv->TextOut(xp, yp, "___");
		}
		xp += get_CharWidth(cv, 4);

		//TabOrder
		int idx = plst->Values["TabOrder"].ToIntDef(-1);
		if (idx<0) idx = plst->Values["PageIndex"].ToIntDef(-1);
		if (idx>=0) {
			cv->Font->Color = GetOptCol("fgNmbr");
			cv->TextOut(xp, yp, UnicodeString().sprintf(_T("%3u"), idx));
		}
		xp += get_CharWidth(cv, 5);

		//Caption
		UnicodeString s = GetCaptionStr(plst);
		if (!s.IsEmpty()) {
			if (SameStr(s, "-") && SameStr(cls, "TMenuItem")) {
				TRect s_rc = Rect; s_rc.Left = xp - 8;
				draw_Separator(cv, s_rc);
			}
			else {
				cv->Font->Color = GetOptCol("fgText");
				cv->TextOut(xp, yp, s);
			}
		}
	}

	draw_ListCursor(lp, Rect, Index, State, GetOptCol("LnCurs"), 2);
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ObjListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	TListBox *lp = ObjListBox;
	int idx = lp->ItemIndex;
	if (idx==-1) return;

	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	if (equal_ENTER(KeyStr)) {
		UnicodeString onam = lp->Items->Strings[idx];
		UnicodeString lbuf = ExtractFileName(onam);
		if (SameStr(lbuf, "..")) {
			UpdateObjList(ExtractFileDir(CurrPath), ExtractFileName(CurrPath));
		}
		else {
			int ch_cnt = 0;
			for (int i=0; i<ObjItemList->Count; i++) {
				if (SameStr(onam, ExtractFileDir(ObjItemList->Strings[i]))) ch_cnt++;
			}
			if (ch_cnt>0) {
				if (!CurrPath.IsEmpty()) CurrPath += "\\";
				UpdateObjList(CurrPath + lbuf);
			}
		}
	}
	else if (SameText(KeyStr, "U") || SameText(KeyStr, "BKSP")) {
		ToParentAction->Execute();
	}
	else if (SameText(KeyStr, "J")) {
		Key = VK_DOWN;
	}
	else if (SameText(KeyStr, "K")) {
		Key = VK_UP;
	}
	else if (SameText(KeyStr, "P")) {
		Key = VK_PRIOR;
	}
	else if (SameText(KeyStr, "N")) {
		Key = VK_NEXT;
	}
	else if (SameText(KeyStr, "Alt+F")) {
		if ( FindComboBox->Enabled) FindComboBox->SetFocus();
	}
	else if (remove_top_s(KeyStr, KeyStr_Ctrl)) {
		if (KeyStr.Length()==1 && _istalpha(KeyStr[1])) {
			int idx0 = -1, idx1 = -1;
			for (int i=0; i<lp->Count && idx1==-1; i++) {
				if (i<=idx && idx0!=-1) continue;
				if (StartsText(KeyStr, ExtractFileName(lp->Items->Strings[i]))) {
					if (i<=idx) idx0 = i; else idx1 = i;
				}
			}
			idx = (idx1!=-1)? idx1 : idx0;
			if (idx!=-1) {
				lp->ItemIndex = idx;
				ObjListBoxClick(NULL);
			}
		}
	}
	else if (SameText(KeyStr, "L") || SameText(KeyStr, "RIGHT")) {
		if (ViewPageControl->ActivePage==TextSheet) TextListBox->SetFocus();
		Key = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ObjListBoxKeyPress(TObject *Sender, System::WideChar &Key)
{
	if ((_istalnum(Key) || Key==VK_SPACE || Key==VK_RETURN || is_KeyDown(VK_CONTROL))) Key = 0;
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ObjListBoxClick(TObject *Sender)
{
	TListBox *o_lp = ObjListBox;
	TListBox *p_lp = PropListBox;
	if (o_lp->ItemIndex!=-1) {
		int idx = o_lp->ItemIndex;
		ActivePath = o_lp->Items->Strings[idx];
		StatusBar1->Panels->Items[0]->Text = ReplaceStr(ActivePath, "\\", "/");

		TStringList *plst = (TStringList *)o_lp->Items->Objects[idx];
		if (plst) {
			p_lp->Items->Assign(plst);
			p_lp->ItemIndex = 0;

			UnicodeString lbuf = plst->Values["FormFile"];
			UnicodeString fnam = get_pre_tab(lbuf);
			int lno = get_in_paren(get_post_tab(lbuf)).ToIntDef(1);
			TListBox *t_lp = TextListBox;
			if (CurrPath.IsEmpty() && !fnam.IsEmpty()) {
				try {
					DfmFileName = fnam;
					t_lp->Items->LoadFromFile(DfmFileName);
				}
				catch (...) {
					DfmFileName = EmptyStr;
					t_lp->Clear();
				}
			}

			if (ViewPageControl->ActivePage==TextSheet && !DfmFileName.IsEmpty()) {
				ListBoxSetIndex(t_lp, lno - 1);
				t_lp->Invalidate();
			}
		}
	}
	else {
		StatusBar1->Panels->Items[0]->Text =EmptyStr;
		p_lp->Items->Clear();
	}

	ViewPaintBox->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ObjListBoxDblClick(TObject *Sender)
{
	perform_Key_RETURN((TControl*)Sender);
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::SubComboBoxClick(TObject *Sender)
{
	UpdateObjList(CurrPath, EmptyStr, SubComboBox->Text);
}

//---------------------------------------------------------------------------
//Draw the Property List
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::PropListBoxDrawItem(TWinControl *Control, int Index,
	TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas *cv  = lp->Canvas;
	cv->Font->Assign(lp->Font);
	cv->Brush->Color = GetOptCol("bgList");
	cv->FillRect(Rect);

	int xp = Rect.Left + 4;
	int yp = Rect.Top + 1;

	UnicodeString lbuf = lp->Items->Strings[Index];
	UnicodeString vnam = split_tkn(lbuf, "=");
	cv->Font->Color = GetOptCol("fgName");
	cv->TextOut(xp + MaxPrpWidth - cv->TextWidth(vnam) - 12, yp, vnam);

	if (!lbuf.IsEmpty()) {
		if (SameStr(vnam, "ObjectPath")) {
			lbuf = ReplaceStr(ExtractFileDir(lbuf), "\\", "/");
		}
		else if (SameStr(vnam, "FormFile")) {
			lbuf = ExtractFileName(ReplaceStr(lbuf, "\t", " "));
		}

		cv->Font->Color = GetOptCol(is_num_str(lbuf)? "fgNmbr" : "fgList");
		cv->TextOut(xp + MaxPrpWidth, yp, lbuf);

		if (Index==3) {
			cv->Pen->Color = SelectWorB(cv->Brush->Color, 0.25);	//***
			cv->Pen->Style = psSolid;
			cv->Pen->Width = 1;
			cv->MoveTo(Rect.Left,      Rect.Top);
			cv->LineTo(Rect.Right + 1, Rect.Top);
		}
	}

	draw_ListCursor(lp, Rect, Index, State, GetOptCol("LnCurs"), 2);
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::PropListBoxClick(TObject *Sender)
{
	TListBox *p_lp = PropListBox;
	if (p_lp->ItemIndex!=-1) {
		UnicodeString vnam = get_tkn(p_lp->Items->Strings[p_lp->ItemIndex], "=");
		UnicodeString lbuf = p_lp->Items->Values["FormFile"];
		UnicodeString fnam = get_pre_tab(lbuf);
		if (ViewPageControl->ActivePage==TextSheet && SameStr(fnam, DfmFileName)) {
			int lno = get_in_paren(get_post_tab(lbuf)).ToIntDef(1);
			int idx = -1;
			TListBox *t_lp = TextListBox;
			if (contained_wd_i("FormFile|ObjectPath|Class|Name", vnam)) {
				idx = lno - 1;
			}
			else {
				for (int i=lno; i<t_lp->Count; i++) {
					UnicodeString lbuf = Trim(t_lp->Items->Strings[i]);
					if (SameText(get_tkn(lbuf, " "), "object")) break;
					if (lbuf.Pos("=") && SameStr(vnam, Trim(get_tkn(lbuf, "=")))) {
						idx = i;
						break;
					}
				}
			}

			if (idx!=-1) {
				ListBoxSetIndex(t_lp, idx);
				t_lp->Invalidate();
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::PropListBoxKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift)
{
	TListBox *lp = PropListBox;
	int idx = lp->ItemIndex;
	if (idx==-1) return;
	UnicodeString lbuf = lp->Items->Strings[idx];
	UnicodeString vnam = split_tkn(lbuf, "=");

	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	if (equal_ENTER(KeyStr)) {
		if (contained_wd_i(_T("PopupMenu|Action"), vnam)) {
			UnicodeString rnam = IncludeTrailingPathDelimiter(get_tkn(CurrPath, "\\"));
			for (int i=0; i<ObjItemList->Count; i++) {
				UnicodeString onam = ObjItemList->Strings[i];
				if(StartsStr(rnam, onam) && SameStr(ExtractFileName(onam), lbuf)) {
					UpdateObjList(ExtractFileDir(onam), ExtractFileName(onam));
					break;
				}
			}
		}
		else if (SameStr(vnam, "FormFile")) {
			TListBox *lp = TextListBox;
			try {
				ViewPageControl->ActivePage = TextSheet;
				DfmFileName = get_pre_tab(lbuf);
				int idx = get_in_paren(get_post_tab(lbuf)).ToIntDef(1) - 1;
				lp->Items->LoadFromFile(DfmFileName);
				ListBoxSetIndex(lp, idx);
				lp->SetFocus();
			}
			catch (...) {
				DfmFileName = EmptyStr;
				lp->Clear();
				::MessageBeep(MB_ICONEXCLAMATION);
			}
		}
	}
	else if (SameText(KeyStr, "J")) {
		Key = VK_DOWN;
	}
	else if (SameText(KeyStr, "K")) {
		Key = VK_UP;
	}
	else if (SameText(KeyStr, "P")) {
		Key = VK_PRIOR;
	}
	else if (SameText(KeyStr, "N")) {
		Key = VK_NEXT;
	}
	else if (SameText(KeyStr, "L") || SameText(KeyStr, "RIGHT")) {
		if (ViewPageControl->ActivePage==TextSheet) TextListBox->SetFocus();
		Key = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::PropListBoxKeyPress(TObject *Sender, System::WideChar &Key)
{
	if ((_istalnum(Key) || Key==VK_SPACE || Key==VK_RETURN || is_KeyDown(VK_CONTROL))) Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::PropListBoxDblClick(TObject *Sender)
{
	perform_Key_RETURN((TControl*)Sender);
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::GetOffPos(int idx, int *x, int*y)
{
	if (idx==-1) return;

	TStringList *plst = (TStringList *)ObjItemList->Objects[idx];
	if (ExtractFileDir(plst->Values["ObjectPath"]).IsEmpty()) {
		if (!SameStr(plst->Values["BorderStyle"], "bsNone")) {
			*y += (CtlFontHi + 8);
		}
	}
	else {
		int l = plst->Values["Left"].ToIntDef(0);
		int t = plst->Values["Top"].ToIntDef(0);
		*x += l;
		*y += t;

		UnicodeString cls = plst->Values["Class"];
		if (SameStr(cls, "TTabSheet")) {
			*x += 4;
			*y += 4;
			TStringList *plst = GetPropList(ExtractFileDir(ObjItemList->Strings[idx]));
			if (plst) {
				UnicodeString tpos = plst->Values["TabPosition"];
				if (tpos.IsEmpty())
					*y += (CtlFontHi + 8);
				else if (SameStr(tpos, "lpLeft"))
					*x += (CtlFontHi + 8);
			}
		}

		idx = ObjItemList->IndexOf(ExtractFileDir(ObjItemList->Strings[idx]));
		GetOffPos(idx, x, y);
	}
}

//---------------------------------------------------------------------------
TRect __fastcall TDfmViewerForm::GetObjRect(int idx)
{
	TRect rc = Rect(0,0,0,0);
	if (idx!=-1) {
		TStringList *plst = (TStringList *)ObjItemList->Objects[idx];
		if (plst) {
			int ofs_x = 0, ofs_y = 0;
			idx = ObjItemList->IndexOf(ExtractFileDir(ObjItemList->Strings[idx]));
			GetOffPos(idx, &ofs_x, &ofs_y);

			int l = 0, t = 0;
			if (!ExtractFileDir(plst->Values["ObjectPath"]).IsEmpty()) {
				l = plst->Values["Left"].ToIntDef(0);
				t = plst->Values["Top"].ToIntDef(0);
			}
			rc.Left = (ofs_x + l) * FrameRatio;
			rc.Top	= (ofs_y + t) * FrameRatio;

			UnicodeString w_str = plst->Values["Width"];
			if (w_str.IsEmpty()) w_str = plst->Values["ClientWidth"];
			UnicodeString h_str = plst->Values["Height"];
			if (h_str.IsEmpty()) h_str = plst->Values["ClientHeight"];
			rc.SetWidth(std::min((int)(w_str.ToIntDef(0) * FrameRatio), ViewPaintBox->Width - 1));
			rc.SetHeight(std::min((int)(h_str.ToIntDef(0) * FrameRatio), ViewPaintBox->Height - 1));
		}
	}
	return rc;
}

//---------------------------------------------------------------------------
//Draw the Control
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::DrawControl(
	TCanvas *cv, UnicodeString onam, TColor col,
	int  level)		// 0: Same/ -n: Parent/ +n: Child
{
	int obj_idx = ObjItemList->IndexOf(onam);
	if (obj_idx==-1) return;

	TStringList *plst = (TStringList *)ObjItemList->Objects[obj_idx];
	UnicodeString cls = plst->Values["Class"];
	if (contained_wd_i(NnVCtrls, cls)) return;

	cv->Font->Size = FrameRatio * 8;
	int f_hi = cv->TextHeight("Q");
	int mgn  = std::max((int)(2 * FrameRatio), 1);

	bool has_tit = ExtractFileDir(plst->Values["ObjectPath"]).IsEmpty() && !SameStr(plst->Values["BorderStyle"], "bsNone");
	bool active  = SameText(onam, ActivePath);

	bool is_comobox   = SameStr(cls, "TComboBox");
	bool is_checkbox  = SameStr(cls, "TCheckBox");
	bool is_radiobtn  = SameStr(cls, "TRadioButton");
	bool is_button	  = SameStr(cls, "TButton");
	bool is_label	  = SameStr(cls, "TLabel");
	bool is_groupbox  = SameStr(cls, "TGroupBox");
	bool is_labeledit = SameStr(cls, "TLabeledEdit");
	bool is_tabsheet  = SameStr(cls, "TTabSheet");

	TRect ct_rc  = GetObjRect(obj_idx);
	if (has_tit) ct_rc.Bottom += ((CtlFontHi + 8) * FrameRatio);

	UnicodeString cpt = GetCaptionStr(plst);

	//Tab
	if (is_tabsheet && StartsStr(onam, ActivePath)) {
		UnicodeString pnam = ExtractFileDir(ObjItemList->Strings[obj_idx]);
		TStringList *pp = GetPropList(pnam);
		if (pp && SameStr(pp->Values["Class"], "TPageControl")) {
			TRect pc_rc = GetObjRect(ObjItemList->IndexOf(pnam));
			if (!pc_rc.IsEmpty()) {
				bool is_btm = SameStr(pp->Values["TabPosition"], "lpBottom");
				int x = pc_rc.Left + mgn*2;
				int y = is_btm? pc_rc.Bottom - f_hi - mgn
							  : (pp->Values["Top"].ToIntDef(0) * FrameRatio) + pc_rc.Top + mgn;
				if (cv->Font->Size>1) {
					cv->Font->Color  = GetOptCol("fgLabl");
					cv->Brush->Color = GetOptCol(active? "fgActv" : "bgImgV");
					cv->TextOut(x, y, plst->Values["PageIndex"] + ": [" + cpt + "]");
					y += (is_btm? -mgn : f_hi + mgn);
					cv->Pen->Style = psSolid;
					cv->Pen->Color = col;
					cv->Pen->Width = 1;
					cv->MoveTo(pc_rc.Left, y);
					cv->LineTo(pc_rc.Right, y);
				}
			}
		}
	}

	if (!ct_rc.IsEmpty()) {
		TRect frm_rc = ct_rc;
		if (is_groupbox) frm_rc.Top += f_hi/2;
		cv->Brush->Color = col;
		if (active) cv->FillRect(frm_rc);
		//Frame
		if (is_label || is_checkbox || is_radiobtn) col = AdjustColor(col, 72);
		cv->Brush->Color = (level==0)? GetOptCol("fgSame") : col;
		cv->FrameRect(frm_rc);

		//Title bar
		if (has_tit) {
			int x = ct_rc.Left + mgn * 2;
			int y = ct_rc.Top  + mgn;
			cv->Font->Color  = GetOptCol("fgLabl");
			cv->Brush->Color = GetOptCol(active? "fgActv" : "bgImgV");
			cv->TextOut(x, y, "[" + cpt + "]");
		}

		//Symbol/Label
		if (cv->Font->Size>1) {
			UnicodeString c = GetCaptionStr(plst);
			UnicodeString s = is_checkbox? (_T("\u2611") + cpt).c_str() :
							  is_radiobtn? (_T("\u25C9") + cpt).c_str() :
							   is_comobox? _T("Å•") :
				(is_label || is_labeledit || is_button || is_groupbox || is_radiobtn)?
							 			   cpt.c_str() : _T("");
			if (!s.IsEmpty()) {
				std::unique_ptr<TStringList> sbuf(new TStringList());
				sbuf->Text = ReplaceStr(s, "\\r\\n", "\n");
				int xp,yp;
				if (is_labeledit) {
					UnicodeString lpos = plst->Values["LabelPosition"];
					if (SameStr(lpos, "lpLeft"))
						xp = ct_rc.Left - cv->TextWidth(s) - mgn;
					else if (SameStr(lpos, "lpRight"))
						xp = ct_rc.Right + mgn;
					else
						xp = ct_rc.Left + mgn;

					if (SameStr(lpos, "lpLeft") || SameStr(lpos, "lpRight"))
					    yp = ct_rc.Top + (ct_rc.Height() - f_hi) / 2;
					else if (SameStr(lpos, "lpBelow"))
						yp = ct_rc.Bottom + f_hi + mgn;
					else
						yp = ct_rc.Top - f_hi - mgn;
				}
				else {
					UnicodeString al = plst->Values["Alignment"];
					xp = (is_comobox || SameStr(al, "taRightJustify"))?
									(ct_rc.Right - cv->TextWidth(s) - mgn) :
						 (is_button || SameStr(al, "taCenter"))?
						 			(ct_rc.Left + (ct_rc.Width() - cv->TextWidth(s)) /2) :
					  is_groupbox ?	(ct_rc.Left + mgn * 4)
								  :	(ct_rc.Left + mgn);	

					yp = is_groupbox? ct_rc.Top
								    : ct_rc.Top + (ct_rc.Height() - (f_hi * sbuf->Count)) / 2;
				}

				cv->Brush->Color = cv->Pixels[xp + 1][yp + 1];
				cv->Font->Color  = GetOptCol("fgLabl");
				for (int i=0; i<sbuf->Count; i++) {
					cv->TextOut(xp, yp, sbuf->Strings[i]);
					yp += f_hi;
				}
			}
		}
	}
}

//---------------------------------------------------------------------------
//Draw the Preview Image
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ViewPaintBoxPaint(TObject *Sender)
{
	TPaintBox *pp = (TPaintBox*)Sender;
	TRect rc = pp->ClientRect;
	TCanvas *cv  = pp->Canvas;
	cv->Brush->Color = GetOptCol("bgImgV");
	cv->FillRect(rc);

	TListBox *lp = ObjListBox;
	int idx = lp->ItemIndex;
	if (idx==-1) return;

	UnicodeString obj_nam = lp->Items->Strings[idx];
	TStringList *obj_plst = (TStringList *)lp->Items->Objects[idx];
	if (!obj_plst) return;
	if (contained_wd_i(NnVCtrls, obj_plst->Values["Class"])) return;

	if (SameStr(ExtractFileName(obj_nam), "..")) obj_nam = ExtractFileDir(obj_nam);
	int top_idx, end_idx;
	if (!GetFormIndex(get_tkn(obj_nam, "\\"), &top_idx, &end_idx)) return;

	TRect max_rc = Rect(0,0,0,0);
	for (int i=top_idx; i<=end_idx; i++) {
		TStringList *plst = (TStringList *)ObjItemList->Objects[i];
		if (contained_wd_i(NnVCtrls, plst->Values["Class"])) continue;

		TRect rc = Rect(0,0,0,0);
		UnicodeString w_str = plst->Values["Width"];
		if (w_str.IsEmpty()) w_str = plst->Values["ClientWidth"];
		UnicodeString h_str = plst->Values["Height"];
		if (h_str.IsEmpty()) h_str = plst->Values["ClientHeight"];
		rc.SetWidth( w_str.ToIntDef(0) * FrameRatio);
		rc.SetHeight(h_str.ToIntDef(0) * FrameRatio);
		max_rc.Union(rc);
	}

	if (max_rc.IsEmpty()) max_rc = MaxRect;
	if (!max_rc.IsEmpty()) {
		double r_w = std::min(1.0 * ViewPaintBox->Width/  max_rc.Width(),  1.0);
		double r_h = std::min(1.0 * ViewPaintBox->Height/ max_rc.Height(), 1.0);
		FrameRatio = std::min(r_w, r_h);
		StatusBar1->Panels->Items[1]->Text = UnicodeString().sprintf(_T("%.1f%%"), FrameRatio * 100);
	}

	UnicodeString onam = obj_nam;
	int obj_lbl = 0;
	while (!onam.IsEmpty()) {
		if (obj_lbl==0)
			DrawControl(cv, onam, GetOptCol("fgActv"), obj_lbl);
		else
			DrawControl(cv, onam, GetOptCol("fgPrnt"), obj_lbl);
		onam = ExtractFileDir(onam);
		obj_lbl--;
	}

	//Same level Controls
	if (!ExtractFileDir(obj_nam).IsEmpty()) {
		for (int i=0; i<lp->Count; i++) {
			if (i==idx) continue;
			UnicodeString snam = lp->Items->Strings[i];
			DrawControl(cv, snam, GetOptCol("fgSame"), 0);
			TStringList *plst = (TStringList *)lp->Items->Objects[i];
			if (!SameStr(plst->Values["Class"], "TTabSheet")) {
				for (int j=top_idx; j<=end_idx; j++) {
					UnicodeString onam = ObjItemList->Strings[j];
					if (SameStr(ExtractFileDir(onam), snam)) {
						DrawControl(cv, onam, GetOptCol("fgChld"), 1);
					}
				}
			}
		}
	}

	//Child Controls
	int obj_idx = ObjItemList->IndexOf(obj_nam);
	if (obj_idx!=-1) {
		for (int i=obj_idx+1; i<=end_idx; i++) {
			UnicodeString onam = ObjItemList->Strings[i];
			if (SameStr(ExtractFileDir(onam), obj_nam)) DrawControl(cv, onam, GetOptCol("fgChld"), 1);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ViewPaintBoxMouseDown(TObject *Sender, TMouseButton Button,
	TShiftState Shift, int X, int Y)
{
	TRect min_rc = MaxRect;
	TPoint p = Point(X, Y);
	int idx = -1;
	UnicodeString pnam;
	TListBox *lp = ObjListBox;
	if (lp->ItemIndex!=-1) pnam = lp->Items->Strings[lp->ItemIndex];

	for (int i=0; i<3 && !pnam.IsEmpty(); i++) {
		pnam = IncludeTrailingPathDelimiter(pnam);
		for (int i=0; i<ObjItemList->Count; i++) {
			if (StartsStr(pnam, ObjItemList->Strings[i])) {
				TRect rc = GetObjRect(i);
				if (rc.PtInRect(p) && min_rc.Contains(rc)) {
					idx = i;
					min_rc = rc;
				}
			}
		}
		if (idx!=-1) break;

		pnam = ExtractFileDir(ExcludeTrailingPathDelimiter(pnam));
		TStringList *plst = GetPropList(pnam);	if (!plst) break;
		if (contained_wd_i(_T("TPageControl"), plst->Values["Class"])) break;
	}

	if (idx!=-1) {
		UnicodeString onam = ObjItemList->Strings[idx];
		UpdateObjList(ExtractFileDir(onam), ExtractFileName(onam));
	}
}

//---------------------------------------------------------------------------
//Draw the Text of DFM
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::TextListBoxDrawItem(TWinControl *Control, int Index,
	TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas *cv  = lp->Canvas;
	cv->Font->Assign(lp->Font);
	cv->Brush->Color = GetOptCol("bgList");
	cv->FillRect(Rect);

	int xp = Rect.Left + 4;
	int yp = Rect.Top + 1;

	UnicodeString csr_mk =  _T("\u25b6");
	if (Index==lp->ItemIndex && !lp->Focused()) {
		cv->Font->Color = GetOptCol("LnCurs");
		cv->TextOut(xp, yp, csr_mk);
	}
	xp += cv->TextWidth(csr_mk);

	//Line Number
	UnicodeString ls;
	ls.sprintf(_T("%5u "), Index + 1);
	cv->Font->Color = GetOptCol("fgNmbr");
	cv->TextOut(xp, yp, ls);
	xp += cv->TextWidth(ls);

	UnicodeString lbuf = lp->Items->Strings[Index];
	int p = lbuf.Pos("=");
	if (p>0) {
		//Property Name
		UnicodeString kstr = TrimRight(get_tkn(lbuf, "=")) + " = ";
		UnicodeString vstr = conv_ValText(TrimLeft(get_tkn_r(lbuf, "=")), true);
		cv->Font->Color = GetOptCol("fgList");
		cv->TextOut(xp, yp, kstr);
		xp += cv->TextWidth(kstr);
		//Value
		if (!vstr.IsEmpty()) {
			cv->Font->Color = GetOptCol(is_num_str(vstr)? "fgNmbr" : (vstr[1]=='\'')? "fgText" : "fgList");
			cv->TextOut(xp, yp, vstr);
		}
	}
	else {
		UnicodeString s = TrimLeft(lbuf);
		if (starts_tchs(_T("\'#"), s)) {
			UnicodeString end_s = remove_end_s(s, ')')? ")" : "";
			lbuf = StringOfChar(_T(' '), lbuf.Length() - TrimLeft(lbuf).Length())
					+ conv_ValText(s, true) + end_s;
		}
		cv->Font->Color = StartsText("object ", Trim(lbuf))? GetOptCol("fgName") : GetOptCol("fgList");
		cv->TextOut(xp, yp, lbuf);
	}

	draw_ListCursor(lp, Rect, Index, State, GetOptCol("LnCurs"), 2);
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::TextListBoxKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift)
{
	TListBox *lp = TextListBox;
	int idx = lp->ItemIndex;
	if (idx==-1) return;
	UnicodeString lbuf = Trim(lp->Items->Strings[idx]);

	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	if (equal_ENTER(KeyStr)) {
		if (remove_top_text(lbuf, "object ")) {
			if (lbuf.Pos(":")) {
				lbuf = get_tkn(lbuf, ":");
				UnicodeString rnam = IncludeTrailingPathDelimiter(get_tkn(CurrPath, "\\"));
				for (int i=0; i<ObjItemList->Count; i++) {
					UnicodeString onam = ObjItemList->Strings[i];
					if(StartsStr(rnam, onam) && SameStr(ExtractFileName(onam), lbuf)) {
						UpdateObjList(ExtractFileDir(onam), ExtractFileName(onam));
						break;
					}
				}
			}
		}
	}
	else if (SameText(KeyStr, "J")) {
		Key = VK_DOWN;
	}
	else if (SameText(KeyStr, "K")) {
		Key = VK_UP;
	}
	else if (SameText(KeyStr, "P")) {
		Key = VK_PRIOR;
	}
	else if (SameText(KeyStr, "N")) {
		Key = VK_NEXT;
	}
	else if (SameText(KeyStr, "H") || SameText(KeyStr, "LEFT")) {
		ObjListBox->SetFocus();
		Key = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::TextListBoxDblClick(TObject *Sender)
{
	perform_Key_RETURN((TControl*)Sender);
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ToParentActionExecute(TObject *Sender)
{
	UpdateObjList(ExtractFileDir(CurrPath), ExtractFileName(CurrPath));
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ToParentActionUpdate(TObject *Sender)
{
	((TAction *)Sender)->Enabled = !CurrPath.IsEmpty();
}

//---------------------------------------------------------------------------
//Reload
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ReloadBtnClick(TObject *Sender)
{
	InitObjList();

	if (MainPageControl->ActivePage==ListSheet) UpdateResultList();
}

//---------------------------------------------------------------------------
//Find Object
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FindObjActionExecute(TObject *Sender)
{
	if (!FindComboBox->Text.IsEmpty()) {
		UnicodeString kwd  = FindComboBox->Text;
		UnicodeString rnam = IncludeTrailingPathDelimiter(get_tkn(CurrPath, "\\"));
		std::unique_ptr<TStringList> rlst(new TStringList());;
		for (int i=0; i<ObjItemList->Count; i++) {
			UnicodeString onam = ObjItemList->Strings[i];
			if (StartsStr(rnam, onam) && pos_r_i(kwd, ExtractFileName(onam))) rlst->Add(onam);
		}

		if (rlst->Count>0) {
			UnicodeString onam = rlst->Strings[0];
			UpdateObjList(ExtractFileDir(onam), ExtractFileName(onam));
		}
		else {
			::MessageBeep(MB_ICONEXCLAMATION);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FindObjActionUpdate(TObject *Sender)
{
	((TAction *)Sender)->Enabled = !FindComboBox->Text.IsEmpty() && !CurrPath.IsEmpty();

	FindComboBox->Enabled = !CurrPath.IsEmpty();
}

//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FindComboBoxChange(TObject *Sender)
{
	if (!FindComboBox->Text.IsEmpty()) {
		UnicodeString kwd  = FindComboBox->Text;
		UnicodeString rnam = IncludeTrailingPathDelimiter(get_tkn(CurrPath, "\\"));
		std::unique_ptr<TStringList> rlst(new TStringList());;
		for (int i=0; i<ObjItemList->Count; i++) {
			UnicodeString onam = ObjItemList->Strings[i];
			if (StartsStr(rnam, onam) && pos_r_i(kwd, ExtractFileName(onam))) rlst->Add(ExtractFileName(onam));
		}

		FindComboBox->Items->Assign(rlst.get());
		if (rlst->Count==1) kwd = rlst->Strings[0];
		FindComboBox->SetFocus();
		FindComboBox->Text		 = kwd;
		FindComboBox->SelLength  = 0;
		FindComboBox->SelStart	 = kwd.Length();
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FindComboBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	if (equal_ENTER(KeyStr)) {
		FindObjAction->Execute();
		Key = 0;
	}
	else if (SameText(KeyStr, "Ctrl+L")) {
		ObjListBox->SetFocus();
		Key = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FindComboBoxKeyPress(TObject *Sender, System::WideChar &Key)
{
	if (is_KeyPress_CtrlNotCV(Key) || Key==VK_RETURN) Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FindComboBoxClick(TObject *Sender)
{
	FindObjAction->Execute();
}

//---------------------------------------------------------------------------
//Copy
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::CopyActionExecute(TObject *Sender)
{
	TWinControl *wp = ActiveControl;	if (!wp) return;
	UnicodeString lbuf;
	if (wp->ClassNameIs("TListBox")) {
		TListBox *lp = (TListBox *)wp;
		int idx = lp->ItemIndex;
		if (idx!=-1) lbuf = lp->Items->Strings[idx];
	}
	else if (wp->ClassNameIs("TStringGrid")) {
		TStringGrid *gp = (TStringGrid *)wp;
		lbuf = gp->Cells[gp->Col][gp->Row];
	}

	if (!lbuf.IsEmpty()) Clipboard()->AsText = lbuf;
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::CopyActionUpdate(TObject *Sender)
{
	((TAction *)Sender)->Enabled
		= (ObjListBox->Focused() || PropListBox->Focused() || TextListBox->Focused()
			|| ResultGrid->Focused());
}
//---------------------------------------------------------------------------
//Copy Value
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::CopyValActionExecute(TObject *Sender)
{
	TWinControl *wp = ActiveControl;
	if (wp && wp->ClassNameIs("TListBox")) {
		TListBox *lp = (TListBox *)wp;
		int idx = lp->ItemIndex;
		if (idx!=-1) {
			UnicodeString lbuf = lp->Items->Strings[idx];
			if (lbuf.Pos("=")) {
				lbuf = get_tkn_r(lbuf, "=");
				if (!lbuf.IsEmpty()) Clipboard()->AsText = lbuf;
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::CopyValActionUpdate(TObject *Sender)
{
	((TAction *)Sender)->Enabled = PropListBox->Focused();
}

//---------------------------------------------------------------------------
//Open by text editor
//---------------------------------------------------------------------------
bool __fastcall TDfmViewerForm::OpenByTextEditor(UnicodeString fnam, int lno)
{
	try {
		if (!FileExists(Editor) || !FileExists(fnam)) Abort();

		UnicodeString p_fnam = add_quot_if_spc(fnam);
		UnicodeString prmstr;
		if (lno>0) {
			if (!EditorFormat.IsEmpty()) {
				prmstr = EditorFormat;
				prmstr = ReplaceStr(prmstr, "$F", p_fnam);
				prmstr = ReplaceStr(prmstr, "$L", IntToStr(lno));
			}
			else {
				prmstr = p_fnam;
			}
		}
		else {
			prmstr = p_fnam;
		}

		return (::ShellExecute(NULL, _T("open"), Editor.c_str(), prmstr.c_str(), NULL,SW_SHOWNORMAL) > (HINSTANCE)32);
	}
	catch (EAbort &e) {
		return false;
	}
}

//---------------------------------------------------------------------------
//Change Class
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ClassComboBoxChange(TObject *Sender)
{
	TComboBox *cp = (TComboBox*)Sender;
	if (InhClassCombo) {
		InhClassCombo = false;
		ResultGrid->SetFocus();
	}
	else {
		cp->Items->Clear();
		if (!cp->Text.IsEmpty()) {
			for (int i=0; i<ClassNameList->Count; i++) {
				UnicodeString lbuf = ClassNameList->Strings[i];
				if (ContainsText(lbuf, cp->Text)) cp->Items->Add(lbuf);
			}
		}
		else {
			cp->Items->Assign(ClassNameList);
		}

		if (cp->Items->Count==1) {
			cp->DroppedDown = false;
			cp->ItemIndex   = 0;
			cp->Text = cp->Items->Strings[0];
			UpdateResultList();
			ResultGrid->SetFocus();
		}
		else {
			cp->DroppedDown = true;
			Screen->Cursor	= crArrow;
			cp->SelLength	= 0;
			cp->SelStart	= cp->Text.Length();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ClassComboBoxClick(TObject *Sender)
{
	InhClassCombo	= true;
	TComboBox *cp	= (TComboBox*)Sender;
	cp->DroppedDown = false;

	UpdateResultList();
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::FilterEdit2Change(TObject *Sender)
{
	UpdateResultList();
}

//---------------------------------------------------------------------------
//Draw Result Grid
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ResultGridDrawCell(TObject *Sender, int ACol, int ARow,
	TRect &Rect, TGridDrawState State)
{
	TStringGrid *gp = dynamic_cast<TStringGrid*>(Sender);	if (gp==NULL) return;
	TCanvas *cv = gp->Canvas;
	cv->Font->Assign(ListFont);

	cv->Brush->Color = (ARow==0)? GetOptCol("bgHead") :
						(ACol<2)? AdjustColor(GetOptCol("bgList"), 48)	//Fixed col ***
								: GetOptCol("bgList");
	cv->FillRect(Rect);

	UnicodeString prop = gp->Cells[ACol][0];
	UnicodeString lbuf = gp->Cells[ACol][ARow];

	if (ARow>1 && ACol==0) {
		if (ARow>gp->TopRow && lbuf==gp->Cells[ACol][ARow - 1]) lbuf = EmptyStr;
	}

	int yp = Rect.Top;  if (!has_Leading(cv)) yp += 2;
	bool is_num = is_num_str(lbuf);
	if (SameStr("Color", prop) && is_num) {
		lbuf = UnicodeString().sprintf(_T("0x%8x"), lbuf.ToIntDef(0));
	}
	int xp = (ACol>1 && is_num_str(lbuf))? Rect.Left + Rect.Width() - cv->TextWidth(lbuf) - 4 : Rect.Left + 4;
	cv->Font->Color = GetOptCol((ARow==0)? "fgHead" : (ACol==0)? "fgName" : is_num? "fgNmbr" : "fgList");
	cv->TextRect(Rect, xp, yp, lbuf);

	if (ARow>0 && ARow<gp->RowCount-1) {
		if (gp->Cells[0][ARow]!=gp->Cells[0][ARow + 1]) {
			cv->Pen->Color = clDkGray;
			cv->MoveTo(Rect.Left,      Rect.Bottom - 1);
			cv->LineTo(Rect.Right + 1, Rect.Bottom - 1);
		}
	}

	//Cursor
	draw_GridCursor(gp, Rect, ARow, State, GetOptCol("LnCurs"), 2);
	if (gp->Row==ARow && gp->Col==ACol) alpha_blend_Rect(cv, Rect, GetOptCol("LnCurs"), 96);
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ResultGridClick(TObject *Sender)
{
	TStringGrid *gp = (TStringGrid *)Sender;
	gp->Invalidate();

	StatusBar1->Panels->Items[0]->Text
		= ReplaceStr(GetAbsObjName(gp->Cells[0][gp->Row], gp->Cells[1][gp->Row]), "\\", "/");
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::ResultGridKeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift)
{
	TStringGrid *gp = (TStringGrid *)Sender;

	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	if (KeyStr.IsEmpty()) return;

	if (equal_ENTER(KeyStr)) {
		UnicodeString abs_onam = GetAbsObjName(gp->Cells[0][gp->Row], gp->Cells[1][gp->Row]);
		if (!abs_onam.IsEmpty()) {
			MainPageControl->ActivePage = ObjectSheet;
			UpdateObjList(ExtractFileDir(abs_onam), ExtractFileName(abs_onam));
			return;
		}
	}
	else if (SameText(KeyStr, "H")) {
		Key = VK_LEFT;
	}
	else if (SameText(KeyStr, "L")) {
		Key = VK_RIGHT;
	}

	((TStringGrid *)Sender)->Invalidate();
}

//---------------------------------------------------------------------------
//Select Project Directory
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::SelectProjectDirectory1Click(TObject *Sender)
{
	UnicodeString dnam = DfmFileDir;
	if (SelectDirectory("Select Project Directory", EmptyStr, dnam, 
		TSelectDirExtOpts() << sdNewUI << sdShowShares, Screen->ActiveForm))
	{
		DfmFileDir = dnam;
		DfmFileList->Clear();
		get_files(DfmFileDir, _T("*.dfm"), DfmFileList);
		Caption = Application->Title + " - " + ReplaceStr(DfmFileDir, "\\", "/");
		InitObjList();
	}
}
//---------------------------------------------------------------------------
//Setup
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::OptionBtnClick(TObject *Sender)
{
	if (OptionDlg->ShowModal()==mrOk) {
		set_ListBoxItemHi(ObjListBox,  ListFont);
		ObjListBox->Color = GetOptCol("bgList");
		ObjListBox->Repaint();

		set_ListBoxItemHi(PropListBox, ListFont);
		PropListBox->Color = GetOptCol("bgList");
		PropListBox->Repaint();

		set_ListBoxItemHi(TextListBox, ListFont);
		TextListBox->Color = GetOptCol("bgList");
		TextListBox->Repaint();

		ResultGrid->Font->Assign(ListFont);
		ResultGrid->Color  = GetOptCol("bgList");
		ResultGrid->Repaint();

		ImagePanel->Color  = GetOptCol("bgImgV");
		ViewPaintBox->Repaint();
	}
}

//---------------------------------------------------------------------------
//Edit Source
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::EditSrcActionExecute(TObject *Sender)
{
	try {
		if (PropListBox->Focused() && PropListBox->ItemIndex!=-1) {
			UnicodeString lbuf = PropListBox->Items->Strings[PropListBox->ItemIndex];
			if (!StartsStr("On", get_tkn(lbuf, "="))) Abort();

			UnicodeString func = "::" + lbuf + "(";
			UnicodeString fnam = get_pre_tab(PropListBox->Items->Values["FormFile"]);
			if (fnam.IsEmpty()) Abort();
			fnam = ChangeFileExt(fnam, ".cpp");
			if (!FileExists(fnam)) Abort();
			std::unique_ptr<TStringList> fbuf(new TStringList());
			fbuf->LoadFromFile(fnam);
			int lno = 0;
			for (int i=0; i<fbuf->Count; i++) {
				UnicodeString s = fbuf->Strings[i];
				if (s.Pos(func) && s.Pos("__fastcall")) {
					lno = i + 1;
					break;
				}
			}
			OpenByTextEditor(fnam, lno);
		}
		else if (TextListBox->Focused() && TextListBox->ItemIndex!=-1) {
			OpenByTextEditor(DfmFileName, TextListBox->ItemIndex + 1);
		}
	}
	catch (...) {
		::MessageBeep(MB_ICONEXCLAMATION);
	}
}
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::EditSrcActionUpdate(TObject *Sender)
{
	TAction *ap = (TAction *)Sender;
	if (PropListBox->Focused() && PropListBox->ItemIndex!=-1
		&& StartsStr("On", get_tkn(PropListBox->Items->Strings[PropListBox->ItemIndex], "=")))
	{
		ap->Enabled = true;
	}
	else if (TextListBox->Focused() && TextListBox->ItemIndex!=-1) {
		ap->Enabled = true;
	}
	else {
		ap->Enabled = false;
	}
}

//---------------------------------------------------------------------------
//About
//---------------------------------------------------------------------------
void __fastcall TDfmViewerForm::AboutItemClick(TObject *Sender)
{
	AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

