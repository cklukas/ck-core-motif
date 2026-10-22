/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/RowColumn.h>
#include <Xm/SpinB.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <X11/Xresource.h>
#include <stdio.h>
#include <string.h>

/* Existing internal DPI query; no new public API is needed for these fixes. */
extern Boolean _XmGetDpiScale(Display *, double *);
extern Dimension _XmDpiDefaultDimension(Widget, Dimension);

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "DpiRegression", NULL, 0,
                                     &argc, argv, NULL, NULL);
    Display *display = XtDisplay(shell);
    XrmDatabase database = XrmGetDatabase(display);
    Widget row = XtVaCreateManagedWidget("row", xmRowColumnWidgetClass, shell, NULL);
    struct { const char *text; int valid; double scale; } values[] = {
        {"75", 1, 1}, {"150", 1, 2}, {"300", 1, 4}, {" 150 ", 1, 2},
        {"NaN", 0, 0}, {"inf", 0, 0}, {"1e999", 0, 0},
        {"0", 0, 0}, {"-75", 0, 0}, {"96junk", 0, 0}, {"", 0, 0}
    };
    unsigned int i;
    int failed = 0;
    for (i = 0; i < XtNumber(values); ++i) {
        double scale = -1;
        int valid;
        XrmPutStringResource(&database, "Xft.dpi", values[i].text);
        XrmSetDatabase(display, database);
        valid = _XmGetDpiScale(display, &scale);
        if (valid != values[i].valid || (valid && scale != values[i].scale)) {
            fprintf(stderr, "DPI '%s': valid=%d scale=%g\n", values[i].text, valid, scale);
            failed = 1;
        }
    }
    for (i = 1; i <= 4; i *= 2) {
        char dpi[32];
        Widget spin, explicit_spin, toggle, gadget;
        XmString empty = XmStringCreateLocalized("");
        Dimension arrows = 0, explicit_arrows = 0, indicator = 0, gadget_indicator = 0;
        snprintf(dpi, sizeof(dpi), "%u", i * 75);
        XrmPutStringResource(&database, "Xft.dpi", dpi);
        spin = XtVaCreateWidget("spin", xmSpinBoxWidgetClass, row, NULL);
        explicit_spin = XtVaCreateWidget("explicit", xmSpinBoxWidgetClass, row,
                                        XmNarrowSize, 19, NULL);
        toggle = XtVaCreateWidget("toggle", xmToggleButtonWidgetClass, row,
                                   XmNlabelString, empty, NULL);
        gadget = XtVaCreateWidget("gadget", xmToggleButtonGadgetClass, row,
                                   XmNlabelString, empty, NULL);
        XtVaGetValues(spin, XmNarrowSize, &arrows, NULL);
        XtVaGetValues(explicit_spin, XmNarrowSize, &explicit_arrows, NULL);
        XtVaGetValues(toggle, XmNindicatorSize, &indicator, NULL);
        XtVaGetValues(gadget, XmNindicatorSize, &gadget_indicator, NULL);
        if (arrows != 16 * i || explicit_arrows != 19 || indicator != 9 * i ||
            gadget_indicator != indicator) {
            fprintf(stderr, "%s DPI: arrows=%u explicit=%u indicators=%u/%u\n",
                    dpi, arrows, explicit_arrows, indicator, gadget_indicator);
            failed = 1;
        }
        XtDestroyWidget(spin);
        XtDestroyWidget(explicit_spin);
        XtDestroyWidget(toggle);
        XtDestroyWidget(gadget);
        XmStringFree(empty);
    }
    XrmPutStringResource(&database, "Xft.dpi", "1e300");
    if (_XmDpiDefaultDimension(row, 16) != (Dimension)~(Dimension)0) {
        fprintf(stderr, "finite oversized default did not saturate\n");
        failed = 1;
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    if (!failed) puts("PASS DPI parsing, bounded sizes, 1x/2x/4x defaults and explicit dimensions");
    return failed;
}
