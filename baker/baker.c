#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "../common/entry.h"

uint8_t sector[512];
int sd_fd;
int current_sector = 1;

struct entry_t header[MAX_VIDEOS];

void die(const char *msg)
{
	printf("%s\n", msg);
	exit(-1);
}

void close_sd(void)
{
	close(sd_fd);
}

void open_sd(char *sd_path)
{
	sd_fd = open(sd_path, O_WRONLY);
	if (sd_fd < 0)
		die("cannot open sd card");

	if (lseek(sd_fd, 512, SEEK_SET) < 0)
	{
		close_sd();
		die("cannot seek to sector 1");
	}
}

void print_header(void)
{
	int i;

	for (i = 0; i < MAX_VIDEOS; i++)
	{
		printf("\tentry %d:\n", i);
		printf("\t\taudio lba %d (len: %d)\n", header[i].audio_lba, header[i].audio_cnt);
		printf("\t\tvideo lba %d (len: %d)\n", header[i].video_lba, header[i].video_cnt);
		printf("\t\truntime: %02d:%02d\n", header[i].length_sec / 60, header[i].length_sec % 60);
	}
}

void print_hori_line(void)
{
	printf("==============================================================================\n");
}

void write_one(char *path, int *sector_cnt, int *filesize)
{
	int fd;
	int read_cnt;

	*sector_cnt = 0;
	fd = open(path, O_RDONLY);
	if (fd < 0)
		die("cannot open at write_one");

	do
	{
		memset(sector, 0, 512);
		read_cnt = read(fd, sector, 512);

		write(sd_fd, sector, 512);

		current_sector++;
		(*sector_cnt)++;
	} while (read_cnt == 512);

	if (filesize)
		*filesize = lseek(fd, 0, SEEK_END);

	close(fd);
}

void write_pair(int index, char *audio_path, char *video_path, int *audio_sector, int *video_sector)
{
	int len;
	int filesize;

	*audio_sector = header[index].audio_lba = current_sector;
	write_one(audio_path, &len, &filesize);
	header[index].audio_cnt = len;
	header[index].length_sec = filesize / 8000;	// sampling rate
	printf("\twrote %d sectors from %d (%s)\n", len, *audio_sector, audio_path);

	*video_sector = header[index].video_lba = current_sector;
	write_one(video_path, &len, NULL);
	header[index].video_cnt = len;
	printf("\twrote %d sectors from %d (%s)\n", len, *video_sector, video_path);
}

void write_header(void)
{
	if (lseek(sd_fd, 0, SEEK_SET) < 0)
	{
		close_sd();
		die("cannot seek to sector 0");
	}

	memset(sector, 0, 512);
	memcpy(sector, header, sizeof(struct entry_t) * MAX_VIDEOS);
	write(sd_fd, sector, 512);
}

int main(int argc, char *argv[])
{
	int i;
	int aud_sector;
	int vid_sector;

	if (argc < 2)
	{
		printf("Usage: %s <sd card path> [<audio.bin> <video.bin>]\n", argv[0]);
		return -1;
	}

	open_sd(argv[1]);

	print_hori_line();
	for (i = 2; i < argc; i += 2)
	{
		printf("BEGINNING OF (%s, %s)\n", argv[i], argv[i + 1]);
		write_pair((i / 2) - 1, argv[i], argv[i + 1], &aud_sector, &vid_sector);
	}
	printf("TOTAL SECTORS WRITTEN: %d\n", current_sector);
	print_hori_line();
	printf("BEGINNING OF HEADER\n");
	print_header();
	write_header();
	print_hori_line();

	close_sd();

	return 0;
}
