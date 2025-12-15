#include "file.h"
#include "sd.h"

#include "fnd.h"

struct entry_t header[MAX_VIDEOS];

uint8_t buffer[512];

void init_video_list(void)
{
	int i;

	for (i = 0; i < sizeof(struct entry_t) * MAX_VIDEOS; i++)
		((char *)&header)[i] = 0;
}

int fetch_video_list(int *pvideo_cnt)
{
	uint8_t ret;
	int i;

	init_video_list();

	ret = sd_read_blk(0, buffer);
	if (ret) return ret;

	for (i = 0; i < sizeof(struct entry_t) * MAX_VIDEOS; i++)
		((char *)&header)[i] = buffer[i];

	for (i = 0; i < MAX_VIDEOS; i++)
		if (!(header[i].audio_lba))
			break;
	*pvideo_cnt = i;

	return 0;
}

struct entry_t *get_video_header(int index)
{
	return &header[index];
}
