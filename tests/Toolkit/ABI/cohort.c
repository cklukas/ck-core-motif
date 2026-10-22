/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Xm.h>
#include <Xm/TextFP.h>
#include <Xm/TextOutP.h>
#include <Xm/TextStrSoP.h>
#include <stdio.h>
int main(void)
{
    if (XmABI_VERSION != 5 || XmToolkitABI() != XmABI_VERSION ||
        XmVERSION != 2 || XmREVISION != 4 || XmUPDATE_LEVEL != 0) {
        fputs("Native toolkit header/runtime ABI cohort mismatch\n", stderr); return 1;
    }
    printf("ABI %u, TextField=%zu Output=%zu Source=%zu LineExtra=%zu\n",
           XmToolkitABI(), sizeof(XmTextFieldRec), sizeof(OutputDataRec),
           sizeof(XmSourceDataRec), sizeof(LineTableExtraRec));
    return 0;
}
