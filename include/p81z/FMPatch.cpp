#include "FMPatch.h"
#include <cstring>

bool load_VMEM_bank(const uint8_t *data, size_t length, FMBank *bank)
{
    if (length < 8 || data[0] != 0xf0 || data[1] != 0x43 || data[3] != 0x04 || data[length - 1] != 0xf7)
        return false;

    std::memset(bank, 0, sizeof(*bank));

    uint32_t count = data[4] & 127;
    bank->count = count;

    for (uint32_t i = 0; i < count; ++i) {
        size_t offset = 6 + 128 * i;
        uint8_t patchdata[128] = {};
        std::memcpy(patchdata, data + offset, std::min(length - offset, sizeof(patchdata)));
        load_VMEM_patch(patchdata, &bank->patch[i]);
    }

    return true;
}

void load_VMEM_patch(const uint8_t data[128], FMPatch *patch)
{
    const unsigned op_order[4] = {3, 1, 2, 0};

    for (unsigned nth_op = 0; nth_op < 4; ++nth_op) {
        size_t offset = nth_op * 10;
        size_t offset_ex = 73 + nth_op * 2;
        load_VMEM_operator(data + offset, data + offset_ex, &patch->op[op_order[nth_op]]);
    }

    patch->sy_fbl_alg = data[40] & 127;
    patch->lfs = std::min(99, data[41] & 127);
    patch->lfd = std::min(99, data[42] & 127);
    patch->pmd = std::min(99, data[43] & 127);
    patch->amd = std::min(99, data[44] & 127);
    patch->pms_ams_lfw = data[45] & 127;
    patch->trps = std::min(48, data[46] & 63);
    patch->pbr = std::min(12, data[47] & 15);
    patch->ch_mo_su_po_pm = data[48] & 31;
    patch->port = std::min(99, data[49] & 127);
    patch->fcvol = std::min(99, data[50] & 127);
    patch->mwpitch = std::min(99, data[51] & 127);
    patch->mwampli = std::min(99, data[52] & 127);
    patch->bcpitch = std::min(99, data[53] & 127);
    patch->bcampli = std::min(99, data[54] & 127);
    patch->bcpbias = std::min(99, data[55] & 127);
    patch->bcebias = std::min(99, data[56] & 127);

    patch->pr1 = data[67] & 127;
    patch->pr2 = data[68] & 127;
    patch->pr3 = data[69] & 127;
    patch->pl1 = data[70] & 127;
    patch->pl2 = data[71] & 127;
    patch->pl3 = data[72] & 127;

    std::memcpy(patch->name, data + 57, 10);
    patch->name[10] = '\0';
}

void load_VMEM_operator(const uint8_t data[10], const uint8_t data_ex[2], FMPatchOperator *op)
{
    op->ar = data[0] & 31;
    op->d1r = data[1] & 31;
    op->d2r = data[2] & 31;
    op->rr = std::max(1, data[3] & 15);
    op->d1l = data[4] & 15;
    op->ls = std::min(99, data[5] & 127);
    op->ame_ebs_kvs = data[6] & 127;
    op->out = std::min(99, data[7] & 127);
    op->f = data[8] & 63;
    op->rs_dbt = data[9] & 31;
    op->egsft_fix_fixrg = data_ex[0] & 63;
    op->opw_fine = data_ex[1] & 127;
}
