#pragma once

class MyTaskBarIcon;

// Define a new application
class MyApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
    virtual int OnExit() wxOVERRIDE;

    bool IsDarkMode() const { return m_isDarkMode; }
	
private:
    MyTaskBarIcon* m_taskBarIcon = nullptr;
    bool m_isDarkMode = false;
};
