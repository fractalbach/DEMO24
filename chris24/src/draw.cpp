#ifndef DRAW_CPP
#define DRAW_CPP

#include <string>
#include <math.h>
#include "font_data.h"
#include "vec3.h"

#define SCREEN_WIDTH 680
#define SCREEN_HEIGHT 480
#define FONT_WIDTH 32
#define FONT_HEIGHT 25

const char hello[13] = { "HELLO WORLD!" };

vec3 framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
int tmp = 0;



void draw_char( char c, int startx, int starty )
{
    int a = (int)c - 32;
    int tilex = a % 10;
    int tiley = a / 10;
    for (int offy = 0; offy < FONT_HEIGHT; offy++)
    {
        for (int offx = 0; offx < FONT_WIDTH; offx++)
        {
            int imgx = FONT_WIDTH * tilex + offx;
            int imgy = FONT_HEIGHT * tiley + offy;
            float r = font_data[imgy][imgx][0] / 63.0f;
            float g = font_data[imgy][imgx][1] / 63.0f;
            float b = font_data[imgy][imgx][2] / 63.0f;
            if ( (r+g+b) < 0.001 ) {
                continue;
            }
            // if (run_once < (32*32)) {
            //     printf("(%d,%d) : (%f, %f, %f)\n", imgx,imgy,r,g,b);
            //     run_once++;
            // }
            int targetx = startx + offx;
            int targety = starty + offy;
            framebuffer[targety][targetx] = vec3{ r, g, b };
        }
    }
}


void draw_string( const char *s, int len, int startx, int starty )
{
    for (int i = 0; i < len; i++ )
    {
        int targetx = startx + i * FONT_WIDTH;
        draw_char( s[i], targetx, starty );
        // if (tmp < len) {
        //     printf("i=%d, x=%d, y=%d \n", i,x,y);
        //     tmp++;
        // }
    }
}


// Draw Function is the primary way we get pixels on the screen.
//
// iTime: floating point number of seconds elapsed since beginning of program
//
void draw( float iTime )
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            const float u = float(x) / SCREEN_WIDTH;
            const float v = float(y) / SCREEN_HEIGHT;
            
            vec3 color = vec3{
                0.5f + 0.5f * cos( iTime + u ),
                0.5f + 0.5f * cos( iTime + v + 2.0f ),
                0.5f + 0.5f * cos( iTime + u + 4.0f )
            };

            framebuffer[y][x] = color;
        }
    }

    constexpr int string_range = (SCREEN_HEIGHT - FONT_HEIGHT);
    int string_x = 10;
    int string_y = string_range/2.0f * cos( iTime ) + string_range/2.0f;
    draw_string( hello, 12, string_x, string_y );

}

#endif // #ifndef DRAW_CPP