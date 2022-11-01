/*  lpcresynth routine by Perry R. Cook
                        Stanford CCRMA 1991.
    Hacks 1999-2002, Princeton U.

        This resynthesizes a sound from lpc
        data with optional modifications.
        See lpcInstructions for details.
        Also see fitlpc for how the data was made.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#define MAX_BLOCK 4096
#define MAX_ORDER 100
#define ONE_OVER_RANDLIMIT (1.0 / 16384.0)

float myfrand() {
    return ((float) rand() - 16384) * ONE_OVER_RANDLIMIT;
}

int main(int argc,char *argv[])
{
    char *file_name_in,*source_file_in,*file_name_out;
    float coeffs[MAX_ORDER],Zs[MAX_ORDER],output,input;
    FILE *fileIn,*fileResIn=0,*fileOut;
    long n_read,n_read2,n_write;
    long i,j,k=0,hop_size,order,ticker,total;
    float pitch=100,last_pitch,pitchfactor=1.0,timemult=1.0;
    int sourceIn = 0;
    
    if (argc>=3)  {
         file_name_in = argv[1];
         file_name_out = argv[2];
         i = 3;
         while (i < argc)       {
            if (!strcmp(argv[i],"-t")) {
                timemult = atof(argv[i+1]);
            }
            if (!strcmp(argv[i],"-p")) {
                pitchfactor = atof(argv[i+1]);
            }
            if (!strcmp(argv[i],"-s")) {
                sourceIn = 1;
                source_file_in = argv[i+1];
            }
            i += 1;
        }
        fileIn = fopen(file_name_in,"rb");
        fseek(fileIn,0,SEEK_SET); /*  THIS AVOIDS A BUG in REDHAT 6.0 */      
        if (sourceIn) {
            fileResIn = fopen(source_file_in,"rb");
            if (!fileResIn)     {
                printf("Bogus Input Excitation File\n");
                fclose(fileIn);
                exit(0);
            }
            fseek(fileResIn,0,SEEK_SET); /*  THIS AVOIDS A BUG in REDHAT 6.0 */      
        }
        fileOut = fopen(file_name_out,"wb");
        fseek(fileOut,0,SEEK_SET); /*  THIS AVOIDS A BUG in REDHAT 6.0 */      
        if (fileIn && fileOut)  {
           n_read = fread(&order,4,1,fileIn);
           for (i=0;i<order;i++) Zs[i] = 0.0;
           n_read = fread(&hop_size,4,1,fileIn);
           last_pitch = pitch;
           n_read = fread(&pitch,4,1,fileIn);
           n_read = fread(&input,4,1,fileIn);
           if (pitchfactor>0) 
                ticker = pitch / pitchfactor;
           else
                ticker = 100.0/ fabs(pitchfactor);
           hop_size *= timemult;
           n_read = fread(coeffs,4,order,fileIn);
           while (n_read>0)     {
               for (i=0;i<hop_size;i++) {
                    output = 0.0;
                    if (!sourceIn) {
                        if ((pitch==0 && pitchfactor>0) || pitchfactor==0)
                            output = input  * 20.0 * myfrand();
                        else {
                             ticker -= 1;
                            if (ticker <= 0) {
                                output = input * pitch * 3;
                                if (pitchfactor>0) 
                                    ticker = pitch / pitchfactor;
                                else            {
                                    output = input * last_pitch * 3;
                                    ticker = last_pitch / fabs(pitchfactor);
                                }
                            }
                        }
                    }
                    else        {
                        n_read2 = fread(&output,4,1,fileResIn);
                        if (!n_read2) {
                            fprintf(stderr,"I'm out of input samples!!!\n");
                            sourceIn = 0;
                        }
                    }
                    for (j=0;j<order;j++)  output += Zs[j]*coeffs[j];
                    for (j=order-1;j>0;j--) Zs[j] = Zs[j-1];
                    Zs[0] = output;
                    n_write = fwrite(&output,4,1,fileOut);
               }
               k += hop_size;
               printf (".",k);
               if (pitch>0) last_pitch = pitch;
               n_read = fread(&pitch,4,1,fileIn);
               n_read = fread(&input,4,1,fileIn);
               n_read = fread(coeffs,4,order,fileIn);
           }
           printf("\nDone!!\n");
           fclose(fileIn);
           fclose(fileOut);
           if (fileResIn) fclose(fileResIn);
         }
        else
            printf("I couldn't find your input file!!!\n");
    }
    else
        printf("Format is lpcresynth infile.lpc outfile.float [-t timemult] [-p pitchmult] [-s soundfile] \n");
    return 1;
}

