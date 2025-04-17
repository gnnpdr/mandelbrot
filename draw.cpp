#include "../include/draw.h"

static void set_pixel(int *const pixel_arr, size_t row, size_t col, size_t step);
static int color_func(int step);

int add_FPS(sf::Clock& clock, sf::Time& current_time, sf::Time& previous_time, sf::RenderWindow& win)
{
    current_time = clock.getElapsedTime();
    float FPS = 1.0 / (current_time.asSeconds() - previous_time.asSeconds());
    int FPS_int  = (int)FPS;
    previous_time = current_time;
        
    char FPS_str[FPS_STR_LEN] = {};
    int snprintf_res = snprintf(FPS_str, FPS_STR_LEN, "%.0f", FPS);
    if (snprintf_res >= FPS_STR_LEN)
    {
        printf("snprintf error\n");
        return ERRVAL;
    }

    sf::Font font;
    if (!font.loadFromFile("source/font.ttf"))
    {
        printf("font error\n");
        return ERRVAL;
    }

    sf::Text text("", font, 25);
    text.setFillColor(sf::Color::Black);
    text.setString(FPS_str);
    win.draw(text);

    return FPS_int;
}

void simple_mandelbrot_processing(Window *const mdb_win)
{
    assert(mdb_win);

    GET_MDB_WIN_DATA

    for (size_t row = 0; row < HEIGHT; row++)
    {
        for (size_t col = 0; col < WIDTH; col++)
        {
            float x0 = col * delta + X_OFFSET + x_center;
            float y0 = row * delta + Y_OFFSET + y_center;
        
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

            if (graph) set_pixel(pixel_arr, row, col, step);
        }
    }
    mdb_win->pixel_arr = pixel_arr;
}

void array_mandelbrot_processing(Window *const mdb_win)
{
    assert(mdb_win);

    GET_MDB_WIN_DATA

    for (size_t row = 0; row < HEIGHT; row++)
    {
        for (size_t col = 0; col < WIDTH; col += VEC_LEN)
        {
            float y0[VEC_LEN] = {0};
            float x0[VEC_LEN] = {0};

            int cur_col = col;
            for (size_t el = 0; el < VEC_LEN; el++)
            {
                x0[el] = cur_col * delta + X_OFFSET + x_center;
                cur_col++;
            }
            
            for (size_t el = 0; el < VEC_LEN; el++)     y0[el] = row * delta + Y_OFFSET + y_center;
        
            float x[VEC_LEN] = {0};
            float y[VEC_LEN] = {0};
            size_t step[VEC_LEN] = {0};
            
            size_t cmp[VEC_LEN] = {0};
            for (size_t el = 0; el < VEC_LEN; el++)       cmp[el] = 1;

            for (size_t step_cnt = 0; step_cnt < MAX_STEP; step_cnt++)
            {
                float x2[VEC_LEN] = {0};
                float y2[VEC_LEN] = {0};
                float xy[VEC_LEN] = {0};
                float dist2[VEC_LEN] = {0};

                for (int i = 0; i < VEC_LEN; i++)
                {
                    x2[i] = x[i] * x[i];
                    y2[i] = y[i] * y[i];
                    xy[i] = x[i] * y[i];

                    dist2[i] = x2[i] + y2[i];
                }

                for (size_t el = 0; el < VEC_LEN; el++)
                    if (dist2[el] > MAX_DIST)                   cmp[el] = 0;
                    
                int mask = 0;
                for (size_t el = 0; el < VEC_LEN; el++)         mask |= (cmp[el] << el);

                if (!mask)                                      break;

                for (size_t el = 0; el < VEC_LEN; el++)
                {
                    x[el] = x2[el] - y2[el] + x0[el];
                    y[el] = xy[el] + xy[el] + y0[el];

                    step[el] = step[el] + cmp[el];
                }
            }

            if (graph)
                for (size_t pixel = 0; pixel < VEC_LEN; pixel++)     set_pixel(pixel_arr, row, col + pixel, step[pixel]);
        }
    }

    mdb_win->pixel_arr = pixel_arr;
}

void SIMD_mandelbrot_processing(Window *const mdb_win)
{
    assert(mdb_win);

    GET_MDB_WIN_DATA

    __m256 max_dist  = _mm256_set1_ps(MAX_DIST);
    __m256 delta_vec = _mm256_set1_ps(delta);
    __m256 x_offset  = _mm256_set1_ps(X_OFFSET + x_center);
    __m256 y_offset  = _mm256_set1_ps(Y_OFFSET + y_center);

    for (size_t row = 0; row < HEIGHT; row++)
    {
        for (size_t col = 0; col < WIDTH; col += VEC_LEN)
        {
            __m256 y0 = _mm256_set1_ps(row);
            y0 = _mm256_mul_ps(y0, delta_vec);
            y0 = _mm256_add_ps(y0, y_offset);

            __m256 x0 = _mm256_set_ps(col + 7, col + 6, col + 5, col + 4, col + 3, col + 2, col + 1, col);
            x0 = _mm256_mul_ps(x0, delta_vec);
            x0 = _mm256_add_ps(x0, x_offset);

            __m256 x = _mm256_setzero_ps();
            __m256 y = _mm256_setzero_ps();
            __m256i step = _mm256_setzero_si256();

            for (size_t step_cnt = 0; step_cnt < MAX_STEP; step_cnt++)
            {
                __m256 x2    = _mm256_mul_ps(x, x);
                __m256 y2    = _mm256_mul_ps(y, y);
                __m256 xy    = _mm256_mul_ps(x, y);
                __m256 dist2 = _mm256_add_ps(x2, y2);

                __m256 cmp = _mm256_cmp_ps(dist2, max_dist, _CMP_LE_OS);

                int mask = _mm256_movemask_ps(cmp);

                if (!mask)  break;

                __m256i tmp = _mm256_castps_si256(cmp);
                tmp  = _mm256_srli_epi32(tmp, 31);
                step = _mm256_add_epi32(step, tmp);

                x = _mm256_sub_ps(x2, y2);
                x = _mm256_add_ps(x, x0);

                y = _mm256_add_ps(xy, xy);
                y = _mm256_add_ps(y, y0);
            }

            uint32_t* size_t_step = (uint32_t*)(&step);

            if (graph)
                for (int i = 0; i < VEC_LEN; i++)   set_pixel(pixel_arr, row, col + i, size_t_step[i]);
        }
    }
    mdb_win->pixel_arr = pixel_arr;
}

void set_pixel(int *const pixel_arr, size_t row, size_t col, size_t step)
{
    assert(pixel_arr);

    int color = color_func(step);

    if (step == MAX_STEP)  pixel_arr[row * 800 + col] = 0x00000000;
    else                   pixel_arr[row * 800 + col] = color;
}

int color_func(int step)
{
    return 0xCCCCCCCC - step * 16;
}