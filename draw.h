#ifndef _DRAW_H_
#define _DRAW_H_

#include <math.h>
#include <immintrin.h>
#include <assert.h>
#include <stdlib.h>
#include <SFML/Graphics.hpp>

struct Window
{
    float x_center;
    float y_center;
    float delta;
    bool graph;
    int* pixel_arr;
};

#define GET_MDB_WIN_DATA    float delta = mdb_win->delta;       \
                            float x_center = mdb_win->x_center; \
                            float y_center = mdb_win->y_center; \
                            bool graph = mdb_win->graph;        \
                            int* pixel_arr = mdb_win->pixel_arr;

const size_t HEIGHT       =  600; 
const size_t WIDTH        =  800;
const size_t MAX_STEP     =  200;
const float  MAX_DIST     =  100;
const size_t VEC_LEN      =    8;
const float  DELTA        = 0.004;
const size_t FPS_STR_LEN  =   4 ;
const float  X_OFFSET     = -2.2;
const float  Y_OFFSET     = -1.2;
const size_t COLOR        = 0xAAEE0011;
const float  DELTA_ADD    = 0.0003;
const float  CENTER_DELTA =  0.3;
const int    ERRVAL       =  303;

void simple_mandelbrot_processing(Window *const mdb_win);
void array_mandelbrot_processing(Window *const mdb_win);
void SIMD_mandelbrot_processing(Window *const mdb_win);

int add_FPS(sf::Clock& clock, sf::Time& current_time, sf::Time& previous_time, sf::RenderWindow& win);

#endif //_DRAW_H_