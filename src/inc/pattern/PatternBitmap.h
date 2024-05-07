#ifndef _PATTERN_BITMAP_H
#define _PATTERN_BITMAP_H

#include <stdint.h>
#include <string.h>

#include "Commons.h"

#define PAT_IDX_UNMAP			0
#define PAT_IDX_ALLZERO			15
#define PAT_IDX_ALLHEXF			1

#define PAT_IDX_BITS			4
#define PAT_IDX_MIN				1
#define PAT_IDX_MAX				15
#define PAT_IDX_TOTAL			16

#define PAT_INVALID_IDX			0xFF

/*		Pattern	,index,	Comments				*/
#define DEFPATINDEXMAPTBL	\
	CC(0x00000000,0x0,"Unmap Index, Reserved")	\
	CC(0xFFFFFFFF,0x1,"Pattern 0xFFFFFFFF")		\
	CC(0x0000FFFF,0x2,"Pattern 0x0000FFFF")		\
	CC(0xF0F00F0F,0x3,"Pattern 0xF0F00F0F")		\
	CC(0xFF00FF00,0x4,"Pattern 0xFF00FF00")		\
	CC(0xFF0000FF,0x5,"Pattern 0xFF0000FF")		\
	CC(0x5AA5A55A,0x6,"Pattern 0x5AA5A55A")		\
	CC(0xCAACACCA,0x7,"Pattern 0xCAACACCA")		\
	CC(0x57757557,0x8,"Pattern 0x57757557")		\
	CC(0xC55C5CC5,0x9,"Pattern 0xC55C5CC5")		\
	CC(0x7AA7A77A,0xA,"Pattern 0x7AA7A77A")		\
	CC(0x7CC7C77C,0xB,"Pattern 0x7CC7C77C")		\
	CC(0x57ACCA75,0xC,"Pattern 0x57ACCA75")		\
	CC(0x7AC55CA7,0xD,"Pattern 0x7AC55CA7")		\
	CC(0xAC5775CA,0xE,"Pattern 0xAC5775CA")		\
	CC(0x00000000,0xF,"Pattern 0x00000000")		\

class PatternBitmap
{
private:
	static uint32_t	gBitmapPatIdxMap[PAT_IDX_TOTAL];

public:
	static uint32_t GetPatternByIndex(uint8_t index)
	{
		if(not IsValidIndex(index))
		{
			LOGERROR("Invalid index = %d",index);
			return 0;
		}

		return gBitmapPatIdxMap[index];
	};
	static uint8_t GetIndexByPattern(uint32_t pattern)
	{
		if(0 == pattern)
		{
			return (uint8_t)PAT_IDX_ALLZERO;
		}
		else if(0xFFFFFFFF == pattern)
		{
			return (uint8_t)PAT_IDX_ALLHEXF;
		}
		else
		{
			for(int idx=(PAT_IDX_MIN+1);idx<PAT_IDX_MAX;idx++)
			{
				if(pattern == gBitmapPatIdxMap[idx])
				{
					return idx;
				}
			}
		}

		return PAT_INVALID_IDX;
	};
	static bool IsValidIndex(uint8_t index)
	{
		return ((index >= PAT_IDX_MIN) && (index <= PAT_IDX_MAX)) || (PAT_IDX_UNMAP == index);
	};
	static bool SetIdxPattern(uint8_t index,uint32_t pattern)
	{
		if(false == IsValidIndex(index) ||
				PAT_IDX_ALLZERO == index ||
				PAT_IDX_ALLHEXF == index)
		{
			LOGERROR("Invalid index");
			return false;
		}

		gBitmapPatIdxMap[index] = pattern;
		return true;
	};
};

#endif
