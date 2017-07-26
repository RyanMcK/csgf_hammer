#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

double pix_size;

unsigned char color(double distance) {
    if (distance < 0.5 * pix_size) {
        return pow(distance / (0.5 * pix_size), 1.0/3.0) * 255;
    } else {
        return 255;
    }
}

unsigned char mandelbrot(double complex c) {
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

    return color(distance);
}

int write_out(int dim_x, int dim_y, unsigned char * pixels) {
    FILE * fp;

    fp = fopen("mandelbrot.pgm", "w");

    fprintf(fp, "P5\n");
    fprintf(fp, "%d %d\n", dim_x, dim_y);
    fprintf(fp, "255\n");

    fwrite(pixels, sizeof(pixels[0]), dim_x * dim_y, fp);

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

    unsigned char * pixels;

    pixels = (unsigned char *) malloc(pix_count_x * pix_count_y * sizeof(pixels[0]));

    for (int ipy = 0; ipy < pix_count_y; ++ipy) {
        printf("ipy = %d of %d\n", ipy, pix_count_y);
        for (int ipx = 0; ipx < pix_count_x; ++ipx) {
            double complex c = min_x + ipx * pix_size + (max_y - ipy * pix_size) * I;

            int i = ipy * pix_count_x + ipx;
            pixels[i] = mandelbrot(c);
        }
    }

    write_out(pix_count_x, pix_count_y, pixels);
    
    return EXIT_SUCCESS;
}
