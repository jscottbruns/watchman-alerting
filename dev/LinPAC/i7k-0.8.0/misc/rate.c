
#define I7K_DEBUG
#include "i7k.h"

main()
{
	int fd;
	char *cmd, *str;
	char rbuf[256];
	int r, i, j;
	__tod_t texp;
	struct timeval deadline;

	fd = sio_open(SIO_DEV1, B9600, DATA_BITS_8, NO_PARITY, ONE_STOP_BIT);

	cmd = "#02\r";
	// cmd = "%0202080600\r"; /* B9600, non-check-sum */
	// cmd = "%0202080A00\r"; /* B115200, non-check-sum */
	// cmd = "%0202080640\r"; /* B9600, check-sum */

	sio_set_noncan(fd);
	sio_set_timer(fd, 0, 40);

	i7k_send_readt(fd, cmd, rbuf, 255, &texp);
	printf("response %s\n", rbuf);
	return;

	i = 0;
	_time_set_deadline(60000000, &deadline);
	while (!_time_out(&deadline)) {
		i7k_send_readt(fd, cmd, rbuf, 255, &texp);
		i++;
	}
	j = 0;
	_time_set_deadline(60000000, &deadline);
	while (!_time_out(&deadline)) {
		i7k_send_read(fd, cmd, rbuf, 255);
		j++;
	}
	printf("Noncanonical mode (with time-out)\n");
	printf("total cycles: %u in 60 secs\n", i);
	printf("average rate: %u/sec\n\n", (i / 60));

	printf("Canonical mode (without time-out)\n");
	printf("total cycles: %u in 60 secs\n", j);
	printf("average rate: %u/sec\n\n", (j / 60));

	j = 0;
	_time_set_deadline(60000000, &deadline);
	while (!_time_out(&deadline)) {
		i7k_send_read(fd, cmd, rbuf, 255);
		j++;
	}
	i = 0;
	_time_set_deadline(60000000, &deadline);
	while (!_time_out(&deadline)) {
		i7k_send_readt(fd, cmd, rbuf, 255, &texp);
		i++;
	}
	printf("Noncanonical mode (with time-out)\n");
	printf("total cycles: %u in 60 secs\n", i);
	printf("average rate: %u/sec\n\n", (i / 60));

	printf("Canonical mode (without time-out)\n");
	printf("total cycles: %u in 60 secs\n", j);
	printf("average rate: %u/sec\n\n", (j / 60));
	return;

	sio_close(fd);
}
