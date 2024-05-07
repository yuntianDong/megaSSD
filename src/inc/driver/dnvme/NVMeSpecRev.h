#ifndef _NVMESPECREV_H_
#define _NVMESPECREV_H_

typedef enum {
    SPECREV_10b,             // http://www.nvmexpress.org/ spec. rev. 1.0b
    SPECREV_11,              // http://www.nvmexpress.org/ spec. rev. 1.1
    SPECREV_12,              // http://www.nvmexpress.org/ spec. rev. 1.2
	SPECREV_121,             // http://www.nvmexpress.org/ spec. rev. 1.2.1
	SPECREV_13,              // http://www.nvmexpress.org/ spec. rev. 1.3
    SPECREV_FENCE
}enNVMeSpecRev;

#endif
