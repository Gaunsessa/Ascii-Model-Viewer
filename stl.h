#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct Tri {
    float normal[3];
    float vert1[3];
    float vert2[3];
    float vert3[3];
    unsigned short bytec;
} Tri;

typedef struct STL {
    unsigned int tri_count;
    Tri *tris;
} STL;

Tri read_tri(FILE *file)
{
    Tri res;

    fread(&res, sizeof(float), 12, file);
    fread(&res + sizeof(float) * 12, sizeof(unsigned short), 1, file);

    return res;
}

STL* read_stl(FILE *file)
{
    STL *res = malloc(sizeof(STL));

    fseek(file, 80, 1);

    fread(res, sizeof(unsigned int), 1, file);

    Tri *tris = malloc(sizeof(Tri) * res->tri_count);

    for (unsigned int i = 0; i < res->tri_count; i++)
        tris[i] = read_tri(file);

    res->tris = tris;

    return res;
}

void normalize_point(float point[3], float min, float max)
{
    for (int i = 0; i < 3; i++)
        point[i] = (point[i] - min) / (max - min);
}

void normalize_stl(STL *stl)
{
    float max = 1;
    float min = 1;

    for (unsigned int i = 0; i < stl->tri_count; i++)
        for (int l = 0; l < 3; l++) {
            if (fabs(stl->tris[i].vert1[l]) > max)
                max = fabs(stl->tris[i].vert1[l]); 
            if (fabs(stl->tris[i].vert2[l]) > max)
                max = fabs(stl->tris[i].vert2[l]);
            if (fabs(stl->tris[i].vert3[l]) > max)
                max = fabs(stl->tris[i].vert3[l]);

            if (fabs(stl->tris[i].vert1[l]) < min)
                min = fabs(stl->tris[i].vert1[l]); 
            if (fabs(stl->tris[i].vert2[l]) < min)
                min = fabs(stl->tris[i].vert2[l]);
            if (fabs(stl->tris[i].vert3[l]) < min) 
                min = fabs(stl->tris[i].vert3[l]);
        }

    for (unsigned int i = 0; i < stl->tri_count; i++) {
        normalize_point(stl->tris[i].vert1, min, max);
        normalize_point(stl->tris[i].vert2, min, max);
        normalize_point(stl->tris[i].vert3, min, max);
    }
}

void free_stl(STL *stl)
{
    free(stl->tris);
    free(stl);
}

