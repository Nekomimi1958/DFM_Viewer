//----------------------------------------------------------------------//
// DFM Viewer															//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop

#include <tchar.h>

//---------------------------------------------------------------------------
USEFORM("OptDlg.cpp", OptionDlg);
USEFORM("Unit1.cpp", DfmViewerForm);
USEFORM("About.cpp", AboutBox);
//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->Title = "DFM Viewer";
		Application->CreateForm(__classid(TDfmViewerForm), &DfmViewerForm);
		Application->CreateForm(__classid(TOptionDlg), &OptionDlg);
		Application->CreateForm(__classid(TAboutBox), &AboutBox);
		Application->Run();
	}
	catch (Exception &exception) {
		Application->ShowException(&exception);
	}
	catch (...) {
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}

	return 0;
}
//---------------------------------------------------------------------------
