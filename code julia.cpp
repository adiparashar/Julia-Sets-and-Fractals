#include <stdio.h>
#include <stdlib.h>
#include <complex>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <time.h>
#include <vector>

using namespace cv;
using namespace std;

#define N 1024
#define SQRT_2 1.4142
#define MAX_ITER 120

void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );
void saveImage(int width, int height, unsigned char * bitmap, complex<float> seed);
void compute_julia(complex<float> c, unsigned char * image);


int main(int argc, char **argv)
{
    complex<float> c(-1.00f, 0.000f);
    if(argc > 2)
    {
        c.real(atof(argv[1]));
        c.imag(atof(argv[2]));
    } else
        fprintf(stderr, "Usage: %s <real> <imag>\nWhere <real> and <imag> form the complex seed for the Julia set.\n", argv[0]);

    unsigned char *image = new unsigned char[N*N*3]; //RGB image
    compute_julia(c, image);
    saveImage(N, N, image, c);
    delete[] image;
}

void compute_julia(complex<float> c, unsigned char * image)
{
    complex<float> z_old(0.0f, 0.0f);
    complex<float> z_new(0.0f, 0.0f);
    complex<float> c1(0.667f,0.0f);
    complex<float> c2(0.333f,0.0f);
    for(int y=0; y<N; y++)
        for(int x=0; x<N; x++)
        {
            z_new.real(4.0f * x / (N) - 2.0f);
            z_new.imag(4.0f * y / (N) - 2.0f);
            int i;
            for(i=0; i<MAX_ITER; i++)
            {
                z_old.real(z_new.real());
                z_old.imag(z_new.imag());
               // z_new = z_new*c1 -c/z_new;
                z_new = c1*z_old+c2*pow(z_old,-2);
                if(norm(z_new-z_old) < 10e-6f) break;
            }
            float brightness = (i<MAX_ITER) ? 1.0f : 0.0f;
            float hue = (i % MAX_ITER)/float(MAX_ITER - 1);
            hue = ((120)*sqrtf(hue)+240);//color control parameters
            float r, g, b;
            HSVtoRGB(&r, &g, &b, hue, 1.0f, brightness);
            image[(x + y*N)*3 + 0] = (unsigned char)(b*255);
            image[(x + y*N)*3 + 1] = (unsigned char)(g*255);
            image[(x + y*N)*3 + 2] = (unsigned char)(r*255);
        }
}

void saveImage(int width, int height, unsigned char * bitmap, complex<float> seed)
{  
   //namedWindow("winname",WINDOW_AUTOSIZE );
       Mat rawData  =  Mat( 1024,1024,CV_8UC3,bitmap);   
   // imshow("winname", rawData);
   // waitKey(0);
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    try {
        imwrite("alpha.png", rawData, compression_params);
    }
    catch (runtime_error& ex) {
        fprintf(stderr, "Exception converting image to PNG format: %s\n", ex.what());
    }

    fprintf(stdout, "Saved PNG file with alpha data.\n");


  
}

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//      if s == 0, then h = -1 (undefined)
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v )
{
    int i;
    float f, p, q, t;
    if( s == 0 ) {
        // achromatic (grey)
        *r = *g = *b = v;
        return;
    }
    h /= 60;            // sector 0 to 5
    i = floor( h );
    f = h - i;          // factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );
    switch( i ) {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        default:        // case 5:
            *r = v;
            *g = p;
            *b = q;
            break;
    }
}