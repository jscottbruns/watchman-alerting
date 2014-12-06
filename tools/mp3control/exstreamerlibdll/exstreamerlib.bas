' description see exstreamerlib.h
Declare Function EXSTREAMERLIB_iSendCmd    Lib "exstreamerlib.dll" (ByVal szIP As String, ByVal iPort As Long, ByVal szCmd As String, ByRef szRet As String, ByVal iRetLen As Long, ByVal bVerbose as Byte) As Long
Declare Function EXSTREAMERLIB_iStreamFile Lib "exstreamerlib.dll" (ByVal szIP As String, ByVal iPort As Long, ByVal szFilePath As String, ByVal bVerbose as Byte) As Long
