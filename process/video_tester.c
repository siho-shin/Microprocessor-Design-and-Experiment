#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

const int width = 128;
const int height = 64;
const int frame_dim = width * height;
const int frame_size = frame_dim / 8;

char buf[1024];

void sleep_ms(int ms)
{
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

void print_frame(void)
{
	int i, j, k;
	char byte;
	
	for (i = 0; i < frame_size; i++)
	{
		byte = buf[i];

		for (k = 0; k < 8; k++)
		{
			if (byte & 0b10000000)
				printf("*");
			else
				printf(" ");
			byte <<= 1;
		}

		if (i % 16 == 15)
			printf("\n");
	}
}

void job(char *path)
{
	int fd;
	int i;

	fd = open(path, O_RDONLY);
	while (read(fd, buf, frame_size) == frame_size)
	{
		print_frame();
		sleep_ms(33);
		system("clear");
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("INSUFFICIENT ARGUMENTS\n");
		return 1;
	}

	job(argv[1]);
	return 0;
}
