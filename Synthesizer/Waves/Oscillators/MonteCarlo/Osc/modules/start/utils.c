
/*******************************************************************************
*
* File utils.c
* 
* Collection of basic utility programs
*
* The externally accessible functions are
*
*   void error(int test,int no,char *name,char *text)
*     Checks whether "test"=0 and if not aborts the program gracefully
*     with error number "no" after printing the program "name" and the
*     error message "text" to stdout
*
*   void *amalloc(size_t size,int p)
*     Allocates an aligned memory area of "size" bytes, with starting
*     address (the return value) that is an integer multiple of 2^p
*
*   void afree(void *addr)
*     Frees the aligned memory area at address "addr" that was 
*     previously allocated using amalloc
*
* Author: Leonardo Giusti <Leonardo.Giusti@cern.ch>
*
*******************************************************************************/
#define UTILS_C

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

struct addr_t
{
   char *addr;
   char *true_addr;
   struct addr_t *next;
};

static struct addr_t *first=NULL;


void error(int test,int no,char *name,char *text)
{
   if (test!=0)
   {
      printf("\n");
      printf("Error in %s\n%s\n",name,text);
      printf("Program aborted\n\n");
      exit(no);
   }
}


void *amalloc(size_t size,int p)
{
   int shift;
   char *true_addr,*addr;
   unsigned long mask;
   struct addr_t *new;

   if ((size<=0)||(p<0))
      return(NULL);

   shift=1<<p;
   mask=(unsigned long)(shift-1);

   true_addr=malloc(size+shift);
   new=malloc(sizeof(*first));
   
   if ((true_addr==NULL)||(new==NULL))
   {
      free(true_addr);
      free(new);
      return(NULL);
   }

   addr=(char*)(((unsigned long)(true_addr+shift))&(~mask));
   (*new).addr=addr;
   (*new).true_addr=true_addr;
   (*new).next=first;
   first=new;
                   
   return((void*)(addr));
}


void afree(void *addr)
{
   struct addr_t *p,*q;

   q=NULL;
   
   for (p=first;p!=NULL;p=(*p).next)
   {
      if ((*p).addr==addr)
      {
         if (q!=NULL)
            (*q).next=(*p).next;
         else
            first=(*p).next;
         
         free((*p).true_addr);
         free(p);
         return;
      }
      
      q=p;
   }
}

