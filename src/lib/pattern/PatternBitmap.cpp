/*
 * file PatternBitmap.cpp
 */

#include "pattern/PatternBitmap.h"

#define CC(a,b,c)		a,
uint32_t	PatternBitmap::gBitmapPatIdxMap[PAT_IDX_TOTAL] = {DEFPATINDEXMAPTBL};
#undef CC
