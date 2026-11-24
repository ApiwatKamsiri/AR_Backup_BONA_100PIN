Set objFSO = CreateObject( "Scripting.FileSystemObject" )

' Enter parameters
templateName = inputbox( "What is the device type?" & vbCrLf & "com, cpx, io, mem" )
if templateName = "" then
msgbox "ERROR: EMPTY DEVICE TYPE"
wscript.quit
end if
templateName = ucase( left(templateName,1) ) & lcase( right(templateName, len(templateName) - 1 ) )
templateName = "_tempDev" & templateName
if not objFSO.fileexists(templateName&"_c") then
msgbox "ERROR: UNKNOWN DEVICE TYPE"
wscript.quit
end if
fileName = inputbox( "What is the name of the device?" & vbCrLf & "example: camel_Case" )
if fileName = "" then
msgbox "ERROR: EMPTY FILE NAME"
wscript.quit
end if
if len(fileName) < 3 then
msgbox "ERROR: SHORT FILE NAME"
wscript.quit
end if
if objFSO.FileExists( lcase( fileName ) & ".c" ) then
msgbox "ERROR: FILE ALREADY EXISTS"
wscript.quit
end if
if objFSO.FileExists( lcase( fileName ) & ".h" ) then
msgbox "ERROR: FILE ALREADY EXISTS"
wscript.quit
end if
devName = replace( fileName, "_", "" )
briefDesc = inputbox( "What is it about?" )
author = inputbox( "Who is the author?" )


' Create C file
Set objFile = objFSO.OpenTextFile( templateName & "_c", 1 )
template = objFile.readall
objFile.Close

template = replace( template, "%FILENAMELC%", lcase( fileName ) )
template = replace( template, "%FILENAMEUC%", ucase( fileName ) )
template = replace( template, "%DEVNAME%", devName )
template = replace( template, "%DEVNAMEPC%", ucase( left(devName,1) ) & right(devName, len(devName) - 1 ) )
template = replace( template, "%AUTHOR%", author )
template = replace( template, "%BRIEF%", briefDesc )
template = replace( template, "%YEAR%", year(now) )
template = replace( template, "%DATE%", formatdatetime( now, vbShortDate ) )

Set objFile = objFSO.CreateTextFile( lcase( fileName ) & ".c", True )
objFile.Write template
objFile.Close


' Create H file
Set objFile = objFSO.OpenTextFile( templateName & "_h", 1 )
template = objFile.readall
objFile.Close

template = replace( template, "%FILENAMELC%", lcase( fileName ) )
template = replace( template, "%FILENAMEUC%", ucase( fileName ) )
template = replace( template, "%DEVNAME%", devName )
template = replace( template, "%DEVNAMEPC%", ucase( left(devName,1) ) & right(devName, len(devName) - 1 ) )
template = replace( template, "%AUTHOR%", author )
template = replace( template, "%BRIEF%", briefDesc )
template = replace( template, "%YEAR%", year(now) )
template = replace( template, "%DATE%", formatdatetime( now, vbShortDate ) )

Set objFile = objFSO.CreateTextFile( lcase( fileName ) & ".h", True )
objFile.Write template
objFile.Close
