#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <stdio.h>
#include <string.h>
#include "draw.h"

enum OptType
{
    NO_OPT,
    ARRAY,
    INTRIN
};

static const char* GRAPH_FLAG = "-graph";

int win_ctor(Window *const mdb_win);
void win_dtor(Window* mdb_win);

int func_selection(int argc, char** argv, bool* graph, void (**draw_type)(Window*));
int win_actions(void (*draw_type)(Window*), Window *const mdb_win);

#endif //_CONTROL_H_