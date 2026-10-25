/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* WP-04 audit: observe default control metrics without changing them. */
#include <Xm/Form.h>
#include <Xm/MessageB.h>
#include <Xm/Notebook.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/TextF.h>
#include <Xm/Xm.h>
#include <X11/Xresource.h>

#include <stdio.h>
#include <stdlib.h>

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
    ++checks; \
} while (0)

static unsigned long checks;

static void
get_dimension(Widget widget, const char *resource, Dimension *value)
{
    XtVaGetValues(widget, resource, value, NULL);
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Display *display;
    XrmDatabase database;
    /* Exercise 1x, 2x, 4x and an explicit high-density boundary. */
    const unsigned dpis[] = {75U, 150U, 300U, 600U};
    unsigned dpi_index;

    shell = XtVaAppInitialize(&app, "DpiControlMetrics", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    database = XrmGetDatabase(display);
    for (dpi_index = 0; dpi_index < XtNumber(dpis); ++dpi_index) {
        Widget row, push, explicit_push, text, scroll, explicit_scroll;
        Widget notebook, message;
        Dimension push_highlight = 0, push_shadow = 0, explicit_highlight = 0;
        Dimension text_highlight = 0, text_margin = 0;
        Dimension scroll_width = 0, scroll_height = 0, scroll_highlight = 0;
        Dimension explicit_scroll_width = 0;
        Dimension notebook_frame = 0, notebook_margin = 0, notebook_back = 0;
        Dimension message_margin = 0, message_highlight = 0;
        XmTextPosition selection_start = 0, selection_end = 0;
        char dpi[32];
        XmString label;

        snprintf(dpi, sizeof(dpi), "%u", dpis[dpi_index]);
        XrmPutStringResource(&database, "Xft.dpi", dpi);
        XrmSetDatabase(display, database);
        row = XmCreateRowColumn(shell, "row", NULL, 0);
        push = XmCreatePushButton(row, "push", NULL, 0);
        explicit_push = XmCreatePushButton(row, "explicitPush", NULL, 0);
        text = XmCreateTextField(row, "text", NULL, 0);
        scroll = XmCreateScrollBar(row, "scroll", NULL, 0);
        /* Keep this control outside RowColumn so its explicit width is not
         * intentionally replaced by a parent-owned child geometry policy. */
        explicit_scroll = XmCreateScrollBar(shell, "explicitScroll", NULL, 0);
        notebook = XmCreateNotebook(row, "notebook", NULL, 0);
        label = XmStringCreateLocalized("Metric audit");
        message = XmCreateMessageDialog(row, "message", NULL, 0);
        XtVaSetValues(push, XmNlabelString, label, NULL);
        XtVaSetValues(explicit_push, XmNhighlightThickness, 19, NULL);
        XtVaSetValues(explicit_scroll, XmNwidth, 123, NULL);
        XmStringFree(label);
        XtManageChild(push);
        XtManageChild(explicit_push);
        XtManageChild(text);
        XtManageChild(scroll);
        XtManageChild(explicit_scroll);
        XtManageChild(notebook);
        XtManageChild(message);
        XtManageChild(row);
        XtRealizeWidget(shell);
        XSync(display, False);
        get_dimension(push, XmNhighlightThickness, &push_highlight);
        get_dimension(push, XmNshadowThickness, &push_shadow);
        get_dimension(explicit_push, XmNhighlightThickness, &explicit_highlight);
        get_dimension(text, XmNhighlightThickness, &text_highlight);
        get_dimension(text, XmNmarginWidth, &text_margin);
        get_dimension(scroll, XmNwidth, &scroll_width);
        get_dimension(scroll, XmNheight, &scroll_height);
        get_dimension(scroll, XmNhighlightThickness, &scroll_highlight);
        get_dimension(explicit_scroll, XmNwidth, &explicit_scroll_width);
        get_dimension(notebook, XmNframeShadowThickness, &notebook_frame);
        get_dimension(notebook, XmNinnerMarginWidth, &notebook_margin);
        get_dimension(notebook, XmNbackPageSize, &notebook_back);
        get_dimension(message, XmNmarginWidth, &message_margin);
        get_dimension(message, XmNhighlightThickness, &message_highlight);
        XmTextFieldSetString(text, "cursor-selection");
        XmTextFieldSetSelection(text, 1, 6, CurrentTime);
        XmTextFieldGetSelectionPosition(text, &selection_start, &selection_end);
        fprintf(stderr, "metrics dpi=%s push=%u/%u explicit=%u text=%u/%u scroll=%u/%u/%u explicit-scroll=%u notebook=%u/%u/%u message=%u/%u selection=%ld/%ld\n",
                dpi, (unsigned)push_highlight, (unsigned)push_shadow,
                (unsigned)explicit_highlight, (unsigned)text_highlight,
                (unsigned)text_margin, (unsigned)scroll_width,
                (unsigned)scroll_height, (unsigned)scroll_highlight,
                (unsigned)explicit_scroll_width, (unsigned)notebook_frame,
                (unsigned)notebook_margin, (unsigned)notebook_back,
                (unsigned)message_margin,
                (unsigned)message_highlight, (long)selection_start,
                (long)selection_end);
        CHECK(push_highlight > 0 && push_shadow > 0);
        CHECK(text_highlight > 0 && text_margin > 0);
        CHECK(scroll_width > 0 && scroll_height > 0);
        CHECK(notebook_frame > 0 && notebook_back > 0);
        CHECK(explicit_highlight == 19 && explicit_scroll_width == 123);
        CHECK(selection_start == 1 && selection_end == 6);
        printf("DPI %u PUSH_HIGHLIGHT %u PUSH_SHADOW %u TEXT_HIGHLIGHT %u "
               "TEXT_MARGIN %u SCROLL_WIDTH %u SCROLL_HEIGHT %u "
               "SCROLL_HIGHLIGHT %u NOTEBOOK_FRAME %u NOTEBOOK_MARGIN %u NOTEBOOK_BACK %u "
               "MESSAGE_MARGIN %u MESSAGE_HIGHLIGHT %u SELECTION %ld %ld\n",
               dpis[dpi_index], (unsigned)push_highlight, (unsigned)push_shadow,
               (unsigned)text_highlight, (unsigned)text_margin,
               (unsigned)scroll_width, (unsigned)scroll_height,
               (unsigned)scroll_highlight, (unsigned)notebook_frame,
               (unsigned)notebook_margin, (unsigned)notebook_back,
               (unsigned)message_margin,
               (unsigned)message_highlight, (long)selection_start,
               (long)selection_end);
        XtDestroyWidget(row);
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS DPI control-metric audit and explicit-size preservation");
    return 0;
}
