#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

#include "stl.h"

#define WIDTH 100
#define HEIGHT 50

#define SHADE " .:+*"
#define STEPSIZE 30

float HWIDTH  = WIDTH / 2;
float HHEIGHT = HEIGHT / 2;
float HSHADE  = strlen(SHADE) / 2;

char DISPLAY[HEIGHT][WIDTH] = { { 0 } };

void print_display()
{
    printf("\033[H\033[J");

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++)
            printf("\033[1;32m%c", SHADE[(int)DISPLAY[y][x]]);

        printf("\n");
    }
}

void set_point(float point[3])
{
    int ny = (int)(HHEIGHT * (-point[1] + 1));
    int nx = (int)(HWIDTH * (point[0] + 1));
    int nz = (int)(HSHADE * (point[2] + 1));
    nz = (nz < 1) ? 1 : nz;

    if ((nz > DISPLAY[ny][nx]) && (ny < HEIGHT) && (nx < WIDTH))
        DISPLAY[ny][nx] = nz;
}

void point_mul(float point[3], float mat[3][3])
{
    float p1 = point[0] * mat[0][0] + point[1] * mat[0][1] + point[2] * mat[0][2];
    float p2 = point[0] * mat[1][0] + point[1] * mat[1][1] + point[2] * mat[1][2];
    float p3 = point[0] * mat[2][0] + point[1] * mat[2][1] + point[2] * mat[2][2];

    point[0] = p1;
    point[1] = p2;
    point[2] = p3;
}

void draw_line(float point1[3], float point2[3])
{
    float spoint[3] = { point1[0], point1[1], point1[2] };
    float dpoint[3] = { 
        (point2[0] - point1[0]) / STEPSIZE, 
        (point2[1] - point1[1]) / STEPSIZE, 
        (point2[2] - point1[2]) / STEPSIZE 
    };

    for (int i = 0; i < STEPSIZE; i++) {
        spoint[0] += dpoint[0];
        spoint[1] += dpoint[1];
        spoint[2] += dpoint[2];

        set_point(spoint);
    }
}

void draw_tri(Tri *tri)
{
    float s1[3] = { tri->vert1[0], tri->vert1[1], tri->vert1[2] };
    float s2[3] = { tri->vert1[0], tri->vert1[1], tri->vert1[2] };
    
    float v1[3] = { 
        (tri->vert2[0] - tri->vert1[0]) / STEPSIZE,
        (tri->vert2[1] - tri->vert1[1]) / STEPSIZE,
        (tri->vert2[2] - tri->vert1[2]) / STEPSIZE
    };

    float v2[3] = { 
        (tri->vert3[0] - tri->vert1[0]) / STEPSIZE,
        (tri->vert3[1] - tri->vert1[1]) / STEPSIZE,
        (tri->vert3[2] - tri->vert1[2]) / STEPSIZE
    };
   
    draw_line(s1, s2);

    for (int i = 0; i < STEPSIZE; i++) {
        s1[0] += v1[0];
        s1[1] += v1[1];
        s1[2] += v1[2];

        s2[0] += v2[0];
        s2[1] += v2[1];
        s2[2] += v2[2];

        draw_line(s1, s2);
    }
}

int main()
{
    FILE *file = fopen("./stls/bigyosh.stl", "rb");

    if (file == NULL) exit(1);

    STL *obj = read_stl(file);

    normalize_stl(obj);

    double angle = 0;

    while (1) {
        memset(&DISPLAY, 0, HEIGHT * WIDTH);
        
        float rotx[3][3] = {
            { 1, 0, 0 },
            { 0, cos(angle), -sin(angle) },
            { 0, sin(angle), cos(angle) },
        };

        float roty[3][3] = {
            { cos(angle), 0, sin(angle) },
            { 0, 1, 0 },
            { -sin(angle), 0, cos(angle) },
        };

        float rotz[3][3] = {
            { cos(angle), -sin(angle), 0 },
            { sin(angle), cos(angle), 0 },
            { 0, 0, 1 },
        };

        for (unsigned int i = 0; i < obj->tri_count; i++) {
            Tri tri = obj->tris[i];

            point_mul(tri.vert1, roty);
            point_mul(tri.vert2, roty);
            point_mul(tri.vert3, roty);

            point_mul(tri.vert1, rotx);
            point_mul(tri.vert2, rotx);
            point_mul(tri.vert3, rotx);

            point_mul(tri.vert1, rotz);
            point_mul(tri.vert2, rotz);
            point_mul(tri.vert3, rotz);

            set_point(tri.vert1);
            set_point(tri.vert2);
            set_point(tri.vert3);

            // draw_tri(&tri);

            // draw_line(tri.vert1, tri.vert2);
            // draw_line(tri.vert2, tri.vert3);
            // draw_line(tri.vert3, tri.vert1);

            // draw_plane(v6, v5, v4);
        }

        print_display();

        angle += 0.1;

        usleep(10000);
    }

    free_stl(obj);
    fclose(file);

    return 0;
}

