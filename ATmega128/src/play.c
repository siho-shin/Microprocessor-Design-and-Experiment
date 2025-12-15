#include <stdint.h>
#include <stdlib.h>

#include "atmega_mem.h"
#include "entry.h"
#include "play.h"
#include "timer.h"
#include "sd.h"
#include "led.h"
#include "fnd.h"
#include "task.h"

// DEBUG
extern time_t ticks;

struct entry_t *cur_video;

struct
{
	struct cache_t video;
	struct cache_t audio;
} cache;

char audio_cache[AUDIO_CACHE_SIZE];

char playing;
char paused;
int playing_time;
char progress_bar_blinking;

uint8_t get_pixel(char *video_buffer, int i, int j)
{
	char byte;

	byte = video_buffer[(i * (VIDEO_WIDTH / BYTES_PER_PIXEL)) + (j / BYTES_PER_PIXEL)];
	return (byte >> (j % BYTES_PER_PIXEL)) & 1;
}

void read_blk(struct cache_t *pc, int index)
{
	uint8_t ret;

	ret = sd_read_blk(
		pc->base + pc->offset,
		(uint8_t *)(pc->mem + (index * 512))
	);
	if (ret) emergency_halt(CACHE_ERROR_CODE);

	pc->offset++;
}

int rounding(double num)
{
	int integer;

	integer = (int)(num * 10.0);
	if (integer % 10 < 5)
		return (int)num;
	return ((int)num) + 1;
}

void progress_bar(int prog, int max)
{
	uint8_t led = 0;
	int i;
	int progress = 0;

	progress = rounding(((double)prog / (double)max) * 8.0);
	for (i = 0; i < 8; i++)
	{
		led <<= 1;
		if (i < progress)
			led |= 1;
		if (i == progress && progress_bar_blinking)
			led |= 1;
	}
	progress_bar_blinking = 1 - progress_bar_blinking;
	led_set(led);
}

void populate_cache(void)
{
	int i;

	// audio
	for (i = 0; i < AUDIO_CACHE_COUNT; i++)
		read_blk(&cache.audio, i);

	// video
	for (i = 0; i < VIDEO_CACHE_COUNT; i++)
		read_blk(&cache.video, i);
}

/**************** TASKS ****************/

void cache_video(void)
{
	int recache_index;

	//time_t start = ticks;
	recache_index = (cache.video.using + VIDEO_CACHE_COUNT - 1) % VIDEO_CACHE_COUNT;
	read_blk(&cache.video, recache_index);
	cache.video.is_uselss = 0;
	//time_t end = ticks;
	//emergency_halt(end - start);
}

void update_time(void)
{
	int frames;
	int duration = cur_video->length_sec;

	frames = cache.video.offset / SECTORS_PER_FRAME;
	playing_time = frames * MS_PER_FRAME / 1000;

	if (playing_time >= duration)
		stop();
}

void play_video(void)
{
	// display
	
	cache.video.using = (cache.video.using + 1) % VIDEO_CACHE_COUNT;
	cache.video.is_uselss = 1;
	
	update_time();

}

void show_playing_time(void)
{
	int min = playing_time / 60;
	int sec = playing_time % 60;

	fnd_display_number_dot(min * 100 + sec, 1);
}

void show_led(void)
{
	progress_bar(playing_time, cur_video->length_sec);
}

TASK_DOIF(cache_video, MS(30), cache.video.is_uselss);
TASK_DOIF(play_video, MS(MS_PER_FRAME), playing);
TASK_DOIF(show_led, MS(500), playing);
TASK_DOIF(show_playing_time, FND_DISPLAY_RATE, TASK_WHILE_TRUE);

/**************** TASKS ****************/

void play_init(struct entry_t *to_play)
{
	cur_video = to_play;

	playing = 0;
	paused = 0;
	playing_time = -1;
	progress_bar_blinking = 0;

	cache.audio.mem = audio_cache;
	cache.audio.base = cur_video->audio_lba;
	cache.audio.offset = 0;
	cache.audio.using = 0;
	cache.audio.is_uselss = 0;

	cache.video.mem = malloc(VIDEO_CACHE_SIZE);
	cache.video.base = cur_video->video_lba;
	cache.video.offset = 0;
	cache.video.using = 0;
	cache.video.is_uselss = 0;

	if (!cache.video.mem)
		emergency_halt(CACHE_MEMORY_ALLOC_ERROR_CODE);
}

void play(void)
{
	playing = 1;

	START_TASK(play_video);
	START_TASK(show_led);

	if (!paused)
	{
		START_TASK(cache_video);
		START_TASK(show_playing_time);
	}
}

void pause(void)
{
	playing = 0;
	paused = 1;

	STOP_TASK(play_video);
	STOP_TASK(show_led);
}

void stop(void)
{
	pause();
	led_set(0);

	STOP_TASK(cache_video);
	STOP_TASK(show_playing_time);

	free(cache.video.mem);
}
