/**********************************************************************
 *
 *  menu.c
 *
 *  Since 1999.11.26 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */
#include "global.h"
#include "menu.h"

/**********************************************************************
 *  _menu_search_dev
 *
 *  search devices on given parameters
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _menu_search_dev()
{
  /*******************************
   *  setup start baudrate items *
   *******************************/
	item_t beg_bauds[] = {
		{B1200, "B1200"},
		{B2400, "B2400"},
		{B4800, "B4800"},
		{B9600, "B9600"},
		{B19200, "B19200"},
		{B38400, "B38400"},
		{B57600, "B57600"},
		{B115200, "B115200"},
		{0, 0}
	};

	userptr_t beg_baud = {
		FORM_FIELD_CHOSING,
		"",
		"",
		i7Kon.beg_baud,			/*
								   * last chosen 
								 */
		&beg_bauds
	};

  /*******************************
   *  setup stop baudrate items *
   *******************************/
	item_t end_bauds[] = {
		{B1200, "B1200"},
		{B2400, "B2400"},
		{B4800, "B4800"},
		{B9600, "B9600"},
		{B19200, "B19200"},
		{B38400, "B38400"},
		{B57600, "B57600"},
		{B115200, "B115200"},
		{0, 0}
	};

	userptr_t end_baud = {
		FORM_FIELD_CHOSING,
		"",
		"",
		i7Kon.end_baud,			/*
								   * last chosen 
								 */
		&end_bauds
	};

  /*******************************
   *  setup start address field  *
   *******************************/
	char b_addr[I7KON_ADR_SIZE];
	userptr_t beg_addr = {
		FORM_FIELD_DEFAULT,
		i7Kon.beg_addr,			/* last command */
		b_addr,					/* get result here when do_form() done */
		0,						/* last chosen, no use */
		0						/* items' list, no use */
	};

  /******************************
   *  setup stop address field  *
   ******************************/
	char e_addr[I7KON_ADR_SIZE];
	userptr_t end_addr = {
		FORM_FIELD_DEFAULT,
		i7Kon.end_addr,			/* last command */
		e_addr,					/* get result here when do_form() done */
		0,						/* last chosen, no use */
		0						/* items' list, no use */
	};

  /******************
   *  work on form  *
   ******************/
	field_t frm[] = {
		{"from addr", 0, 0, 1, I7KON_ADR_SIZE - 1, NOSECURE, &beg_addr},
		{"to addr", 0, 17, 1, I7KON_ADR_SIZE - 1, NOSECURE, &end_addr},
		{"from baud", 3, 0, 1, 7, NOSECURE, &beg_baud},
		{"to buad", 3, 17, 1, 7, NOSECURE, &end_baud},
		{0, 0, 0, 0, 0, 0, 0}
	};

	int last_key = 0;
	char cmd[6];
	cc_t vmin = 0;
	cc_t vtime = I7KON_DEF_TIMEOUT;
	__tod_t t_exp;
	int baddr, eaddr, a, i, k;
	char c;
	char *s;

	msg_puts("Search Devices - press Q to break when searching", CLEAR);
	if (!i7Kon.sio_fd) {
		msg_puts("\n  there is no port opened, please open one.", NOCLEAR);
		return (-1);
	}

	msg_puts
		("\n  input the serching range of module's address\n  or leave as empty for default from 00 to 255\n  use direction keys to chose the searching range of baudrate",
		 NOCLEAR);

	scr_dump("scrdmp.tmp");
	do_form(&frm, &last_key);
	scr_restore("scrdmp.tmp");
	refresh();

  /**********************
   *  preserve setting  *
   **********************/
	i7Kon.beg_baud = beg_baud.chosen;
	i7Kon.end_baud = end_baud.chosen;
	strcpy(i7Kon.beg_addr, b_addr);
	strcpy(i7Kon.end_addr, e_addr);
	s = (char *) misc_trim_str(e_addr);
	if (!*s) {
		e_addr[0] = '2';
		e_addr[1] = '5';
		e_addr[2] = '5';
		e_addr[3] = 0;
	}

  /********************
   *  search devices  *
   ********************/
	msg_puts("Search Devices -", CLEAR);
	dev_puts("", CLEAR);
	baddr = atoi(b_addr);		/* start address */
	eaddr = atoi(e_addr);		/* stop address */
	i = beg_baud.chosen;		/* index to beginning baudrate */

	sio_set_timer(i7Kon.sio_fd, vmin, vtime);	/* time-out */

	cmd[0] = '$';
	cmd[3] = 'M';
	cmd[4] = '\r';
	cmd[5] = 0;

	nodelay(stdscr, TRUE);		/*
								   * scan keyboard without block 
								 */

	while (i <= end_baud.chosen) {
		sio_set_baud(i7Kon.sio_fd, (speed_t) (*beg_baud.list)[i].value);
		msg_puts("\n  searching on ", NOCLEAR);
		msg_puts((*beg_baud.list)[i].name, NOCLEAR);
		msg_puts(" ...", NOCLEAR);
		a = baddr;
		k = 0;
		while (a <= eaddr) {
			cmd[2] = ASCII(a & 0xf);	/*
										   * cL 
										 */
			cmd[1] = ASCII((a >> 4) & 0xf);	/*
											   * cH 
											 */
			scope_puts(cmd, NOCLEAR);
			*i7Kon.rcv = 0;		/*
								   * clear receive buffer 
								 */
			i7k_send_readt(i7Kon.sio_fd, cmd, i7Kon.rcv, I7KON_RCV_SIZE, &t_exp);
			scope_puts(i7Kon.rcv, NOCLEAR);
			if (*i7Kon.rcv == '!') {
				k++;
				if (k == 1) {
					dev_puts((*beg_baud.list)[i].name, NOCLEAR);
				}
				dev_addmod(i7Kon.rcv);
			}
			a++;

			c = (char) getch();
			if (c == 'q' || c == 'Q') {
				nodelay(stdscr, FALSE);
				msg_puts(" user breaks.", NOCLEAR);
				return (0);
			}
		}
		msg_puts(" done.", NOCLEAR);
		i++;
	}

	nodelay(stdscr, FALSE);
	return (0);
}


/**********************************************************************
 *  _menu_find_lnkit
 *
 *  find learnning kit
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _menu_find_lnkit()
{
	char *cmd;
	cc_t vmin = 0;
	cc_t vtime = I7KON_DEF_TIMEOUT;
	__tod_t t_exp;

	msg_puts("find Learning Kit (9600N81)-", CLEAR);
	if (i7Kon.sio_fd) {

		dev_puts("B9600", CLEAR);
		sio_set_timer(i7Kon.sio_fd, vmin, vtime);	/* set timeout */

		cmd = "$01M\r";
		*i7Kon.rcv = 0;			/* clear receive buffer */
		msg_puts("\n  search i7012D at address 01...", NOCLEAR);
		scope_puts(cmd, NOCLEAR);
		i7k_send_readt(i7Kon.sio_fd, cmd, i7Kon.rcv, I7KON_RCV_SIZE, &t_exp);
		scope_puts(i7Kon.rcv, NOCLEAR);
		if (strcmp("!017012D\r", i7Kon.rcv)) {
			msg_puts(" not found.", NOCLEAR);
		} else {
			msg_puts(" found.", NOCLEAR);
			dev_addmod(i7Kon.rcv);
		}

		cmd = "$02M\r";
		*i7Kon.rcv = 0;			/*
								   * clear receive buffer 
								 */
		msg_puts("\n  search i7021  at address 02...", NOCLEAR);
		scope_puts(cmd, NOCLEAR);
		i7k_send_readt(i7Kon.sio_fd, cmd, i7Kon.rcv, I7KON_RCV_SIZE,
					   &t_exp);
		scope_puts(i7Kon.rcv, NOCLEAR);
		if (strcmp("!027021\r", i7Kon.rcv)) {
			msg_puts(" not found.", NOCLEAR);
		} else {
			msg_puts(" found.", NOCLEAR);
			dev_addmod(i7Kon.rcv);
		}

		cmd = "$03M\r";
		*i7Kon.rcv = 0;			/*
								   * clear receive buffer 
								 */
		msg_puts("\n  search i7060  at address 03...", NOCLEAR);
		scope_puts(cmd, NOCLEAR);
		i7k_send_readt(i7Kon.sio_fd, cmd, i7Kon.rcv, I7KON_RCV_SIZE,
					   &t_exp);
		scope_puts(i7Kon.rcv, NOCLEAR);
		if (strcmp("!037060\r", i7Kon.rcv)
			&& strcmp("!037060D\r", i7Kon.rcv)) {
			msg_puts(" not found.", NOCLEAR);
		} else {
			msg_puts(" found.", NOCLEAR);
			dev_addmod(i7Kon.rcv);
		}

	} else {
		msg_puts("\n  There is no port opened, please open one.", NOCLEAR);
	}
	return (0);
}


/**********************************************************************
 *  _menu_send_cmd
 *
 *  open port
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _menu_send_cmd()
{
	char sndbuf[I7KON_CMD_SIZE];
	userptr_t snd = {
		FORM_FIELD_DEFAULT,
		i7Kon.cmd,				/*
								   * last command 
								 */
		sndbuf,					/*
								   * get result here when do_form() done 
								 */
		0,						/*
								 * last chosen, no use 
								 */
		0						/*
								 * items' list, no use 
								 */
	};

  /******************
   *  work on form  *
   ******************/
	field_t frm[] = {
		{"command", 0, 0, 1, I7KON_CMD_SIZE - 1, NOSECURE, &snd},
		{0, 0, 0, 0, 0, 0, 0}
	};

	char *cmd;
	int last_key = 0;
	cc_t vmin = 0;
	cc_t vtime = I7KON_DEF_TIMEOUT;
	__tod_t t_exp;

	msg_puts("Send Command -\n  input command in field, <ENTER> to go",
			 CLEAR);
	if (i7Kon.sio_fd) {
		scr_dump("scrdmp.tmp");
		do_form(&frm, &last_key);
		scr_restore("scrdmp.tmp");
		refresh();

		cmd = (char *) misc_trim_str(sndbuf);

	/**********************
     *  preserve command  *
     **********************/
		strcpy(i7Kon.cmd, sndbuf);

	/******************
     *  send command  *
     ******************/
		if (*cmd == 0) {
			last_key = 0;
		}						/*
								   * if no command, abort 
								 */
		if ((last_key == ENCTRL('J')) || (last_key == ENCTRL('M'))) {
			if (i7Kon.sio_fd) {

				msg_puts("\n  send command ", NOCLEAR);
				msg_puts(cmd, NOCLEAR);
				msg_puts(" ...", NOCLEAR);

				_i7k_gum_cr(sndbuf, cmd);
				scope_puts(sndbuf, NOCLEAR);
				sio_set_timer(i7Kon.sio_fd, vmin, vtime);	/*
															   * set *
															   * timeout 
															 */
				*i7Kon.rcv = 0;	/*
								   * clear receive buffer 
								 */
				i7k_send_readt(i7Kon.sio_fd, sndbuf, i7Kon.rcv,
							   I7KON_RCV_SIZE, &t_exp);
				scope_puts(i7Kon.rcv, NOCLEAR);

				msg_puts(" done.", NOCLEAR);
				if (*i7Kon.rcv) {
					msg_puts("\n  got response ", NOCLEAR);
					msg_puts(i7Kon.rcv, NOCLEAR);
				} else {
					msg_puts("\n  no response.", NOCLEAR);
				}
			} else {
				msg_puts("\n  There is no port opened, please open one.",
						 NOCLEAR);
			}
		} else {
			msg_puts("\n  command aborted.", NOCLEAR);
		}

	} else {
		msg_puts("\n  There is no port opened, please open one.", NOCLEAR);
	}
	return (0);
}


/**********************************************************************
 *  _menu_close_port
 *
 *  open port
 *
 *  Arguments: none
 *
 *  Returned:  0 ok, -1 no port opened
 *
 **********************************************************************/
int _menu_close_port()
{
  /****************
   *  close port  *
   ****************/
	if (i7Kon.sio_fd) {
		sprintf(i7Kon.msg, "\nclose port %s...", i7Kon.port_name);
		msg_puts(i7Kon.msg, NOCLEAR);
		sio_close(i7Kon.sio_fd);
		i7Kon.sio_fd = 0;
		msg_puts(" done.", NOCLEAR);
		return (0);
	} else {
		msg_puts("\nno port opened!", NOCLEAR);
		return (-1);
	}
}


/**********************************************************************
 *  _menu_open_port
 *
 *  open port
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _menu_open_port()
{
  /**********************
   *  setup port items  *
   **********************/
	item_t ports[] = {
		{0, "/dev/ttyS0"},
		{0, "/dev/ttyS1"},
		{0, "/dev/ttyS2"},
		{0, "/dev/ttyS3"},
		{0, 0}
	};

	userptr_t port = {
		FORM_FIELD_CHOSING,
		"",
		"",
		i7Kon.port,				/*
								   * last chosen 
								 */
		&ports
	};

  /**********************
   *  setup baud items  *
   **********************/
	item_t bauds[] = {
		{B1200, "B1200"},
		{B2400, "B2400"},
		{B4800, "B4800"},
		{B9600, "B9600"},
		{B19200, "B19200"},
		{B38400, "B38400"},
		{B57600, "B57600"},
		{B115200, "B115200"},
		{0, 0}
	};

	userptr_t baud = {
		FORM_FIELD_CHOSING,
		"",
		"",
		i7Kon.baud,				/*
								   * last chosen 
								 */
		&bauds
	};

  /**********************
   *  setup data items  *
   **********************/
	item_t databits[] = {
		{DATA_BITS_5, "5 bits"},
		{DATA_BITS_6, "6 bits"},
		{DATA_BITS_7, "7 bits"},
		{DATA_BITS_8, "8 bits"},
		{0, 0}
	};

	userptr_t data = {
		FORM_FIELD_CHOSING,
		"",
		"",
		i7Kon.data,				/*
								   * last chosen 
								 */
		&databits
	};

  /************************
   *  setup parity items  *
   ************************/
	item_t parities[] = {
		{NO_PARITY, "No Parity"},
		{ODD_PARITY, "Odd Parity"},
		{EVEN_PARITY, "Even Parity"},
		{0, 0}
	};

	userptr_t parity = {
		FORM_FIELD_CHOSING,
		"",
		"",
		i7Kon.parity,			/*
								   * last chosen 
								 */
		&parities
	};

  /*************************
   *  setup stopbit items  *
   *************************/
	item_t stopbits[] = {
		{ONE_STOP_BIT, "1 Stop bit"},
		{TWO_STOP_BITS, "2 Stop bits"},
		{0, 0}
	};

	userptr_t stop = {
		FORM_FIELD_CHOSING,
		"",
		"",
		i7Kon.stop,				/*
								   * last chosen 
								 */
		&stopbits
	};

  /******************
   *  work on form  *
   ******************/
	field_t frm[] = {
		{"Port", 0, 0, 1, 12, NOSECURE, &port},
		{"Speed", 0, 13, 1, 7, NOSECURE, &baud},
		{"Data", 0, 21, 1, 6, NOSECURE, &data},
		{"Parity", 0, 28, 1, 11, NOSECURE, &parity},
		{"Stop", 0, 40, 1, 11, NOSECURE, &stop},
		{0, 0, 0, 0, 0, 0, 0}
	};

	int last_key = 0;

	msg_puts
		("Open Port -\n  <TAB>: field  <UP><DOWN>: change  <ENTER>: go    <ESC>: abort",
		 CLEAR);

	scr_dump("scrdmp.tmp");
	do_form(&frm, &last_key);
	scr_restore("scrdmp.tmp");
	refresh();

  /***************
   *  open port  *
   ***************/
	if ((last_key == ENCTRL('J')) || (last_key == ENCTRL('M'))) {
		if (i7Kon.sio_fd) {
			sprintf(i7Kon.msg, "\n  close opened port %s...",
					i7Kon.port_name);
			msg_puts(i7Kon.msg, NOCLEAR);
			sio_close(i7Kon.sio_fd);
			msg_puts(" done.", NOCLEAR);
		}
		sprintf(i7Kon.msg, "\n  open port %s...",
				(*port.list)[port.chosen].name);
		msg_puts(i7Kon.msg, NOCLEAR);
		i7Kon.sio_fd =
			sio_open((*port.list)[port.chosen].name,
					 (speed_t) (*baud.list)[baud.chosen].value,
					 (tcflag_t) (*data.list)[data.chosen].value,
					 (tcflag_t) (*parity.list)[parity.chosen].value,
					 (tcflag_t) (*stop.list)[stop.chosen].value);
		if (i7Kon.sio_fd) {
			msg_puts(" done.", NOCLEAR);
			i7Kon.port_name = (*port.list)[port.chosen].name;
		} else {
			msg_puts(" failed!", NOCLEAR);
		}
	} else {
		msg_puts("\n  command aborted.", NOCLEAR);
	}

  /**********************
   *  preserve setting  *
   **********************/
	i7Kon.port = port.chosen;
	i7Kon.baud = baud.chosen;
	i7Kon.data = data.chosen;
	i7Kon.parity = parity.chosen;
	i7Kon.stop = stop.chosen;

	// demo_forms();
	return (0);
}


/**********************************************************************
 *  _menu_exec
 *
 *  execute command (when enter pressed)
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _menu_exec()
{
	switch (Menu.list[Menu.bar].id) {
	case MNU_OPEN_PORT:
		_menu_open_port();
		break;
	case MNU_CLOSE_PORT:
		_menu_close_port();
		break;
	case MNU_SEND_CMD:
		_menu_send_cmd();
		break;
	case MNU_FIND_LEARNNING_KIT:
		_menu_find_lnkit();
		break;
	case MNU_SEARCH_DEVICE:
		_menu_search_dev();
		break;
	case MNU_EXIT:
		_menu_close_port();
		return (I7KON_KEY_Q);
	default:
		break;
	}
	return (0);
}


/**********************************************************************
 *  _menu_bar
 *
 *  menu selection bar
 *
 *  Arguments:
 *    w        width of bar
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _menu_bar(int w)
{
	int l, y;
	char *s, *d;
	char bar[80];

	s = Menu.list[Menu.bar].name;
	l = strlen(s);
	d = bar;
	y = Menu.bar - Menu.list_beg;
	while (w--) {
		if (l-- > 0) {
			*d++ = *s++;
		} else {
			*d++ = ' ';
		}
	}
	*d = 0;
	wattrset(Menu.win->content, A_REVERSE);
	mvwaddstr(Menu.win->content, y, 0, bar);
	wattrset(Menu.win->content, A_NORMAL);
	return (0);
}


/**********************************************************************
 *  _menu_list
 *
 *  (re)show menu list
 *
 *  Arguments:
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _menu_list()
{
	int h, w;
	int i, id;

	w = Menu.win->w - 2;
	sb[w] = 0;

	h = Menu.win->h - 2;
	i = 0;
	id = Menu.list_beg + i;
	while ((i < h) && (Menu.list[id].name)) {
		if (Menu.bar == id) {
			_menu_bar(w);
		} else {
			mvwaddnstr(Menu.win->content, i, 0, sb, w);
			mvwaddnstr(Menu.win->content, i, 0, Menu.list[id].name, w);
		}
		id = Menu.list_beg + (++i);
	}
	sb[w] = ' ';
	return (0);
}


/**********************************************************************
 *  menu_show
 *
 *  (re)show menu window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int menu_show()
{
	_i7kon_draw_bwin(Menu.win);
	return (0);
}


/**********************************************************************
 *  menu_release
 *
 *  release menu window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int menu_release()
{
	return (0);
}


/**********************************************************************
 *  menu_init
 *
 *  initiates menu window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int menu_init()
{
	Menu.win = win3;
	Menu.win->title = "MENU";
	_menu_list();
	return (0);
}


/**********************************************************************
 *  menu
 *
 *  menu main rutine
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int menu_main()
{
	int key;

	key = 0;
	while ((key != I7KON_KEY_Q) &&
		   (key != I7KON_KEY_q) && (key != I7KON_KEY_ESC)) {

		key = getch();
		misc_key_stream(key);

		switch (key) {
		case I7KON_KEY_UP:
			if (Menu.bar > 0) {
				Menu.bar--;
			}
			_menu_list();
			menu_show();
			break;
		case I7KON_KEY_DOWN:
			if (!Menu.list[++Menu.bar].name) {
				Menu.bar--;
			}
			_menu_list();
			menu_show();
			break;
		case I7KON_KEY_LEFT:
			break;
		case I7KON_KEY_RIGHT:
			break;
		case I7KON_KEY_ENTER:
			key = _menu_exec();
			break;
		default:
			break;
		}
	}
	return (key);
}
