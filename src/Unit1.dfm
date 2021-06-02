object DfmViewerForm: TDfmViewerForm
  Left = 0
  Top = 0
  Caption = 'DFM Viewer'
  ClientHeight = 570
  ClientWidth = 790
  Color = clBtnFace
  ParentFont = True
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDesigned
  StyleElements = [seBorder]
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object StatusBar1: TStatusBar
    Left = 0
    Top = 550
    Width = 790
    Height = 20
    DoubleBuffered = True
    Panels = <
      item
        Style = psOwnerDraw
        Width = 400
      end
      item
        Style = psOwnerDraw
        Width = 100
      end
      item
        Style = psOwnerDraw
        Width = 50
      end>
    ParentDoubleBuffered = False
    OnDrawPanel = StatusBar1DrawPanel
  end
  object MainPanel: TPanel
    Left = 0
    Top = 0
    Width = 790
    Height = 550
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object MainPageControl: TPageControl
      Left = 0
      Top = 0
      Width = 790
      Height = 550
      ActivePage = ObjectSheet
      Align = alClient
      OwnerDraw = True
      TabOrder = 0
      TabWidth = 120
      OnDrawTab = TabPageControlDrawTab
      object ObjectSheet: TTabSheet
        Caption = 'Object'
        object Splitter1: TSplitter
          Left = 400
          Top = 0
          Width = 6
          Height = 522
        end
        object ListPanel: TPanel
          Left = 0
          Top = 0
          Width = 400
          Height = 522
          Align = alLeft
          BevelOuter = bvNone
          TabOrder = 0
          OnResize = ListPanelResize
          object Splitter2: TSplitter
            Left = 0
            Top = 343
            Width = 400
            Height = 6
            Cursor = crVSplit
            Align = alBottom
          end
          object ObjListBox: TListBox
            Left = 0
            Top = 29
            Width = 400
            Height = 289
            Style = lbOwnerDrawFixed
            Align = alClient
            BorderStyle = bsNone
            Color = clBlack
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'MeiryoKe_Gothic'
            Font.Style = []
            ItemHeight = 15
            ParentFont = False
            TabOrder = 1
            TabWidth = 40
            OnClick = ObjListBoxClick
            OnDblClick = ObjListBoxDblClick
            OnDrawItem = ObjListBoxDrawItem
            OnKeyDown = ObjListBoxKeyDown
            OnKeyPress = ObjListBoxKeyPress
          end
          object PropListBox: TListBox
            Left = 0
            Top = 349
            Width = 400
            Height = 173
            Style = lbOwnerDrawFixed
            Align = alBottom
            BorderStyle = bsNone
            Color = clBlack
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'MeiryoKe_Gothic'
            Font.Style = []
            ItemHeight = 18
            ParentFont = False
            PopupMenu = PopupMenu1
            TabOrder = 3
            OnClick = PropListBoxClick
            OnDblClick = PropListBoxDblClick
            OnDrawItem = PropListBoxDrawItem
            OnKeyDown = PropListBoxKeyDown
            OnKeyPress = PropListBoxKeyPress
          end
          object ObjSubPanel: TPanel
            Left = 0
            Top = 318
            Width = 400
            Height = 25
            Align = alBottom
            AutoSize = True
            BevelOuter = bvNone
            BorderWidth = 2
            TabOrder = 2
            object ObjSttLabel: TLabel
              Left = 237
              Top = 6
              Width = 28
              Height = 13
              Caption = '---/---'
            end
            object Label2: TLabel
              Left = 20
              Top = 6
              Width = 45
              Height = 13
              Alignment = taRightJustify
              Caption = 'Category'
            end
            object SubComboBox: TComboBox
              Left = 69
              Top = 2
              Width = 160
              Height = 21
              Style = csDropDownList
              TabOrder = 0
              OnClick = SubComboBoxClick
            end
          end
          object ObjFindPanel: TPanel
            Left = 0
            Top = 0
            Width = 400
            Height = 29
            Align = alTop
            AutoSize = True
            BevelOuter = bvNone
            BorderWidth = 2
            TabOrder = 0
            object FindComboBox: TComboBox
              Left = 2
              Top = 2
              Width = 371
              Height = 21
              Align = alClient
              AutoComplete = False
              DropDownCount = 12
              TabOrder = 0
              OnChange = FindComboBoxChange
              OnClick = FindComboBoxClick
              OnKeyDown = FindComboBoxKeyDown
              OnKeyPress = FindComboBoxKeyPress
            end
            object FindObjBtn: TButton
              Left = 373
              Top = 2
              Width = 25
              Height = 25
              Action = FindObjAction
              Align = alRight
              TabOrder = 1
            end
          end
        end
        object ViewPageControl: TPageControl
          Left = 406
          Top = 0
          Width = 376
          Height = 522
          ActivePage = ImageSheet
          Align = alClient
          DoubleBuffered = True
          OwnerDraw = True
          ParentDoubleBuffered = False
          TabOrder = 1
          TabWidth = 120
          OnDrawTab = TabPageControlDrawTab
          object ImageSheet: TTabSheet
            Caption = 'Form Image'
            object ImagePanel: TPanel
              Left = 0
              Top = 0
              Width = 368
              Height = 494
              Align = alClient
              BevelOuter = bvNone
              BorderWidth = 8
              Color = clBlack
              ParentBackground = False
              TabOrder = 0
              StyleElements = [seFont, seBorder]
              object ViewPaintBox: TPaintBox
                Left = 8
                Top = 8
                Width = 352
                Height = 478
                Align = alClient
                OnMouseDown = ViewPaintBoxMouseDown
                OnPaint = ViewPaintBoxPaint
              end
            end
          end
          object TextSheet: TTabSheet
            Caption = 'DFM Text'
            ImageIndex = 1
            object TextListBox: TListBox
              Left = 0
              Top = 0
              Width = 368
              Height = 494
              Style = lbOwnerDrawFixed
              Align = alClient
              BorderStyle = bsNone
              DoubleBuffered = False
              ParentDoubleBuffered = False
              TabOrder = 0
              OnDblClick = TextListBoxDblClick
              OnDrawItem = TextListBoxDrawItem
              OnKeyDown = TextListBoxKeyDown
            end
          end
        end
      end
      object ListSheet: TTabSheet
        Caption = 'List by Class'
        ImageIndex = 1
        object ResultGrid: TStringGrid
          Tag = 2
          Left = 0
          Top = 35
          Width = 782
          Height = 487
          Align = alClient
          BorderStyle = bsNone
          Color = 2097152
          DefaultDrawing = False
          DoubleBuffered = False
          FixedCols = 0
          Options = [goFixedVertLine, goVertLine, goColSizing, goRowSelect, goThumbTracking]
          ParentDoubleBuffered = False
          TabOrder = 0
          StyleElements = [seBorder]
          OnClick = ResultGridClick
          OnDrawCell = ResultGridDrawCell
          OnKeyDown = ResultGridKeyDown
          ColWidths = (
            64
            64
            64
            64
            64)
          RowHeights = (
            24
            24
            24
            24
            24)
        end
        object FindOpePanel: TPanel
          Left = 0
          Top = 0
          Width = 782
          Height = 35
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 1
          object FindPanel1: TPanel
            Left = 0
            Top = 0
            Width = 641
            Height = 35
            Align = alLeft
            BevelOuter = bvNone
            TabOrder = 0
            DesignSize = (
              641
              35)
            object FilterSttLabel2: TLabel
              Left = 425
              Top = 10
              Width = 28
              Height = 13
              Anchors = [akTop, akRight]
              Caption = '__/__'
            end
            object Label1: TLabel
              Left = 8
              Top = 11
              Width = 40
              Height = 13
              Alignment = taRightJustify
              AutoSize = False
              Caption = 'Class'
            end
            object FilterEdit2: TLabeledEdit
              Left = 257
              Top = 7
              Width = 160
              Height = 21
              Anchors = [akLeft, akTop, akRight]
              EditLabel.Width = 24
              EditLabel.Height = 13
              EditLabel.Caption = 'Filter'
              LabelPosition = lpLeft
              TabOrder = 1
              OnChange = FilterEdit2Change
            end
            object ClassComboBox: TComboBox
              Left = 52
              Top = 8
              Width = 160
              Height = 21
              DropDownCount = 24
              TabOrder = 0
              OnChange = ClassComboBoxChange
              OnClick = ClassComboBoxClick
            end
          end
        end
      end
    end
  end
  object ActionList1: TActionList
    Left = 447
    Top = 82
    object CopyAction: TAction
      Caption = '&Copy'
      ShortCut = 16451
      OnExecute = CopyActionExecute
      OnUpdate = CopyActionUpdate
    end
    object CopyValAction: TAction
      Caption = 'Copy value'
      OnExecute = CopyValActionExecute
      OnUpdate = CopyValActionUpdate
    end
    object FileExit1: TFileExit
      Category = 'File'
      Caption = #32066#20102'(&X)'
      Hint = #32066#20102'|'#12450#12503#12522#12465#12540#12471#12519#12531#12434#32066#20102#12375#12414#12377
      ImageIndex = 43
    end
    object EditSrcAction: TAction
      Caption = '&Edit source'
      ShortCut = 69
      OnExecute = EditSrcActionExecute
      OnUpdate = EditSrcActionUpdate
    end
    object FindObjAction: TAction
      Caption = #55357#56590
      Hint = 'Find object'
      OnExecute = FindObjActionExecute
      OnUpdate = FindObjActionUpdate
    end
    object ToParentAction: TAction
      OnExecute = ToParentActionExecute
      OnUpdate = ToParentActionUpdate
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 65
    Top = 236
    object PopCopyItem: TMenuItem
      Action = CopyAction
    end
    object PopCopyValItem: TMenuItem
      Action = CopyValAction
    end
    object Sep_p_1: TMenuItem
      Caption = '-'
    end
    object Editsource1: TMenuItem
      Action = EditSrcAction
    end
  end
  object MainMenu1: TMainMenu
    OwnerDraw = True
    Left = 69
    Top = 100
    object File1: TMenuItem
      Caption = '&File'
      OnClick = MainMenuClick
      object SelectProjectDirectory1: TMenuItem
        Caption = 'Select Project Directory...'
        OnClick = SelectProjectDirectory1Click
      end
      object ReloadItem: TMenuItem
        Caption = '&Reload'
        OnClick = ReloadBtnClick
      end
      object Sep_f1: TMenuItem
        Caption = '-'
      end
      object ExitItem: TMenuItem
        Action = FileExit1
        Caption = 'E&xit'
      end
    end
    object Edit1: TMenuItem
      Caption = '&Edit'
      OnClick = MainMenuClick
      object CopyItem: TMenuItem
        Action = CopyAction
      end
      object CopyValueItem: TMenuItem
        Action = CopyValAction
      end
      object Sep_e1: TMenuItem
        Caption = '-'
      end
      object EditSrcItem: TMenuItem
        Action = EditSrcAction
      end
    end
    object Setup1: TMenuItem
      Caption = '&Setup'
      OnClick = MainMenuClick
      object OptionsItem: TMenuItem
        Caption = '&Options...'
        OnClick = OptionBtnClick
      end
    end
    object Help1: TMenuItem
      Caption = 'Help'
      OnClick = MainMenuClick
      object AboutItem: TMenuItem
        Caption = '&About...'
        OnClick = AboutItemClick
      end
    end
  end
end
