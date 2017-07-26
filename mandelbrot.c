#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <assert.h>

double pix_size;
#pragma acc declare copyin (pix_size)

typedef struct hsv {double h; double s; double v;} hsv;
typedef struct rgb {unsigned char r; unsigned char g; unsigned char b;} rgb;

#pragma acc routine (my_cabs) seq
double my_cabs(double complex c) {
    return sqrt(creal(c) * creal(c) + cimag(c) * cimag(c));
}

#pragma acc routine (color) seq
void color(double distance, int iter, int iter_max, double r, double r_max, hsv * pixel) {
    if (iter >= iter_max) {
        pixel -> h = 0.0;
        pixel -> s = 0.0;
        pixel -> v = 1.0;
        return;
    }

    if (distance < 0.5 * pix_size) {
        pixel -> v = pow(distance / (0.5 * pix_size), 1.0/3.0);
    } else {
        pixel -> v = 1.0;
    }

    pixel -> s = 0.7;

    double iter_cont = iter - log2(log(r) / log(r_max));
    double hue = 10.0 * log(iter_cont) / log(iter_max);
    hue = hue - floor(hue);
    pixel -> h = hue * 360.0;
}

#pragma acc routine (mandelbrot) seq
void mandelbrot(double complex c, hsv * pixel) {
    int iter = 0;
    int iter_max = 10000;
    double r = 0.0;
    double r_max = 1 << 18;
    double complex z = 0.0 + 0.0 * I;
    double complex dz = 0.0 + 0.0 * I;

    while (r < r_max && iter < iter_max) {
        dz = 2.0 * z * dz + 1.0;
        z = z * z + c;
        r = my_cabs(z);
        iter += 1;
    }

    double distance = 2.0 * log(my_cabs(z)) * my_cabs(z) / my_cabs(dz);

    color(distance, iter, iter_max, r, r_max, pixel);
}

rgb hsv2rgb(hsv hsv_val) {
    double c = hsv_val.v * hsv_val.s;
    double h_prime = hsv_val.h / 60.0;
    double x = c * (1.0 - fabs(fmod(h_prime, 2.0) - 1.0));

    rgb rgb_val;
    double r, g, b;

    assert(h_prime >= 0.0);
    assert(h_prime <= 6.0);

    if (h_prime >= 0.0 && h_prime <= 1.0) {
        r = c;
        g = x;
        b = 0.0;
    } else if (h_prime <= 2.0) {
        r = x;
        g = c;
        b = 0.0;
    } else if (h_prime <= 3.0) {
        r = 0.0;
        g = c;
        b = x;
    } else if (h_prime <= 4.0) {
        r = 0.0;
        g = x;
        b = c;
    } else if (h_prime <= 5.0) {
        r = x;
        g = 0.0;
        b = c;
    } else if (h_prime <= 6.0) {
        r = c;
        g = 0.0;
        b = x;
    }

    double m = hsv_val.v - c;
    r += m;
    g += m;
    b += m;

    rgb_val.r = r * 255;
    rgb_val.g = g * 255;
    rgb_val.b = b * 255;
    
    return rgb_val;
}

int write_out(int dim_x, int dim_y, hsv * pixels) {
    FILE * fp;

    fp = fopen("image.pgm", "w");

    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", dim_x, dim_y);
    fprintf(fp, "255\n");

    for (int i = 0; i < dim_x * dim_y; ++i) {
        rgb rgb_tmp = hsv2rgb(pixels[i]);
        fwrite(&rgb_tmp, sizeof(rgb), 1, fp);
    }

    fclose(fp);

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    double ctr_x = atof(argv[1]);
    double ctr_y = atof(argv[2]);

    double len_x = atof(argv[3]);
    double len_y = atof(argv[3]);

    double min_x = ctr_x - len_x/2.0;
    double max_y = ctr_y + len_y/2.0;

    int pix_count_x = atoi(argv[4]);
    pix_size = len_x / pix_count_x;
    int pix_count_y = len_y / pix_size;

    hsv * restrict pixels = (hsv *) malloc(pix_count_x * pix_count_y * sizeof(pixels[0]));
    #pragma acc enter data create(pixels[0:pix_count_x * pix_count_y])

    #pragma acc parallel loop collapse(2) present(pixels[0:pix_count_x * pix_count_y])
    for (int ipy = 0; ipy < pix_count_y; ++ipy) {
        // printf("ipy = %d of %d\n", ipy, pix_count_y);
        for (int ipx = 0; ipx < pix_count_x; ++ipx) {
            double complex c = min_x + ipx * pix_size + (max_y - ipy * pix_size) * I;

            int i = ipy * pix_count_x + ipx;
            mandelbrot(c, &pixels[i]);
        }
    }

    #pragma acc exit data copyout(pixels[0:pix_count_x * pix_count_y])

    write_out(pix_count_x, pix_count_y, pixels);
    
    return EXIT_SUCCESS;
}
