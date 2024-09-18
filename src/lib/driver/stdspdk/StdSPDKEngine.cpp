/*
 * file StdSPDKEngine.cpp
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#include "driver/stdspdk/StdSPDKEngine.h"
#include "spdk/env.h"
#include "spdk/nvme.h"
#include "Logging.h"

NVMeCmdResp *StdSPDKEngine::NVMePassthru(NVMeCmdReq &req, void *dBuf, uint32_t dBufLen, void *mBuf,
                                         uint32_t mBufLen, uint32_t timeout)
{
    int ret = 0;
    struct timeval start;
    struct timeval end;

    NVMeCmdResp *resp = new NVMeCmdResp();

    spdk_nvme_cmd cmd = {};

    memcpy(&cmd, req.GetCmdAddr(), req.GetCmdSize());

    outstanding_cmds++;

    gettimeofday(&start, NULL);
    if (req.isAdminCmd())
    {
        ret = spdk_nvme_ctrlr_cmd_admin_raw(mCtrlr, &cmd, dBuf, dBufLen, CmdCompletion, (void *)resp);
        while (outstanding_cmds)
        {
            spdk_nvme_ctrlr_process_admin_completions(mCtrlr);
        };
    }
    else
    {
        struct spdk_nvme_qpair *qpair;
        qpair = spdk_nvme_ctrlr_alloc_io_qpair(mCtrlr, NULL, 0);
        if (!qpair)
            return resp;

        ret = spdk_nvme_ctrlr_cmd_io_raw_with_md(mCtrlr, qpair, &cmd, dBuf, dBufLen, mBuf, CmdCompletion, (void *)resp);
        while (outstanding_cmds)
        {
            spdk_nvme_qpair_process_completions(qpair, 0);
        };
        spdk_nvme_ctrlr_free_io_qpair(qpair);
    }
    gettimeofday(&end, NULL);

    if (0 > ret || resp->Succeeded() != true)
    {
        LOGERROR("spdk cmd excute Failed, ret = %d, status code = %x", ret,resp->GetSC());
        this->DumpCmd(&cmd);
        return resp;
    }

    resp->SetExecTime((end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec));
    return resp;
}
void StdSPDKEngine::CmdCompletion(void *ctx, const struct spdk_nvme_cpl *cpl)
{
    NVMeCmdResp *resp = (NVMeCmdResp *)ctx;
    resp->SetRespVal(cpl->cdw0);
    resp->SetDW1(cpl->cdw1);
    resp->SetSQHP(cpl->sqhd);
    resp->SetSQID(cpl->sqid);
    resp->SetCID(cpl->cid);
    resp->SetPhaseTag(cpl->status.p);
    resp->SetMFlag(cpl->status.m);
    resp->SetDNRFlag(cpl->status.dnr);
    resp->SetSCT(cpl->status.sct);
    resp->SetSC(cpl->status.sc);

    outstanding_cmds--;
}
bool StdSPDKEngine::NVMeReset(void)
{
    int ret = spdk_nvme_ctrlr_reset(mCtrlr);

    return ret >= 0;
}

bool StdSPDKEngine::NVMeSubSysReset(void)
{
    int ret = spdk_nvme_ctrlr_reset_subsystem(mCtrlr);

    return ret >= 0;
}

void StdSPDKEngine::DumpCmd(void *ctx)
{
    spdk_nvme_cmd *cmd = (spdk_nvme_cmd *)ctx;

    LOGINFO("opc\t=0x%x", cmd->opc);
    LOGINFO("nsid\t=0x%x", cmd->nsid);
    LOGINFO("cdw10\t=0x%x", cmd->cdw10);
    LOGINFO("cdw11\t=0x%x", cmd->cdw11);
    LOGINFO("cdw12\t=0x%x", cmd->cdw12);
    LOGINFO("cdw13\t=0x%x", cmd->cdw13);
    LOGINFO("cdw14\t=0x%x", cmd->cdw14);
    LOGINFO("cdw15\t=0x%x\n", cmd->cdw15);
}
