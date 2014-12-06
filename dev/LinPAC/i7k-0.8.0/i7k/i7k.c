/**********************************************************************
 *
 *  i7k.c
 *
 *  v 0.00 1999.10.27 by Reed Lai
 *
 *  modules for i7000 series
 *
 *  History:
 *
 *  v 0.00 1999.10.27 by Reed Lai
 *      create, hmmmm...
 *
 **********************************************************************/

/*
 * unmark following line to enable debug message 
 */
// #define I7K_DEBUG

#include "i7k.h"

/**********************************************************************
 *  _i7k_gum_cr
 *
 *  gum CR (0x0d) to the given command string
 *
 *  Arguments:
 *    cbuf     the size of buffer must > strlen(cmd)+2
 *    cmd      command string with ASCIIZ format
 *
 *  Returned:  a point to cbuf
 *
 **********************************************************************/
char *_i7k_gum_cr(void *cbuf, const char *cmd)
{
	char *p;
	const char *q;

	p = cbuf;
	q = cmd;
	while ((*q != 0) && (*q != ASCII_CR) && (*q != ASCII_LF)) {
		*p++ = *q++;			/* copy string */
	}

	*p++ = ASCII_CR;
	*p = 0;

	return (cbuf);
}


/**********************************************************************
 *  _i7k_gum_chksum
 *
 *  transfer integer chksum value to two bytes ASCII
 *  then gum it with CR (0x0d) to the given command string
 *
 *  Arguments:
 *    cbuf     the size of buffer must > strlen(cmd)+4
 *    cmd      command string with ASCIIZ format
 *
 *  Returned:  a point to cbuf
 *
 **********************************************************************/
char *_i7k_gum_chksum(void *cbuf, const char *cmd)
{
	char cL, cH;
	char *p;
	const char *q;
	unsigned int chksum;

	p = cbuf;
	q = cmd;
	chksum = 0;

	while ((*q != 0) && (*q != ASCII_CR) && (*q != ASCII_LF)) {	/* copy
																   * string */
		chksum += *q;
		*p++ = *q++;
	}
	PDEBUG("_i7k_gum_chksum: check-sum = 0x%x\n", chksum);

	cL = ASCII(chksum & 0xf);
	cH = ASCII((chksum >> 4) & 0xf);

	*p++ = cH;
	*p++ = cL;
	*p++ = ASCII_CR;
	*p = 0;

	return (cbuf);
}


/**********************************************************************
 *  _i7k_chksum
 *
 *  test the check-sum of i7k command or received string
 *  string must has tail of ASCII_CR or ASCII_LF, or 0
 *
 *  Arguments:
 *    str      string to test check-sum
 *
 *  Returned:  0 ok, -1 check-sum error!
 *
 **********************************************************************/
int _i7k_chksum(const char *str)
{
	char cL, cH;
	const char *p;
	unsigned int chksum;

	p = str;
	chksum = 0;

	while ((*p != ASCII_LF) && (*p != ASCII_CR) && (*p != 0)) {
		chksum += *p++;
	}
	chksum -= *(--p);
	chksum -= *(--p);
	PDEBUG("_i7k_chksum: check-sum = 0x%x\n", chksum);

	cL = ASCII(chksum & 0xf);
	cH = ASCII((chksum >> 4) & 0xf);

	if (cH != *p++) {
		return -1;
	}
	if (cL != *p) {
		return -1;
	}

	return (0);
}


/**********************************************************************
 *  i7k_send_readt_cs
 *
 *  Send command with check-sum manipulation,  then read its response
 *  with timer.
 *
 *  This function only works in NON-CANONICAL mode.
 *
 *  Code sio_set_noncan(fd) and sio_set_timer(fd, 0, tsec) before
 *  this function.
 *
 *  This function has less efficient than i7k_send_cs_read() that works
 *  in CANONICAL and without timer mode
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    cmd      command string to send
 *    rbuf     buffer for reading
 *    rmax     max bytes to read, must < (size of rbuf -1)
 *    texp     total time this function expended in microsecond
 *
 *  Returned:
 *    number of bytes read, 0 time-out, -1 error
 *
 **********************************************************************/
int
i7k_send_readt_cs(int fd, const char *cmd, char *rbuf, int rmax,
				  __tod_t * texp)
{
	char cmd_buf[CMD_BUF_SIZE];
	char c;
	char *p;
	unsigned long limit;
	__tod_t t_start;
	int r;

	t_start = _time_get_tod();
	p = rbuf;
	limit = (unsigned long) p + rmax;

  /************************************************
   *  gum checksum and CR code to command string  *
   ************************************************/
	if (strlen(cmd) <= CMD_MAX_SIZE) {
		_i7k_gum_chksum(cmd_buf, cmd);	/* check sum */
	} else {
		PDEBUG("i7k_send_cs_readt: Out of command buffer!\n");
		*texp = _time_get_tod() - t_start;
		return (-1);
	}

  /******************
   *  send command  *
   ******************/
	if (write(fd, cmd_buf, strlen(cmd_buf)) == -1) {
		PDEBUG("i7k_send_cs_readt: Write error!\n");
		*texp = _time_get_tod() - t_start;
		return (-1);
	}
	PDEBUG("i7k_send_cs_readt: sent command: %s\n", cmd_buf);

  /*******************
   *  read response  *
   *******************/
	while ((unsigned long) p < limit) {
		r = read(fd, &c, sizeof(c));
		if ((r == -1) || (r == 0)) {
			*texp = _time_get_tod() - t_start;
			return (r);
		}
		*p++ = c;
		if (c == ASCII_CR) {
			break;
		}
	}
	*p = 0;
	r = p - rbuf;
	if (_i7k_chksum(rbuf)) {
		return (-1);
	}

	/*
	 * check-sum error 
	 */
 /**********
   *  done  *
   **********/
	*texp = _time_get_tod() - t_start;
	return (r);
}


/**********************************************************************
 *  i7k_send_readt
 *
 *  Send command then read its response with timer.
 *
 *  This function only works on NON-CANONICAL mode.
 *
 *  Code sio_set_noncan(fd) and sio_set_timer(fd, 0, tsec) before
 *  this function.
 *
 *  This function has less efficient than i7k_send_read() that works
 *  in CANONICAL and without timer mode
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    cmd      command string to send
 *    rbuf     buffer for reading
 *    rmax     max bytes to read, must < (size of rbuf -1)
 *    texp     total time expended for this function in microsecond
 *
 *  Returned:
 *    number of bytes read, 0 time-out, -1 error
 *
 **********************************************************************/
int
i7k_send_readt(int fd, const char *cmd, char *rbuf, int rmax,
			   __tod_t * texp)
{
	char c;
	char *p;
	unsigned long limit;
	__tod_t t_start;
	int r;

	t_start = _time_get_tod();
	p = rbuf;
	limit = (unsigned long) p + rmax;

  /******************
   *  send command  *
   ******************/
	if (write(fd, cmd, strlen(cmd)) == -1) {
		PDEBUG("i7k_send_readt: Write error!\n");
		*texp = _time_get_tod() - t_start;
		return (-1);
	}
	PDEBUG("i7k_send_readt: sent command: %s\n", cmd);

  /*******************
   *  read response  *
   *******************/
	while ((unsigned long) p < limit) {
		r = read(fd, &c, sizeof(c));
		if ((r == -1) || (r == 0)) {
			*texp = _time_get_tod() - t_start;
			return (r);
		}
		*p++ = c;
		if (c == ASCII_CR) {
			break;
		}
	}
	*p = 0;
	r = p - rbuf;

  /**********
   *  done  *
   **********/
	*texp = _time_get_tod() - t_start;
	return (r);
}


/**********************************************************************
 *  i7k_send_read_cs
 *
 *  Send command with check-sum manipulation, then read its response.
 *  This function only works on canonical mode.
 *
 *  If it's required, code sio_set_canon(fd) before this function.
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    cmd      command string to send
 *    rbuf     buffer for reading
 *    rmax     max bytes to read, must < (size of rbuf -1)
 *
 *  Returned:
 *    number of bytes read, -1 error
 *
 **********************************************************************/
int i7k_send_read_cs(int fd, const char *cmd, char *rbuf, int rmax)
{
	char cmd_buf[CMD_BUF_SIZE];
	int r;

  /************************************************
   *  gum checksum and CR code to command string  *
   ************************************************/
	if (strlen(cmd) <= CMD_MAX_SIZE) {
		_i7k_gum_chksum(cmd_buf, cmd);	/* check sum */
	} else {
		PDEBUG("i7k_send_cs_read: Out of command buffer!\n");
		return (-1);
	}

  /******************
   *  send command  *
   ******************/
	if (write(fd, cmd_buf, strlen(cmd_buf)) == -1) {
		PDEBUG("i7k_send_cs_read: Write error!\n");
		return (-1);
	}
	PDEBUG("i7k_send_cs_read: sent command: %s\n", cmd_buf);

  /*******************
   *  read response  *
   *******************/
	r = read(fd, rbuf, rmax);
	if (r != -1) {
		rbuf[r] = 0;
		if (_i7k_chksum(rbuf)) {
			return (-1);
		}						/* check-sum error */
	} else {
		PDEBUG("i7k_send_cs_read: time-out!\n");
	}

  /**********
   *  done  *
   **********/
	return (r);
}


/**********************************************************************
 *  i7k_send_read
 *
 *  Send command then read its response.
 *  This function only works in CANONICAL mode.
 *
 *  If it's required, code sio_set_canon(fd) before this function.
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    cmd      command string to send
 *    rbuf     buffer for reading
 *    rmax     max bytes to read, must < (size of rbuf -1)
 *
 *  Returned:
 *    number of bytes read, -1 error
 *
 **********************************************************************/
int i7k_send_read(int fd, const char *cmd, char *rbuf, int rmax)
{
	int r;

  /******************
   *  send command  *
   ******************/
	if (write(fd, cmd, strlen(cmd)) == -1) {
		PDEBUG("i7k_send_read: Write error!\n");
		return (-1);
	}
	PDEBUG("i7k_send_read: sent command: %s\n", cmd);

  /*******************
   *  read response  *
   *******************/
	r = read(fd, rbuf, rmax);
	if (r != -1) {
		rbuf[r] = 0;
	} else {
		PDEBUG("i7k_send_read: time-out!\n");
	}

  /**********
   *  done  *
   **********/
	return (r);
}


/**********************************************************************
 *  i7k_readt_cs
 *
 *  read response and test the check-sum  with timer.
 *
 *  This function only works in NON-CANONICAL mode.
 *
 *  Code sio_set_noncan(fd) and sio_set_timer(fd, 0, tsec) before
 *  this function.
 *
 *  This function has less efficient than i7k_send_read() that works
 *  in CANONICAL and without timer mode
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    rbuf     buffer for reading
 *    rmax     max bytes to read, must < (size of rbuf -1)
 *    texp     total time expended for this function in microsecond
 *
 *  Returned:
 *    number of bytes read, 0 time-out, -1 error
 *
 **********************************************************************/
int i7k_readt_cs(int fd, char *rbuf, int rmax, __tod_t * texp)
{
	char c;
	char *p;
	unsigned long limit;
	__tod_t t_start;
	int r;

	t_start = _time_get_tod();
	p = rbuf;
	limit = (unsigned long) p + rmax;

  /*******************
   *  read response  *
   *******************/
	while ((unsigned long) p < limit) {
		r = read(fd, &c, sizeof(c));
		if ((r == -1) || (r == 0)) {
			*texp = _time_get_tod() - t_start;
			return (r);
		}
		*p++ = c;
		if (c == ASCII_CR) {
			break;
		}
	}
	*p = 0;
	if (_i7k_chksum(rbuf)) {
		return (-1);
	}							/* check-sum error */
	r = p - rbuf;

  /**********
   *  done  *
   **********/
	*texp = _time_get_tod() - t_start;
	return (r);
}


/**********************************************************************
 *  i7k_readt
 *
 *  read response with timer.
 *
 *  This function only works in NON-CANONICAL mode.
 *
 *  Code sio_set_noncan(fd) and sio_set_timer(fd, 0, tsec) before
 *  this function.
 *
 *  This function has less efficient than i7k_send_read() that works
 *  in CANONICAL and without timer mode
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    rbuf     buffer for reading
 *    rmax     max bytes to read, must < (size of rbuf -1)
 *    texp     total time expended for this function in microsecond
 *
 *  Returned:
 *    number of bytes read, 0 time-out, -1 error
 *
 **********************************************************************/
int i7k_readt(int fd, char *rbuf, int rmax, __tod_t * texp)
{
	char c;
	char *p;
	unsigned long limit;
	__tod_t t_start;
	int r;

	t_start = _time_get_tod();
	p = rbuf;
	limit = (unsigned long) p + rmax;

  /*******************
   *  read response  *
   *******************/
	while ((unsigned long) p < limit) {
		r = read(fd, &c, sizeof(c));
		
		if ((r == -1) || (r == 0)) {
			*texp = _time_get_tod() - t_start;
			return (r);
		}
		*p++ = c;
		if (c == ASCII_CR) {
			break;
		}
	}
	*p = 0;
	r = p - rbuf;

  /**********
   *  done  *
   **********/
	*texp = _time_get_tod() - t_start;
	return (r);
}

/**********************************************************************
 *  i7k_readt_binary
 *
 *  read response with timer.
 *
 *  This function only works in NON-CANONICAL mode.
 *
 *  Code sio_set_noncan(fd) and sio_set_timer(fd, 0, tsec) before
 *  this function.
 *
 *  This function has less efficient than i7k_send_read() that works
 *  in CANONICAL and without timer mode
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    rbuf     buffer for reading
 *    rmax     max bytes to read, must < (size of rbuf -1)
 *    texp     total time expended for this function in microsecond
 *
 *  Returned:
 *    number of bytes read, 0 time-out, -1 error
 *
 **********************************************************************/
int i7k_readt_binary(int fd, char *rbuf, int rmax, __tod_t * texp)
{
        char c;
        char *p;
        unsigned long limit;
        __tod_t t_start;
        int r;
	int i = 0;

        t_start = _time_get_tod();
        p = rbuf;
        limit = (unsigned long) p + rmax;

  /*******************
   *  read response  *
   *******************/
        while ((unsigned long) p < limit) {
                r = read(fd, &c, sizeof(c));

		//printf("i7k_readt_binary ret = %d\n",r);
                if ((r == -1) || (r == 0)) 
		{
                        *texp = _time_get_tod() - t_start;
                        return (r);
                }
		else
			i++;

//		printf("c value : %c - %x\n",c,c);
                *p++ = c;

                if (i == 20) 
		{
//			printf("i7k_readt_binary ... break\n");
                        break;
                }
        }

        *p = 0;
        r = p - rbuf;

//	printf("rbuf[0] : %x : ID 0x%x\n", ((unsigned long *)&rbuf[0]),*((unsigned long *)&rbuf[0]));
	/**********
         *  done  *
        **********/
        *texp = _time_get_tod() - t_start;

        return (r);
}

int i7k_read_binary(int fd, char *rbuf, int rmax)
{
        int r;

  /*******************
   *  read response  *
   *******************/
        r = read(fd, rbuf, rmax);
        if (r != -1) {
                rbuf[r] = 0;
        } else {
                PDEBUG("i7k_send_read: time-out!\n");
        }

  /**********
   *  done  *
   **********/
        return (r);
}

/**********************************************************************
 *  i7k_read_cs
 *
 *  read response with check-sum test
 *  This function only works in CANONICAL mode.
 *
 *  If it's required, code sio_set_canon(fd) before this function.
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    rbuf     buffer for reading
 *    rmax     max bytes to read, must < (size-of-rbuf - 1)
 *
 *  Returned:
 *    number of bytes read, -1 error
 *
 **********************************************************************/
int i7k_read_cs(int fd, char *rbuf, int rmax)
{
	int r;

  /*******************
   *  read response  *
   *******************/
	r = read(fd, rbuf, rmax);
	if (r != -1) {
		rbuf[r] = 0;
		if (_i7k_chksum(rbuf)) {
			return (-1);
		}						/* check-sum error */
	} else {
		PDEBUG("i7k_send_read: time-out!\n");
	}

  /**********
   *  done  *
   **********/
	return (r);
}


/**********************************************************************
 *  i7k_read
 *
 *  read response from given port.
 *  This function only works in CANONICAL mode.
 *
 *  If it's required, code sio_set_canon(fd) before this function.
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    rbuf     buffer for reading
 *    rmax     max bytes to read, must < (size-of-rbuf - 1)
 *
 *  Returned:
 *    number of bytes read, -1 error
 *
 **********************************************************************/
int i7k_read(int fd, char *rbuf, int rmax)
{
	int r;

  /*******************
   *  read response  *
   *******************/
	r = read(fd, rbuf, rmax);
	if (r != -1) {
		rbuf[r] = 0;
	} else {
		PDEBUG("i7k_send_read: time-out!\n");
	}

  /**********
   *  done  *
   **********/
	return (r);
}


/**********************************************************************
 *  i7k_send_cs
 *
 *  Sends command with check-sum manipulation
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    cmd      command string to send
 *
 *  Returned:
 *    0 ok, -1 error
 *
 **********************************************************************/
int i7k_send_cs(int fd, const char *cmd)
{
	char cmd_buf[CMD_BUF_SIZE];

  /************************************************
   *  gum checksum and CR code to command string  *
   ************************************************/
	if (strlen(cmd) <= CMD_MAX_SIZE) {
		_i7k_gum_chksum(cmd_buf, cmd);	/* check sum */
	} else {
		PDEBUG("i7k_send_cs_read: Out of command buffer!\n");
		return (-1);
	}

  /******************
   *  send command  *
   ******************/
	if (write(fd, cmd_buf, strlen(cmd_buf)) == -1) {
		PDEBUG("i7k_send_cs_read: Write error!\n");
		return (-1);
	}
	PDEBUG("i7k_send_cs_read: sent command: %s\n", cmd_buf);

  /**********
   *  done  *
   **********/
	return (0);
}


/**********************************************************************
 *  i7k_send
 *
 *  sends command
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    cmd      command string to send
 *
 *  Returned:
 *    0 ok, -1 error
 *
 **********************************************************************/
int i7k_send(int fd, const char *cmd)
{
  /******************
   *  send command  *
   ******************/
	if (write(fd, cmd, strlen(cmd)) == -1) {
		PDEBUG("i7k_send: Write error!\n");
		return (-1);
	}
	PDEBUG("i7k_send: sent command: %s\n", cmd);

  /**********
   *  done  *
   **********/
	return (0);
}

/**********************************************************************
 *  i7k_send_binary
 *
 *  sends command without CR
 *
 *  Arguments:
 *    fd       file descriptor of the port to operate
 *             fd is obtained by call sio_open()
 *    cmd      command string to send
 *    len      the length of data
 *
 *  Returned:
 *    0 ok, -1 error
 *
 **********************************************************************/

int i7k_send_binary(int fd, const char *cmd, int len)
{
  /******************
   *  send command  *
   ******************/
	//printf("i7k_send_binary ... len : %d\n",len);
        if (write(fd, cmd, len) == -1) {
                PDEBUG("i7k_send_binary : Write error!\n");
                return (-1);
        }
        PDEBUG("i7k_send_binary: sent command: %s\n", cmd);

  /**********
   *  done  *
   **********/
        return (0);
}

