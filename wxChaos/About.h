/** 
* @file About.h 
* @brief This header file contains the About dialog.
*
*
* @author Carlos Manuel Rodriguez y Martinez
*
* @date 7/18/2012
*/

#pragma once
#ifndef _aboutDialog
#define _aboutDialog

#include <wx/statline.h>
#include <wx/gbsizer.h>
#include <wx/gdicmn.h>
#include <wx/wx.h>

#define SYMBOL_ABOUTDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_ABOUTDIALOG_TITLE _("About ")
#define SYMBOL_ABOUTDIALOG_IDNAME ID_ABOUTDIALOG
#define SYMBOL_ABOUTDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_ABOUTDIALOG_POSITION wxDefaultPosition

/**
* @class AboutDialog
* @brief A purely informative dialog.
*/
class AboutDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( AboutDialog )
    DECLARE_EVENT_TABLE()

public:
    // Constructors
    AboutDialog();
    AboutDialog( wxWindow* parent, 
		wxWindowID id = SYMBOL_ABOUTDIALOG_IDNAME, 
		const wxString& caption = SYMBOL_ABOUTDIALOG_TITLE, 
		const wxPoint& pos = SYMBOL_ABOUTDIALOG_POSITION, 
		const wxSize& size = SYMBOL_ABOUTDIALOG_SIZE, 
		long style = SYMBOL_ABOUTDIALOG_STYLE );

    // Creation
    bool Create( wxWindow* parent, 
		wxWindowID id = SYMBOL_ABOUTDIALOG_IDNAME, 
		const wxString& caption = SYMBOL_ABOUTDIALOG_TITLE, 
		const wxPoint& pos = SYMBOL_ABOUTDIALOG_POSITION, 
		const wxSize& size = SYMBOL_ABOUTDIALOG_SIZE, 
		long style = SYMBOL_ABOUTDIALOG_STYLE );

    // Destructor
    ~AboutDialog();

    // Initializes member variables
    void Init();

    // Creates the controls and sizers
    void CreateControls();

    wxString GetAppName() const { return m_AppName ; }
    void SetAppName(wxString value) { m_AppName = value ; }

    wxString GetVersion() const { return m_Version ; }
    void SetVersion(wxString value) { m_Version = value ; }

    wxString GetCopyright() const { return m_Copyright ; }
    void SetCopyright(wxString value) { m_Copyright = value ; }

    wxString GetCustomBuildInfo() const { return m_CustomBuildInfo ; }
    void SetCustomBuildInfo(wxString value) { m_CustomBuildInfo = value ; }

    // Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    // Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );

    //helper functions
    enum wxBuildInfoFormat 
    {
        wxBUILDINFO_SHORT,
        wxBUILDINFO_LONG
    };

    static wxString GetBuildInfo(wxBuildInfoFormat format);
    
    void SetHeaderBitmap(const wxBitmap & value);
    void ApplyInfo();

private:
	//AboutDialog member variables
    wxPanel* m_ContentPanel;
    wxStaticBitmap* m_HeaderStaticBitmap;
    wxStaticText* m_AppNameStaticText;
    wxStaticText* m_CopyrightStaticText;
    wxStaticText* m_VersionStaticText;
    wxStaticText* m_BuildInfoStaticText;
    wxString m_AppName;
    wxString m_Version;
    wxString m_Copyright;
    wxString m_CustomBuildInfo;
    /// Control identifiers
    enum {
        ID_ABOUTDIALOG = 10000,
        ID_ContentPanel = 10001
    };
};

#endif