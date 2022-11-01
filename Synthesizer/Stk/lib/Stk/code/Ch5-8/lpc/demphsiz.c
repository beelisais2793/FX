/*  lpc deemphasis routine by Perry R. Cook
      Princeton, 2001
*/    

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "../../waveio.h"

void main(int argc,char *argv[])
{
     struct soundhdr hdr;
    short data[2];
    FILE *fd, *fd2;
    long i,n_read;
    float coeff = 0.97,temp=0.0;

    if (argc==3 || argc==4)  {
        if (argc==4)   {
            coeff = atof(argv[3]);
            printf("setting coefficient to %f\n",coeff);
        }
        fd = opensoundin(argv[1],&hdr);
        if (fd)     {
            fd2 = opensoundout(argv[2],&hdr);
            if (fd2)     {
                n_read = fread(data,2,1,fd);
            }
            else {
                printf("output file problems\n");
                n_read = 0;
            }
            while (n_read>0)     {
                temp = data[0]*(1.5-coeff) + coeff*data[1];
                data[1] = temp;
                fwrite(&data[1],2,1,fd2);
                n_read = fread(data,2,1,fd);
            }
            fclose(fd2);
            fclose(fd);
        }
        else  {
            printf("input file problems\n");
            exit(0);
        }
    }
    else  {
        printf("useage: demphsiz fileIn.wav fileOut.wav [coeff]\n");
        exit(0);
    }
}
