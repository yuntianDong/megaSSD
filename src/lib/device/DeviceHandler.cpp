#include "device/DeviceHandler.h"
#include <boost/format.hpp>

SPDKHandler::SPDKHandler(const char *bdf, int idx, bool ifrestore) : DeviceHandler()
{
    devoperator = new Operator;

    // Init SPDK env
    struct spdk_env_opts opts;

    spdk_env_opts_init(&opts);
    opts.name = SPDK_ENV_OPT_NAME;
    if (ifrestore == true)
    {
        if (spdk_env_init(NULL) < 0)
        {
            LOGWARN("Unable to reinitialize SPDK env\n");
        }
    }
    else
    {
        if (spdk_env_init(&opts) < 0)
        {
            LOGWARN("Unable to initialize SPDK env\n");
        }
    }
    // Generate operator, get SPDK ctrlr
    GenOperator(bdf);

    // Generate SPDK device
    mdev = (Device *)new SPDKDevice(devoperator);
    mdev->SetBdf(bdf);
    mdev->SetBdfID(idx);
    mdev->SetDriver(UIO_IN_USE);
}

void SPDKHandler::GenOperator(const char *bdf, int nsid)
{
    // use spdk fuc to connect ctrlr
    struct spdk_nvme_transport_id trid;
    struct spdk_nvme_ctrlr_opts ctrlr_opts;

    strcpy(trid.trstring, SPDK_NVME_TRANSPORT_NAME_PCIE);
    trid.trtype = SPDK_NVME_TRANSPORT_PCIE;
    strcpy(trid.traddr, bdf);
    snprintf(trid.subnqn, sizeof(trid.subnqn), "%s", SPDK_NVMF_DISCOVERY_NQN);
    spdk_nvme_ctrlr_get_default_ctrlr_opts(&ctrlr_opts, sizeof(ctrlr_opts));

    devoperator->SetSpdkCtrlr(spdk_nvme_connect(&trid, &ctrlr_opts, sizeof(ctrlr_opts)));

    assert(devoperator->GetSpdkCtrlr() != nullptr);
}

void SPDKHandler::ReleaseOperator(void)
{
    struct spdk_nvme_detach_ctx *detach_ctx = nullptr;
    spdk_nvme_detach_async(devoperator->GetSpdkCtrlr(), &detach_ctx);
    if (detach_ctx)
    {
        spdk_nvme_detach_poll(detach_ctx);
    }
    else
    {
        detach_ctx = nullptr;
    }
}

KernelHandler::KernelHandler(const char *bdf, int idx, int nsid) : DeviceHandler()
{
    devoperator = new Operator;
    if (nsid != -1 && nsid != 0)
    {
        GenOperator(bdf, nsid);
    }
    else
    {
        GenOperator(bdf);
    }
    // Generate Kernel device
    mdev = (Device *)new KernelDevice(devoperator);
    mdev->SetBdf(bdf);
    mdev->SetBdfID(idx);
    mdev->SetDriver(NVME_IN_USE);
}

void KernelHandler::GenOperator(const char *bdf, int nsid)
{
    PCIAddr *pciaddr = new PCIAddr(bdf);
    NVMeScan *nvmeScan = new NVMeScan;
    std::string kernelDevName(nvmeScan->GetDevNameByPciAddr(pciaddr));

    if (nsid != -1 && nsid != 0)
    {
        kernelDevName += "n" + std::to_string(nsid);
        LOGDEBUG("open device : %s", kernelDevName.c_str());
    }
    int fd = 0;

    if ((fd = open(kernelDevName.c_str(), O_RDWR)) < 0)
    {
        LOGERROR("Cannot open device: %s,Error:%d\n", kernelDevName, fd);
        devoperator->SetNVMeFd(INVALID_DEVHDLR);
    }

    assert(fd != INVALID_DEVHDLR);
    devoperator->SetNVMeFd(fd);

    if (nullptr != pciaddr)
    {
        delete pciaddr;
        pciaddr = nullptr;
    }
    if (nullptr != nvmeScan)
    {
        delete nvmeScan;
        nvmeScan = nullptr;
    }
}

void KernelHandler::ReleaseOperator(void)
{
    close(devoperator->GetNVMeFd());
    devoperator->SetNVMeFd(INVALID_DEVHDLR);
}

PcieHandler::PcieHandler(const char *bdf, int idx) : DeviceHandler()
{
    devoperator = new Operator;
    mPacc = pci_alloc();
    pci_init(mPacc);
    GenOperator(bdf);
    mdev = (Device *)new PcieDevice(devoperator);
    mdev->SetBdf(bdf);
    mdev->SetBdfID(idx);
    mdev->SetDriver(PCIE_IN_USE);
}
void PcieHandler::GenOperator(const char *bdf, int nsid)
{
    int mPciFd;
    uint8_t domain, bus, dev, func;

    // get domain,bus,dev,func through bdf
    std::string mbdf(bdf);
    uint32_t temp[4];
    std::istringstream iss(mbdf);
    char delim;

    iss >> std::hex >> temp[0] >> delim >> temp[1] >> delim >> temp[2] >> delim >> temp[3];

    domain = static_cast<uint8_t>(temp[0]);
    bus = static_cast<uint8_t>(temp[1]);
    dev = static_cast<uint8_t>(temp[2]);
    func = static_cast<uint8_t>(temp[3]);

    // Generate operator
    devoperator->SetPcieDev(pci_get_dev(mPacc, domain, bus, dev, func));
    assert(devoperator->GetPcieDev() != nullptr);

    char mPCIeBarRes[100];
    sprintf(mPCIeBarRes, PCI_BAR_PATH, domain, bus, dev, func);
    mPciFd = open(mPCIeBarRes, O_RDWR);

    assert(mPciFd != INVALID_DEVHDLR);

    devoperator->SetBarMemBase(mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mPciFd, 0));

    if (mPciFd != INVALID_DEVHDLR)
    {
        close(mPciFd);
        mPciFd = INVALID_DEVHDLR;
    }
}

void PcieHandler::ReleaseOperator(void)
{
    if (devoperator->GetPcieDev())
    {
        pci_free_dev(devoperator->GetPcieDev());
    }
    if (devoperator->GetBarMemBase())
    {
        munmap(devoperator->GetBarMemBase(), MAP_SIZE);
    }
}