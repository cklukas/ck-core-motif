/*
 * Fuzz target: the resource converters of lib/Xm/ResConvert.c that parse a
 * string given by a user (an X resource file, a command line, a UIL file):
 * tab lists, render tables, font lists, key symbol tables, character set
 * tables, string tables, cardinal lists, atom lists, compound strings,
 * positions and dimensions with units.
 *
 * ResConvert.c is included into this file so that its static converters can
 * be called directly, exactly as the Intrinsics call them; XmString.c,
 * XmRenderT.c, XmTabList.c, Direction.c, Xmos.c, ResEncod.c and ResInd.c are
 * compiled unchanged and linked (run-fuzz.py builds them with coverage
 * instrumentation).  xm-stubs.c stands in for the display and the widgets.
 *
 * Input: the first byte selects the converter (modulo the number of
 * converters), the rest is the string.  The string is handed over in a heap
 * block of exactly its length plus the terminating NUL.  The result of a
 * successful conversion is released with the converter's own destructor where
 * it has one.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <X11/IntrinsicP.h>

#include "ResConvert.c"

extern Display *fuzz_display(void);

typedef Boolean (*Converter)(Display *, XrmValue *, Cardinal *, XrmValue *, XrmValue *, XtPointer *);
typedef void (*Destructor)(XtAppContext, XrmValue *, XtPointer, XrmValue *, Cardinal *);

static const struct {
    const char *name;
    Converter convert;
    Destructor destroy;
} converters[] = {
    { "XmTabList", CvtStringToXmTabList, CvtStringToXmTabListDestroy },
    { "RenderTable", CvtStringToRenderTable, CvtStringToXmRenderTableDestroy },
    { "ButtonRenderTable", CvtStringToButtonRenderTable, CvtStringToXmRenderTableDestroy },
    { "XmFontList", CvtStringToXmFontList, CvtStringToXmFontListDestroy },
    { "KeySymTable", CvtStringToKeySymTable, CvtStringToKeySymTableDestroy },
    { "CharSetTable", CvtStringToCharSetTable, CvtStringToCharSetTableDestroy },
    { "XmStringTable", CvtStringToXmStringTable, NULL },
    { "StringTable", CvtStringToStringTable, NULL },
    { "CardinalList", CvtStringToCardinalList, NULL },
    { "AtomList", CvtStringToAtomList, NULL },
    { "XmString", CvtStringToXmString, CvtStringToXmStringDestroy },
    { "KeySym", CvtStringToKeySym, NULL },
    { "HorizontalPosition", CvtStringToHorizontalPosition, NULL },
    { "HorizontalDimension", CvtStringToHorizontalDimension, NULL },
    { "VerticalPosition", CvtStringToVerticalPosition, NULL },
    { "VerticalDimension", CvtStringToVerticalDimension, NULL },
    { "BooleanDimension", CvtStringToBooleanDimension, NULL },
    { "Cardinal", CvtStringToCardinal, NULL },
    { "TextPosition", CvtStringToTextPosition, NULL },
    { "Char", CvtStringToChar, NULL },
};

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    (void) argc; (void) argv;
    setlocale(LC_ALL, "C");
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* the widget that the converters read names and classes from: a root
     * widget with a class record, no parent, no resource database */
    static WidgetClassRec fake_class;
    static WidgetRec fake_widget;
    if (!fake_widget.core.widget_class) {
        fake_class.core_class.class_name = "Fuzz";
        fake_class.core_class.xrm_class = XrmStringToQuark("Fuzz");
        fake_widget.core.widget_class = &fake_class;
        fake_widget.core.xrm_name = XrmStringToQuark("fuzz");
        fake_widget.core.self = &fake_widget;
    }
    if (size < 1) return 0;

    char *text = malloc(size);           /* size - 1 bytes of string and the NUL */
    if (!text) abort();
    memcpy(text, data + 1, size - 1);
    text[size - 1] = '\0';

    size_t index = data[0] % (sizeof converters / sizeof *converters);
    /* FUZZ_SKIP_CONVERTERS=0,9 leaves out converters by number, to look past a finding in them */
    const char *skip = getenv("FUZZ_SKIP_CONVERTERS");
    for (const char *p = skip; p && *p; ) {
        char *next;
        unsigned long n = strtoul(p, &next, 10);
        if (next == p) break;
        if (n == index) { free(text); return 0; }
        p = *next ? next + 1 : next;
    }
    Widget widget = &fake_widget;
    XrmValue arg = { sizeof widget, (XPointer) &widget };
    Cardinal num_args = 1;
    XrmValue from = { (unsigned) size, text };
    _Alignas(16) unsigned char storage[64] = { 0 };
    XrmValue to = { sizeof storage, (XPointer) storage };
    XtPointer converter_data = NULL;

    if (converters[index].convert(fuzz_display(), &arg, &num_args, &from, &to, &converter_data) &&
        converters[index].destroy)
        converters[index].destroy(NULL, &to, NULL, &arg, &num_args);
    free(text);
    return 0;
}
