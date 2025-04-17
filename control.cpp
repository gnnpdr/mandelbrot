#include "../include/control.h"

static void camera_control(sf::Keyboard& keyboard, Window *const mdb_win);
static void temporary_stop_main_loop(sf::RenderWindow& win);

int win_ctor(Window *const mdb_win)
{
    assert(mdb_win);

    int* pixel_arr = (int*)calloc(HEIGHT * WIDTH, sizeof(int));
    if (!pixel_arr)
    {
        printf("allocation error\n");
        return ERRVAL;
    }

    mdb_win->x_center = 0;
    mdb_win->y_center = 0;
    mdb_win->delta = DELTA;

    mdb_win->pixel_arr = pixel_arr;

    return 0;
}

void win_dtor(Window* mdb_win)
{
    free(mdb_win->pixel_arr);
}

int draw_graph(void (*calc_values)(Window*), Window *const mdb_win)
{
    assert(calc_values);
    assert(mdb_win);

    sf::RenderWindow win (sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot");

    sf::Image image;
    image.create(WIDTH, HEIGHT, sf::Color::Black);
    sf::Texture texture = {};
    texture.loadFromImage(image);

    sf::Sprite sprite = {};
    sprite.setTexture(texture);

    sf::Clock clock;
    sf::Time previous_time = clock.getElapsedTime();
    sf::Time current_time;

    while (win.isOpen())
    {
        sf::Event event;
        while (win.pollEvent(event))
            if (event.type == sf::Event::Closed) win.close();
    
        if (event.type == sf::Event::LostFocus) temporary_stop_main_loop(win);

        sf::Keyboard keyboard;
        camera_control(keyboard, mdb_win);
    
        calc_values(mdb_win);    
    
        texture.update((sf::Uint8*)(mdb_win->pixel_arr), WIDTH, HEIGHT, 0, 0);
        win.clear();
        win.draw(sprite);
    
        int FPS = add_FPS(clock, current_time, previous_time, win);
        if (FPS == ERRVAL)
            return ERRVAL;
    
        win.display();
    }   

    return 0;
}

void camera_control(sf::Keyboard& keyboard, Window *const mdb_win)
{
    assert(mdb_win);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))    mdb_win->x_center -= CENTER_DELTA;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))   mdb_win->x_center += CENTER_DELTA;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))      mdb_win->y_center -= CENTER_DELTA;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))    mdb_win->y_center += CENTER_DELTA; 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Equal))   mdb_win->delta    -= DELTA_ADD;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Dash))    mdb_win->delta    += DELTA_ADD; 
}


int processing_func_selection(int argc, char** argv, bool* graph, void (**draw_type)(Window*))
{
    assert(argv);
    assert(draw_type);

    if (argc == 1 || argc > 3)
    {
        printf("wrong arguments amount\n");
        return ERRVAL;
    }

    if (argv[2] && !strcmp(argv[2], GRAPH_FLAG))
        *graph = true;

    int opt_type = atoi(argv[1]);

    switch(opt_type)
    {
        case (NO_OPT):
            *draw_type = simple_mandelbrot_processing;
            break;
        case (ARRAY):
            *draw_type = array_mandelbrot_processing;
            break;
        case (INTRIN):
            *draw_type = SIMD_mandelbrot_processing;
            break;
        default:
            printf("error optimization type\n");
            return ERRVAL;
    }

    return 0;
}

void temporary_stop_main_loop(sf::RenderWindow& win) 
{
    sf::Event event;

    while (true) 
        if (win.waitEvent(event) && event.type == sf::Event::GainedFocus) return;
}

long long int ticks()
{
    long long int res = 0;
    asm(".intel_syntax noprefix\n\t" 
        "rdtsc\n\t"
        "shl rdx, 32\n\t"
        "add rax, rdx\n\t"
        ".att_syntax prefix\n\t"
        :"=a"(res)
        :
        :"%rdx"
    );

    return res;
}