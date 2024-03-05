#ifndef DRAW_CPP
#define DRAW_CPP

#include <math.h>
#include <stdio.h>
#include "font_data.h"
#include "credits.h"
#include "vec3.h"

#define SCREEN_WIDTH 680
#define SCREEN_HEIGHT 480
#define FONT_WIDTH 32
#define FONT_HEIGHT 25

const char hello[13] = { "HELLO WORLD!" };
const char special[10] = { "CREDITS!!" };

vec3  framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
float time_previous_frame = 0.0f;
char  str_fps[5] = { "0000" };
char  str_time[6] = { "00:00" };
int   debug_frame = 0;


void draw_char( char c, int startx, int starty )
{
    assert( c >= ' ' && c <= 'Z' );
    int a = (int)c - 32;
    int tilex = a % 10;
    int tiley = a / 10;

    for (int offy = 0; offy < FONT_HEIGHT; offy++)
    {
        int targety = starty + offy;
        int imgy = FONT_HEIGHT * tiley + offy;
        if (targety > SCREEN_HEIGHT || targety < 0)
            continue;

        for (int offx = 0; offx < FONT_WIDTH; offx++)
        {
            int targetx = startx + offx;
            if (targetx > SCREEN_WIDTH || targetx < 0)
                continue;
            int imgx = FONT_WIDTH * tilex + offx;
            float r = font_data[imgy][imgx][0] / 63.0f;
            float g = font_data[imgy][imgx][1] / 63.0f;
            float b = font_data[imgy][imgx][2] / 63.0f;
            if ( (r+g+b) < 0.001 )
                continue;

            framebuffer[targety][targetx] = vec3{ r, g, b };

            // HACK: wastefully draws a drop shadow
            if (targety+2 > SCREEN_HEIGHT || targetx+2 > SCREEN_WIDTH)
                continue;
            framebuffer[ targety + 1 ][ targetx + 1 ] = vec3{ 0.f, 0.f, 0.f };
            framebuffer[ targety + 2 ][ targetx + 2 ] = vec3{ 0.f, 0.f, 0.f };
        }
    }
}


void draw_string( const char *s, int len, int startx, int starty )
{
    for (int i = 0; i < len; i++ )
    {
        int targetx = startx + i * FONT_WIDTH;
        draw_char( s[i], targetx, starty );
    }
}


void draw_string_centered_around_point( const char *s, int len, int centerx, int centery )
{
    // HACK: ignore whitespace at the end
    int i;
    for ( i = len-1; i >= 0; i-- )
        if ( s[i] != ' ')
            break;
    if (i == 0 && s[i] == ' ')
        return;
    len = i+1;
    const int startx = centerx - (FONT_WIDTH * len / 2);
    const int starty = centery - (FONT_HEIGHT / 2);
    draw_string( s, len, startx, starty );
}

void draw_string_centered( const char *s, int len, int starty )
{
    constexpr int centerx = SCREEN_WIDTH / 2;

    // HACK: ignore whitespace at the end
    int i;
    for ( i = len-1; i >= 0; i-- )
        if ( s[i] != ' ')
            break;
    if (i == 0 && s[i] == ' ')
        return;
    const int startx = centerx - (i+1) * FONT_WIDTH / 2;

    draw_string( s, (i+1), startx, starty );
}


void draw_credits( float time )
{
    const float speed_lines = 1.5;
    const int line_height = FONT_HEIGHT * 2;
    const int speed_pix = FONT_HEIGHT * speed_lines;
    const int credits_start_pos = SCREEN_HEIGHT - speed_pix * time;
    for (int i = 0; i < credits_len; i++)
    {
        const int pos = credits_start_pos + line_height * i;
        if (pos > -FONT_HEIGHT && pos < SCREEN_HEIGHT)
            draw_string_centered(credits[i], sizeof(credits[i])-1, pos);
    }
}

void draw_part1( float time )
{
    // Draw interesting text (bounces up and down)
    // const int RANGE1 = (SCREEN_HEIGHT - FONT_HEIGHT + 50);
    // const int string_y = RANGE1/2.0f * cos( time ) + SCREEN_HEIGHT/2.0F;
    // draw_string_centered( hello, sizeof(hello)-1, string_y );

    // Draw more text (bounces left to right)
    const int RANGE2 = (SCREEN_WIDTH - FONT_WIDTH);
    const int string_x = (SCREEN_WIDTH/2-150) * sin( time ) + (SCREEN_WIDTH/2);
    const int string_y = (SCREEN_HEIGHT+20) -  ((SCREEN_HEIGHT+50)/10.0f) * time;
    draw_string_centered_around_point( special, sizeof(special)-1, string_x, string_y );
}

void draw_part2( float time )
{
    // Roll the credits!
    
}

// Draw Function is the primary way we get pixels on the screen.
// iTime: floating point number of seconds elapsed since beginning of program
void draw( float time, int frame )
{
    debug_frame = frame; // HACK: remove after debugging
    if (frame % 1000 == 0) {
        printf("frame=(%d)", frame);
    }


    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            const float u = float(x) / SCREEN_WIDTH;
            const float v = float(y) / SCREEN_HEIGHT;
            
            vec3 color = vec3{
                0.5f + 0.5f * cos( time + u ),
                0.5f + 0.5f * cos( time + v + 2.0f ),
                0.5f + 0.5f * cos( time + u + 4.0f )
            };

            framebuffer[y][x] = color;
        }
    }


    static const float TIME_TRANSITION_1 = 6.0f;

    if (time > TIME_TRANSITION_1) {
        draw_credits(time - TIME_TRANSITION_1);
    }

    if (time < 10.0) {
        draw_part1(time);
    }
    
    // Draw FPS in upper-left corner
    float dt = time - time_previous_frame;
    uint fps = 1.0f / dt;
    snprintf( str_fps, 5, "%04d", uint(dt*1000));
    draw_string( str_fps, 4, 0, 0 );
    time_previous_frame = time;

    // draw the time elapsed since the demo began
    snprintf( str_time, 6, "%02d:%02d", uint(time)/60, uint(time)%60 );
    draw_string( str_time, 5, 0, FONT_WIDTH );
}



#endif // #ifndef DRAW_CPP
