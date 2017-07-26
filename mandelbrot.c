#include <stdio.h>

int main(int argc, char *argv[])
{
    double ctr_x = -0.75;
    double ctr_y = 0.00;

    double len_x = 2.75;
    double len_y = 2.0;

    double min_x = ctr_x - len_x/2.0;
    double max_y = ctr_y + len_y/2.0;

    int pix_count_x = 8192;
    double pix_size = len_x / pix_count_x;
    int pix_count_y = len_y / pix_size;

    

    return 0;
}
