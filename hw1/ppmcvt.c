/* 
Clarence Jiang, yjia279
THIS  CODE  WAS MY OWN WORK , IT WAS  WRITTEN  WITHOUT  CONSULTING  ANYSOURCES  OUTSIDE  OF  THOSE  APPROVED  BY THE  INSTRUCTOR. 
Clarence Jiang  */


#include "pbm.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Define the different types of errors at the beginning, since this is something not going to change
*/
char * malform = "Useage:  ppmcvt [-bgirsmtno] [FILE]\n";
char * invalid_color = "Error:  Invalid channel specification:  (%s); should be ‘red’, ‘green’, or ‘blue’\n";
char * invalid_gray = "Error:  Invalid max grayscale pixel value:  %s; must be less than 65,536\n";
char * invalid_scale = "Error:  Invalid scale factor:  %d; must be 1-8\n";
char * input_err = "Error:  No input file specified\n";
char * output_err = "Error:  No output file specified\n";
char * multiple_err = "Error:  Multiple transformations specified\n";

PBMImage * convert_pbm(PPMImage * ppm){
    PBMImage * image = new_pbmimage(ppm->width, ppm->height);
    int rgb = 0;

    /*
    Here I initially want to use triple for-loop, but then realize the strucutre is in a way that
    we should add the first [0][0] position for the three RGB 2d arrays. It is not accumulating but 
    adding three different values while tracing 2d arrays, so the logic of using triple for-loop is 
    incorrect.  
    */

    for(int j = 0; j<ppm->height; j++){
        for (int z = 0; z<ppm->width; z++){
            rgb = ppm->pixmap[0][j][z] + ppm->pixmap[1][j][z] + ppm->pixmap[2][j][z];
            rgb /= 3; 
            if (rgb<(ppm->max/2)){
                image->pixmap[j][z] = 1;
            }
            else{
                image->pixmap[j][z] = 0;
            }
        }
    }
    return image;
}

PGMImage * convert_pgm(PPMImage * ppm, unsigned int grayscale){
    PGMImage * image = new_pgmimage(ppm->width, ppm->height, grayscale);
    double rgb = 0; 
    for(int j = 0; j<ppm->height; j++){
        for (int z = 0; z<ppm->width; z++){
            rgb = ppm->pixmap[0][j][z] + ppm->pixmap[1][j][z] + ppm->pixmap[2][j][z];
            // printf("Number %f", rgb);
            rgb /= 3; 
            // printf("Number %f", rgb);
            rgb /= ppm->max;
            // printf("Number %f max", ppm->max);
            // printf("Number %f", rgb);
            rgb *= grayscale;
            // printf("Number %f", rgb);
            image->pixmap[j][z] = rgb;
        }
    }
    return image;
}

void isolate_rgb(PPMImage * ppm, char * color){
    if (strcmp(color, "red") == 0){
        for (int i = 0; i< ppm->height; i++){
            for (int j = 0; j< ppm->width; j++){
                ppm->pixmap[1][i][j] = 0;
                ppm->pixmap[2][i][j] = 0;
            }
        }
    }
    else if (strcmp(color, "green")==0){
         for (int i = 0; i< ppm->height; i++){
            for (int j = 0; j< ppm->width; j++){
                ppm->pixmap[0][i][j] = 0;
                ppm->pixmap[2][i][j] = 0;

            }
        }
    }
    else if (strcmp(color, "blue") == 0){
         for (int i = 0; i< ppm->height; i++){
            for (int j = 0; j< ppm->width; j++){
                ppm->pixmap[1][i][j] = 0;
                ppm->pixmap[0][i][j] = 0;
            }
        }
    }
    // else{
    //     fputs(invalid_color, stderr);
    //     exit(1);
    // }
}

void remove_rgb(PPMImage * ppm, char * color){
    int color_index;
    if (strcmp(color, "red") == 0){
        color_index = 0;
    }
    else if (strcmp(color, "green")==0){
        color_index = 1;
    }
    else if (strcmp(color, "blue") == 0){
        color_index = 2;
    }
    // else{
    //     fputs(invalid_color, stderr);
    //     exit(1);
    // }
    for (int i = 0; i< ppm->height; i++){
        for (int j = 0; j< ppm->width; j++){
            ppm->pixmap[color_index][i][j] = 0;
        }
    }
}

void sepia_transformation(PPMImage *ppm){
    for(int i = 0; i<ppm->height; i++){
        for(int j = 0; j<ppm->width; j++){            
            int old_red = ppm->pixmap[0][i][j];
            int old_green = ppm->pixmap[1][i][j];
            int old_blue = ppm->pixmap[2][i][j];
            /*
            Follow inclase discussion of considering the side case if the average exceeds the max value
            */
            if (0.393 * old_red +0.769* old_green+0.189*old_blue > ppm->max){
                ppm->pixmap[0][i][j] = ppm->max;
            }
            else{
                ppm->pixmap[0][i][j] = 0.393 * old_red +0.769* old_green+0.189*old_blue;
            }
            if (0.349 * old_red +0.686* old_green+0.168*old_blue > ppm->max){
                ppm->pixmap[1][i][j] = ppm->max;
            }
            else{
                ppm->pixmap[1][i][j] = 0.349 * old_red +0.686* old_green+0.168*old_blue;
            }
            if (0.272 * old_red +0.534* old_green+0.131*old_blue > ppm->max){
                ppm->pixmap[2][i][j] = ppm->max;
            }
            else{
                ppm->pixmap[2][i][j] = 0.272 * old_red +0.534* old_green+0.131*old_blue;
            }
        }
    }
}

void mirror(PPMImage * ppm){
    if (ppm->width % 2 == 0){
        for (int i =0; i< ppm->height ; i++){
            for (int j = ppm->width - 1; j>=ppm->width/2; j--){
                ppm->pixmap[0][i][j] = ppm->pixmap[0][i][ppm->width-1-j];
                ppm->pixmap[1][i][j] = ppm->pixmap[1][i][ppm->width-1-j];
                ppm->pixmap[2][i][j] = ppm->pixmap[2][i][ppm->width-1-j];
            }
        }
        
    }
    else{
        for (int i =0; i< ppm->height ; i++){
            for (int j = ppm->width - 1; j>ppm->width/2; j--){
                ppm->pixmap[0][i][j] = ppm->pixmap[0][i][ppm->width-1-j];
                ppm->pixmap[1][i][j] = ppm->pixmap[1][i][ppm->width-1-j];
                ppm->pixmap[2][i][j] = ppm->pixmap[2][i][ppm->width-1-j];
            }
        }
    }
}

PPMImage* thumbnail(PPMImage * ppm, unsigned int scale){
    int new_width = (ppm->width - 1)/scale + 1;
    int new_height = (ppm->height - 1)/scale + 1; 
    PPMImage * new_image = new_ppmimage(new_width, new_height, ppm->max);
    for (int i=0; i< new_image->height; i++){
        for (int j = 0; j< new_image->width; j++){
            new_image->pixmap[0][i][j] = ppm->pixmap[0][i*scale][j*scale];
            new_image->pixmap[1][i][j] = ppm->pixmap[1][i*scale][j*scale];
            new_image->pixmap[2][i][j] = ppm->pixmap[2][i*scale][j*scale];
        }
    }
    return new_image;
}

void tile(PPMImage * ppm, unsigned int scale){
    PPMImage* unit_image = thumbnail(ppm, scale);
    for(int i = 0; i<ppm->height; i++){
        for (int j = 0; j<ppm->width; j++){
            if (i < unit_image->height && j<unit_image->width){
                ppm->pixmap[0][i][j] = unit_image->pixmap[0][i][j];
                ppm->pixmap[1][i][j] = unit_image->pixmap[1][i][j];
                ppm->pixmap[2][i][j] = unit_image->pixmap[2][i][j];
            }
            else{
                ppm->pixmap[0][i][j] = unit_image->pixmap[0][i%unit_image->height][j%unit_image->width];
                ppm->pixmap[1][i][j] = unit_image->pixmap[1][i%unit_image->height][j%unit_image->width];
                ppm->pixmap[2][i][j] = unit_image->pixmap[2][i%unit_image->height][j%unit_image->width];
            }
        }
    }
    del_ppmimage(unit_image);
}

int main( int argc, char *argv[] )
{
    /*
    Create useful local variables in the beginning, since creating it within the branch of while loop
    seems not working. 
    */
    int o; //keep track of option
    char option = 'b'; //keep track of the option that we enter in.  
    // char input_parameter; 
    char * output_name; //keep track of the output name associated with -o
    PBMImage * pbmimage; //keep track of the pbm_image associated with -b
    PGMImage * pgmimage; //keep track of the pgm_image associated with -g
    PPMImage * thumbnail_ppm; //keep track of the scaled-down thumbnail ppm
    // PPMImage * tile_image; //keep track of the tile ppm photo
    int gray_scale; //keep track of the gray_scale associated with -g
    char * isolate_color; //keep track of the color input, it will be used into two options
    int scale; //keep track of the scale in range of 1-8 for changing size
    int bool_output = 0;
    int opt_count = 0;
    char * gray_scale_string;


    //Apply similar code structure in "print-opts.c" in demo. 

    //The first while loop is created to extract different parts ot the input command 
    while ((o = getopt(argc, argv, "bg:i:r:smt:n:o:")) != -1) {
        switch (o) {
            case 'b':
                option = 'b';
                opt_count++;
                break;
            case 'g':
                option = 'g';
                opt_count++;
                gray_scale_string = optarg;
                gray_scale = atoi(gray_scale_string);
                // printf("%d", gray_scale);
                if (gray_scale<1 || gray_scale>65535){
                    // fputs(invalid_gray, stderr);
                    fprintf(stderr, invalid_gray, gray_scale_string);
                    exit(1);
                }
                break;
            case 'i':
                option = 'i';
                opt_count++;
                isolate_color = optarg;
                if (strcmp(isolate_color, "red") && strcmp(isolate_color, "green") && strcmp(isolate_color, "blue")){
                    // fputs(invalid_color, stderr);
                    fprintf(stderr, invalid_color, isolate_color);
                    exit(1);
                }
                // printf("The color is %s", isolate_color);
                break;
            case 'r':
                option = 'r';
                opt_count++;
                isolate_color = optarg;
                if (strcmp(isolate_color, "red") && strcmp(isolate_color, "green") && strcmp(isolate_color, "blue")){
                    // fputs(invalid_color, stderr);
                    fprintf(stderr, invalid_color, isolate_color);
                    exit(1);
                }
                break;
            case 's':
                option = 's';
                opt_count++;
                break;
            case 'm':
                option = 'm';
                opt_count++;
                break;
            case 't':
                option = 't';
                opt_count++;
                scale = atoi(optarg);
                if (scale < 1 || scale>8){
                    // fputs(invalid_scale, stderr);
                    fprintf(stderr, invalid_scale, scale);
                    exit(1);
                }
                break;
            case 'n':
                option = 'n';
                opt_count++;
                scale = atoi(optarg);
                if (scale < 1 || scale>8){
                    // fputs(invalid_scale, stderr);
                    fprintf(stderr, invalid_scale, scale);
                    exit(1);
                }
                break;
            case 'o':
                output_name = optarg; 
                bool_output = 1; 
                break;
            default: /* '?' */
                fputs(malform, stderr);
                exit(1);
                break;
        }
    }

    //Here is a really really subtle point, we should test output error before input error
    //cuz the output parameter is supposed to come before the input parameter, so this process is in a reasonable order
    //Similar logic applied to the multi-transformation error 

    if (opt_count>1){
        fputs(multiple_err, stderr);
        exit(1);
    }

    if (!bool_output){
        fputs(output_err, stderr);
        exit(1);
    }

    char * input_name = argv[optind];
    if (input_name == NULL){
        fputs(input_err, stderr);
        exit(1);
    }
    PPMImage * ppm = read_ppmfile(input_name);

    switch (option) {
        case 'b':
            // PBMImage * pbm =convert_pbm(ppm);
            pbmimage =convert_pbm(ppm);
            write_pbmfile(pbmimage, output_name);
            del_pbmimage(pbmimage);
            break;
        case 'g':
            // printf("Option 'b' present. Arg = %s\n", optarg);
            pgmimage =convert_pgm(ppm, gray_scale);
            write_pgmfile(pgmimage, output_name);
            del_pgmimage(pgmimage);
            break;
        case 'i':
            isolate_rgb(ppm, isolate_color);
            write_ppmfile(ppm, output_name);
            // del_ppmimage(ppm);
            break;
        case 'r':
            remove_rgb(ppm, isolate_color);
            write_ppmfile(ppm, output_name);
            break;
        case 's':
            sepia_transformation(ppm);
            write_ppmfile(ppm, output_name);
            break;
        case 'm':
            mirror(ppm);
            write_ppmfile(ppm, output_name);
            break;
        case 't':
            thumbnail_ppm = thumbnail(ppm, scale);
            write_ppmfile(thumbnail_ppm, output_name);
            del_ppmimage(thumbnail_ppm);
            break;
        case 'n':
            tile(ppm, scale);
            write_ppmfile(ppm, output_name);
            // del_ppmimage(tile_image);
            break;
        case 'o':
            break;
        default: /* '?' */
            printf("Unexpected option\n");
            break;
    }
    del_ppmimage(ppm);
    return 0;
}
