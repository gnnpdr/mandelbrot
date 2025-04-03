#include <stdio.h>
#include <string.h>
#include "include/draw.h"

void temporary_stop_main_loop(sf::RenderWindow& win);

int main(int argc, char** argv)
{
    if (argc == 1 || argc > 3)
    {
        printf("wrong arguments amount\n");
        return ERRVAL;
    }

    bool graph = false;
    if (argv[2] && strcmp(argv[2], "-graph") == 0)
        graph = true;

    int opt_type = atoi(argv[1]);

    int* pixel_arr = (int*)calloc(HEIGHT * WIDTH, sizeof(int));
        
    float x_center = 0;
    float y_center = 0;
    float delta = DELTA;

    sf::Clock main_clock;

    if (graph)
    {
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
                if (event.type == sf::Event::Closed)                    win.close();
        
            if (event.type == sf::Event::LostFocus)                     temporary_stop_main_loop(win);
        
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))    x_center -= CENTER_DELTA;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))   x_center += CENTER_DELTA;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))      y_center -= CENTER_DELTA;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))    y_center += CENTER_DELTA; 
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Equal))   delta    -= DELTA_ADD;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Dash))    delta    += DELTA_ADD; 
        
            switch(opt_type)
            {
                case (NO_OPT): 
                    draw_mandelbrot(x_center, y_center, delta, pixel_arr, graph);
                    break;
                case (ARRAY):
                    draw_mandelbrot_array(x_center, y_center, delta, pixel_arr, graph);
                    break;
                case (INTRIN):
                    draw_mandelbrot_SIMD(x_center, y_center, delta, pixel_arr, graph);
                    break;
                default:
                    printf("error optimization type\n");
                    return ERRVAL;
            }
        
            texture.update((sf::Uint8*)pixel_arr, WIDTH, HEIGHT, 0, 0);
            win.clear();
            win.draw(sprite);
        
            int FPS = add_FPS(clock, current_time, previous_time, win);
            if (FPS == ERRVAL)
                return 0;
        
            win.display();
        }
    
        
    }
    else
    {
        switch(opt_type)
        {
            case (NO_OPT): 
                draw_mandelbrot(x_center, y_center, delta, pixel_arr, graph);
                break;
            case (ARRAY):
                draw_mandelbrot_array(x_center, y_center, delta, pixel_arr, graph);
                break;
            case (INTRIN):
                draw_mandelbrot_SIMD(x_center, y_center, delta, pixel_arr, graph);
                break;
            default:
                printf("error optimization type\n");
                return ERRVAL;
        }
    }   
    free(pixel_arr);

    sf::Time prog_time = main_clock.getElapsedTime();
    printf("prog time: %lg sec\n", prog_time.asSeconds());

    return 0;
    
}

void temporary_stop_main_loop(sf::RenderWindow& win) 
{
    sf::Event event;

    while (true) 
        if (win.waitEvent(event) && event.type == sf::Event::GainedFocus)   return;
}