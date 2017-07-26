#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

double pix_size;

typedef struct hsv {double h; double s; double v;} hsv;
typedef struct rgb {double r; double g; double b;} rgb;

void color(double distance, int iter, int iter_max, hsv * pixel) {
    if (iter >= iter_max) {
        pixel -> h = 0.0;
        pixel -> s = 0.0;
        pixel -> v = 1.0;
    }

    if (distance < 0.5 * pix_size) {
        pixel -> v = pow(distance / (0.5 * pix_size), 1.0/3.0);
    } else {
        pixel -> v = 1.0;
    }

    pixel -> s = 0.7;

    double hue = 10.0 * log(iter) / log(iter_max);
    hue = hue - floor(hue);
    pixel -> h = hue;
}

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
        r = cabs(z);
        iter += 1;
    }

    double distance = 2.0 * log(cabs(z)) * cabs(z) / cabs(dz);

    color(distance, iter, iter_max, pixel);
}

rgb hsv2rgb(hsv hsv_val) {
    double c = hsv.v * hsv.s;
    double h_prime = hsv.h / 60.0;
    double x = c * (1.0 - fabs(fmod(h_prime, 2.0) - 1.0));

    rgb rgb_val;

    assert(h_prime >= 0.0);
    assert(h_prime <= 6.0);

    if (h_prime >= 0.0 && h_prime <= 1.0) {
        rgb_val.r = c;
        rgb_val.g = x;
        rgb_val.b = 0.0;
    } else if (h_prime <= 2.0) {
        rgb_val.r = x;
        rgb_val.g = c;
        rgb_val.b = 0.0;
    } else if (h_prime <= 3.0) {
        rgb_val.r = 0.0;
        rgb_val.g = c;
        rgb_val.b = x;
    } else if (h_prime <= 4.0) {
        rgb_val.r = 0.0;
        rgb_val.g = x;
        rgb_val.b = c;
    } else if (h_prime <= 5.0) {
        rgb_val.r = x;
        rgb_val.g = 0.0;
        rgb_val.b = c;
    } else if (h_prime <= 6.0) {
        rgb_val.r = c;
        rgb_val.g = 0.0;
        rgb_val.b = x;
    }

    double m = hsv.v - c;
    rgb_val.r += m;
    rgb_val.g += m;
    rgb_val.b += m;

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
    double ctr_x = -0.75;
    double ctr_y = 0.00;

    double len_x = 2.75;
    double len_y = 2.0;

    double min_x = ctr_x - len_x/2.0;
    double max_y = ctr_y + len_y/2.0;

    int pix_count_x = 1024;
    pix_size = len_x / pix_count_x;
    int pix_count_y = len_y / pix_size;

    hsv * pixels;

    pixels = (hsv *) malloc(pix_count_x * pix_count_y * sizeof(pixels[0]));

    for (int ipy = 0; ipy < pix_count_y; ++ipy) {
        printf("ipy = %d of %d\n", ipy, pix_count_y);
        for (int ipx = 0; ipx < pix_count_x; ++ipx) {
            double complex c = min_x + ipx * pix_size + (max_y - ipy * pix_size) * I;

            int i = ipy * pix_count_x + ipx;
            mandelbrot(c, &pixels[i]);
        }
    }

    write_out(pix_count_x, pix_count_y, pixels);
    
    return EXIT_SUCCESS;
}
