#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
#include "wx/msw/registry.h"

#include "TaskBarIcon.h"

#include "DShowUtils.h"

enum
{
    PU_EXIT = 10001,
    PU_DEVICES,
};

const int PU_DEVICES_COUNT = 20;
const char* REG_KEY = "Software\\CaptureDeviceProperties";

wxBEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_EXIT, MyTaskBarIcon::OnMenuExit)    
    EVT_TASKBAR_LEFT_DCLICK(MyTaskBarIcon::OnLeftButtonDClick)
    EVT_MENU_RANGE(PU_DEVICES, PU_DEVICES + PU_DEVICES_COUNT, MyTaskBarIcon::OnMenuDevice)
wxEND_EVENT_TABLE()

//constructor
MyTaskBarIcon::MyTaskBarIcon(wxApp* pParent)
    : m_parent(pParent)
{
    //read from registry
    ReadDefaultDevice();
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
    
    wxMenu* menu = new wxMenu;
    
    if (m_deviceNames.IsEmpty())
    {
        wxMenuItem *item = new wxMenuItem(menu, PU_DEVICES, wxT("No capture devices found"));
        item->Enable(false);
        menu->Append(item);
    }
    else
    {
        //minimum between device count and max devices to show
        int count = m_deviceNames.Count() < PU_DEVICES_COUNT ? m_deviceNames.Count() : PU_DEVICES_COUNT;        
        for (int i = 0; i < count; i++)
        {
            wxMenuItem* item = new wxMenuItem(menu, PU_DEVICES + i, m_deviceNames[i], wxEmptyString, wxITEM_CHECK);
            menu->Append(item);
            if (m_deviceNames[i] == m_defaultDeviceName)
                item->Check();            
        }
    }
    
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
    wxRegKey key(wxRegKey::HKCU, REG_KEY);
    if (key.Exists())
    {
        m_defaultDeviceName = key.QueryDefaultValue();
    }
}

//write to registry the default device
void MyTaskBarIcon::WriteDefaultDevice()
{
    wxRegKey key(wxRegKey::HKCU, REG_KEY);
    key.Create();
    key.SetValue("", m_defaultDeviceName);
}
