#include <stdlib.h>
#include <stdio.h>

#include "../waveio.h"

void main(int argc, char *argv[])       {
    short *data,*windata;
    struct soundhdr hdr;
    long i,winlength,halflength,num_read,total=0;
    FILE *inFile,*outFile;
    float wintime,env,windelta;

    if (argc==4)        {
        inFile = opensoundin(argv[2],&hdr);
        if (!inFile)    {
            printf("Can't open intput file\n");
            exit(0);
        }
        outFile = opensoundout(argv[3],&hdr);
        if (!outFile)   {
            printf("Can't open output file\n");
            fclose(outFile);
            exit(0);
        }
        wintime = atof(argv[1]);
        if (wintime < 0.005) {
            printf("Your window time is silly, setting to 5ms.\n");
            wintime = 0.005;
        }
        if (wintime > 2.0) {
            printf("Your window time is silly, setting to 1 s.\n");
            wintime = 2.0;
        }
        winlength = (long) ((float) hdr.srate * wintime);
        halflength = winlength / 2;
        windelta = 2.0 / (float) winlength;
        data = (short *) malloc(winlength*2);
        windata = (short *) malloc(winlength*2);
        if (!data || !windata)  {
            printf("Out of memory!\n");
            fclose(inFile);
            fclose(outFile);
            free(data);
            free(windata);
            exit(0);
        }
        num_read = fread(data,2,winlength,inFile);
        while (num_read > 0)  {
            printf("%li  ",num_read);
            env = 0;
            for (i=0;i<halflength;i++) {
                env += windelta;
                windata[i] = env * data[i];
            }
            for (i=halflength;i<winlength;i++) {
                env -= windelta;
                windata[i] = env * data[i];
            }
            total += fwrite(windata,2,winlength,outFile);
            for (i=0;i<winlength;i++) windata[i] = 0;
            total += fwrite(windata,2,winlength,outFile);
            total += fwrite(windata,2,winlength,outFile);
            for (i=0;i<halflength;i++) data[i] = data[i+halflength];
            num_read = fread(&data[halflength],2,halflength,inFile);
            if (num_read < halflength)
                for (i=halflength+num_read-44;i<winlength;i++)
                    data[i] = 0;                 //  Hack.  trust me.
        }
        free(data);
        free(windata);
        closesoundout(outFile,total);
        fclose(inFile);
    }
    else        {
        printf("useage: frames windowtime fileIn.wav fileOut.wav\n");
    }
}
