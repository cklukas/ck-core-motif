/*
 * Resource converters that split a user string into a list: the tab list unit
 * name buffer and the token reader that the atom list converter uses.  Run by
 * run-resconvert-list-bounds.py with the production lib/Xm sources compiled
 * under AddressSanitizer and UndefinedBehaviorSanitizer.
 *
 *   check-resconvert-list-bounds CASE [TMPDIR]
 *
 * ResConvert.c is included, to reach its static functions.  Every string that
 * a converter reads is held in a heap block of exactly its length, so a read
 * before its start or after its end is an address error.
 */
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/IntrinsicP.h>

#include "ResConvert.c"

/* The toolkit headers redefine assert(), so the checks use their own macro. */
#define CHECK(condition) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #condition); \
            abort(); \
        } \
    } while (0)


extern Display *fuzz_display(void);

static char *exact_string(const char *text)
{
    size_t size = strlen(text) + 1;
    char *copy = malloc(size);

    CHECK(copy != NULL);
    memcpy(copy, text, size);
    return copy;
}

/* Collects up to MAX tokens of SOURCE; returns their number, -1 when there are more. */
static int tokens(const char *source, const char *delimiters, char *out[], int max)
{
    char *text = exact_string(source);
    char *context = NULL;
    int count = 0;
    char *token;

    for (token = GetNextToken(text, (char *) delimiters, &context); token != NULL;
         token = GetNextToken(NULL, (char *) delimiters, &context)) {
        if (count == max) {
            free(token);
            free(text);
            return -1;
        }
        out[count++] = token;
    }
    free(text);
    return count;
}

static void expect_tokens(const char *source, int expected, ...)
{
    char *out[8];
    int count = tokens(source, ",", out, 8);
    va_list list;

    CHECK(count == expected);
    va_start(list, expected);
    for (int i = 0; i < expected; i++) {
        const char *want = va_arg(list, const char *);

        if (strcmp(out[i], want) != 0)
            fprintf(stderr, "token %d of \"%s\" is \"%s\", expected \"%s\"\n", i, source, out[i], want);
        CHECK(strcmp(out[i], want) == 0);
        free(out[i]);
    }
    va_end(list);
}

/* ---- the unit name of a tab ---- */

/* A 12 character unit name does not fit the 12 byte buffer of the converter. */
static void tab_unit_converter(void)
{
    static WidgetClassRec fake_class;
    static WidgetRec fake_widget;
    XrmValue arg = { sizeof(Widget), NULL };
    Widget widget = &fake_widget;
    Cardinal num_args = 1;
    char *text = exact_string("1 abcdefghijkl");
    XrmValue from = { (unsigned) strlen(text) + 1, text };
    _Alignas(16) unsigned char storage[64] = { 0 };
    XrmValue to = { sizeof storage, (XPointer) storage };
    XtPointer converter_data = NULL;

    fake_class.core_class.class_name = "Check";
    fake_widget.core.widget_class = &fake_class;
    fake_widget.core.self = &fake_widget;
    arg.addr = (XPointer) &widget;
    CHECK(!CvtStringToXmTabList(fuzz_display(), &arg, &num_args, &from, &to, &converter_data));
    free(text);
}

static void valid_tab_list(void)
{
    XrmValue arg = { sizeof(Widget), NULL };
    static WidgetClassRec fake_class;
    static WidgetRec fake_widget;
    Widget widget = &fake_widget;
    Cardinal num_args = 1;
    char *text = exact_string("1 inches, +2.5 millimeters, 3 pixels");
    XrmValue from = { (unsigned) strlen(text) + 1, text };
    _Alignas(16) unsigned char storage[64] = { 0 };
    XrmValue to = { sizeof storage, (XPointer) storage };
    XtPointer converter_data = NULL;
    XmTabList list;
    float value;
    unsigned char units;
    XmOffsetModel model;
    unsigned char alignment;
    char *decimal;

    fake_class.core_class.class_name = "Check";
    fake_widget.core.widget_class = &fake_class;
    fake_widget.core.self = &fake_widget;
    arg.addr = (XPointer) &widget;
    CHECK(CvtStringToXmTabList(fuzz_display(), &arg, &num_args, &from, &to, &converter_data));
    list = *(XmTabList *) to.addr;
    CHECK(XmTabListTabCount(list) == 3);
    value = XmTabGetValues(XmTabListGetTab(list, 1), &units, &model, &alignment, &decimal);
    CHECK(value == 2.5f && units == XmMILLIMETERS && model == XmRELATIVE);
    value = XmTabGetValues(XmTabListGetTab(list, 0), &units, &model, &alignment, &decimal);
    CHECK(value == 1.0f && units == XmINCHES && model == XmABSOLUTE);
    XmTabListFree(list);
    free(text);
}

/* ---- the token reader ---- */

static void token_leading_delimiter(void)
{
    expect_tokens(",a", 2, "", "a");
    expect_tokens(",", 1, "");
    expect_tokens(",,b", 3, "", "", "b");
}

static void token_leading_white_space_delimiter(void)
{
    expect_tokens("  ,a", 2, "", "a");
}

static void atom_list_leading_comma(void)
{
    static WidgetClassRec fake_class;
    static WidgetRec fake_widget;
    XrmValue arg = { sizeof(Widget), NULL };
    Widget widget = &fake_widget;
    Cardinal num_args = 1;
    char *text = exact_string(",a");
    XrmValue from = { (unsigned) strlen(text) + 1, text };
    _Alignas(16) unsigned char storage[64] = { 0 };
    XrmValue to = { sizeof storage, (XPointer) storage };
    XtPointer converter_data = NULL;
    Atom *atoms;

    fake_class.core_class.class_name = "Check";
    fake_widget.core.widget_class = &fake_class;
    fake_widget.core.self = &fake_widget;
    arg.addr = (XPointer) &widget;
    CHECK(CvtStringToAtomList(fuzz_display(), &arg, &num_args, &from, &to, &converter_data));
    atoms = *(Atom **) to.addr;
    CHECK(atoms[0] != atoms[1]);        /* the empty name, then "a" */
    XtFree((char *) atoms);
    free(text);
}

/* An escaped character at the end of a token must not be read past its end. */
static void token_escape_at_end(void)
{
    expect_tokens("a\\ ", 1, "a ");
    expect_tokens("\\ ,b", 2, " ", "b");
    expect_tokens("x\\,", 1, "x,");
    expect_tokens("x\\", 1, "x\\");
}

static void token_single_characters(void)
{
    expect_tokens("a", 1, "a");
    expect_tokens("a,b", 2, "a", "b");
    expect_tokens("a,b,c", 3, "a", "b", "c");
}

static void token_white_space(void)
{
    expect_tokens("  x ,y", 2, "x", "y");
    expect_tokens("x, ,y", 3, "x", "", "y");
    expect_tokens("one two, three  ", 2, "one two", "three");
    expect_tokens("   \\ token  token\\ , next token", 2, " token  token ", "next token");
    expect_tokens("", 0);
    expect_tokens("   ", 0);
}

static void token_delimiter_escapes(void)
{
    expect_tokens("a\\,b,c", 2, "a,b", "c");
}

static void valid_token_multiple_characters(void)
{
    expect_tokens("ab,cd,ef", 3, "ab", "cd", "ef");
    expect_tokens("alpha, beta ,gamma", 3, "alpha", "beta", "gamma");
    expect_tokens("one two", 1, "one two");
}

static const struct {
    const char *name;
    void (*run)(void);
} cases[] = {
    { "tab-unit-converter", tab_unit_converter },
    { "token-leading-delimiter", token_leading_delimiter },
    { "token-leading-white-space-delimiter", token_leading_white_space_delimiter },
    { "atom-list-leading-comma", atom_list_leading_comma },
    { "token-escape-at-end", token_escape_at_end },
    { "token-single-characters", token_single_characters },
    { "token-empty-and-white-space", token_white_space },
    { "token-escaped-delimiter", token_delimiter_escapes },
    { "valid-tab-list", valid_tab_list },
    { "valid-token-multiple-characters", valid_token_multiple_characters },
};

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "C");
    if (argc < 2) {
        fprintf(stderr, "usage: %s CASE [TMPDIR]\n", argv[0]);
        return 2;
    }
    for (size_t i = 0; i < sizeof cases / sizeof *cases; i++)
        if (strcmp(argv[1], cases[i].name) == 0) {
            cases[i].run();
            printf("PASS %s\n", cases[i].name);
            return 0;
        }
    fprintf(stderr, "unknown case %s\n", argv[1]);
    return 2;
}
