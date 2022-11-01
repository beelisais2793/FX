#include "FMPatch.h"
#include <algorithm>
#include <cstdio>

///
void print_patch(FMPatch *patch)
{
    printf("%s\n", patch->name);

#define Print4(fmt, member) \
    printf((fmt),                                                       \
           patch->op[0].member, patch->op[1].member,                    \
           patch->op[2].member, patch->op[3].member)

    Print4("AR               %02X %02X %02X %02X\n", ar);
    Print4("D1R              %02X %02X %02X %02X\n", d1r);
    Print4("D2R              %02X %02X %02X %02X\n", d2r);
    Print4("RR               %02X %02X %02X %02X\n", rr);
    Print4("D1L              %02X %02X %02X %02X\n", d1l);
    Print4("LS               %02X %02X %02X %02X\n", ls);
    Print4("AME              %02X %02X %02X %02X\n", ame());
    Print4("EBS              %02X %02X %02X %02X\n", ebs());
    Print4("KVS              %02X %02X %02X %02X\n", kvs());
    Print4("OUT              %02X %02X %02X %02X\n", out);
    Print4("F                %02X %02X %02X %02X\n", f);
    Print4("RS               %02X %02X %02X %02X\n", rs());
    Print4("DBT              %02X %02X %02X %02X\n", dbt());

    Print4("EGSFT            %02X %02X %02X %02X\n", egsft());
    Print4("FIX              %02X %02X %02X %02X\n", fix());
    Print4("FIXRG            %02X %02X %02X %02X\n", fixrg());
    Print4("OPW              %02X %02X %02X %02X\n", opw());
    Print4("FINE             %02X %02X %02X %02X\n", fine());

#undef Print4

    printf("---\n");
    printf("SY/FBL/ALG       %02X %02X %02X\n", patch->sy(), patch->fbl(), patch->alg());
    printf("LFS/LFD          %02X %02X\n", patch->lfs, patch->lfd);
    printf("PMD/AMD          %02X %02X\n", patch->pmd, patch->amd);
    printf("PMS/AMS          %02X %02X\n", patch->pms(), patch->ams());
    printf("LFW              %02X\n", patch->lfw());
    printf("TRPS             %02X\n", patch->trps);
    printf("PBR              %02X\n", patch->pbr);
    printf("CH/MO/SU/PO/PM   %02X %02X %02X %02X %02X\n", patch->ch(), patch->mo(), patch->su(), patch->po(), patch->pm());
    printf("PORT             %02X\n", patch->port);
    printf("FCVOL            %02X\n", patch->fcvol);
    printf("MWPITCH/MWAMPLI  %02X %02X\n", patch->mwpitch, patch->mwampli);
    printf("BCPITCH/BCAMPLI  %02X %02X\n", patch->bcpitch, patch->bcampli);
    printf("BCPBIAS/BCEBIAS  %02X %02X\n", patch->bcpbias, patch->bcebias);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        return 1;

    const char *filepath = argv[1];
    FILE *filehandle = fopen(filepath, "rb");

    if (!filehandle)
        return 1;

    uint8_t filedata[64 * 1024] = {};
    size_t filesize = fread(filedata, 1, sizeof(filedata), filehandle);

    FMBank bank;
    if (!load_VMEM_bank(filedata, filesize, &bank))
        return 1;

    for (unsigned i = 0; i < bank.count; ++i) {
        printf("###\nPatch %u: ", i + 1);
        print_patch(&bank.patch[i]);
        printf("\n");
    }

    return 0;
}
