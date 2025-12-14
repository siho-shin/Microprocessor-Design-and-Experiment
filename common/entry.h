#ifndef __ENTRY_H__
#define __ENTRY_H__

#include <stdint.h>

#define MAX_VIDEOS	8

struct entry_t 
{
	uint32_t audio_lba;
	uint32_t audio_cnt;
	uint32_t video_lba;
	uint32_t video_cnt;
	uint32_t length_sec;
};

#endif
