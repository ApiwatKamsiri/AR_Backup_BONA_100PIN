Set objShell = WScript.CreateObject ("WScript.shell")

targetCfg = "RCoS+"
targetDoxy = targetCfg & ".Doxyfile"
targetChm = targetCfg & ".chm"

'Compile doxygen configuration
objShell.run """c:\Program Files\doxygen\bin\doxygen.exe"" """ & targetDoxy & """", 1, true

'Compile into chm
'objShell.run """c:\Program Files (x86)\HTML Help Workshop\hhc.exe"" .\html\index.hhp", 1, true
'objShell.run "cmd /c copy /y .\html\index.chm "".\" & targetChm & """", 1, true 'Move chm file to "doc" folder

'Delete "html" folder
'objShell.run "cmd /c rd .\html /s /q", 0, true
