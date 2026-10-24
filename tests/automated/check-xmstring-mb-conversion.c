/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Xm.h>
#include <stdio.h>
#include <string.h>

extern String _XmGetMBStringFromXmString(XmString);

int main(void)
{
    XmString compound = XmStringCreateLocalized("alpha");
    String text;

    if (!compound)
        return 1;
    text = _XmGetMBStringFromXmString(compound);
    if (!text || strcmp(text, "alpha") != 0) {
        fprintf(stderr, "unexpected compound-string conversion: %s\n",
                text ? text : "(null)");
        XtFree(text);
        XmStringFree(compound);
        return 1;
    }
    XtFree(text);
    XmStringFree(compound);
    puts("PASS bounded XmString multibyte conversion");
    return 0;
}
