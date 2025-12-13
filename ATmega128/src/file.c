#include "file.h"
#include "sd.h"

#include "fnd.h"

struct file_t videos[MAX_VIDEOS];

uint8_t buffer[512];

void init_video_list(void)
{
	int i;

	for (i = 0; i < sizeof(struct file_t) * MAX_VIDEOS; i++)
		((char *)&videos)[i] = 0;
}

int fetch_video_list(int *pvideo_cnt)
{
	uint8_t ret;
	int i;

	init_video_list();

	ret = sd_read_blk(0, buffer);
	if (ret) return ret;

	for (i = 0; i < sizeof(struct file_t) * MAX_VIDEOS; i++)
		((char *)&videos)[i] = buffer[i];

	for (i = 0; i < MAX_VIDEOS; i++)
		if (!(videos[i].is_valid))
			break;
	*pvideo_cnt = i;

	return 0;
}
