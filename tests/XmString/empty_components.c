/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Zero-byte public text components retain their type and owned terminator. */
#include <Xm/Xm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static void require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "empty text component: %s\n", message);
        exit(1);
    }
}

static void verify(XmString string, XmStringComponentType expected)
{
    XmStringContext context;
    XmStringComponentType type;
    XtPointer value;
    unsigned int length, count = 0, text_count = 0;

    require(string != NULL, "zero length with NULL value must be accepted");
    require(XmStringInitContext(&context, string), "initialize public context");
    for (;;) {
        value = NULL;
        type = XmStringGetNextTriple(context, &length, &value);
        if (type == XmSTRING_COMPONENT_END) break;
        require(++count < 32, "public component iteration must terminate");
        if (type == XmSTRING_COMPONENT_TEXT ||
            type == XmSTRING_COMPONENT_LOCALE_TEXT ||
            type == XmSTRING_COMPONENT_WIDECHAR_TEXT) {
            require(type == expected, "empty text component type changed");
            require(length == 0, "empty component acquired text bytes");
            require(value != NULL, "copied empty text must have owned storage");
            if (type == XmSTRING_COMPONENT_WIDECHAR_TEXT)
                require(*(wchar_t *)value == L'\0', "wide text lacks terminator");
            else
                require(*(char *)value == '\0', "text lacks terminator");
            ++text_count;
        }
        XtFree((char *)value);
    }
    XtFree((char *)value);
    XmStringFreeContext(context);
    require(text_count == 1, "empty typed component was dropped or duplicated");
}


static unsigned int separator_count(XmString string)
{
    XmStringContext context;
    XmStringComponentType type;
    XtPointer value;
    unsigned int length, separators = 0, components = 0;

    if (string == NULL) return 0;
    require(XmStringInitContext(&context, string), "initialize separator context");
    for (;;) {
        value = NULL;
        type = XmStringGetNextTriple(context, &length, &value);
        if (type == XmSTRING_COMPONENT_END) break;
        require(++components < 64, "separator iteration must terminate");
        if (type == XmSTRING_COMPONENT_SEPARATOR) {
            require(length == 0 && value == NULL, "separator has no payload");
            ++separators;
        }
        XtFree((char *)value);
    }
    XtFree((char *)value);
    XmStringFreeContext(context);
    return separators;
}

static void verify_separator_table(void)
{
    static const char *const expected[] = {"alpha", "", "beta"};
    /* Each portion is terminated, including an empty middle portion. This
     * exercises zero-data matching without assuming an unterminated suffix. */
    XmString string = XmStringCreateLtoR("alpha\n\nbeta\n", "empty-components");
    XmString separator = XmStringComponentCreate(XmSTRING_COMPONENT_SEPARATOR,
                                                 0, NULL);
    XmStringTable table = NULL;
    Cardinal count, i;

    require(string != NULL && separator != NULL, "construct separator table input");
    require(separator_count(separator) == 1, "break component is one separator");
    count = XmStringToXmStringTable(string, separator, NULL);
    require(count == XtNumber(expected), "counts-only zero-data separator match");
    require(XmStringToXmStringTable(string, separator, &table) == count,
            "allocated-table count differs from counts-only result");
    require(table != NULL, "allocated-table storage missing");
    for (i = 0; i < count; ++i) {
        char *text = (char *)XmStringUnparse(table[i], NULL, XmCHARSET_TEXT,
                                            XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
        require(text != NULL && strcmp(text, expected[i]) == 0,
                "split table content or empty portion changed");
        require(separator_count(table[i]) == 0, "break separator retained in table entry");
        XtFree(text);
        XmStringFree(table[i]);
    }
    XtFree((char *)table);
    require(separator_count(string) == count, "table split mutated input separators");
    XmStringFree(separator);
    XmStringFree(string);
}


/* Distinct tags retain an empty segment between nonempty neighbors. A final
 * separator also leaves a zero-byte single entry after the array of segments. */
static XmString comparison_neighbors(const char *right)
{
    XmString string = XmStringCreate("left", "compare-left");
    string = XmStringConcatAndFree(string, XmStringCreate("", "compare-empty"));
    string = XmStringConcatAndFree(string, XmStringCreate((char *)right, "compare-right"));
    return XmStringConcatAndFree(string, XmStringSeparatorCreate());
}

static XmString comparison_empty(XmStringDirection direction, const char *tag)
{
    XmString string = XmStringConcatAndFree(XmStringDirectionCreate(direction),
                                            XmStringCreate("", (char *)tag));
    return XmStringConcatAndFree(string, XmStringSeparatorCreate());
}

static void verify_comparisons(void)
{
    XmString a = comparison_neighbors("right");
    XmString b = comparison_neighbors("right");
    XmString changed = comparison_neighbors("wrong");
    XmString empty = comparison_empty(XmSTRING_DIRECTION_UNSET, "compare-empty");
    XmString equal = comparison_empty(XmSTRING_DIRECTION_UNSET, "compare-empty");
    XmString rtl = comparison_empty(XmSTRING_DIRECTION_R_TO_L, "compare-empty");
    XmString rtl_equal = comparison_empty(XmSTRING_DIRECTION_R_TO_L, "compare-empty");
    XmString tag = comparison_empty(XmSTRING_DIRECTION_UNSET, "compare-other");

    require(XmStringCompare(a, b) && XmStringCompare(b, a),
            "empty segment between equal nonempty neighbors must compare equal");
    require(!XmStringCompare(a, changed) && !XmStringCompare(changed, a),
            "nonempty neighbor comparison must retain byte differences");
    require(XmStringCompare(empty, equal), "independent empty segments must compare equal");
    require(XmStringCompare(rtl, rtl_equal), "equal empty RTL metadata must compare equal");
    require(!XmStringCompare(empty, rtl) && !XmStringCompare(rtl, empty),
            "empty UNSET versus RTL direction checks must remain effective");
    require(!XmStringCompare(empty, tag) && !XmStringCompare(tag, empty),
            "empty segment tag checks must remain effective");
    XmStringFree(tag);
    XmStringFree(rtl_equal);
    XmStringFree(rtl);
    XmStringFree(equal);
    XmStringFree(empty);
    XmStringFree(changed);
    XmStringFree(b);
    XmStringFree(a);
}

int main(void)
{
    static const XmStringComponentType types[] = {
        XmSTRING_COMPONENT_TEXT, XmSTRING_COMPONENT_LOCALE_TEXT,
        XmSTRING_COMPONENT_WIDECHAR_TEXT
    };
    unsigned int i;
    for (i = 0; i < XtNumber(types); ++i) {
        XmString string = XmStringComponentCreate(types[i], 0, NULL);
        XmString copy;
        XtPointer bytes;
        wchar_t *wide;
        require(XmStringComponentCreate(types[i], 1, NULL) == NULL,
                "nonzero length with NULL value must still be rejected");
        verify(string, types[i]);
        copy = XmStringCopy(string);
        verify(copy, types[i]);
        bytes = XmStringUnparse(string, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                               NULL, 0, XmOUTPUT_ALL);
        require(bytes != NULL && *(char *)bytes == '\0', "empty byte unparse");
        XtFree((char *)bytes);
        wide = (wchar_t *)XmStringUnparse(string, NULL, XmCHARSET_TEXT,
                                         XmWIDECHAR_TEXT, NULL, 0, XmOUTPUT_ALL);
        require(wide != NULL && *wide == L'\0', "empty wide unparse");
        XtFree((char *)wide);
        XmStringFree(copy);
        XmStringFree(string);
    }
    verify_separator_table();
    verify_comparisons();
    puts("XmString empty component, separator table and comparison checks passed");
    return 0;
}
