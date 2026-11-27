/*
 * Fuzz target: ISO-2022 Compound Text decoding/encoding (XmCvtCTToXmString,
 * XmCvtXmStringToCT) and XmStringParseText / XmStringUnparse (lib/Xm/ResEncod.c,
 * lib/Xm/XmString.c, compiled unchanged).
 */
#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

extern Display *fuzz_display(void);
extern Boolean XmCvtTextToXmString(Display *, XrmValuePtr, Cardinal *,
                                   XrmValue *, XrmValue *, XtPointer *);

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    (void) argc;
    (void) argv;
    setlocale(LC_ALL, "C");
    return 0;
}

static void walk_xmstring(XmString s)
{
    XmStringContext ctx;
    XtPointer value;
    unsigned int len;
    XmStringComponentType t;
    char *text, *tag;
    XmStringDirection dir;
    Boolean sep;

    if (XmStringInitContext(&ctx, s)) {
        int guard = 0;
        while ((t = XmStringGetNextTriple(ctx, &len, &value)) != XmSTRING_COMPONENT_END &&
               guard++ < 4096) {
            volatile unsigned char sink = 0;
            for (unsigned int i = 0; value && i < len; ++i)
                sink ^= ((unsigned char *) value)[i];
            (void) sink;
            if (value)
                XtFree((char *) value);
        }
        XmStringFreeContext(ctx);
    }
    if (XmStringInitContext(&ctx, s)) {
        int guard = 0;
        while (XmStringGetNextSegment(ctx, &text, &tag, &dir, &sep) && guard++ < 4096) {
            if (text) {
                (void) strlen(text);
                XtFree(text);
            }
            if (tag)
                XtFree(tag);
        }
        XmStringFreeContext(ctx);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size > 8192)
        return 0;

    /* Exact-sized NUL-terminated heap block so any overread triggers ASan */
    char *ct = (char *) malloc(size + 1);
    if (!ct)
        abort();
    if (size > 0)
        memcpy(ct, data, size);
    ct[size] = '\0';

    /* 1. Compound Text -> XmString -> walk -> Compound Text round-trip */
    XmString s = XmCvtCTToXmString(ct);
    if (s) {
        walk_xmstring(s);
        XmString copy = XmStringCopy(s);
        if (copy) {
            (void) XmStringCompare(s, copy);
            XmStringFree(copy);
        }
        char *rt_ct = XmCvtXmStringToCT(s);
        if (rt_ct) {
            XmString s2 = XmCvtCTToXmString(rt_ct);
            if (s2) {
                walk_xmstring(s2);
                XmStringFree(s2);
            }
            XtFree(rt_ct);
        }
        XmStringFree(s);
    }

    /* 1b. Length-bounded XmCvtTextToXmString converter path */
    {
        char *raw = (char *) malloc(size ? size : 1);
        if (!raw)
            abort();
        if (size > 0)
            memcpy(raw, data, size);
        XrmValue from_val, to_val;
        Cardinal zero = 0;
        from_val.addr = (XPointer) raw;
        from_val.size = (unsigned int) size;
        to_val.addr = NULL;
        to_val.size = 0;
        if (XmCvtTextToXmString(fuzz_display(), NULL, &zero, &from_val, &to_val, NULL) &&
            to_val.addr != NULL) {
            XmString ts = (XmString) to_val.addr;
            walk_xmstring(ts);
            XmStringFree(ts);
        }
        free(raw);
    }

    /* 2. XmStringParseText (newline & tab parse mappings) -> XmStringUnparse */
    XmParseMapping mappings[2];
    Arg args[4];
    Cardinal n = 0;
    XmString sep_comp = XmStringSeparatorCreate();
    XtSetArg(args[n], XmNpattern, "\n"); n++;
    XtSetArg(args[n], XmNpatternType, XmCHARSET_TEXT); n++;
    XtSetArg(args[n], XmNsubstitute, sep_comp); n++;
    XtSetArg(args[n], XmNincludeStatus, XmINSERT); n++;
    mappings[0] = XmParseMappingCreate(args, n);

    n = 0;
    XmString tab_comp = XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0, NULL);
    XtSetArg(args[n], XmNpattern, "\t"); n++;
    XtSetArg(args[n], XmNpatternType, XmCHARSET_TEXT); n++;
    XtSetArg(args[n], XmNsubstitute, tab_comp); n++;
    XtSetArg(args[n], XmNincludeStatus, XmINSERT); n++;
    mappings[1] = XmParseMappingCreate(args, n);

    XtPointer text_end = NULL;
    XmString parsed = XmStringParseText(
        (XtPointer) ct, &text_end, XmFONTLIST_DEFAULT_TAG,
        (size > 0 && (data[0] & 1u)) ? XmMULTIBYTE_TEXT : XmCHARSET_TEXT,
        mappings, 2, NULL);
    if (parsed) {
        walk_xmstring(parsed);
        XtPointer unparsed = XmStringUnparse(
            parsed, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
            mappings, 2, XmOUTPUT_ALL);
        if (unparsed)
            XtFree((char *) unparsed);
        XmStringFree(parsed);
    }

    XmParseMappingFree(mappings[0]);
    XmParseMappingFree(mappings[1]);
    XmStringFree(sep_comp);
    XmStringFree(tab_comp);
    free(ct);
    return 0;
}
