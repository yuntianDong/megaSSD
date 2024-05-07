/*
 * file Misc.cpp
 */
#include <unistd.h>

#include "utility/Misc.h"

int GetProcessID(void)
{
	return (int)getpid();
}
