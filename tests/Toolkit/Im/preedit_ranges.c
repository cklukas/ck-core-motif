/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Xm.h>
#include <stdio.h>
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
    puts("XIM preedit bounds regression passed"); return 0;
}
