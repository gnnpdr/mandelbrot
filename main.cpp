#include <stdio.h>
#include "include/draw.h"
#include "include/control.h"

int main(int argc, char** argv)
{
    Window mdb_win;
    int err_code = win_ctor(&mdb_win);
    if (err_code)
        return ERRVAL;

    void (*calc_values)(Window*);
    bool graph = false;
    processing_func_selection(argc, argv, &graph, &calc_values);
    if (err_code)
        return ERRVAL;

    if (graph)
        draw_graph(calc_values, &mdb_win);
    else
    {
        long long int time_start = ticks();

        calc_values(&mdb_win);

        long long int time_end = ticks();
        long long int time = time_end - time_start;
        printf("ticks: %lld\n", time);
    }

    win_dtor(&mdb_win);    

    return 0;   
}