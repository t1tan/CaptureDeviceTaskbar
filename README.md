# CaptureDeviceTaskbar
A very low overhead application that installs itself into the traybar and can present the selected capture device system property dialog.

Supported platforms:
- Microsoft Windows only

Features:
- Low resource overhead (size, memory, CPU)
- Lists all capture devices on the system at right-click
- Presents them as a list
- Click on one device and the program shows the Capture Device specific Property Dialog (Windows - DirectShow)
- Once a device was chosen the system persists the option in memory and in the registry
- With a valid default device, a double click will suffice to show the Property Dialog again
- Supports a DarkMode icon

Requirements:
- Visual Studio 2022
- wxWidgets library
- Microsoft Windows SDK (DShow)

Compilation:
- create an external\wxWidgets folder that contains only the include and the lib folders (these have to be compiled in both Release and Debug on x64)

Icon:
-Free from https://www.iconfinder.com/bitfreak86
