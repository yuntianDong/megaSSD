#ifndef _BYTE_HDLR_H
#define _BYTE_HDLR_H

void *ConvertEndian(void *ptr, size_t size);
void ByteToString(void *ptr,char vstr[],int maxLen);

#endif
