///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/statusbr.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/textctrl.h>
#include <wx/treectrl.h>
#include <wx/timer.h>
#include <wx/frame.h>
#include <wx/aui/aui.h>
#include <wx/stattext.h>
#include <wx/hyperlink.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define ID_Menu_Refresh 1000
#define ID_Menu_About 1001
#define wxID_UpdateUI_timer 1002

///////////////////////////////////////////////////////////////////////////////
/// Class GUIDialog
///////////////////////////////////////////////////////////////////////////////
class GUIDialog : public wxFrame
{
	DECLARE_EVENT_TABLE()
	private:

		// Private event handlers
		void _wxFB_OnMenuRefresh( wxCommandEvent& event ){ OnMenuRefresh( event ); }
		void _wxFB_OnAbout( wxCommandEvent& event ){ OnAbout( event ); }
		void _wxFB_OnUpdateUITimer( wxTimerEvent& event ){ OnUpdateUITimer( event ); }


	protected:
		wxStatusBar* m_statusBar1;
		wxMenuBar* m_menubar1;
		wxMenu* Menu_Operate;
		wxMenu* Menu_Help;
		wxTextCtrl* m_textCtrl1;
		wxTreeCtrl* m_treeCtrl1;
		wxTimer m_UpdateUI_timer;

		// Virtual event handlers, override them in your derived class
		virtual void OnMenuRefresh( wxCommandEvent& event ) = 0;
		virtual void OnAbout( wxCommandEvent& event ) = 0;
		virtual void OnUpdateUITimer( wxTimerEvent& event ) = 0;


	public:

		GUIDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("WinBleTool"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		wxAuiManager m_mgr;

		~GUIDialog();

};

///////////////////////////////////////////////////////////////////////////////
/// Class GuiDialogAbout
///////////////////////////////////////////////////////////////////////////////
class GuiDialogAbout : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText2;
		wxStaticText* m_staticText3;
		wxHyperlinkCtrl* m_hyperlink1;
		wxHyperlinkCtrl* m_hyperlink2;

	public:

		GuiDialogAbout( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("关于"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );

		~GuiDialogAbout();

};

