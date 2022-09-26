/* 
Clarence Jiang, yjia279
THIS  CODE  WAS MY OWN WORK , IT WAS  WRITTEN  WITHOUT  CONSULTING  ANYSOURCES  OUTSIDE  OF  THOSE  APPROVED  BY THE  INSTRUCTOR. 
Clarence Jiang  */

#include "pbm.h"
#include <stdlib.h>

PPMImage * new_ppmimage( unsigned int w, unsigned int h, unsigned int m )
{
    PPMImage *image;
    image = malloc(sizeof(PPMImage));

    //Assign the value of height and width
    image->height = h;
    image->width = w;
    image->max = m;

    //Create RGB 3 arrays, iteratre three times for what we did in the other image initialization
    for (int i = 0; i < 3; i++)
    {
        image->pixmap[i] = malloc(h * sizeof(unsigned int **));
        for (int j = 0; j < image->height; j++){
            image->pixmap[i][j] = malloc(image->width * sizeof(unsigned int *));
        }
    }
    
    return image;
}

PBMImage * new_pbmimage( unsigned int w, unsigned int h )
{
    PBMImage *image;
    image = malloc(sizeof(PBMImage));

    //Assign the value of height and width
    image->height = h;
    image->width = w;

    //Work on the creation of empty 2d array
    image->pixmap = malloc(h * sizeof(unsigned int *));
    for (int i = 0; i < image->height; i++){
        image->pixmap[i] = malloc(image->width * sizeof(unsigned int));
    }

    return image;
}

PGMImage * new_pgmimage( unsigned int w, unsigned int h, unsigned int m )
{
    PGMImage *image;
    image = malloc(sizeof(PGMImage));

    //Assign the value of height and width
    image->height = h;
    image->width = w;
    image->max = m;

    //Work on the creation of empty 2d array
    image->pixmap = malloc(h * sizeof(unsigned int *));
    for (int i = 0; i < image->height; i++){
        image->pixmap[i] = malloc(image->width * sizeof(unsigned int));
    }

    return image;
}


void del_ppmimage( PPMImage * p )
{
    for(int i = 0; i<3; i++){
        for (int j = 0; j<p->height; j++){
            free(p->pixmap[i][j]);
        }
        free(p->pixmap[i]);
    }
    free(p);
}

void del_pgmimage( PGMImage * p )
{
     for(int i = 0; i < p->height; i++){
        free(p->pixmap[i]);
    }
    free(p->pixmap);
    free(p);
}

void del_pbmimage( PBMImage * p )
{
     for(int i = 0; i < p->height; i++){
        free(p->pixmap[i]);
    }
    free(p->pixmap);
    free(p);
}

