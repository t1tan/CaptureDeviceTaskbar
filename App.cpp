#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/taskbar.h"
#include "wx/msw/registry.h"

#include "App.h"
#include "TaskBarIcon.h"

wxIMPLEMENT_APP(MyApp);

//check the Windows Dark Mode setting
static bool locCheckDarkMode() {
#ifdef _WIN32
    wxRegKey rk(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize");
    if (rk.Exists() && rk.HasValue("AppsUseLightTheme"))
    {
        long value = -1;
        rk.QueryValue("AppsUseLightTheme", &value);
        return value <= 0;
    }
#endif
    
    return wxSystemSettings::GetAppearance().IsDark();
}

//create the taskbar icon
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    if ( !wxTaskBarIcon::IsAvailable() )
    {
        wxMessageBox("There appears to be no system tray support in your current environment. This sample may not behave as expected.", "Warning", wxOK | wxICON_EXCLAMATION);
    }

    m_isDarkMode = locCheckDarkMode();
	
    //needed to load the taskbar icon, as it's a PNG file
    wxImage::AddHandler(new wxPNGHandler());

    //create the taskbar icon object/logic
    m_taskBarIcon = new MyTaskBarIcon(this);
    
    //load the icon from resources, depending on the system theme    
    if(!m_taskBarIcon->SetIcon(wxBitmapBundle::FromResources((IsDarkMode())?"IDI_PNG2":"IDI_PNG1"), "Capture device properties\nRight click to show a list of devices"))
    {
        wxLogError("Could not set icon.");
    }

    return true;
}

//cleanup
int MyApp::OnExit()
{
    delete m_taskBarIcon;

    return wxApp::OnExit();
}
