#include <stdio.h>
#include <math.h>
#include <SFML/Graphics.hpp>

const size_t HEIGHT   = 800; 
const size_t WIDTH    = 600;
const size_t MAX_STEP = 200;
const float  MAX_DIST = 100;

void temporary_stop_main_loop(sf::RenderWindow& win);
void draw_mandelbrot(float x_center, float y_center, sf::RenderWindow& win);
void color_pixel(size_t step, sf::RectangleShape& pixel);

int main()
{
    sf::RenderWindow win (sf::VideoMode(HEIGHT, WIDTH), "Mandelbrot");
    
    float x_center = 0;
    float y_center = 0;

    while (win.isOpen())
    {
        sf::Event event;
        while (win.pollEvent(event))
            if (event.type == sf::Event::Closed)                    win.close();

        if (event.type == sf::Event::LostFocus)                     temporary_stop_main_loop(win);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))    x_center -= 0.03;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))   x_center += 0.03;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))      y_center -= 0.03;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))    y_center += 0.03; 

        draw_mandelbrot(x_center, y_center, win);
    }
}

void draw_mandelbrot(float x_center, float y_center, sf::RenderWindow& win)
{
    sf::RectangleShape pixel(sf::Vector2f(1, 1));

    float x0 = 0;
    float y0 = 0;

    win.clear(sf::Color::Black);

    for (int col = 0; col < WIDTH; col++)
    {
            for (int row = 0; row < HEIGHT; row++)
        {
            x0 = (float)(row)/250 - 2.2 + x_center;
            y0 = (float)(col)/250 - 1.2 + y_center;
        
            float x = 0;
            float y = 0;
            size_t step = 0;

            for (; step < MAX_STEP; step++)
            {
                float x2 = x * x;
                float y2 = y * y;
                float xy = x * y;
                float dist2 = x2 + y2;
                if (dist2 > MAX_DIST)   break;

                x = x2 - y2 + x0;
                y = xy + xy + y0;
            }

            pixel.setPosition(sf::Vector2f(row, col));
            
            color_pixel(step, pixel);

            win.draw(pixel);
        }
    }

    win.display();
}

void color_pixel(size_t step, sf::RectangleShape& pixel)
{
    size_t color_parameter = 0.5 * step;

    sf::Color basecolor(40, 0, 50);
    sf::Color color(color_parameter * 10 + 100, 0, log(color_parameter) + 60);
                
    if (step == MAX_STEP)  pixel.setFillColor(basecolor);
    else                   pixel.setFillColor(color);
}

void temporary_stop_main_loop(sf::RenderWindow& win) 
{
    sf::Event event;

    while (true) 
    {
        if (win.waitEvent(event) && event.type == sf::Event::GainedFocus) 
            return;
    }
}