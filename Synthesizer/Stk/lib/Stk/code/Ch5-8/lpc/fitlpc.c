/*  fitlpc routine by Perry R. Cook
      Stanford CCRMA 1991.

    Hacked in 1998 and 2000 for CCRMA Summer DSP Workshop
    Hacked in 1999 for new System ID of Shaker Parameters.
                 (NSF CAREER grant, Jan 1999 - Jan 2003)
    Hacked in 2001 for Princeton COS/MUS 325 course
    Hacked in 2002 for AK Peters Book
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#define MAX_BLOCK 4096
#define MAX_ORDER 100

float autocorr(long size,float *data,float *result)
{
    long i,j,k;
    float temp,norm;

    for (i=0;i<size/2;i++)      {
        result[i] = 0.0;
	for (j=0;j<size-i-1;j++)	{
	    result[i] += data[i+j] * data[j];
	}
    }
    temp = result[0];
    j = (long) size*0.02;
    while (result[j]<temp && j < size/2)	{
	temp = result[j];
        j += 1;
    }
    temp = 0.0;
    for (i=j;i<size*0.5;i++) {
        if (result[i]>temp) {
	    j = i;
	    temp = result[i];
	}
    }
    norm = 1.0 / size;
    k = size/2;
    for (i=0;i<size/2;i++)
        result[i] *=  (k - i) * norm;
    if (result[j] == 0) j = 0;
    else if ((result[j] / result[0]) < 0.4) j = 0;
    else if (j > size/4) j = 0;
    return (float) j;
}

long minvert(long size,float mat[][MAX_ORDER])
{
    long item,row,col,rank=0,t2;
    float temp,res[MAX_ORDER][MAX_ORDER];
    long ok,zerorow;
        
    for (row=1;row<=size;row++)     {
        for (col=1;col<=size;col++)	{
            //    printf(stdout," %f ",mat[row][col]);
	    if (row==col) 
	        res[row][col] = 1.0;
	    else
	        res[row][col] = 0.0;
	}
        //    fprintf(stdout,"\n");
    }
    for (item=1;item<=size;item++) {
        if (mat[item][item]==0)		{
            for (row=item;row<=size;row++)   {
    		for (col=1;col<=size;col++)	{
    		    mat[item][col] = mat[item][col] + mat[row][col];
    		    res[item][col] = res[item][col] + res[row][col];
		}
	    }
	} 
	for (row=item;row<=size;row++)  {
	    temp=mat[row][item];
	    if (temp!=0)	{
		for (col=1;col<=size;col++)	{
		    mat[row][col] = mat[row][col] / temp;
		    res[row][col] = res[row][col] / temp;
		}
	    } 
	}
	if (item!=size)	{
	    for (row=item+1;row<=size;row++)	{
		temp=mat[row][item];
		if (temp!=0)	{
		    for (col=1;col<=size;col++)	{
			mat[row][col] = mat[row][col] - mat[item][col];
			res[row][col] = res[row][col] - res[item][col];
		    }
		} 
	    }
	} 
    }
    for (item=2;item<=size;item++)   {
        for (row=1;row<item;row++)	{
    	    temp = mat[row][item];
	    for (col=1;col<=size;col++)	   {
		mat[row][col] = mat[row][col] - temp * mat[item][col];
		res[row][col] = res[row][col] - temp * res[item][col];
	    }
	}
    }
/*    ok = TRUE;
    rank = 0;
    for (row=1;row<=size;row++)	{
	zerorow = TRUE;
	for (col=1;col<=size;col++)	{
	    if (mat[row][col]!=0) zerorow = FALSE;
	    t2 = (mat[row][col] + 0.5);
	    if (row==col&&t2!=1) ok = FALSE;
	    t2 = fabs(mat[row][col]*100.0);
	    if (row!=col&&t2!=0) ok = FALSE;
	}
	if (!zerorow) rank += 1;
    }
    if (!ok)	{
        fprintf(stdout,"Matrix Not Invertible\n");
        fprintf(stdout,"Rank is Only %i of %i\n",rank,size);
    }									*/
    for (row=1;row<=size;row++)	{
        for (col=1;col<=size;col++)	{
	    mat[row][col] = res[row][col];
	}
    }	
    return rank;
}

float lpc_from_data(long order, long size, float *data, float *coeffs)
{
    float r_mat[MAX_ORDER][MAX_ORDER];
    long i,j;
    float pitch;
    float corr[MAX_BLOCK];

    pitch = autocorr(size, data,corr);
    for (i=1;i<order;i++) {
	for (j=1;j<order;j++) r_mat[i][j] = corr[abs(i-j)];
    }
    minvert(order-1,r_mat);
    for (i=0;i<order-1;i++)     {
        coeffs[i] = 0.0;
	for (j=0;j<order-1;j++)	{
	    coeffs[i] += r_mat[i+1][j+1] * corr[1+j];
	}
    }
    return pitch;
}

float predict(long order,long length,float *data,float *coeffs, FILE* resFile)
{
    long i,j;
    float power=0.0,error,tmp;
    static float Zs[MAX_ORDER] = {0.0};
//    short shortError;       //  Use this if want error to be soundfile

    for (i=0;i<length;i++)     {         //  0 to hopsize??????????
        tmp = 0.0;
	for (j=0;j<order;j++)  tmp += Zs[j]*coeffs[j];
	for (j=order-1;j>0;j--) Zs[j] = Zs[j-1];
	Zs[0] = data[i];
        error = data[i] - tmp;
        fwrite(&error,4,1,resFile);
//        shortError = error;      //  Use these if want error to be soundfile
//        fwrite(&shortError,2,1,resFile);
	power += error * error;
    }
    return sqrt(power) / length;  
}

long check_stable(long order,float *coeffs)
{
    long i,j;
    float output,Zs[MAX_ORDER];
    for (i=0;i<order;i++) Zs[i] = 0.0;
    Zs[0] = 1.0;
    for (i=0;i<2*order*order;i++)     {
        output = 0.0;
	for (j=0;j<order;j++)  output += Zs[j]*coeffs[j];
	for (j=order-1;j>0;j--) Zs[j] = Zs[j-1];
	Zs[0] = output;
    }
    j = 1;
    if (output>1.0) j = 0;
    return j; 	
}

#include "../../waveio.h"

void main(int argc,char *argv[])
{
     struct soundhdr hdr;
    char *file_name_in,*file_name_out,residual_file[200];
    float data[MAX_BLOCK],out_data[MAX_ORDER];
    short shortdata[MAX_BLOCK];
    FILE *fd, *fd2, *fdr;
    long i,n_read,time=0,block = 0;
    long block_size,hop_size,order;
    float pitch,power=0,total_power=0,temp;

    if (argc==7)  {
        order = atoi(argv[1]);
        if (order >= MAX_ORDER) {
            order = MAX_ORDER-1;
            printf("setting order to %i\n",order);
        }
        block_size = atoi(argv[2]);
        if (block_size >= MAX_BLOCK) {
            block_size = MAX_BLOCK-1;
            printf("setting block size to %i\n",block_size);
        }
        hop_size = atoi(argv[3]);
	if (hop_size>block_size)	{
	    hop_size = block_size - order;
	    fprintf(stderr,"Setting hop size to block size - order.\n");
	}
        file_name_in = argv[4];
        file_name_out = argv[5];
        fd = opensoundin(file_name_in,&hdr);
        if (fd)     {

           fd2 = fopen(file_name_out,"wb");
           fwrite(&order,1,4,fd2);
           fwrite(&hop_size,1,4,fd2);
           n_read = fread(shortdata,2,block_size,fd);
//         n_read = fread(&shortdata[block_size-hop_size],2,hop_size,fd);

//           strcpy(residual_file,file_name_out);
//           strchr(residual_file,'.')[0] = 0;
//           strcat(residual_file,".res");
           fdr = fopen(argv[6], "wb");
//           fwrite(&hdr,1,44,fdr);
           while (n_read>0)     {
               for (i=0;i<block_size;i++)   {
                   data[i] = shortdata[i];
               }
               pitch = lpc_from_data(order+1,block_size,data,out_data);
	       if (!check_stable(order,out_data))	{
	           fprintf(stderr,"This set is not stable!!\n");
//		   stabilize(order,out_data);
	       }
               power = predict(order,n_read,&data[block_size-n_read],out_data,fdr);
               total_power += power;
               time+= hop_size;
//               printf("%li %f %.3f, coeffs =  ",time,pitch,power);
               printf(".");
               for (i=0;i<order;i++)
//                   printf("%.6f, ",out_data[i]);
//               printf("/n");
               for (i=0;i<block_size-hop_size;i++)
                   shortdata[i] = shortdata[hop_size+i];
               n_read = fread(&shortdata[block_size-hop_size],2,hop_size,fd);
               if (n_read < hop_size)   {
                   for (i=block_size-hop_size+n_read;i<block_size;i++)  {
                       data[i] = 0;
                   }
               }
               fwrite(&pitch,4,1,fd2);
               fwrite(&power,4,1,fd2);
               fwrite(&out_data,4,order,fd2);

               block += 1;
           }
           printf("\n");
           printf("%s error power = %f\n",file_name_in,total_power / block);
           fclose(fd);
           fclose(fd2);
           fclose(fdr);
         }
        else
            printf("I couldn't find your input file!!!  %s\n",file_name_in);
    }
    else   {
        printf("Format is fitlpc order blocksize hopsize infile.wav(snd) outfile.lpc errfile.res\n");
        printf("residual file is raw floats, use float2wv to convert to wave file\n");
//        for (i=0;i<argc;i++)    {
//            printf("%s\n",argv[i]);
//        }
    }
    return;
}

