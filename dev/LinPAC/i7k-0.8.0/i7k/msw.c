/**********************************************************************
 *
 *  msw.c
 *
 *  Microsoft Windows Wrap for i7000 series
 *
 *  Since 1999.11.04 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */

/* unmark following line to enable debug message */
// #define I7K_DEBUG

#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

#include "msw.h"

int _fd[MAX_COM_PORTS];

extern char *_i7k_gum_cr(void *cbuf, const char *cmd);

WORD SetLineStatus(BYTE cPort, BYTE DTR_Trig, BYTE RTS_Trig)
{
	int fd;
        int status;
#ifdef STD
        if ((cPort > 0) && (cPort <= MAX_COM_PORTS)) 
#else
        if ((cPort >= 0) && (cPort <= MAX_COM_PORTS)) 
#endif
	{
#ifdef STD
		fd = _fd[cPort - 1];
#else
		fd = _fd[cPort];
#endif
        }
	else 
	{
                return (PortError);
        }

	ioctl(fd, TIOCMGET, &status); /* get the serial port status */

        if(DTR_Trig==1)
                status |= TIOCM_DTR;    //enable DTR
	else
                status &= ~TIOCM_DTR;   //disable DTR

	if(RTS_Trig==1)
                status |= TIOCM_RTS;    //enable RTS
        else
                status &= ~TIOCM_RTS;   //disable RTS

        ioctl(fd, TIOCMSET, &status); /* set the serial port status */

	return NoError;	
}

WORD FW_C2U_SendTrig(BYTE cPort, BYTE Trig)
{
	int fd;
	int status;

  /****************
   *  check prot  *
   ****************/
#ifdef STD
        if ((cPort > 0) && (cPort <= MAX_COM_PORTS))
#else
        if ((cPort >= 0) && (cPort <= MAX_COM_PORTS))
#endif
	{
#ifdef STD
		fd = _fd[cPort - 1];
#else
		fd = _fd[cPort];
#endif
        } else {
                return (PortError);
        }

	//printf("%s ... fd : %d port : %d\n",__FUNCTION__, fd, cPort);
	ioctl(fd, TIOCMGET, &status); /* get the serial port status */

        if(Trig==1)
        {
		status |= TIOCM_DTR;	//enable DTR
		status |= TIOCM_RTS;	//enable RTS
		//SetLineStatus(ComPort, 2, 0);
                //SetLineStatus(ComPort, 1, 1);
        }
        else
        {
		status &= ~TIOCM_DTR;	//disable DTR
		status &= ~TIOCM_RTS;	//disable RTS
                //SetLineStatus(ComPort, 2, 1);
                //SetLineStatus(ComPort, 1, 1);
        }

	ioctl(fd, TIOCMSET, &status); /* set the serial port status */

	return NoError;	
}

/**********************************************************************
 *  Send_Receive_Cmd (Windows)
 *
 *  send command then read its response
 *  warps to i7k_send_readt() or i7k_send_readt_cs()
 *
 *  Arguments:
 *    cPort     COM1, COM2, COM3, COM4
 *    szCmd     ASCIIZ command string
 *    szResult  receive buffer
 *    wTimeOut  interbyte timer
 *              the number of tenths-of-a-second to wait for data to arrive
 *    wChksum   0 non-check-sum, 1 check-sum
 *    wT        total time expended in microsecond
 *
 *  Returned:  NoError (0), PortError, TimeOut, FunctionError
 *
 **********************************************************************/
WORD Send_Receive_Cmd(char cPort, char szCmd[], char szResult[], WORD wTimeOut, WORD wChksum, WORD * wT)
{
	int fd, cr;
	cc_t vmin, vtime;
	__tod_t T;
	int r;
	//WORD r;
	char cmd[MAX_CMD_SIZE + 4];

  /****************
   *  check prot  *
   ****************/
#ifdef STD
        if ((cPort > 0) && (cPort <= MAX_COM_PORTS))
#else
        if ((cPort >= 0) && (cPort <= MAX_COM_PORTS))
#endif
	{
#ifdef STD
		fd = _fd[cPort - 1];
#else
		fd = _fd[cPort];
#endif
	} else {
		return (PortError);
	}

  /****************************************
   *  check length of szCmd, then gum CR  *
   ****************************************/
	if (strlen(szCmd) <= MAX_CMD_SIZE) {
		_i7k_gum_cr(cmd, szCmd);
	} else {
		PDEBUG("Send_Receive_Cmd: the length of command exceeds limit!\n");
		return (FunctionError);
	}

  /***************************
   *  set receiving timeout  *
   ***************************/
	vmin = 0;
	vtime = (cc_t) wTimeOut;
	// sio_set_noncan(fd); /* when open, port is already noncanonical mode 
	// 
	// 
	// */
	sio_set_timer(fd, vmin, vtime);

  /***********************
   *  send then receive  *
   ***********************/
	if (wChksum) {				/* with check-sum */
		r = i7k_send_readt_cs(fd, cmd, szResult, DEFAULT_READ_BUF_SIZE, &T);
	} else {					/* without check-sum */
		r = i7k_send_readt(fd, cmd, szResult, DEFAULT_READ_BUF_SIZE, &T);
	}
	cr = strlen(szResult) - 1;
	if (szResult[cr] == ASCII_CR)
		szResult[cr] = '\0';
	*wT = (WORD) T;				/* time expended */

  /****************
   *  finish job  *
   ****************/
	switch (r) {
	case 0:
		return (TimeOut);
	case -1:
		return (FunctionError);
	default:
		return (NoError);
	}
}

/**********************************************************************
 *  Receive_String
 *
 *  read response
 *  warps to i7k_readt() or i7k_readt_cs()
 *
 *  Arguments:
 *    cPort     COM1, COM2, COM3, COM4
 *    szCmd     ASCIIZ command string
 *    szResult  receive buffer
 *    wTimeOut  interbyte timer
 *           the number of tenths-of-a-second to wait for data to arrive
 *    wChksum   0 non-check-sum, 1 check-sum
 *
 *  Returned:  NoError (0), PortError, TimeOut, FunctionError
 *
 **********************************************************************/
WORD Receive_String(char cPort, char szResult[], WORD wTimeOut, WORD wChksum)
{
        int fd;
        cc_t vmin, vtime;
        __tod_t T;
        int r;

#ifdef STD
        if ((cPort > 0) && (cPort <= MAX_COM_PORTS))
#else
        if ((cPort >= 0) && (cPort <= MAX_COM_PORTS))
#endif
        {
#ifdef STD
                fd = _fd[cPort - 1];
#else
                fd = _fd[cPort];
#endif
        } else {
                return (PortError);
        }

        vmin = 0;
        vtime = (cc_t) wTimeOut;

	sio_set_timer(fd, vmin, vtime);

        if (wChksum)
        {                               /* with check-sum */
                r = i7k_readt_cs(fd, szResult, DEFAULT_READ_BUF_SIZE, &T);
        }
        else
        {                                       /* without check-sum */
                r = i7k_readt(fd, szResult, DEFAULT_READ_BUF_SIZE, &T);
        }

	switch (r) {
        case 0:
                return (TimeOut);
        case -1:
                return (FunctionError);
        default:
                PDEBUG("Receive_String: receive response %s\n", szResult);
                return (NoError);
        }
}


/**********************************************************************
 *  Receive_Cmd
 *
 *  read response
 *  warps to i7k_readt() or i7k_readt_cs()
 *
 *  Arguments:
 *    cPort     COM1, COM2, COM3, COM4
 *    szCmd     ASCIIZ command string
 *    szResult  receive buffer
 *    wTimeOut  interbyte timer
 *              the number of tenths-of-a-second to wait for data to arrive
 *    wChksum   0 non-check-sum, 1 check-sum
 *
 *  Returned:  NoError (0), PortError, TimeOut, FunctionError
 *
 **********************************************************************/
WORD Receive_Cmd(char cPort, char szResult[], WORD wTimeOut, WORD wChksum)
{
	int fd;
	cc_t vmin, vtime;
	__tod_t T;
	int r;

#ifdef STD
        if ((cPort > 0) && (cPort <= MAX_COM_PORTS))
#else
        if ((cPort >= 0) && (cPort <= MAX_COM_PORTS))
#endif
	{
#ifdef STD
                fd = _fd[cPort - 1];
#else
                fd = _fd[cPort];
#endif
	} else {
		return (PortError);
	}

	vmin = 0;
	vtime = (cc_t) wTimeOut;

	// sio_set_noncan(fd); /* when open, port is already noncanonical mode 
	// 
	// 
	// */
	sio_set_timer(fd, vmin, vtime);

	if (wChksum) 
	{				/* with check-sum */
		r = i7k_readt_cs(fd, szResult, DEFAULT_READ_BUF_SIZE, &T);
	}
	else 
	{					/* without check-sum */
		r = i7k_readt(fd, szResult, DEFAULT_READ_BUF_SIZE, &T);
	}
	// *wT = (WORD)T;

	switch (r) {
	case 0:
		return (TimeOut);
	case -1:
		return (FunctionError);
	default:
		PDEBUG("Receive_Cmd: receive response %s\n", szResult);
		return (NoError);
	}
}

WORD Receive_Binary(char cPort, char szResult[], WORD wTimeOut, DWORD wLen, WORD *wT)
{
	int fd;
        cc_t vmin, vtime;
        __tod_t T;
        int r;

#ifdef STD
        if ((cPort > 0) && (cPort <= MAX_COM_PORTS))
#else
        if ((cPort >= 0) && (cPort <= MAX_COM_PORTS))
#endif
	{
#ifdef STD
                fd = _fd[cPort - 1];
#else
                fd = _fd[cPort];
#endif
        } else {
                return (PortError);
        }

        vmin = 0;
        vtime = (cc_t) wTimeOut;

        // sio_set_noncan(fd); /* when open, port is already noncanonical mode
        //
        //
        // */
#ifndef I7565H1H2
        sio_set_timer(fd, vmin, vtime);
#endif

	r = i7k_read_binary(fd, szResult, wLen);
	//r = i7k_readt_binary(fd, szResult, wLen, &T);
        
        // *wT = (WORD)T;

        switch (r) {
        case 0:
                return (TimeOut);
        case -1:
                return (FunctionError);
        default:
                PDEBUG("Receive_Cmd: receive response %s\n", szResult);
                return (NoError);
        }

	return NoError;
}

/**********************************************************************
 *  Send_String
 *
 *  send command
 *  warps to i7k_send() or i7k_send_cs()
 *
 *  Arguments:
 *    cPort     COM1, COM2, COM3, COM4
 *    szCmd     ASCIIZ command string
 *    wTimeOut  interbyte timer
 *              the number of tenths-of-a-second to wait for data to arrive
 *    wChksum   0 non-check-sum, 1 check-sum
 *    wT        total time expended in microsecond
 *
 *  Returned:  NoError (0), PortError, FunctionError
 *
 **********************************************************************/

WORD Send_String(char cPort, char szCmd[], WORD wChksum)
{
        int fd, r;
        char cmd[MAX_CMD_SIZE + 4];

#ifdef STD
        if ((cPort > 0) && (cPort <= MAX_COM_PORTS))
#else
        if ((cPort >= 0) && (cPort <= MAX_COM_PORTS))
#endif
        {
#ifdef STD
                fd = _fd[cPort - 1];
#else
                fd = _fd[cPort];
#endif
        } else {
                return (PortError);
        }

        if (strlen(szCmd) <= MAX_CMD_SIZE) {
                _i7k_gum_cr(cmd, szCmd);
        } else {
                PDEBUG("Send_String: the length of command exceeds limit!\n");
                return (FunctionError);
        }

	 if (wChksum) {                          /* with check-sum */
                r = i7k_send_cs(fd, cmd);
        } else {                                        /* without check-sum */
                r = i7k_send(fd, cmd);
        }
        PDEBUG("Send_String: sent string %s\n", szCmd);
        if (r == -1) {
                r == FunctionError;
        }
        return (WORD) r;
}

/**********************************************************************
 *  Send_Cmd
 *
 *  send command
 *  warps to i7k_send() or i7k_send_cs()
 *
 *  Arguments:
 *    cPort     COM1, COM2, COM3, COM4
 *    szCmd     ASCIIZ command string
 *    wTimeOut  interbyte timer
 *              the number of tenths-of-a-second to wait for data to arrive
 *    wChksum   0 non-check-sum, 1 check-sum
 *    wT        total time expended in microsecond
 *
 *  Returned:  NoError (0), PortError, FunctionError
 *
 **********************************************************************/
WORD Send_Cmd(char cPort, char szCmd[], WORD wChksum)
{
	int fd, r;
	char cmd[MAX_CMD_SIZE + 4];

#ifdef STD
        if ((cPort > 0) && (cPort <= MAX_COM_PORTS))
#else
        if ((cPort >= 0) && (cPort <= MAX_COM_PORTS))
#endif
	{
#ifdef STD
                fd = _fd[cPort - 1];
#else
                fd = _fd[cPort];
#endif
	} else {
		return (PortError);
	}

	if (strlen(szCmd) <= MAX_CMD_SIZE) {
		_i7k_gum_cr(cmd, szCmd);
	} else {
		PDEBUG("Send_Receive_Cmd: the length of command exceeds limit!\n");
		return (FunctionError);
	}

	if (wChksum) {				/* with check-sum */
		r = i7k_send_cs(fd, cmd);
	} else {					/* without check-sum */
		r = i7k_send(fd, cmd);
	}
	PDEBUG("Send_Cmd: sent command %s\n", szCmd);
	if (r == -1) {
		r == FunctionError;
	}
	return (WORD) r;
}

/***************** Send_Binary ****************/
/*                                            */
/* Let users use another protocol that the    */
/* terminal char is not (cr).                 */
/* If users want to use another protocol,     */
/* they must add chksum bytes and terminal    */
/* char into szCmd[] themself.                */
/*                                            */
/* [Oct,26,2009] by Golden                    */
/**********************************************/

WORD Send_Binary(char cPort, char szCmd[],int iLen) //@9
{
	int iRet;
	int fd, r;
        char cmd[MAX_CMD_SIZE + 4];

#ifdef STD
        if ((cPort > 0) && (cPort <= MAX_COM_PORTS))
#else
        if ((cPort >= 0) && (cPort <= MAX_COM_PORTS))
#endif
	{
#ifdef STD
                fd = _fd[cPort - 1];
#else
                fd = _fd[cPort];
#endif
        } else {
                return (PortError);
        }

	r = i7k_send_binary(fd, szCmd, iLen);
/*
	if (write(fd, szCmd, iLen) == -1) {
                PDEBUG("i7k_send: Write error!\n");
                return (-1);
        }
*/
	if (r == -1) 
	{
                r == FunctionError;
        }

	return r;
}



/**********************************************************************
 *  Close_Com (Windows)
 *
 *  Close and restore  COM port
 *  warps to sio_close()
 *
 *  Arguments:
 *    port     COM1, COM2, COM3, COM4
 *
 *  Returned:  NoError (0), PortError, FunctionError
 *
 **********************************************************************/
BOOL Close_Com(char port)
{
	int fd;

#ifdef STD
        if ((port > 0) && (port <= MAX_COM_PORTS))
#else
        if ((port >= 0) && (port <= MAX_COM_PORTS))
#endif
	{
#ifdef STD
                fd = _fd[port - 1];
#else
                fd = _fd[port];
#endif
		if (sio_close(fd) == ERR_PORT_CLOSE) {
			return (FunctionError);
		}
	} else {
		return (PortError);
	}

#ifdef STD
	_fd[port - 1] = 0;
#else
	_fd[port] = 0;
#endif

	return (NoError);
}


/**********************************************************************
 *  Open_Com (Windows)
 *
 *  Open and initiate COM port, set default to noncanonical mode
 *  warps to sio_open()
 *
 *  Arguments:
 *    port     COM1, COM2, COM3, COM4
 *
 *    baudrate baudrate value
 *             1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
 *
 *    cData    Data5Bit, Data6Bit, Data7Bit, Data8Bit
 *
 *    cParity  NonParity, OddParity, EvenParity
 *
 *    cStop    OneStopBit, One5StopBit, TwoStopBit
 *             do not use One5StopBiy which is not defined in posix of linux
 *
 *  Returned:  NoError (0), PortError, BaudRateError, FunctionError
 *
 **********************************************************************/

WORD Open_Com(char port, DWORD baudrate, char cData, char cParity, char cStop)
{
	int fd;
#ifdef STD
	char *device;
#else
	char device[16];
#endif
	speed_t baud;
	tcflag_t data, parity, stop;

  /**********
   *  port  *
   **********/

#ifdef STD
	switch (port) {
	case COM1:
		device = SIO_DEV0;
		break;
	case COM2:
		device = SIO_DEV1;
		break;
	case COM3:
		device = SIO_DEV2;
		break;
	case COM4:
		device = SIO_DEV3;
		break;
	default:
		return (PortError);
	}

	//printf("I7000 : MAX_COM_PORTS %d\n", MAX_COM_PORTS);
	//printf("%s ... Device : %s\n",__FUNCTION__, device);
#else
	if(port < 0 || port > MAX_COM_PORTS)
		return PortError;

#ifdef I7565H1H2
	sprintf(device, "/dev/ttyUSB%d", port);
#else
	sprintf(device, "/dev/ttyVCOM%d", port);
#endif
	//printf("I7565H1H2 : MAX_COM_PORTS %d\n", MAX_COM_PORTS);
	//printf("%s ... Device : %s\n",__FUNCTION__, device);
#endif

  /**************
   *  baudrate  *
   **************/
	switch (baudrate) {
	case 1200:
		baud = B1200;
		break;
	case 2400:
		baud = B2400;
		break;
	case 4800:
		baud = B4800;
		break;
	case 9600:
		baud = B9600;
		break;
	case 19200:
		baud = B19200;
		break;
	case 38400:
		baud = B38400;
		break;
	case 57600:
		baud = B57600;
		break;
	case 115200:
		baud = B115200;
		break;
	default:
		return (BaudRateError);
	}

  /***************
   *  data bits  *
   ***************/
	switch (cData) {
	case Data5Bit:
		data = DATA_BITS_5;
		break;
	case Data6Bit:
		data = DATA_BITS_6;
		break;
	case Data7Bit:
		data = DATA_BITS_7;
		break;
	case Data8Bit:
		data = DATA_BITS_8;
		break;
	default:
		return (FunctionError);
	}

  /******************
   *  parity check  *
   ******************/
	switch (cParity) {
	case NonParity:
		parity = NO_PARITY;
		break;
	case OddParity:
		parity = ODD_PARITY;
		break;
	case EvenParity:
		parity = EVEN_PARITY;
		break;
	default:
		return (FunctionError);
	}

  /*****************
   *  stop bit(s)  *
   *****************/
	switch (cStop) {
	case OneStopBit:
		stop = ONE_STOP_BIT;
		break;
	case TwoStopBit:
		stop = TWO_STOP_BITS;
		break;
	default:
		return (FunctionError);
	}

  /**********
   *  open  *
   **********/
	fd = sio_open(device, baud, data, parity, stop);
	if (fd == ERR_PORT_OPEN) {
		return (PortError);
	}


#ifdef STD
	_fd[port - 1] = fd;			/* record this port's file descriptor */
#else
	_fd[port] = fd;
#endif
	//printf("%s ... fd[%d] = %d\n",__FUNCTION__, port, fd);

	return (NoError);
}

