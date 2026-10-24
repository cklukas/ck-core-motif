/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <stdio.h>

extern int _XmParsePrintResolution(const char *text);

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, \
                                #condition); return 1; } \
    ++checks; \
} while (0)

static unsigned long checks;

int main(void)
{
    CHECK(_XmParsePrintResolution("300") == 300);
    CHECK(_XmParsePrintResolution("300dpi") == 300);
    CHECK(_XmParsePrintResolution(NULL) == 0);
    CHECK(_XmParsePrintResolution("0") == 0);
    CHECK(_XmParsePrintResolution("-75") == 0);
    CHECK(_XmParsePrintResolution("999999999999999999999") == 0);
    printf("PASS bounded print resolution parsing: %lu checks\n", checks);
    return 0;
}
