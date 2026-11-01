/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include "WmGlobal.h"
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef GETMESSAGE
#define GETMESSAGE(set, number, text) (text)
#endif
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

/* Isolate argument parsing from configuration-file I/O and diagnostics. */
static void ScanWhitespace(unsigned char **line)
{
    while (isspace(**line)) ++*line;
}
static void ScanAlphanumeric(unsigned char **line)
{
    while (isalnum(**line)) ++*line;
}
static void ToLower(unsigned char *text)
{
    for (; *text; ++text) *text = (unsigned char)tolower(*text);
}
static unsigned char *GetString(unsigned char **line)
{
    unsigned char *text = *line;
    *line += strlen((char *)text);
    return *text ? text : NULL;
}
static void PWarning(char *text) { (void)text; }

/* The runner extracts these complete functions from WmResParse.c. */
#include "modern-mwm-functions.h"

#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "Mwm modern C: %s\n", #test); return 1; } } while (0)

int main(void)
{
    Boolean (*parsers[])(unsigned char **, WmFunction, String *) = {
        ParseWmFuncNbrArg, ParseWmFuncGrpArg
    };
    unsigned char number[] = "4294967295", overflow[] = "4294967296";
    unsigned char groups[] = "icon | transient", empty[] = "";
    unsigned char key[] = "1234", bad_key[] = "4294967296";
    unsigned char *line;
    unsigned int detail;
    String argument;
    CHECK(UINT_MAX == 4294967295U);
    line = number;
    CHECK(parsers[0](&line, NULL, &argument));
    CHECK((uintptr_t)argument == UINT_MAX);
    line = overflow;
    CHECK(!parsers[0](&line, NULL, &argument) && argument == NULL);
    line = groups;
    CHECK(parsers[1](&line, NULL, &argument));
    CHECK((uintptr_t)argument == (F_GROUP_ICON | F_GROUP_TRANSIENT));
    line = empty;
    CHECK(parsers[1](&line, NULL, &argument));
    CHECK((uintptr_t)argument == F_GROUP_DEFAULT);
    line = key;
    CHECK(ParseKeySym(&line, 0, &detail) && detail == 1234);
    line = bad_key;
    CHECK(!ParseKeySym(&line, 0, &detail) && detail == NoSymbol);
    puts("PASS typed Mwm parser dispatch, numeric overflow and numeric keysyms");
    return 0;
}
