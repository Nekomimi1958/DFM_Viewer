object OptionDlg: TOptionDlg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Setup'
  ClientHeight = 333
  ClientWidth = 474
  Color = clBtnFace
  ParentFont = True
  OldCreateOrder = True
  Position = poMainFormCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object OKBtn: TButton
    Left = 147
    Top = 300
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
    OnClick = OKBtnClick
  end
  object CancelBtn: TButton
    Left = 253
    Top = 300
    Width = 75
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 474
    Height = 294
    ActivePage = TabSheet2
    Align = alTop
    OwnerDraw = True
    TabOrder = 0
    TabWidth = 100
    OnDrawTab = PageControl1DrawTab
    object TabSheet2: TTabSheet
      Caption = 'General'
      ImageIndex = 1
      object Label1: TLabel
        Left = 143
        Top = 81
        Width = 79
        Height = 26
        Caption = '$F : File name'#13#10'$L : Line number'
      end
      object EditorEdit: TLabeledEdit
        Left = 12
        Top = 27
        Width = 409
        Height = 21
        EditLabel.Width = 53
        EditLabel.Height = 13
        EditLabel.Caption = 'Text Editor'
        TabOrder = 0
      end
      object RefEditorBtn: TButton
        Left = 423
        Top = 27
        Width = 32
        Height = 22
        Caption = '...'
        TabOrder = 1
        OnClick = RefEditorBtnClick
      end
      object EditPrmEdit: TLabeledEdit
        Left = 143
        Top = 56
        Width = 278
        Height = 21
        EditLabel.Width = 50
        EditLabel.Height = 13
        EditLabel.Caption = 'Parameter'
        LabelPosition = lpLeft
        TabOrder = 2
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Font and Color'
      ImageIndex = 2
      object ColorGroupBox: TGroupBox
        Left = 10
        Top = 70
        Width = 447
        Height = 187
        Caption = '&Color'
        TabOrder = 1
        DesignSize = (
          447
          187)
        object RefColBtn: TButton
          Left = 410
          Top = 18
          Width = 30
          Height = 22
          Anchors = [akTop, akRight]
          Caption = '...'
          TabOrder = 0
          OnClick = RefColBtnClick
        end
        object ColorListBox: TListBox
          Left = 10
          Top = 18
          Width = 398
          Height = 159
          Style = lbOwnerDrawFixed
          ItemHeight = 13
          TabOrder = 1
          OnDrawItem = ColorListBoxDrawItem
        end
      end
      object GroupBox1: TGroupBox
        Left = 10
        Top = 8
        Width = 447
        Height = 46
        Caption = '&Font'
        TabOrder = 0
        DesignSize = (
          447
          46)
        object RefFontBtn: TButton
          Left = 410
          Top = 17
          Width = 30
          Height = 22
          Anchors = [akTop, akRight]
          Caption = '...'
          TabOrder = 0
          OnClick = RefFontBtnClick
        end
        object FontComboBox: TComboBox
          Left = 10
          Top = 18
          Width = 398
          Height = 22
          Style = csOwnerDrawFixed
          Anchors = [akLeft, akTop, akRight]
          DropDownCount = 20
          TabOrder = 1
          OnDrawItem = FontComboBoxDrawItem
        end
      end
    end
  end
  object OpenDialog1: TOpenDialog
    Left = 384
    Top = 136
  end
  object FontDialog1: TFontDialog
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    Left = 332
    Top = 224
  end
  object ColorDialog1: TColorDialog
    Left = 396
    Top = 224
  end
end
