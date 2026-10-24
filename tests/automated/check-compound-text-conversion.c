/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Xm.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    XmString compound = NULL;
    char *text = NULL;

    compound = XmStringCreateLocalized("compound text");
    if (!compound || !XmStringGetLtoR(compound, XmFONTLIST_DEFAULT_TAG, &text) ||
        !text) {
        fputs("compound text conversion failed\n", stderr);
        XmStringFree(compound);
        return 1;
    }
    if (strcmp(text, "compound text") != 0)
        return 1;
    XtFree(text);
    XmStringFree(compound);
    puts("PASS compound-text conversion length checks");
    return 0;
}
