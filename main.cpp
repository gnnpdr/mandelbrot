#include <stdio.h>
#include "include/draw.h"
#include "include/control.h"

int main(int argc, char** argv)
{
    Window mdb_win;
    int err_code = win_ctor(&mdb_win);
    if (err_code)
        return ERRVAL;

    void (*draw_type)(Window*);
    bool graph = false;
    func_selection(argc, argv, &graph, &draw_type);
    if (err_code)
        return ERRVAL;

    sf::Clock main_clock;

    if (graph)
        win_actions(draw_type, &mdb_win);
    else
        draw_type(&mdb_win);

    win_dtor(&mdb_win);

    sf::Time prog_time = main_clock.getElapsedTime();
    printf("prog time: %lg sec\n", prog_time.asSeconds());

    return 0;   
}