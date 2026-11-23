/* Verify live scaling of toolkit-owned ScrolledWindow dimensions. */
#include <Xm/Display.h>
#include <Xm/DrawingA.h>
#include <Xm/List.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <stdio.h>

static int
fail(const char *message)
{
    fprintf(stderr, "display-scale scrolled window: %s\n", message);
    return 1;
}

static Dimension
read_width(Widget widget)
{
    Dimension width;
    XtVaGetValues(widget, XmNwidth, &width, NULL);
    return width;
}

static Dimension
read_height(Widget widget)
{
    Dimension height;
    XtVaGetValues(widget, XmNheight, &height, NULL);
    return height;
}

static Dimension
expected_classic_bar(Widget bar, Dimension inner)
{
    Dimension ht = 0, st = 0;
    XtVaGetValues(bar,
                  XmNhighlightThickness, &ht,
                  XmNshadowThickness, &st, NULL);
    return (Dimension)(2 * (ht + st) + inner);
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, ordinary, explicit_shell, explicit, scaled_shell, scaled;
    Widget content, vb = NULL, hb = NULL, list_shell, list, list_sw, list_vb = NULL;
    Display *display;
    Dimension ordinary_width, explicit_width, scaled_width;
    XtWidgetGeometry preferred;
    Arg automatic_args[1];
    Arg list_args[2];
    int i;

    shell = XtVaAppInitialize(&app, "DisplayScaleScrolled", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    XtVaSetValues(shell, XmNwidth, (Dimension)100,
                  XmNheight, (Dimension)100,
                  XmNallowShellResize, True, NULL);
    if (!XmSetDisplayScaleOverride(display, 1.0))
        return fail("initial scale override failed");
    XtSetArg(automatic_args[0], XmNscrollingPolicy, XmAUTOMATIC);
    ordinary = XmCreateScrolledWindow(shell, "ordinary", automatic_args, 1);
    XtManageChild(ordinary);
    XtRealizeWidget(shell);
    ordinary_width = read_width(ordinary);

    explicit_shell = XtVaCreatePopupShell("explicitShell",
                                          topLevelShellWidgetClass, shell,
                                          NULL);
    explicit = XmCreateScrolledWindow(explicit_shell, "explicit",
                                      automatic_args, 1);
    XtVaSetValues(explicit, XmNwidth, (Dimension)140,
                  XmNheight, (Dimension)120, NULL);
    content = XtVaCreateManagedWidget("content", xmDrawingAreaWidgetClass,
                                      explicit,
                                      XmNwidth, (Dimension)400,
                                      XmNheight, (Dimension)400, NULL);
    (void)content;
    XtManageChild(explicit);
    XtRealizeWidget(explicit_shell);
    XtVaGetValues(explicit,
                  XmNverticalScrollBar, &vb,
                  XmNhorizontalScrollBar, &hb, NULL);
    if (vb == NULL || hb == NULL)
        return fail("scrolled window scrollbars were not created");
    if (read_width(vb) != expected_classic_bar(vb, 11) ||
        read_height(hb) != expected_classic_bar(hb, 11))
        return fail("initial classic scrollbar thickness was unexpected");

    list_shell = XtVaCreatePopupShell("listShell",
                                      topLevelShellWidgetClass, shell,
                                      XmNwidth, (Dimension)220,
                                      XmNheight, (Dimension)160, NULL);
    XtSetArg(list_args[0], XmNvisibleItemCount, 4);
    XtSetArg(list_args[1], XmNlistSizePolicy, XmCONSTANT);
    list = XmCreateScrolledList(list_shell, "backdropList", list_args, 2);
    for (i = 0; i < 12; ++i) {
        XmString item = XmStringCreateLocalized("BackdropItemName");
        XmListAddItem(list, item, 0);
        XmStringFree(item);
    }
    XtManageChild(list);
    XtRealizeWidget(list_shell);
    list_sw = XtParent(list);
    XtVaGetValues(list_sw, XmNverticalScrollBar, &list_vb, NULL);
    if (list_vb == NULL ||
        read_width(list_vb) != expected_classic_bar(list_vb, 11))
        return fail("scrolled list classic scrollbar initial width was unexpected");

    explicit_width = read_width(explicit);
    if (ordinary_width != 100 || explicit_width != 140) {
        fprintf(stderr, "initial widths: ordinary=%u explicit=%u\n",
                ordinary_width, explicit_width);
        return fail("initial default or explicit width was unexpected");
    }

    if (!XmSetDisplayScaleOverride(display, 2.0))
        return fail("live scale override failed");
    XSync(display, False);
    ordinary_width = read_width(ordinary);
    explicit_width = read_width(explicit);
    if (ordinary_width != 200 || explicit_width != 140 ||
        read_height(explicit) != 120) {
        fprintf(stderr, "live widths: ordinary=%u explicit=%u\n",
                ordinary_width, explicit_width);
        return fail("live default reflow changed the wrong width");
    }
    if (read_width(vb) != expected_classic_bar(vb, 22) ||
        read_height(hb) != expected_classic_bar(hb, 22) ||
        read_width(list_vb) != expected_classic_bar(list_vb, 22)) {
        fprintf(stderr, "live bars: vb=%u hb=%u list_vb=%u\n",
                read_width(vb), read_height(hb), read_width(list_vb));
        return fail("classic child scrollbars did not scale with display override");
    }

    scaled_shell = XtVaCreatePopupShell("scaledShell",
                                        topLevelShellWidgetClass, shell,
                                        NULL);
    scaled = XmCreateScrolledWindow(scaled_shell, "scaled",
                                    automatic_args, 1);
    XtManageChild(scaled);
    XtRealizeWidget(scaled_shell);
    scaled_width = read_width(scaled);
    XtQueryGeometry(scaled, NULL, &preferred);
    if (!(preferred.request_mode & CWWidth) || preferred.width != 200) {
        fprintf(stderr, "scaled width=%u preferred=%u mode=%lu\n",
                scaled_width, preferred.width,
                (unsigned long)preferred.request_mode);
        return fail("construction did not use the active scale");
    }

    if (!XmSetDisplayScaleOverride(display, 1.0))
        return fail("scale reset failed");
    XSync(display, False);
    if (read_width(ordinary) != 100 || read_width(explicit) != 140 ||
        read_width(vb) != expected_classic_bar(vb, 11) ||
        read_height(hb) != expected_classic_bar(hb, 11) ||
        read_width(list_vb) != expected_classic_bar(list_vb, 11))
        return fail("default reflow was not reversible");
    XtDestroyWidget(list_shell);
    XtDestroyWidget(scaled_shell);
    XtDestroyWidget(explicit_shell);
    XtDestroyWidget(shell);
    puts("PASS WP-05.2 display-scale scrolled window");
    return 0;
}
