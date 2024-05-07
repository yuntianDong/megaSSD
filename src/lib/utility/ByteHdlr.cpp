/*
 * file:ByteHdlr.cpp
 */

#include <stdio.h>
#include "utility/ByteHdlr.h"

void *ConvertEndian(void *ptr, size_t size)
{
   char *head = (char *)ptr;
   char *tail = head + size -1;
   char temp;

   for(; tail > head; tail--, head++)
   {
      temp = *head;
      *head = *tail;
      *tail = temp;
   }

   return ptr;
}

void ByteToString(void *ptr,char vstr[],int maxLen)
{
	while(0 <= --maxLen)
	{
		char tmp = *((char*)ptr+maxLen);
		vstr[maxLen]	= (0 == tmp)?'\0':tmp;
	}
}
