#include <Xm/TextP.h>
#include <Xm/TextStrSoP.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (sizeof(XmTextPosition) > sizeof(int)) {
        CHECK(_XmStringSourceGetString(&widget, 0, oversized, False) == NULL);
        CHECK(_XmStringSourceGetString(&widget, oversized, oversized + 1,
                                       False) == NULL);
    }

    _XmStringSourceDestroy(source);
    printf("PASS text source range bounds: %lu checks\n", checks);
    return 0;
}
