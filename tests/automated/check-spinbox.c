/* SPDX-License-Identifier: LGPL-2.0-or-later
 * Build against an installed Motif and run each case with an X server:
 * cc -Wall -Wextra -Werror check-spinbox.c -I$prefix/include -L$prefix/lib \
 *    -Wl,-rpath,$prefix/lib -lXm -lXt -lX11 -o check-spinbox
 * Cases are separate processes so allocator/sanitizer failures remain visible.
 */
#include <Xm/RowColumn.h>
#include <Xm/SSpinB.h>
#include <Xm/SpinB.h>
#include <Xm/TextF.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failed;
static Boolean timer_finished;
static Boolean crossed_boundary;

static void timer_done(XtPointer closure, XtIntervalId *id)
{
    (void)closure; (void)id;
    timer_finished = True;
}

static void changed(Widget widget, XtPointer closure, XtPointer data)
{
    XmSpinBoxCallbackStruct *change = (XmSpinBoxCallbackStruct *)data;
    (void)widget; (void)closure;
    if (change->reason == XmCR_SPIN_NEXT || change->reason == XmCR_SPIN_PRIOR)
        crossed_boundary = change->crossed_boundary;
}

static void step(Widget spin, const char *action)
{
    XEvent event;
    memset(&event, 0, sizeof(event));
    event.xany.type = KeyRelease;
    event.xany.display = XtDisplay(spin);
    event.xany.window = XtWindow(spin);
    crossed_boundary = False;
    XtCallActionProc(spin, (char *)action, &event, NULL, 0);
    XtCallActionProc(spin, "SpinBDisarm", &event, NULL, 0);
}

static void expect_int(const char *name, int actual, int expected)
{
    if (actual != expected) {
        fprintf(stderr, "%s: got %d, expected %d\n", name, actual, expected);
        failed = 1;
    }
}

static Widget numeric(Widget parent, int minimum, int maximum, int position,
                      int increment, short decimals, Widget *field)
{
    Widget spin = XtVaCreateManagedWidget("numeric", xmSimpleSpinBoxWidgetClass,
        parent, XmNspinBoxChildType, XmNUMERIC, XmNminimumValue, minimum,
        XmNmaximumValue, maximum, XmNposition, position,
        XmNincrementValue, increment, XmNdecimalPoints, decimals, NULL);
    XtVaGetValues(spin, XmNtextField, field, NULL);
    return spin;
}

static void expect_text(Widget field, const char *expected)
{
    char *actual = XmTextFieldGetString(field);
    if (strcmp(actual, expected)) {
        fprintf(stderr, "display: got '%s', expected '%s'\n", actual, expected);
        failed = 1;
    }
    XtFree(actual);
}

static void validate(Widget field, const char *input, int status, int position)
{
    int actual_position = 123;
    int actual_status;
    XmTextFieldSetString(field, (char *)input);
    actual_status = XmSpinBoxValidatePosition(field, &actual_position);
    expect_int(input, actual_status, status);
    expect_int("validated position", actual_position, position);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, row, spin, field;
    int position, count;
    const char *mode = argc > 1 ? argv[1] : "format";

    setlocale(LC_ALL, "");
    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "SpinBoxRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell, NULL);
    XtRealizeWidget(shell);
    if (!strcmp(mode, "format")) {
        spin = numeric(row, INT_MIN, INT_MAX, 16777217, 1, 0, &field);
        expect_text(field, "16777217");
        XtVaSetValues(spin, XmNposition, 99999999, NULL);
        expect_text(field, "99999999");
        XtVaSetValues(spin, XmNposition, INT_MIN, NULL);
        expect_text(field, "-2147483648");
        XtVaSetValues(spin, XmNposition, INT_MAX, NULL);
        expect_text(field, "2147483647");
        XtVaSetValues(spin, XmNposition, -29, XmNdecimalPoints, 2, NULL);
        expect_text(field, "-0.29");
        XtVaSetValues(spin, XmNposition, 1, XmNdecimalPoints, 10, NULL);
        expect_text(field, "0.0000000001");
    } else if (!strcmp(mode, "increment")) {
        int increment = 0;
        spin = numeric(row, 0, 1000000, 0, 70000, 0, &field);
        XtVaGetValues(spin, XmNincrementValue, &increment, NULL);
        expect_int("increment", increment, 70000);
        XtVaSetValues(field, XmNincrementValue, 90000, NULL);
        XtVaGetValues(spin, XmNincrementValue, &increment, NULL);
        expect_int("child increment", increment, 90000);
    } else if (!strcmp(mode, "validate")) {
        spin = numeric(row, INT_MIN, INT_MAX, 17, 1, 0, &field);
        validate(field, "16777217", XmVALID_VALUE, 16777217);
        validate(field, "2147483647", XmVALID_VALUE, INT_MAX);
        validate(field, "-2147483648", XmVALID_VALUE, INT_MIN);
        validate(field, "1e1000", XmMAXIMUM_VALUE, INT_MAX);
        validate(field, "-1e1000", XmMINIMUM_VALUE, INT_MIN);
        validate(field, "nan", XmCURRENT_VALUE, 17);
        validate(field, "", XmCURRENT_VALUE, 17);
        validate(field, "invalid", XmCURRENT_VALUE, 17);
        XtVaSetValues(spin, XmNminimumValue, -1000, XmNmaximumValue, 1000,
                      XmNdecimalPoints, 2, NULL);
        validate(field, "0.29", XmVALID_VALUE, 29);
        validate(field, "-0.29", XmVALID_VALUE, -29);
        validate(field, "1.009", XmVALID_VALUE, 100);
        validate(field, "-1.009", XmVALID_VALUE, -100);
        validate(field, "2.9e-1", XmVALID_VALUE, 29);
        validate(field, "1e-1000", XmVALID_VALUE, 0);
        validate(field, "0x1p-1", XmVALID_VALUE, 50);
        validate(field, "1.00suffix", XmVALID_VALUE, 100);
        XtVaSetValues(spin, XmNminimumValue, -100, XmNmaximumValue, 100,
                      XmNdecimalPoints, 0, XmNincrementValue, 3, NULL);
        validate(field, "0", XmINCREMENT_VALUE, -1);
        XtVaSetValues(spin, XmNminimumValue, INT_MIN, XmNmaximumValue, INT_MAX,
                      XmNincrementValue, 3, NULL);
        validate(field, "2147483646", XmINCREMENT_VALUE, 2147483644);
    } else if (!strcmp(mode, "index")) {
        spin = XtVaCreateManagedWidget("index", xmSimpleSpinBoxWidgetClass, row,
            XmNspinBoxChildType, XmNUMERIC, XmNminimumValue, -100,
            XmNmaximumValue, 100, XmNincrementValue, 3,
            XmNpositionType, XmPOSITION_INDEX, XmNposition, 33, NULL);
        XtVaGetValues(spin, XmNtextField, &field, NULL);
        expect_text(field, "-1");
        validate(field, "0", XmINCREMENT_VALUE, 33);
        XtVaGetValues(spin, XmNposition, &position, NULL);
        expect_int("negative indexed value", position, 33);
        XtVaSetValues(spin, XmNposition, INT_MAX, NULL);
        expect_text(field, "100");
    } else if (!strcmp(mode, "matrix")) {
        FILE *corpus = argc > 2 ? fopen(argv[2], "r") : NULL;
        char input[128];
        int decimals, kind, expected, cases = 0;
        if (!corpus) return 2;
        spin = numeric(row, INT_MIN, INT_MAX, 17, 1, 0, &field);
        while (fscanf(corpus, "%127s %d %d %d", input, &decimals, &kind, &expected) == 4) {
            XtVaSetValues(spin, XmNdecimalPoints, decimals, NULL);
            validate(field, input, kind < 0 ? XmMINIMUM_VALUE :
                                  kind > 0 ? XmMAXIMUM_VALUE : XmVALID_VALUE, expected);
            ++cases;
        }
        fclose(corpus);
        expect_int("decimal oracle case count", cases, 2000);
    } else if (!strcmp(mode, "step")) {
        spin = numeric(row, INT_MIN, INT_MAX, INT_MAX, 1, 0, &field);
        XtAddCallback(spin, XmNvalueChangedCallback, changed, NULL);
        step(spin, "SpinBNext");
        XtVaGetValues(spin, XmNposition, &position, NULL);
        expect_int("increment wraps", position, INT_MIN);
        expect_int("increment boundary callback", crossed_boundary, True);
        step(spin, "SpinBPrior");
        XtVaGetValues(spin, XmNposition, &position, NULL);
        expect_int("decrement wraps", position, INT_MAX);
        expect_int("decrement boundary callback", crossed_boundary, True);
        XtVaSetValues(spin, XmNwrap, False, NULL);
        step(spin, "SpinBNext");
        XtVaGetValues(spin, XmNposition, &position, NULL);
        expect_int("no wrap preserves limit", position, INT_MAX);
    } else if (!strcmp(mode, "timer")) {
        spin = numeric(row, 0, 10, 0, 1, 0, &field);
        XtVaSetValues(spin, XmNinitialDelay, 10, XmNrepeatDelay, 10, NULL);
        XtCallActionProc(spin, "SpinBNext", NULL, NULL, 0);
        XtDestroyWidget(spin);
        XtAppAddTimeOut(app, 60, timer_done, NULL);
        while (!timer_finished) XtAppProcessEvent(app, XtIMTimer);
    } else if (!strcmp(mode, "locale")) {
        if (!setlocale(LC_NUMERIC, "de_DE.UTF-8")) {
            fprintf(stderr, "de_DE.UTF-8 locale required\n");
            return 2;
        }
        spin = numeric(row, -1000, 1000, -29, 1, 2, &field);
        expect_text(field, "-0,29");
        validate(field, "0,29", XmVALID_VALUE, 29);
        validate(field, "-0,29", XmVALID_VALUE, -29);
        validate(field, "2,9e-1", XmVALID_VALUE, 29);
    } else if (!strcmp(mode, "items") || !strcmp(mode, "delete-bound")) {
        XmString values[3];
        XmString inserted;
        values[0] = XmStringCreateLocalized("alpha");
        values[1] = XmStringCreateLocalized("\303\251clair");
        values[2] = XmStringCreateLocalized("\346\235\261\344\272\254");
        spin = XtVaCreateManagedWidget("strings", xmSimpleSpinBoxWidgetClass,
            row, XmNvalues, values, XmNnumValues, 3, XmNposition, 1, NULL);
        XtVaGetValues(spin, XmNtextField, &field, NULL);
        for (count = 0; count < 3; ++count) XmStringFree(values[count]);
        if (!strcmp(mode, "delete-bound")) {
            XmSimpleSpinBoxDeletePos(spin, 4);
            XtVaGetValues(spin, XmNnumValues, &count, NULL);
            expect_int("delete out-of-range uses last item", count, 2);
            expect_text(field, "\303\251clair");
        } else {
            inserted = XmStringCreateLocalized("new");
            XmSimpleSpinBoxAddItem(spin, inserted, 2);
            XmStringFree(inserted);
            XtVaGetValues(spin, XmNposition, &position, NULL);
            expect_int("insert preserves selected item", position, 2);
            expect_text(field, "\303\251clair");
            XmSimpleSpinBoxDeletePos(spin, 2);
            expect_text(field, "\303\251clair");
            XmSimpleSpinBoxDeletePos(spin, 0);
            XmSimpleSpinBoxDeletePos(spin, 0);
            XmSimpleSpinBoxDeletePos(spin, 0);
            XtVaGetValues(spin, XmNnumValues, &count, NULL);
            expect_int("delete final item", count, 0);
            expect_text(field, "");
            inserted = XmStringCreateLocalized("again");
            XmSimpleSpinBoxAddItem(spin, inserted, INT_MIN);
            XmStringFree(inserted);
            expect_text(field, "again");
        }
    } else {
        fprintf(stderr, "Unknown case: %s\n", mode);
        return 2;
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    if (!failed) printf("PASS %s\n", mode);
    return failed ? 1 : 0;
}
