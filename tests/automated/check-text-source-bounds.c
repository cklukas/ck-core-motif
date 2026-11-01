#define _GNU_SOURCE
#include <Xm/TextP.h>
#include <Xm/TextStrSoP.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <dlfcn.h>

extern XmTextSource _XmStringSourceCreate(char *, Boolean);
extern void _XmStringSourceDestroy(XmTextSource);
extern char *_XmStringSourceGetString(XmTextWidget, XmTextPosition,
                                       XmTextPosition, Boolean);

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, \
                                #condition); return 1; } \
    ++checks; \
} while (0)

static unsigned long checks;

int main(void)
{
    XmTextSource source = _XmStringSourceCreate("abc", False);
    XmTextRec widget;
    XmTextPosition oversized;
    char *value;
    Dl_info loaded;
    const char *expected = getenv("CKMOTIF_EXPECT_LIBRARY");
    char *actual_library, *expected_library;

    CHECK(expected != NULL);
    CHECK(dladdr((void *)_XmStringSourceGetString, &loaded) != 0);
    actual_library = realpath(loaded.dli_fname, NULL);
    expected_library = realpath(expected, NULL);
    CHECK(actual_library != NULL && expected_library != NULL);
    CHECK(strcmp(actual_library, expected_library) == 0);
    printf("Loaded library: %s\n", actual_library);
    free(actual_library);
    free(expected_library);

    CHECK(source != NULL);
    memset(&widget, 0, sizeof(widget));
    widget.text.source = source;
    widget.text.char_size = 1;
    oversized = INT_MAX;
    if (sizeof(XmTextPosition) > sizeof(int)) ++oversized;
    source->data->widgets = (XmTextWidget *)XtMalloc(sizeof(XmTextWidget));
    source->data->widgets[0] = &widget;
    source->data->numwidgets = 1;

    value = _XmStringSourceGetString(&widget, 0, 3, False);
    CHECK(value != NULL && strcmp(value, "abc") == 0);
    XtFree(value);
    value = _XmStringSourceGetString(&widget, 1, 3, False);
    CHECK(value != NULL && strcmp(value, "bc") == 0);
    XtFree(value);
    for (int wide = 0; wide <= 1; ++wide) {
        CHECK(_XmStringSourceGetString(NULL, 0, 1, (Boolean)wide) == NULL);
        CHECK(_XmStringSourceGetString(&widget, -1, 1, (Boolean)wide) == NULL);
        CHECK(_XmStringSourceGetString(&widget, 2, 1, (Boolean)wide) == NULL);
        CHECK(_XmStringSourceGetString(&widget, 0, 7, (Boolean)wide) == NULL);
        value = _XmStringSourceGetString(&widget, 1, 1, (Boolean)wide);
        CHECK(value != NULL);
        CHECK(wide ? ((wchar_t *)value)[0] == L'\0' : value[0] == '\0');
        XtFree(value);
        if (sizeof(XmTextPosition) > sizeof(int)) {
            CHECK(_XmStringSourceGetString(&widget, 0, oversized, (Boolean)wide) == NULL);
            CHECK(_XmStringSourceGetString(&widget, oversized, oversized + 1,
                                           (Boolean)wide) == NULL);
        }
    }
    value = _XmStringSourceGetString(&widget, 0, 3, True);
    CHECK(value != NULL && wcscmp((wchar_t *)value, L"abc") == 0);
    XtFree(value);
    widget.text.char_size = 0;
    CHECK(_XmStringSourceGetString(&widget, 0, 3, False) == NULL);
    widget.text.source = NULL;
    CHECK(_XmStringSourceGetString(&widget, 0, 1, False) == NULL);

    _XmStringSourceDestroy(source);
    printf("PASS text source range bounds: %lu checks\n", checks);
    return 0;
}
