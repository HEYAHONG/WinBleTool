///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "GUIDialog.h"

///////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE( GUIDialog, wxFrame )
	EVT_MENU( ID_Menu_Refresh, GUIDialog::_wxFB_OnMenuRefresh )
	EVT_MENU( ID_Menu_About, GUIDialog::_wxFB_OnAbout )
	EVT_TIMER( wxID_UpdateUI_timer, GUIDialog::_wxFB_OnUpdateUITimer )
END_EVENT_TABLE()

GUIDialog::GUIDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 1000,600 ), wxDefaultSize );
	m_mgr.SetManagedWindow(this);
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT);

	m_statusBar1 = this->CreateStatusBar( 2, wxSTB_DEFAULT_STYLE|wxSTB_SIZEGRIP, wxID_ANY );
	m_menubar1 = new wxMenuBar( 0 );
	Menu_Operate = new wxMenu();
	wxMenuItem* Menu_Refresh;
	Menu_Refresh = new wxMenuItem( Menu_Operate, ID_Menu_Refresh, wxString( wxT("刷新设备") ) , wxEmptyString, wxITEM_NORMAL );
	Menu_Operate->Append( Menu_Refresh );

	m_menubar1->Append( Menu_Operate, wxT("操作") );

	Menu_Help = new wxMenu();
	wxMenuItem* Menu_About;
	Menu_About = new wxMenuItem( Menu_Help, ID_Menu_About, wxString( wxT("关于") ) , wxEmptyString, wxITEM_NORMAL );
	Menu_Help->Append( Menu_About );

	m_menubar1->Append( Menu_Help, wxT("帮助") );

	this->SetMenuBar( m_menubar1 );

	m_textCtrl1 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	m_mgr.AddPane( m_textCtrl1, wxAuiPaneInfo() .Left() .CaptionVisible( false ).PinButton( true ).Dock().Resizable().FloatingSize( wxDefaultSize ).CentrePane() );

	m_treeCtrl1 = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT );
	m_mgr.AddPane( m_treeCtrl1, wxAuiPaneInfo() .Top() .CloseButton( false ).PinButton( true ).Dock().Resizable().FloatingSize( wxDefaultSize ).BottomDockable( false ).LeftDockable( false ).RightDockable( false ).MinSize( wxSize( 800,300 ) ) );

	m_UpdateUI_timer.SetOwner( this, wxID_UpdateUI_timer );
	m_UpdateUI_timer.Start( 5 );


	m_mgr.Update();
	this->Centre( wxBOTH );
}

GUIDialog::~GUIDialog()
{
	m_mgr.UnInit();

}

GuiDialogAbout::GuiDialogAbout( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );


	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("WinBleTool BY 何亚红"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer6->Add( m_staticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("依赖库："), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer4->Add( m_staticText3, 0, wxALL, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_hyperlink1 = new wxHyperlinkCtrl( this, wxID_ANY, wxT("wxWidgets"), wxT("https://www.wxwidgets.org/"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer5->Add( m_hyperlink1, 0, wxALL, 5 );

	m_hyperlink2 = new wxHyperlinkCtrl( this, wxID_ANY, wxT("WinBle"), wxT("https://github.com/DerekGn/WinBle.git"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	bSizer5->Add( m_hyperlink2, 0, wxALL, 5 );


	bSizer4->Add( bSizer5, 1, wxEXPAND, 5 );


	bSizer6->Add( bSizer4, 1, wxEXPAND, 5 );


	bSizer3->Add( bSizer6, 1, wxEXPAND, 5 );


	bSizer3->Add( 0, 0, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer3 );
	this->Layout();
	bSizer3->Fit( this );

	this->Centre( wxBOTH );
}

GuiDialogAbout::~GuiDialogAbout()
{
}
