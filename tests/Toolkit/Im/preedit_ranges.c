/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Xm.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
extern Boolean _XmImInputPrintable(const char *, size_t, Boolean);
extern Boolean _XmImPreeditRangeValid(unsigned int, int, int, unsigned int, int);
int main(void)
{
    if (!_XmImPreeditRangeValid(5, 2, 3, 2, 4) ||
        !_XmImPreeditRangeValid(0, 0, 0, 0, 0) ||
        !_XmImPreeditRangeValid(65535, 65535, 0, 0, 65535) ||
        _XmImPreeditRangeValid(5, -1, 0, 0, 0) ||
        _XmImPreeditRangeValid(5, 6, 0, 0, 0) ||
        _XmImPreeditRangeValid(5, 4, 2, 0, 0) ||
        _XmImPreeditRangeValid(5, 0, -1, 0, 0) ||
        _XmImPreeditRangeValid(65535, 0, 0, 1, 0) ||
        _XmImPreeditRangeValid(5, 0, 0, 1, 7) ||
        _XmImPreeditRangeValid(5, 0, 0, 0, -1)) {
        fputs("XIM preedit bounds regression failed\n", stderr); return 1;
    }
    if (!setlocale(LC_ALL, "C.UTF-8") ||
        !_XmImInputPrintable("e\xcc\x81", 3, False) ||
        !_XmImInputPrintable("\xe2\x80\x8d", 3, False) ||
        !_XmImInputPrintable("\xf0\x9f\x99\x82", 4, False) ||
        _XmImInputPrintable("\xcc", 1, False) ||
        _XmImInputPrintable("\xc0\xaf", 2, False) ||
        _XmImInputPrintable("x\ny", 3, False) ||
        _XmImInputPrintable("\t", 1, False) ||
        !_XmImInputPrintable("x\ty", 3, True)) {
        fputs("XIM logical input acceptance regression failed\n", stderr); return 1;
    }
    puts("XIM preedit bounds and logical input regression passed"); return 0;
}
