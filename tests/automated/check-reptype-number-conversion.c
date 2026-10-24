/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/RepType.h>
#include <Xm/Xm.h>
#include <stdio.h>

extern Boolean _XmConvertActionParamToRepTypeId(Widget, XmRepTypeId,
                                                 char *, Boolean, int *);

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, \
                                #condition); return 1; } \
    ++checks; \
} while (0)

static unsigned long checks;

int main(void)
{
    static String names[] = { (String)"zero", (String)"one", (String)"two" };
    static unsigned char values[] = { 0, 1, 2 };
    XmRepTypeId id = XmRepTypeRegister("NumberConversionRegression", names,
                                       values, 3);
    int result = -1;

    CHECK(_XmConvertActionParamToRepTypeId(NULL, id, (char *)"2suffix",
                                            True, &result));
    CHECK(result == 2);
    CHECK(!_XmConvertActionParamToRepTypeId(NULL, id,
                                             (char *)"999999999999999999999",
                                             True, &result));
    printf("PASS bounded rep-type numeric conversion: %lu checks\n", checks);
    return 0;
}
