#ifndef __SWITCH_H__
#define __SWITCH_H__

typedef void(*switch_handler_t)(void);

void switch_init(switch_handler_t handler0, switch_handler_t handler1);
switch_handler_t switch_change_handler(int index, switch_handler_t new_handler);

#endif
