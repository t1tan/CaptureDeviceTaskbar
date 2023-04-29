#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/msw/registry.h"
#include "wx/stdpaths.h"

#include "TaskBarIcon.h"

#include "DShowUtils.h"

enum
{
    PU_AUTORUN = 10001,
    PU_EXIT,
    PU_DEVICES,
};

//max listed capture devices
const int PU_DEVICES_MAX = 20;

//registry key path our software 
const char* REG_KEY_SOFTWARE_CAPTUREDEVICE_PROPERTIES = "SOFTWARE\\CaptureDeviceTaskbar\\Properties";

//Windows autorun key
const char* REG_KEY_SOFTWARE_WINDOWS_AUTORUN = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
const char* REG_KEY_SOFTWARE_WINDOWS_AUTORUN_CAPTUREDEVICE = "CaptureDeviceTaskbar";

wxBEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_AUTORUN, MyTaskBarIcon::OnMenuAutoRun)
    EVT_MENU(PU_EXIT, MyTaskBarIcon::OnMenuExit)    
    EVT_TASKBAR_LEFT_DCLICK(MyTaskBarIcon::OnLeftButtonDClick)
    EVT_MENU_RANGE(PU_DEVICES, PU_DEVICES + PU_DEVICES_MAX, MyTaskBarIcon::OnMenuDevice)
wxEND_EVENT_TABLE()

//constructor
MyTaskBarIcon::MyTaskBarIcon(wxApp* pParent)
    : m_parent(pParent)
{
    //read from registry
    ReadDefaultDevice();
}

//close app
void MyTaskBarIcon::OnMenuAutoRun(wxCommandEvent&)
{
    SetAutoRun(!IsAutoRun());
}

//close app
void MyTaskBarIcon::OnMenuExit(wxCommandEvent&)
{
    m_parent->Exit();
}

//show property dialog for selected device
void MyTaskBarIcon::ShowDevicePropertiesDialog()
{    
    ShowDevicePropertyDialog(m_defaultDeviceName);
}

//show property dialog for a specific device, also update default device
void MyTaskBarIcon::ShowDevicePropertiesDialog(int id)
{
    if (m_deviceNames.Count() > id)
    {
        m_defaultDeviceName = m_deviceNames[id];
        WriteDefaultDevice();
                
        ShowDevicePropertiesDialog();
    }
}

//when user clicks on a specific device from the popup menu
void MyTaskBarIcon::OnMenuDevice(wxCommandEvent& event)
{
    ShowDevicePropertiesDialog(event.GetId() - PU_DEVICES);
}

//interogate the devices and populate the popup menu
wxMenu* MyTaskBarIcon::CreatePopupMenu()
{
    GetDevicesFriendlyName(m_deviceNames);
    
    wxMenu* menu = new wxMenu();
    
    if (m_deviceNames.IsEmpty())
    {
        wxMenuItem *item = new wxMenuItem(menu, PU_DEVICES, wxT("No capture devices found"));
        item->Enable(false);
        menu->Append(item);
    }
    else
    {
        //minimum between device count and max devices to show
        int count = m_deviceNames.Count() < PU_DEVICES_MAX ? m_deviceNames.Count() : PU_DEVICES_MAX;        
        for (int i = 0; i < count; i++)
        {
            menu->Append(new wxMenuItem(menu, PU_DEVICES + i, m_deviceNames[i], wxEmptyString, wxITEM_CHECK))->Check(m_deviceNames[i] == m_defaultDeviceName);
        }
    }
    
    menu->AppendSeparator();
    menu->Append(new wxMenuItem(menu, PU_AUTORUN, "&Auto start", wxEmptyString, wxITEM_CHECK))->Check(IsAutoRun());
    menu->AppendSeparator();
    menu->Append(PU_EXIT, "E&xit");

    return menu;
}

//quick-open the property dialog for the default device
void MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
    if(!m_defaultDeviceName.IsEmpty())        
        ShowDevicePropertiesDialog();
}

//read from registry the default device
void MyTaskBarIcon::ReadDefaultDevice()
{
    wxRegKey key(wxRegKey::HKCU, REG_KEY_SOFTWARE_CAPTUREDEVICE_PROPERTIES);
    if (key.Exists())
    {
        m_defaultDeviceName = key.QueryDefaultValue();
    }
}

//write to registry the default device
void MyTaskBarIcon::WriteDefaultDevice()
{
    wxRegKey key(wxRegKey::HKCU, REG_KEY_SOFTWARE_CAPTUREDEVICE_PROPERTIES);
    key.Create();
    key.SetValue("", m_defaultDeviceName);
}

bool MyTaskBarIcon::IsAutoRun()
{
    wxRegKey key(wxRegKey::HKCU, REG_KEY_SOFTWARE_WINDOWS_AUTORUN);
    key.Create();
    return key.HasValue(REG_KEY_SOFTWARE_WINDOWS_AUTORUN_CAPTUREDEVICE);
}

void MyTaskBarIcon::SetAutoRun(bool autoRun)
{
    wxRegKey key(wxRegKey::HKCU, REG_KEY_SOFTWARE_WINDOWS_AUTORUN);

    if (autoRun)
    {
        key.Create();
        wxString path = "\"" + wxStandardPaths::Get().GetExecutablePath() + "\"";
        key.SetValue(REG_KEY_SOFTWARE_WINDOWS_AUTORUN_CAPTUREDEVICE, path);
    }
    else
    {
        key.DeleteValue(REG_KEY_SOFTWARE_WINDOWS_AUTORUN_CAPTUREDEVICE);
    }
}
