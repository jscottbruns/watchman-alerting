/**********************************************************************
 *
 *  sio.c
 *
 *  v 0.00 1999.10.8 by Reed Lai
 *
 *  Serial I/O
 *
 *  History:
 *
 *  v 0.00 1999.10.8 by Reed Lai
 *      create
 *
 **********************************************************************/
/*
 * unmark following line to enable debug message 
 */
// #define I7K_DEBUG

#include "sio.h"

struct sio *sio_list;			/* point to the first struct of sio */


/**********************************************************************
 *  _sio_device
 *
 *  found device in sio_listis (opened or not)
 *
 *  Arguments:
 *    port    a string point to the name of serial device,
 *            for example "/dev/ttyS1"
 *
 *  Returned:
 *    0 not found, file descriptor of device found(opened) in list
 *
 **********************************************************************/
int _sio_device(const char *port)
{
	struct sio *sio;

	sio = sio_list;

#ifndef I7565H1H2
	if (sio) {
		while (strcmp(sio->name, port) != 0) {
			if (!sio->next) {	/* not found */
				return (0);
			}
			sio = sio->next;	/* go to check next one */
		}
	} else {					/* list is empty, so create it */
		return (0);
	}
	return sio->fd;				/* found, return its file descriptor */
#else
	return 0;
#endif
}


/**********************************************************************
 *  _sio_follow
 *
 *  Follow list to find device fd, or create it.
 *
 *  Arguments:
 *    fd      file descriptor for the port to find or create
 *
 *  Returned:
 *    a point to a structure sio which with file descriptor fd
 **********************************************************************/
struct sio *_sio_follow(int fd)
{
	struct sio *sio;

	sio = sio_list;

	if (sio) {
		while (sio->fd != fd) {
			if (!sio->next) {	/* no such device, allocate new one */
				sio->next = (struct sio *) malloc(sizeof(struct sio));
				memset(sio->next, 0, sizeof(struct sio));
				sio->next->fd = fd;
				sio->next->options =
					(struct termios *) malloc(sizeof(struct termios));
				sio->next->old_options =
					(struct termios *) malloc(sizeof(struct termios));
				return sio->next;
			}
			sio = sio->next;	/* go to check next one */
		}
	} else {					/* list is empty, so create it */
		sio = (struct sio *) malloc(sizeof(struct sio));
		memset(sio, 0, sizeof(struct sio));
		sio->fd = fd;
		sio->options = (struct termios *) malloc(sizeof(struct termios));
		sio->old_options =
			(struct termios *) malloc(sizeof(struct termios));
		sio_list = sio;
	}

	return sio;					/* got it */
}


/**********************************************************************
 *  _sio_trim
 *
 *  find and eliminate device
 *
 *  Arguments:
 *    sio     list of struct sio
 *    fd      file descriptor for the port to eliminate
 *
 *  Returned:
 *    0 found and eliminated, -1 no such device
 **********************************************************************/
int _sio_trim(int fd)
{
	struct sio *sio;
	struct sio *pre_sio;

	sio = sio_list;
	pre_sio = sio;

	if (sio) {

		while (sio->fd != fd) {
			if (!sio->next) {	/* no such device */
				return -1;
			}
			pre_sio = sio;
			sio = sio->next;	/* go next */
		}

		if (sio->next) {
			if (pre_sio == sio) {
				sio_list = sio->next;	/* the first device will be
										   * deleted */
			} else {
				pre_sio->next = sio->next;	/* jumper */
			}
		} else {
			pre_sio->next = 0;	/* no next, terminated */
		}

		/*
		 * say goodbye to this device 
		 */
		if (sio == sio_list) {
			sio_list = 0;
		}						/* if this device is first and last one, * 
								   * * then set sio_list to empty */
		free(sio->options);
		free(sio->old_options);
		free(sio);

	} else {					/* list empty, no device */

		return -1;

	}
	return 0;
}


/**********************************************************************
 *  sio_set_noncan
 *
 *  turns port to operate in non-canonical mode
 *
 *  Arguments:
 *    fd       file descriptor for the port
 *
 *  Returned:  void
 *
 **********************************************************************/
void sio_set_noncan(int fd)
{
	struct sio *sio;

	PDEBUG("sio_set_noncan: start\n");
	sio = _sio_follow(fd);

	// sio->options->c_iflag &= INLCR;
	sio->options->c_iflag &= ~ICRNL;	/* do not translate CR */
	// sio->options->c_iflag &= ~IGNPAR;

	sio->options->c_lflag &= ~ICANON;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, sio->options);
}


/**********************************************************************
 *  sio_set_canon
 *
 *  turns port to operate in canonical mode
 *
 *  Arguments:
 *    fd       file descriptor for the port
 *
 *  Returned:  void
 *
 **********************************************************************/
void sio_set_canon(int fd)
{
	struct sio *sio;

	PDEBUG("sio_set_canon: start\n");
	sio = _sio_follow(fd);

	// sio->options->c_iflag |= INLCR;
	sio->options->c_iflag |= ICRNL;	/* map CR to NL for canonical mode 
									 */
	// sio->options->c_iflag |= IGNPAR;

	sio->options->c_lflag |= ICANON;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, sio->options);
}


/**********************************************************************
 *  sio_set_timer
 *
 *  set input condictions for given port, this function for
 *  non-canonical mode only
 *
 *  Arguments:
 *    fd       file descriptor for the port
 *    vmin     minmum number of bytes to read
 *    vtime    interbyte timer
 *             the number of tenths-of-a-second to wait for data to arrive
 *             
 *  Returned:  void
 *
 *  There are four combinations for vmin and vtime
 *
 *  A. vmin > 0 , vtime > 0
 *     read returns [VMIN, nbytes] before timer expires;
 *     read returns [1, VMIN] if timer expires
 *     (caller can block indefinitely)
 *
 *  B. vmin > 0 , vtime == 0
 *     read returns [VMIN, nbytes] when available
 *     (caller can block indefinitely)
 *
 *  C. vmin == 0, vtime > 0
 *     read returns [1, nbytes] before timer expires;
 *     read returns 0 if timer expires
 *     (VTIME = read timer)
 *
 *  D. vmin == 0, vtime == 0
 *     read returns [0, nbytes] immediately
 *
 **********************************************************************/
void sio_set_timer(int fd, cc_t vmin, cc_t vtime)
{
	struct sio *sio;

	PDEBUG("sio_set_timer: start\n");
	sio = _sio_follow(fd);

	sio->options->c_cc[VMIN] = vmin;
	sio->options->c_cc[VTIME] = vtime;

	tcflush(fd, TCIFLUSH);
	tcsetattr(fd, TCSANOW, sio->options);
}


/**********************************************************************
 *  _sio_poptions
 *
 *  printout options of port
 *
 *  Arguments:
 *    fd       file descriptor for the port
 *
 *  Returned:  void
 *
 **********************************************************************/
#ifdef I7K_DEBUG
void _sio_poptions(int fd)
{
	struct sio *sio;

	sio = _sio_follow(fd);

	PDEBUG("sio->old_options->c_iflag = %08x\n",
		   sio->old_options->c_iflag);
	PDEBUG("sio->old_options->c_oflag = %08x\n",
		   sio->old_options->c_oflag);
	PDEBUG("sio->old_options->c_cflag = %08x\n",
		   sio->old_options->c_cflag);
	PDEBUG("sio->old_options->c_lflag = %08x\n",
		   sio->old_options->c_lflag);
	PDEBUG("sio->old_options->c_line  = %08x\n", sio->old_options->c_line);
	PDEBUG("-----------------------------------\n");
	PDEBUG("sio->options->c_iflag = %08x\n", sio->options->c_iflag);
	PDEBUG("sio->options->c_oflag = %08x\n", sio->options->c_oflag);
	PDEBUG("sio->options->c_cflag = %08x\n", sio->options->c_cflag);
	PDEBUG("sio->options->c_lflag = %08x\n", sio->options->c_lflag);
	PDEBUG("sio->options->c_line  = %08x\n", sio->options->c_line);
	PDEBUG("-----------------------------------\n");
}

#endif							/* I7K_DEBUG */

/**********************************************************************
 *  sio_set_baud
 *
 *  set baudrate
 *
 *  Arguments:
 *    fd       file descriptor for the port
 *
 *    baud    B0, B50... B9600, B19200, B38400...
 *
 *  Returned:
 *    0 ok, -1 fail
 *
 **********************************************************************/
int sio_set_baud(int fd, speed_t baud)
{
	struct sio *sio;			/* point to current sio structure */
	int r;
	char *cmd;

	cmd = "\r";

  /********************************
   *  allocate new sio structure  *
   ********************************/
	sio = _sio_follow(fd);
	if (!sio) {					/* out of memory */
		close(fd);
		return (-1);
	}

  /*********************
   *  select Baudrate  *
   *********************/
	r = cfsetispeed(sio->options, baud);	/* input */
	if (r) {
		PDEBUG("sioi_set_baud: fails to set input baudrate!\n", r);
		close(fd);
		return (-1);
	}
	r = cfsetospeed(sio->options, baud);	/* output */
	if (r) {
		PDEBUG("sio_set_baud: fails to set output baudrate!\n", r);
		close(fd);
		return (-1);
	}

  /**************************************
   *  set the new options for the port  *
   **************************************/
	tcflush(fd, TCIOFLUSH);
	// tcsetattr(fd, TCSANOW, sio->options);
	tcsetattr(fd, TCSAFLUSH, sio->options);	/* flush input and output
											   * buffers, and make the
											   * change */
	write(fd, cmd, strlen(cmd));
	return (0);
}


/**********************************************************************
 *  sio_open
 *
 *  Open and initiate serial port
 *  default open port in noncanonical mode
 *
 *  Arguments:
 *    port    a string point to the name of serial device,
 *				such as "/dev/ttyS0"
 *    baud    B0, B50... B9600, B19200, B38400...
 *    data    Data bit(s), unsigned char
 *    parity  Parity, unsigned char
 *    stop    Stop bit(s), unsigned char
 *
 *  Returned:
 *    This function returns int port descriptor for the port opened
 *    successfully. Return value ERR_PORT_OPEN if failed.
 *
 **********************************************************************/
int
sio_open(const char *port, speed_t baud, tcflag_t data, tcflag_t parity,
		 tcflag_t stop)
{
	struct sio *sio;			/* point to current sio structure */

	int fd;						/* file descriptor for current port */
	int r;

	struct termios *options;	/* options for current port */

	PDEBUG("sio_open: start\n");

  /***************
   *  open port  *
   ***************/
	fd = _sio_device(port);
	if (fd == 0) {				/* device is not yet opened, so, open it */
		// fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
		//printf("%s ... open %s\n",__FUNCTION__, port);
		fd = open(port, O_RDWR | O_NOCTTY);
	}
	PDEBUG("          fd = %u\n", fd);

	if (fd == -1) {				/* Could not open the port */
		PDEBUG("sio_open: Unable to open %s - %s\n", strerror(errno),
			   port);
		return (ERR_PORT_OPEN);
	}

  /********************************
   *  allocate new sio structure  *
   ********************************/
	sio = _sio_follow(fd);
	if (!sio) {					/* out of memory */
		close(fd);
		return (ERR_PORT_OPEN);
	}

	sio->fd = fd;
	sio->name = port;

	// fcntl(fd, F_SETFL, FNDELAY); /* nonblocking */

	tcgetattr(fd, sio->old_options);	/* save the original options */
	tcgetattr(fd, sio->options);	/* Get the current options */

	options = sio->options;

	/*
	 * printf("\nsio_open\n"); printf("name : %s\n", sio->name);
	 * printf("fd : %d\n", fd); printf("sio : %lx\n", sio);
	 * printf("sio->options : %lx\n", sio->options);
	 * printf("sio->old_options : %lx\n", sio->old_options);
	 * printf("options : %lx\n", options); printf("\n"); 
	 */

  /*********************
   *  select Baudrate  *
   *********************/
	r = cfsetispeed(options, baud);	/* input */
	if (r) {
		PDEBUG("sio_open: fails to set input baudrate!\n", r);
		close(fd);
		return (r);
	}
	r = cfsetospeed(options, baud);	/* output */
	if (r) {
		PDEBUG("sio_open: fails to set output baudrate!\n", r);
		close(fd);
		return (r);
	}

  /**********************
   *  select data bits  *
   **********************/
	options->c_cflag &= ~CSIZE;	/* bit mask for data bits */
	options->c_cflag |= data;
	/*
	 * close(fd); PDEBUG("undefined data bits number %d, port %s
	 * closed.\n", data, port); return (ERR_PORT_OPEN); 
	 */

  /*******************
   *  select parity  *
   *******************/
	switch (parity) {
	case NO_PARITY:
		options->c_cflag &= ~(PARENB | PARODD);
		break;
	case ODD_PARITY:
		options->c_cflag |= PARODD;
		break;
	case EVEN_PARITY:
		options->c_cflag |= PARENB;
		break;
	default:
		tcsetattr(fd, TCSANOW, sio->old_options);
		close(fd);
		_sio_trim(fd);
		PDEBUG("undefined parity code %d, port %s closed.\n", parity,
			   port);
		return (ERR_PORT_OPEN);
	}

  /************************
   *  select stop bit(s)  *
   ************************/
	switch (stop) {
	case ONE_STOP_BIT:
		options->c_cflag &= ~CSTOPB;
		break;
	case TWO_STOP_BITS:
		options->c_cflag |= CSTOPB;
		break;
	default:
		tcsetattr(fd, TCSANOW, sio->old_options);
		close(fd);
		_sio_trim(fd);
		PDEBUG("undefined stop bits code %d, port %s closed.\n", stop,
			   port);
		return (ERR_PORT_OPEN);
	}

  /**********************
   *  other parameterm  *
   **********************/
	/*
	 * posix input mode flags 
	 */
	options->c_iflag &= ~ICRNL;	/* disable map CR to NL for noncanonical */
	options->c_iflag &= ~INLCR;
	options->c_iflag &= ~IXON;	/* disable software flow control
								   * (outgoing) */
	options->c_iflag &= ~IXOFF;	/* disable software flow control
								   * (incoming) */

	/*
	 * posix output mode flags 
	 */
	options->c_oflag &= ~OPOST;	/* raw output */
	options->c_oflag &= ~OLCUC;	/* do not transfer the case */
	options->c_oflag &= ~ONLCR;	/* do not translate the CR and NL */
	options->c_oflag &= ~OCRNL;
	options->c_oflag &= ~NLDLY;	/* no delay for NL */
	options->c_oflag &= ~CRDLY;	/* no delay for CR */
	options->c_oflag &= ~TABDLY;	/* no delay for TAB */
	options->c_oflag &= ~BSDLY;	/* no delay for BS */
	options->c_oflag &= ~VTDLY;	/* no delay for VT */
	options->c_oflag &= ~FFDLY;	/* no delay for FF */

	/*
	 * posix control mode flags 
	 */
	options->c_cflag |= CLOCAL;	/* Local line */
	/*
	 * do not change "owner" of port 
	 */
	options->c_cflag |= CREAD;	/* Enable receiver */

	options->c_cflag &= ~CRTSCTS;	/* Disable hardware flow control */

	/*
	 * posix local mode flags 
	 */
	options->c_lflag &= ~ICANON;	/* default for noncanonical mode */
	options->c_lflag &= ~ECHO;	/* Disable echoing of input characters */
	options->c_lflag &= ~ISIG;	/* disable signals */

	/*
	 * posix control characters 
	 */
	options->c_cc[VINTR] = 0;
	options->c_cc[VQUIT] = 0;
	options->c_cc[VERASE] = 0;
	options->c_cc[VKILL] = 0;
	options->c_cc[VEOF] = 4;
	options->c_cc[VTIME] = 0;	/* Time to wait for data (tenths of
								   * seconds) */
	options->c_cc[VMIN] = 1;
	options->c_cc[VSWTC] = 0;
	options->c_cc[VSTART] = 0;
	options->c_cc[VSTOP] = 0;
	options->c_cc[VSUSP] = 0;
	options->c_cc[VEOL] = 0;
	options->c_cc[VREPRINT] = 0;
	options->c_cc[VDISCARD] = 0;
	options->c_cc[VWERASE] = 0;
	options->c_cc[VLNEXT] = 0;
	options->c_cc[VEOL2] = 0;

  /**************************************
   *  set the new options for the port  *
   **************************************/
	// tcsetattr(fd, TCSANOW, options);
	tcsetattr(fd, TCSAFLUSH, options);	/* flush input and output buffers,
										   * and make the change */
# ifdef I7K_DEBUG
	_sio_poptions(fd);
# endif							/* I7K_DEBUG */

	return (fd);
}


/**********************************************************************
 *  sio_close
 *
 *  Close and restore serial port
 *
 *  Arguments:
 *    fd      File descriptor for the port to close
 *
 *  Returned:
 *    value OK_PORT_CLOSE or ERR_PORT_CLOSE
 *
 **********************************************************************/
int sio_close(int fd)
{
	struct sio *sio;

	PDEBUG("sio_close: start\n");
	PDEBUG("           fd = %u\n", fd);

	sio = _sio_follow(fd);

	/*
	 * printf("\nsio_close\n"); printf("name : %s\n", sio->name);
	 * printf("fd : %d\n", fd); printf("sio : %lx\n", sio);
	 * printf("sio->options : %lx\n", sio->options);
	 * printf("sio->old_options : %lx\n", sio->old_options); printf("\n"); 
	 */

  /***********************************************
   *  restore the original options for the port  *
   ***********************************************/
	tcsetattr(fd, TCSANOW, sio->old_options);

  /************************************
   *  releasse memory to this device  *
   ************************************/
	_sio_trim(fd);

  /*******************
   *  then close it  *
   *******************/
	if (close(fd) == -1) {
		return (ERR_PORT_CLOSE);
	}
	return (OK_PORT_CLOSE);
}
