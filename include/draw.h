#include <math.h>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <immintrin.h>
#include <assert.h>

enum OptType
{
    NO_OPT,
    ARRAY,
    INTRIN
};

const size_t       HEIGHT       =  600; 
const size_t       WIDTH        =  800;
const size_t       MAX_STEP     =  200;
const float        MAX_DIST     =  100;
const size_t       VEC_LEN      =    8;
const float        DELTA        = 0.004;
const size_t       FPS_STR_LEN  =   4 ;
const int          ERRVAL       =  303;
const float        CENTER_DELTA =  0.3;
const float        X_OFFSET     = -2.2;
const float        Y_OFFSET     = -1.2;
const float        DELTA_ADD    = 0.0003;
const unsigned int COLOR        = 0xAAEE0011;


void draw_mandelbrot(float x_center, float y_center, float delta, int * pixel_arr, bool graph);
void draw_mandelbrot_array(float x_center, float y_center, float delta, int * pixel_arr, bool graph);
void draw_mandelbrot_SIMD(float x_center, float y_center, float delta, int * pixel_arr, bool graph);

int add_FPS(sf::Clock& clock, sf::Time& current_time, sf::Time& previous_time, sf::RenderWindow& win);