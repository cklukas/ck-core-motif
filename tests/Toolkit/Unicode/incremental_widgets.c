/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/TextFP.h>
#include <Xm/TextP.h>
#include <Xm/TextStrSoP.h>
#include <Xm/TextUnicode.h>
#include <Xm/TextLayout.h>
#include <Xm/Form.h>
#include <Xm/Display.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <fontconfig/fontconfig.h>

/* Internal entry point used by native Text's grapheme actions. The source and
 * widget records inspected here are installed private ABI, not application API. */
extern XmTextPosition _XmStringSourceGraphemeStep(XmTextSource,
    XmTextPosition, XmTextScanDirection);
static size_t assertions;
static unsigned int refusals;
static const char *stage = "startup";
static void require(int condition, const char *detail)
{
    ++assertions;
    if (!condition) { fprintf(stderr, "incremental widgets: %s: %s\n", stage, detail); exit(1); }
}
static void retired(Widget widget, XtPointer closure, XtPointer call)
{
    (void)widget; (void)call; *(Widget *)closure = NULL;
}
static void refuse(Widget widget, XtPointer closure, XtPointer call)
{
    XmTextVerifyCallbackStruct *verify = (XmTextVerifyCallbackStruct *)call;
    (void)widget; (void)closure;
    ++refusals; verify->doit = False;
}
static void equal(const XmTextUnicodeMap *map, const char *value)
{
    XmTextUnicodeMap *fresh = XmTextUnicodeMapCreate(value, strlen(value));
    size_t i, a, b, bytes, scalars, graphemes;
    int ar, br;
    require(map && fresh, "widget map/fresh oracle missing");
    bytes = XmTextUnicodeByteCount(fresh); scalars = XmTextUnicodeScalarCount(fresh);
    graphemes = XmTextUnicodeGraphemeCount(fresh);
    require(XmTextUnicodeByteCount(map) == bytes && XmTextUnicodeScalarCount(map) == scalars &&
            XmTextUnicodeGraphemeCount(map) == graphemes, "widget map counts differ from full value");
    for (i = 0; i <= bytes; ++i) {
        ar = XmTextUnicodeScalarAtByte(map, i, &a);
        br = XmTextUnicodeScalarAtByte(fresh, i, &b);
        require(ar == br && (!ar || a == b), "widget byte/scalar mapping");
    }
    for (i = 0; i <= scalars; ++i) {
        require(XmTextUnicodeByteAtScalar(map, i, &a) &&
                XmTextUnicodeByteAtScalar(fresh, i, &b) && a == b, "widget scalar/byte mapping");
        require(XmTextUnicodeIsGraphemeBoundary(map, i) ==
                XmTextUnicodeIsGraphemeBoundary(fresh, i), "widget grapheme membership");
        require(XmTextUnicodePreviousGrapheme(map, i, &a) &&
                XmTextUnicodePreviousGrapheme(fresh, i, &b) && a == b, "widget previous grapheme");
        require(XmTextUnicodeNextGrapheme(map, i, &a) &&
                XmTextUnicodeNextGrapheme(fresh, i, &b) && a == b, "widget next grapheme");
    }
    for (i = 0; i <= graphemes; ++i)
        require(XmTextUnicodeScalarAtGrapheme(map, i, &a) &&
                XmTextUnicodeScalarAtGrapheme(fresh, i, &b) && a == b, "widget grapheme/scalar mapping");
    XmTextUnicodeMapFree(fresh);
}
static XmTextUnicodeMap *cached(Widget widget, int field)
{
    return field ? ((XmTextFieldWidget)widget)->text.unicode_map :
        XmTextGetSource(widget)->data->unicode_map;
}
static char *value(Widget widget, int field)
{
    return field ? XmTextFieldGetString(widget) : XmTextGetString(widget);
}
static void warm_field(Widget field)
{
    XEvent event;
    memset(&event, 0, sizeof event);
    event.xkey.type = KeyPress; event.xkey.display = XtDisplay(field);
    event.xkey.window = XtWindow(field); event.xkey.time = CurrentTime;
    XmTextFieldSetInsertionPosition(field, 0);
    XtCallActionProc(field, "forward-character", &event, NULL, 0);
    require(cached(field, 1) != NULL, "native field movement did not warm cache");
    XtCallActionProc(field, "backward-character", &event, NULL, 0);
    require(XmTextFieldGetInsertionPosition(field) == 0, "field cursor round trip");
}
static void movement(Widget widget, int field, XmTextUnicodeMap *identity)
{
    size_t repeat;
    for (repeat = 0; repeat < 12; ++repeat) {
        if (field) warm_field(widget);
        else {
            XmTextSource source = XmTextGetSource(widget);
            XmTextPosition next = _XmStringSourceGraphemeStep(source, 0, XmsdRight);
            require(next > 0 && _XmStringSourceGraphemeStep(source, next, XmsdLeft) == 0,
                    "shared source grapheme round trip");
        }
        require(cached(widget, field) == identity, "unchanged movement rebuilt map");
    }
}
static void accepted(Widget widget, int field, size_t first, size_t last,
                     const char *insert, int bounded)
{
    XmTextUnicodeMap *old = XmTextUnicodeMapRetain(cached(widget, field)), *next;
    XmTextUnicodeMapUsage usage;
    char *before = value(widget, field), *after, *expected;
    size_t begin, end, bytes = strlen(before), added = strlen(insert);
    require(old && XmTextUnicodeByteAtScalar(old, first, &begin) &&
            XmTextUnicodeByteAtScalar(old, last, &end), "accepted edit old map/range");
    expected = malloc(bytes - (end - begin) + added + 1);
    require(expected != NULL, "fixture splice allocation");
    memcpy(expected, before, begin); memcpy(expected + begin, insert, added);
    memcpy(expected + begin + added, before + end, bytes - end + 1);
    if (field) XmTextFieldReplace(widget, first, last, (char *)insert);
    else XmTextReplace(widget, first, last, (char *)insert);
    next = cached(widget, field);
    require(next && next != old, "accepted delta did not replace owner cache");
    after = value(widget, field);
    require(after && strcmp(after, expected) == 0, "accepted widget source differs from requested splice");
    equal(next, after); equal(old, before);
    if (bounded) {
        require(XmTextUnicodeMapGetUsage(next, &usage) && usage.mapped_bytes <= 1024 &&
                usage.mapped_scalars <= 1024 && usage.rebuilt_paragraphs <= 4 &&
                usage.reused_paragraphs >= 8188, "native source edit remapped unrelated paragraphs");
        printf("native delta mapped_bytes=%lu mapped_scalars=%lu rebuilt=%lu reused=%lu\n",
               (unsigned long)usage.mapped_bytes, (unsigned long)usage.mapped_scalars,
               (unsigned long)usage.rebuilt_paragraphs, (unsigned long)usage.reused_paragraphs);
    }
    XmTextUnicodeMapFree(old); movement(widget, field, next);
    XtFree(before); XtFree(after); free(expected);
}
static void rejected(Widget widget, int field)
{
    XmTextUnicodeMap *old = XmTextUnicodeMapRetain(cached(widget, field));
    char *before = value(widget, field), *after;
    unsigned int count = refusals;
    require(old != NULL, "refused edit cache not warmed");
    XtAddCallback(widget, XmNmodifyVerifyCallback, refuse, NULL);
    if (field) XmTextFieldReplace(widget, 0, 1, "Q");
    else XmTextReplace(widget, 0, 1, "Q");
    XtRemoveCallback(widget, XmNmodifyVerifyCallback, refuse, NULL);
    after = value(widget, field);
    require(refusals > count && strcmp(before, after) == 0 && cached(widget, field) == old,
            "refused edit changed source or map identity");
    equal(old, before); XmTextUnicodeMapFree(old); XtFree(before); XtFree(after);
}
static void inspect_proposal_then_refuse(Widget widget, XtPointer closure, XtPointer call)
{
    XmTextVerifyCallbackStruct *verify = (XmTextVerifyCallbackStruct *)call;
    unsigned int *observed = closure;
    XmTextFieldWidget field = (XmTextFieldWidget)widget;
    char *proposed = XmTextFieldGetString(widget);
    Position x, y;
    double caret;
    require(proposed && strcmp(proposed, "e\xcc\x81") == 0,
            "verification did not expose proposed equal-scalar-count value");
    require(verify->startPos == 0 && verify->endPos == 2 &&
            verify->text && verify->text->length == 3 &&
            memcmp(verify->text->ptr, "e\xcc\x81", 3) == 0,
            "rejected verification received incorrect bytes or scalar range");
    XmTextFieldSetInsertionPosition(widget, 1);
    require(XmTextFieldPosToXY(widget, 1, &x, &y), "proposed value caret capture");
    equal(cached(widget, 1), proposed);
    require(!XmTextUnicodeIsGraphemeBoundary(cached(widget, 1), 1) &&
            field->text.unicode_layout &&
            !XmTextLayoutCaret(field->text.unicode_layout, 1, 0, &caret),
            "proposal did not create combining-cluster map and layout");
    ++*observed; verify->doit = False; XtFree(proposed);
}
static void rejected_set_values(Widget widget)
{
    XmTextFieldWidget field = (XmTextFieldWidget)widget;
    XmTextUnicodeMap *old;
    unsigned int observed = 0;
    char *actual;
    Position x0, y0, x1, y1;
    double caret;
    stage = "rejected SetValues proposal capture";
    XmTextFieldSetString(widget, "ab"); warm_field(widget);
    old = XmTextUnicodeMapRetain(cached(widget, 1));
    require(old != NULL, "retained pre-proposal map");
    XtAddCallback(widget, XmNmodifyVerifyCallback, inspect_proposal_then_refuse, &observed);
    XtVaSetValues(widget, XmNvalue, "e\xcc\x81", NULL);
    XtRemoveCallback(widget, XmNmodifyVerifyCallback, inspect_proposal_then_refuse, &observed);
    actual = XmTextFieldGetString(widget);
    require(observed == 1 && actual && strcmp(actual, "ab") == 0,
            "rejected SetValues did not restore original bytes");
    XmTextFieldSetInsertionPosition(widget, 1);
    require(XmTextFieldGetInsertionPosition(widget) == 1 &&
            XmTextUnicodeIsGraphemeBoundary(cached(widget, 1), 1),
            "restored value reused proposed combining boundary map");
    require(XmTextFieldPosToXY(widget, 0, &x0, &y0) &&
            XmTextFieldPosToXY(widget, 1, &x1, &y1) && x1 > x0 && y1 == y0 &&
            field->text.unicode_layout &&
            XmTextLayoutCaret(field->text.unicode_layout, 1, 0, &caret),
            "restored value reused proposed combining layout capture");
    equal(cached(widget, 1), actual); equal(old, "ab");
    XmTextUnicodeMapFree(old); XtFree(actual);
}
typedef struct {
    const char *proposal;
    const char *replacement;
    XmTextPosition old_length;
    unsigned int calls;
    XmTextBlockRec replacement_block;
} ByteProposal;
static void replace_byte_proposal(Widget widget, XtPointer closure, XtPointer call)
{
    ByteProposal *expected = closure;
    XmTextVerifyCallbackStruct *verify = call;
    size_t bytes = strlen(expected->proposal);
    (void)widget;
    require(verify->startPos == 0 && verify->endPos == expected->old_length,
            "accepted byte callback range is not in source scalars");
    require(verify->text && verify->text->format == XmFMT_8_BIT &&
            verify->text->length == (int)bytes &&
            (!bytes || (verify->text->ptr &&
             memcmp(verify->text->ptr, expected->proposal, bytes) == 0)),
            "accepted byte callback received incorrect proposal bytes");
    expected->replacement_block.ptr = (char *)expected->replacement;
    expected->replacement_block.length = (int)strlen(expected->replacement);
    expected->replacement_block.format = XmFMT_8_BIT;
    verify->text = &expected->replacement_block;
    verify->doit = True;
    ++expected->calls;
}
typedef struct {
    const wchar_t *proposal;
    const wchar_t *replacement;
    XmTextPosition old_length;
    unsigned int calls;
    XmTextBlockRecWcs replacement_block;
} WideProposal;
static void replace_wide_proposal(Widget widget, XtPointer closure, XtPointer call)
{
    WideProposal *expected = closure;
    XmTextVerifyCallbackStructWcs *verify = call;
    size_t scalars = wcslen(expected->proposal);
    (void)widget;
    require(verify->startPos == 0 && verify->endPos == expected->old_length,
            "accepted wide callback range is not in source scalars");
    require(verify->text && verify->text->length == (int)scalars &&
            (!scalars || (verify->text->wcsptr &&
             wmemcmp(verify->text->wcsptr, expected->proposal, scalars) == 0)),
            "accepted wide callback received incorrect proposal scalars");
    expected->replacement_block.wcsptr = (wchar_t *)expected->replacement;
    expected->replacement_block.length = (int)wcslen(expected->replacement);
    verify->text = &expected->replacement_block;
    verify->doit = True;
    ++expected->calls;
}
static void check_set_values_result(Widget widget, const char *expected,
                                  XmTextPosition scalars, XmTextPosition caret)
{
    char *actual = XmTextFieldGetString(widget);
    require(actual && strcmp(actual, expected) == 0 &&
            XmTextFieldGetLastPosition(widget) == scalars,
            "accepted SetValues did not install callback replacement bytes/scalars");
    XmTextFieldSetInsertionPosition(widget, caret);
    require(XmTextFieldGetInsertionPosition(widget) == caret,
            "accepted SetValues replacement has incorrect scalar caret");
    equal(cached(widget, 1), expected);
    XtFree(actual);
}
static void accepted_set_values(Widget widget)
{
    static const wchar_t wide_proposal[] = { L'e', 0x301, 0 };
    static const wchar_t wide_replacement[] = { 0x1f469, 0x301, L'K', L'L', 0 };
    ByteProposal bytes = { "e\xcc\x81", "\xf0\x9f\x98\x80\xcc\x81Z", 2, 0, {0} };
    WideProposal wide = { wide_proposal, wide_replacement, 3, 0, {0} };
    ByteProposal deletion = { "discard", "", 4, 0, {0} };
    stage = "accepted SetValues byte callback replacement";
    XmTextFieldSetString(widget, "ab"); warm_field(widget);
    XtAddCallback(widget, XmNmodifyVerifyCallback, replace_byte_proposal, &bytes);
    XtVaSetValues(widget, XmNvalue, bytes.proposal, NULL);
    XtRemoveCallback(widget, XmNmodifyVerifyCallback, replace_byte_proposal, &bytes);
    require(bytes.calls == 1, "accepted byte callback count");
    check_set_values_result(widget, bytes.replacement, 3, 2);
    require(!XmTextUnicodeIsGraphemeBoundary(cached(widget, 1), 1),
            "byte callback replacement lost combining cluster boundary");
    stage = "accepted SetValues wide callback replacement";
    XtAddCallback(widget, XmNmodifyVerifyCallbackWcs, replace_wide_proposal, &wide);
    XtVaSetValues(widget, XmNvalueWcs, wide_proposal, NULL);
    XtRemoveCallback(widget, XmNmodifyVerifyCallbackWcs, replace_wide_proposal, &wide);
    require(wide.calls == 1, "accepted wide callback count");
    check_set_values_result(widget, "\xf0\x9f\x91\xa9\xcc\x81KL", 4, 2);
    require(!XmTextUnicodeIsGraphemeBoundary(cached(widget, 1), 1),
            "wide callback replacement lost combining cluster boundary");
    stage = "accepted SetValues callback deletion";
    XtAddCallback(widget, XmNmodifyVerifyCallback, replace_byte_proposal, &deletion);
    XtVaSetValues(widget, XmNvalue, deletion.proposal, NULL);
    XtRemoveCallback(widget, XmNmodifyVerifyCallback, replace_byte_proposal, &deletion);
    require(deletion.calls == 1, "accepted deletion callback count");
    check_set_values_result(widget, "", 0, 0);
}
typedef struct {
    int inside;
    unsigned int outer_calls;
    unsigned int inner_calls;
} ReentrantProposal;
static void replace_proposal_reentrantly(Widget widget, XtPointer closure, XtPointer call)
{
    ReentrantProposal *state = closure;
    XmTextVerifyCallbackStruct *verify = call;
    if (state->inside) {
        require(verify->text && verify->text->length == 5 &&
                memcmp(verify->text->ptr, "inner", 5) == 0,
                "nested setter callback received incorrect inner bytes");
        ++state->inner_calls;
        verify->doit = True;
        return;
    }
    require(verify->text && verify->text->length == 3 &&
            memcmp(verify->text->ptr, "e\xcc\x81", 3) == 0,
            "reentrant outer callback received incorrect proposal bytes");
    ++state->outer_calls;
    state->inside = 1;
    XmTextFieldSetString(widget, "inner");
    state->inside = 0;
    /* The outer operation may still read its callback input after the setter
     * replaced widget-owned storage; this must remain valid until return. */
    require(verify->text->length == 3 &&
            memcmp(verify->text->ptr, "e\xcc\x81", 3) == 0,
            "nested setter invalidated outer callback input");
    verify->doit = True;
}
static void reentrant_set_values(Widget widget)
{
    ReentrantProposal state = {0, 0, 0};
    stage = "SetValues callback nested source replacement";
    XmTextFieldSetString(widget, "ab"); warm_field(widget);
    XtAddCallback(widget, XmNmodifyVerifyCallback, replace_proposal_reentrantly, &state);
    XtVaSetValues(widget, XmNvalue, "e\xcc\x81", NULL);
    XtRemoveCallback(widget, XmNmodifyVerifyCallback, replace_proposal_reentrantly, &state);
    require(state.outer_calls == 1 && state.inner_calls == 1 && !state.inside,
            "nested setter callback admission/count");
    check_set_values_result(widget, "inner", 5, 3);
}
int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, form, field, first, second, native_display;
    Display *display;
    XmTextSource source;
    XmTextUnicodeMap *retained;
    XmRendition rendition;
    XmRenderTable fonts;
    Arg args[3];
    char *large, *actual;
    const char *initial = "e\xcc\x81 ab \xf0\x9f\x91\xa9\xe2\x80\x8d\xf0\x9f\x91\xa7";
    size_t i;
    require(setlocale(LC_ALL, "C.UTF-8") && XmTextUnicodeAvailable(), "UTF8 Unicode locale unavailable");
    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "IncrementalWidgets", NULL, 0, &argc, argv, NULL, NULL);
    display = XtDisplay(shell); native_display = XmGetXmDisplay(display);
    require(native_display != NULL, "native display owner");
    XtAddCallback(native_display, XmNdestroyCallback, retired, &native_display);
    form = XtVaCreateManagedWidget("container", xmFormWidgetClass, shell,
        XmNwidth, 520, XmNheight, 330, NULL);
    field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, form,
        XmNx, 10, XmNy, 10, XmNwidth, 490, XmNvalue, initial, NULL);
    first = XtVaCreateManagedWidget("first", xmTextWidgetClass, form,
        XmNx, 10, XmNy, 70, XmNwidth, 490, XmNheight, 110,
        XmNeditMode, XmMULTI_LINE_EDIT, XmNwordWrap, False, NULL);
    second = XtVaCreateManagedWidget("second", xmTextWidgetClass, form,
        XmNx, 10, XmNy, 195, XmNwidth, 490, XmNheight, 110,
        XmNeditMode, XmMULTI_LINE_EDIT, XmNwordWrap, False, NULL);
    XtRealizeWidget(shell);
    stage = "TextField owned cache"; warm_field(field);
    retained = XmTextUnicodeMapRetain(cached(field, 1));
    require(retained != NULL, "retained field snapshot");
    movement(field, 1, retained);
    accepted(field, 1, 3, 4, "z\xcc\x81", 0);
    equal(retained, initial); rejected(field, 1);
    XmTextUnicodeMapFree(retained);
    retained = XmTextUnicodeMapRetain(cached(field, 1));
    XtSetArg(args[0], XmNfontType, XmFONT_IS_XFT);
    XtSetArg(args[1], XmNfontName, "DejaVu Sans"); XtSetArg(args[2], XmNfontSize, 18);
    rendition = XmRenditionCreate(field, XmFONTLIST_DEFAULT_TAG, args, 3);
    require(rendition != NULL, "changed font rendition");
    fonts = XmRenderTableAddRenditions(NULL, &rendition, 1, XmMERGE_REPLACE);
    XmRenditionFree(rendition); require(fonts != NULL, "changed font table");
    XtVaSetValues(field, XmNfontList, fonts, XmNmarginWidth, 9, XmNwidth, 475, NULL);
    XmRenderTableFree(fonts);
    require(cached(field, 1) == retained, "font/appearance update discarded source map");
    actual = value(field, 1); equal(retained, actual); XtFree(actual);
    XmTextFieldSetString(field, "new e\xcc\x81 value");
    require(cached(field, 1) != retained, "full SetString retained obsolete map");
    warm_field(field); actual = value(field, 1); equal(cached(field, 1), actual); XtFree(actual);
    XmTextUnicodeMapFree(retained);
    rejected_set_values(field);
    accepted_set_values(field);
    reentrant_set_values(field);

    stage = "shared Text source incremental cache";
    large = malloc(1048576 + 1); require(large != NULL, "large source allocation");
    memset(large, 'a', 1048576);
    for (i = 127; i < 1048576; i += 128) large[i] = '\n';
    large[1048576] = 0;
    XmTextSetString(first, large); source = XmTextGetSource(first);
    XmTextSetSource(second, source, 0, 0);
    require(XmTextGetSource(second) == source && source->data->numwidgets == 2,
            "native widgets did not share source ownership");
    require(_XmStringSourceGraphemeStep(source, 0, XmsdRight) == 1 &&
            source->data->unicode_map != NULL, "native source cache warmup");
    retained = XmTextUnicodeMapRetain(source->data->unicode_map);
    movement(first, 0, retained); movement(second, 0, retained);
    accepted(first, 0, 524305, 524306, "e\xcc\x81", 1);
    require(cached(second, 0) == cached(first, 0), "peer widget missed committed cache");
    actual = XmTextGetString(second); equal(cached(second, 0), actual); XtFree(actual);
    equal(retained, large); rejected(second, 0);
    accepted(second, 0, 127, 128, "", 1); /* Join two existing hard paragraphs. */
    accepted(first, 0, 10, 10, "\r\n", 1); /* Split a paragraph with CRLF. */
    accepted(second, 0, 11, 11, "x", 1); /* Split the CRLF grapheme itself. */
    free(large);
    XtDestroyWidget(shell);
    /* VendorShell teardown may already have retired its independent XmDisplay. */
    if (native_display) XtDestroyWidget(native_display);
    XtCloseDisplay(display); XtDestroyApplicationContext(app);
    /* The retained immutable map is independent of widgets, source and Display. */
    require(XmTextUnicodeByteCount(retained) == 1048576 &&
            XmTextUnicodeScalarCount(retained) == 1048576, "snapshot outlived native source incorrectly");
    XmTextUnicodeMapFree(retained);
    /* Release Fontconfig's process-global configuration only after all native
     * fonts and Display connections are gone, as in the font-lifetime fixture. */
    FcFini();
    printf("incremental widgets assertions=%lu\n", (unsigned long)assertions);
    fflush(stdout);
    return 0;
}
