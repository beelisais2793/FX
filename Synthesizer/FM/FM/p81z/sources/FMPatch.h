#pragma once
#include <algorithm>
#include <cstdint>

struct FMPatchOperator {
    uint8_t ar;
    uint8_t d1r;
    uint8_t d2r;
    uint8_t rr;
    uint8_t d1l;
    uint8_t ls;
    uint8_t ame_ebs_kvs;
    bool ame() const { return ame_ebs_kvs >> 6; }
    uint8_t ebs() const { return (ame_ebs_kvs >> 3) & 7; }
    uint8_t kvs() const { return ame_ebs_kvs & 7; }
    uint8_t out;
    uint8_t f;
    uint8_t rs_dbt;
    uint8_t rs() const { return (rs_dbt >> 3) & 3; }
    uint8_t dbt() const { return std::min(6, rs_dbt & 7); }
    uint8_t egsft_fix_fixrg;
    uint8_t egsft() const { return egsft_fix_fixrg >> 4; }
    bool fix() const { return (egsft_fix_fixrg >> 3) & 1; }
    uint8_t fixrg() const { return egsft_fix_fixrg & 3; }
    uint8_t opw_fine;
    uint8_t opw() const { return opw_fine >> 4; }
    uint8_t fine() const { return opw_fine & 15; }
};

struct FMPatch {
    FMPatchOperator op[4];

    uint8_t sy_fbl_alg;
    uint8_t sy() const { return sy_fbl_alg >> 6; }
    uint8_t fbl() const { return (sy_fbl_alg >> 3) & 7; }
    uint8_t alg() const { return sy_fbl_alg & 7; }
    uint8_t lfs;
    uint8_t lfd;
    uint8_t pmd;
    uint8_t amd;
    uint8_t pms_ams_lfw;
    uint8_t pms() const { return pms_ams_lfw >> 4; }
    uint8_t ams() const { return (pms_ams_lfw >> 2) & 3; }
    uint8_t lfw() const { return pms_ams_lfw & 3; }
    uint8_t trps;
    uint8_t pbr;
    uint8_t ch_mo_su_po_pm;
    bool ch() const { return (ch_mo_su_po_pm >> 4) & 1; }
    bool mo() const { return (ch_mo_su_po_pm >> 3) & 1; }
    bool su() const { return (ch_mo_su_po_pm >> 2) & 1; }
    bool po() const { return (ch_mo_su_po_pm >> 1) & 1; }
    bool pm() const { return ch_mo_su_po_pm & 1; }
    uint8_t port;
    uint8_t fcvol;
    uint8_t mwpitch;
    uint8_t mwampli;
    uint8_t bcpitch;
    uint8_t bcampli;
    uint8_t bcpbias;
    uint8_t bcebias;

    uint8_t pr1;
    uint8_t pr2;
    uint8_t pr3;
    uint8_t pl1;
    uint8_t pl2;
    uint8_t pl3;

    char name[11];
};

struct FMBank {
    enum { MaxPatches = 128 };
    uint32_t count;
    FMPatch patch[MaxPatches];
};

bool load_VMEM_bank(const uint8_t *data, size_t length, FMBank *bank);
void load_VMEM_patch(const uint8_t data[128], FMPatch *patch);
void load_VMEM_operator(const uint8_t data[10], const uint8_t data_ex[2], FMPatchOperator *op);
