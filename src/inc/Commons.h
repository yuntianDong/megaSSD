#ifndef _COMMONS_H
#define _COMMONS_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include <string>

#include "Logging.h"
#include "spdk/nvme.h"
#include "spdk/env.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "pci/pci.h"
#include "pci/header.h"

#ifdef __cplusplus
}
#endif

#define NVME_IN_USE						"nvme"
#define UIO_IN_USE						"uio_pci_generic"
#define PCIE_IN_USE                     "pcie"

/* Delete objects common method */
#define del_obj(name) \
    if(name != nullptr){ \
        delete name; \
        name = nullptr;}

#endif