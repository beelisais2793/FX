#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*************************************************/
/*   Decimate a mono wave file by integer factor */
/*      Write out new file a new sample rate     */
/*      (so it still sounds the same pitch)      */
/*   Note that this operation is a bad idea!!!   */
/*                                               */
/*      2002, Perry R. Cook                      */
/*************************************************/


#include "../waveio.h"

void err_msg()
{
    fprintf(stderr,"format is: decimate factor inputFile outputFile\n\n");
}

void main(int argc, char *argv[]) {
    FILE *file_in,*file_out;
    struct soundhdr hdr;
    long factor,num_samps,i,j;
    short data;

    if (argc==4)        {
        file_in = opensoundin(argv[2],&hdr);
	factor = atoi(argv[1]);
        hdr.srate /= factor;
        hdr.dlength /= factor;
#ifdef LITTLENDIAN
        hdr.bytes_per_sec = hdr.srate * 2;
        hdr.flength = ((hdr.flength-44)/factor) + 44;
#endif
        file_out = opensoundout(argv[3],&hdr);
        if (file_in && file_out){
	    i = 0;
            j = 0;
            while(fread(&data,2,1,file_in)) {
		i++;
                if (i % factor==0) {
                    fwrite(&data,2,1,file_out);
                    j += 1;
                }
	    }
	}
	else {
	    fprintf(stderr,"File troubles.  Check names, paths, etc.\n");
	}
	fclose(file_in);
        closesoundout(file_out,j);
    }
    else {
	err_msg();
	exit(0);
    }
    exit(0);
}

