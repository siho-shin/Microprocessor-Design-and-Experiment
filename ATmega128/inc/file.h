#ifndef __FILE_H__
#define __FILE_H__

#include <stdint.h>

#define MAX_VIDEOS	8

struct file_t
{
	uint32_t is_valid;
	uint32_t starting_lba;
	uint32_t sector_cnt;

	uint32_t length_sec;
};

int fetch_video_list(int *pvideo_cnt);

#endif
