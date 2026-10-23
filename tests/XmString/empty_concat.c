/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Zero-byte segments still carry direction and line-boundary metadata. */
#include <Xm/Xm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *case_name;

static void require(int condition, const char *message)
{
    if (!condition) {
        fprintf(stderr, "%s: %s\n", case_name, message);
        exit(1);
    }
}

/* Read public components, preserving separator placement rather than dropping
 * empty lines. Direction checks also cover zero-byte metadata before text. */
static void verify(XmString string, const char *expected, int text_direction)
{
    XmStringContext context;
    XmStringComponentType type;
    unsigned int length, components = 0;
    XtPointer value;
    XmStringDirection direction = XmSTRING_DIRECTION_UNSET;
    size_t offset = 0, expected_length = strlen(expected);

    require(string != NULL, "construction returned NULL");
    require(XmStringInitContext(&context, string), "initialize component context");
    for (;;) {
        value = NULL;
        type = XmStringGetNextTriple(context, &length, &value);
        if (type == XmSTRING_COMPONENT_END) break;
        require(++components < 256, "component traversal did not terminate");
        if (type == XmSTRING_COMPONENT_DIRECTION) {
            require(value != NULL && length == sizeof direction,
                    "direction component has the correct size");
            direction = *(XmStringDirection *)value;
        } else if (type == XmSTRING_COMPONENT_TEXT ||
                   type == XmSTRING_COMPONENT_LOCALE_TEXT) {
            require(length <= expected_length - offset, "unexpected text bytes");
            if (length > 0) {
                require(value != NULL, "nonempty text has storage");
                require(memcmp(value, expected + offset, length) == 0,
                        "text content or separator position changed");
                if (text_direction >= 0)
                    require(direction == (XmStringDirection)text_direction,
                            "zero-byte segment lost explicit text direction");
            }
            offset += length;
        } else if (type == XmSTRING_COMPONENT_SEPARATOR) {
            require(offset < expected_length && expected[offset] == '\n',
                    "unexpected or displaced separator");
            ++offset;
        }
        XtFree((char *)value);
    }
    XtFree((char *)value);
    XmStringFreeContext(context);
    require(offset == expected_length, "text or trailing separator was lost");
}

static XmString sample(unsigned int index)
{
    switch (index) {
    case 0: return XmStringCreate("", "empty-concat");
    case 1: return XmStringDirectionCreate(XmSTRING_DIRECTION_L_TO_R);
    case 2: return XmStringDirectionCreate(XmSTRING_DIRECTION_R_TO_L);
    case 3: return XmStringSeparatorCreate();
    case 4: return XmStringCreateLtoR("", "empty-concat");
    case 5: return XmStringCreateLtoR("\n", "empty-concat");
    case 6: return XmStringCreateLtoR("a\n\nb\n", "empty-concat");
    case 7: return XmStringCreate("x", "empty-concat");
    default: return XmStringCreateLtoR("\nq", "empty-concat");
    }
}

int main(void)
{
    static const char *const text[] = {"", "", "", "\n", "", "\n",
                                       "a\n\nb\n", "x", "\nq"};
    unsigned int i, j;
    char name[80], expected[64];
    XmString a, b, result;

    for (i = 0; i < XtNumber(text); ++i) {
        for (j = 0; j < XtNumber(text); ++j) {
            snprintf(name, sizeof name, "sample %u + %u", i, j);
            case_name = name;
            snprintf(expected, sizeof expected, "%s%s", text[i], text[j]);
            a = sample(i);
            b = sample(j);
            verify(a, text[i], -1);
            verify(b, text[j], -1);
            result = XmStringConcat(a, b);
            verify(result, expected, -1);
            /* Non-consuming concat must preserve reusable input objects. */
            verify(a, text[i], -1);
            verify(b, text[j], -1);
            XmStringFree(result);
            /* Same values, now exercise the ownership-transfer merge path. */
            result = XmStringConcatAndFree(a, b);
            verify(result, expected, -1);
            XmStringFree(result);
        }
    }

    for (i = 0; i < 2; ++i) {
        XmStringDirection direction = i == 0 ? XmSTRING_DIRECTION_L_TO_R :
                                               XmSTRING_DIRECTION_R_TO_L;
        case_name = i == 0 ? "empty LTR metadata with neighbors" :
                             "empty RTL metadata with neighbors";
        result = XmStringConcatAndFree(XmStringSeparatorCreate(),
                                       XmStringDirectionCreate(direction));
        result = XmStringConcatAndFree(result, XmStringCreate("", "empty-concat"));
        result = XmStringConcatAndFree(result, XmStringCreate("left", "empty-concat"));
        result = XmStringConcatAndFree(result, XmStringDirectionCreate(direction));
        result = XmStringConcatAndFree(result, XmStringCreate("", "empty-concat"));
        result = XmStringConcatAndFree(result, XmStringCreate("right", "empty-concat"));
        verify(result, "\nleftright", direction);
        XmStringFree(result);
    }
    puts("XmString zero-byte concatenation content/direction/separator checks passed");
    return 0;
}
