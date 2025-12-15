#ifndef __PLAY_H__
#define __PLAY_H__

#include "atmega_mem.h"

#define AUDIO_CACHE_SIZE	1024
#define AUDIO_CACHE_COUNT	(AUDIO_CACHE_SIZE / 512)
#define VIDEO_CACHE_SIZE	(MAX_MALLOC_SIZE / 2)
#define VIDEO_CACHE_COUNT	(VIDEO_CACHE_SIZE / 512)

#define CACHE_ERROR_CODE		485
#define CACHE_MEMORY_ALLOC_ERROR_CODE	500
#define CACHE_POPULATING_ERROR_CODE	503

#define VIDEO_WIDTH		128
#define VIDEO_HEIGHT		64
#define BYTES_PER_PIXEL		8

#define FPS			30
#define MS_PER_FRAME		((int)(1000.0 / FPS))
#define AUDIO_SAMPLE_RATE	8000

#define VIDEO_BUFFER_SIZE	(VIDEO_WIDTH * VIDEO_HEIGHT / BYTES_PER_PIXEL)

#define AUDIO_TASK_DEADLINE_MS	((int)(1000.0 * CACHE_SIZE / (double)AUDIO_SAMPLE_RATE))
#define VIDEO_TASK_DEADLINE_MS	MS_PER_FRAME

#define SECTORS_PER_FRAME	2

struct cache_t
{
	char *mem;
	int base;
	int offset;
	int using;
	int is_uselss;
};

void play_init(struct entry_t *to_play);
void play(void);
void pause(void);
void stop(void);

#endif
