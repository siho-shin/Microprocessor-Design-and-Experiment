#ifndef __FILE_H__
#define __FILE_H__

#include "entry.h"

int fetch_video_list(int *pvideo_cnt);
struct entry_t *get_video_header(int index);

#endif
