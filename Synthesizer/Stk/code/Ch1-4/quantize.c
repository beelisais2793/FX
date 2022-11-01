#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../waveio.h"

/********************************************************/
/**  Quantize a 16 bit wave file to N bits      *********/
/**  Perry R. Cook, 2002                        *********/
/********************************************************/

void err_msg()
{
    fprintf(stderr,"format is: quantize numBits inputFile outputFile\n\n");
}

void main(int argc, char *argv[]) {
    FILE *file_in,*file_out;
    struct soundhdr hdr;
    int num_bits;
    long i;
    short data;
    float temp,temp2;

    if (argc==4)        {
        file_in = opensoundin(argv[2],&hdr);
        file_out = opensoundout(argv[3],&hdr);
	num_bits = atoi(argv[1]);
	num_bits = 16 - num_bits;
	temp = pow(2.0,num_bits);
	if (file_in && file_out) {
            while(fread(&data,2,1,file_in)) {
                temp2 = (float) data / temp;    /*  This does the most honest */
                temp2 += 0.5;                   /*  job of re-quantization */
                data = temp2;                   /*  rounding to the nearest sample */
                temp2 = (float) data * temp;
                data = temp2;
/*  data  = (data >> num_bits) << num_bits;  /*  This truncates, which does worse */
/*  we could do this with an AND bit mask too */
		fwrite(&data,2,1,file_out);
	    }
	}
	else {
	    fprintf(stderr,"File troubles.  Check names, paths, etc.\n");
	}
        closesoundin(file_in);
        closesoundout(file_out,hdr.dlength/2);
    }
    else {
	err_msg();
	exit(0);
    }
    exit(0);
}

