//****************************************************************************
// @Module        EXSTREAMERLIB
// @Filename      exstreamerlib.h
// @Version       1.0
// @Project       Exstreamer
//----------------------------------------------------------------------------
// @Description   This file contains the interfache to exstreamerlib.dll
//----------------------------------------------------------------------------
// @Company       Barix AG
// @Author        R. Staeheli
// @Date          06/12/02/rs
//****************************************************************************

//****************************************************************************
// @Project Includes
//****************************************************************************


//***************************************************************************
// @Global Constants
//***************************************************************************
#define DEFAULT_PORT        0
#define REMOTE_UDP_PORT     12301
#define STREAM_LISTEN_PORT  2020


//****************************************************************************
// @Function      EXSTREAMERLIB_iSendCmd
//----------------------------------------------------------------------------
// @Description   This function sends a udp command to an exstreamer.
//----------------------------------------------------------------------------
// @Returnvalue   0 if successfull
//----------------------------------------------------------------------------
// @Parameters    szIP      : 0 terminated ip address of the exstreamer
//                iPort     : 0 for default destination port (REMOTE_UDP_PORT)
//                            else command will be sent to iPort
//                szCmd     : 0 terminated command string (ex. "c=2").
//                            see document Technical Description for commands.
//                szRet     : pointer to a string buffer for the answer of the 
//                            exstreamer.
//                iRetLen   : length in bytes of szRet
//                bVerbose  : if true output log and error messages to stdout
//----------------------------------------------------------------------------
// @Date          06/12/02/rs
//****************************************************************************
const int WINAPI EXSTREAMERLIB_iSendCmd   (char *szIP, int iPort,
                                           char *szCmd,
                                           char *szRet, const int iRetLen,
                                           const bool bVerbose);


//****************************************************************************
// @Function      EXSTREAMERLIB_iStreamFile
//----------------------------------------------------------------------------
// @Description   This function streams a file to an exstreamer. The function
//                returns after song was streamed successful.
//----------------------------------------------------------------------------
// @Returnvalue   0 if successfull
//----------------------------------------------------------------------------
// @Parameters    szIP        : 0 terminated ip address of the exstreamer
//                iPort       : 0 for default destination port (STREAM_LISTEN_PORT)
//                              else song will be sent to iPort
//                szFilePath  : path to song
//                bVerbose    : if true output log and error messages to stdout
//----------------------------------------------------------------------------
// @Date          06/12/02/rs
//****************************************************************************
const int WINAPI EXSTREAMERLIB_iStreamFile(char *szIP, int iPort,
                                           char *szFilePath,
                                           const bool bVerbose);