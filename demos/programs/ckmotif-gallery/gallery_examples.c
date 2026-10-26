/*
 * The source tab is extracted from this file.  Keep the example body real:
 * it is the same public-API construction path used by gallery.c.
 */
#include <Xm/XmAll.h>
#include <Xm/Label.h>
#include <Xm/DrawingA.h>
#include <Xm/DragC.h>
#include <Xm/MenuShell.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/SegmentedControl.h>
#include <Xm/TabStack.h>
#include <Xm/Toolbar.h>
#include <Xm/TreeView.h>
#include <Xm/DataGrid.h>
#include <Xm/Calendar.h>
#include <X11/Xlib.h>

static void free_toolbar_icons(Widget widget, XtPointer client, XtPointer call)
{
    Pixmap *icons = client;
    unsigned int i;
    (void)call;
    for (i = 0; i < 4; ++i) if (icons[i]) XFreePixmap(XtDisplay(widget), icons[i]);
    XtFree((char *)icons);
}

static void gallery_toolbar_icons(Widget widget, Pixmap icons[4])
{
    static const unsigned char open_bits[32] = {
        0x00,0x00,0xfc,0x1f,0x04,0x10,0x04,0x10,
        0x04,0x10,0x04,0x10,0x04,0x10,0xfc,0x1f,
        0x04,0x10,0x04,0x10,0x04,0x10,0x04,0x10,
        0x04,0x10,0x04,0x10,0xfc,0x1f,0x00,0x00
    };
    static const unsigned char save_bits[32] = {
        0xfc,0x1f,0x04,0x10,0x04,0x10,0xfc,0x1f,
        0x84,0x11,0x84,0x11,0x84,0x11,0xfc,0x1f,
        0x04,0x10,0x04,0x10,0x04,0x10,0x04,0x10,
        0x04,0x10,0x04,0x10,0x04,0x10,0xfc,0x1f
    };
    static const unsigned char print_bits[32] = {
        0x00,0x00,0xfc,0x1f,0x04,0x10,0xfc,0x1f,
        0x04,0x10,0x04,0x10,0xfc,0x1f,0x04,0x10,
        0xfc,0x1f,0x04,0x10,0x04,0x10,0xfc,0x1f,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    };
    static const unsigned char undo_bits[32] = {
        0x00,0x00,0x10,0x10,0x30,0x18,0x7c,0x1e,
        0xfc,0x1f,0x7c,0x1e,0x30,0x18,0x10,0x10,
        0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
        0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10
    };
    Display *display = XtDisplay(widget);
    Window root = RootWindowOfScreen(XtScreen(widget));
    Pixel foreground, background;
    XtVaGetValues(widget, XmNforeground, &foreground, XmNbackground, &background, NULL);
    const unsigned char *bits[4] = {open_bits, save_bits, print_bits, undo_bits};
    Cardinal i;
    for (i = 0; i < 4; ++i)
        icons[i] = XCreatePixmapFromBitmapData(display, root, (char *)bits[i],
                                               16, 16, foreground, background,
                                               DefaultDepthOfScreen(XtScreen(widget)));
}

static void show_popup_example(Widget widget, XtPointer client, XtPointer call)
{
    Widget example = client;
    (void)widget; (void)call;
    if (XmIsFlyout(example)) XmFlyoutShow(example);
    else if (XmIsTeachingTip(example)) XmTeachingTipShow(example);
}

static Widget sample_label(Widget parent, const char *name, const char *caption)
{
    XmString value = XmStringCreateLocalized((char *)caption);
    Widget label = XtVaCreateManagedWidget(name, xmLabelWidgetClass, parent,
        XmNlabelString, value, XmNalignment, XmALIGNMENT_BEGINNING, NULL);
    XmStringFree(value);
    return label;
}

static void draw_activity(Widget widget, XtPointer client, XtPointer call)
{
    static const unsigned int heights[] = {40, 72, 54, 96, 82};
    XGCValues values;
    GC gc;
    unsigned int i;
    (void)client; (void)call;
    XtVaGetValues(widget, XmNforeground, &values.foreground, NULL);
    gc = XCreateGC(XtDisplay(widget), XtWindow(widget), GCForeground, &values);
    for (i = 0; i < XtNumber(heights); ++i)
        XFillRectangle(XtDisplay(widget), XtWindow(widget), gc,
                       30 + i * 64, 150 - heights[i], 32, heights[i]);
    XFreeGC(XtDisplay(widget), gc);
}

typedef struct {
    XmMultiListRowInfo rows[3];
    XmString values[3][3];
} GalleryListData;

static void free_list_data(Widget widget, XtPointer client, XtPointer call)
{
    GalleryListData *data = client;
    unsigned int row, column;
    (void)widget; (void)call;
    for (row = 0; row < 3; ++row)
        for (column = 0; column < 3; ++column) XmStringFree(data->values[row][column]);
    XtFree((char *)data);
}

/* GALLERY-EXAMPLE-BEGIN: shared-public-construction */
static void
gallery_attach_public_callback(Widget example, XtCallbackProc callback,
                               XtPointer client_data)
{
    static const char *names[] = {
        XmNactivateCallback, XmNvalueChangedCallback, XmNselectionCallback,
        XmNtreeViewSelectionCallback, "dataGridSelectionCallback",
        XmNexpandedCallback, XmNtoolbarActivateCallback,
        "commandPaletteInvokeCallback", "rowActionsInvokeCallback",
        XmNtoastActionCallback, XmNnumberValueChangedCallback,
        XmNstepperValueChangedCallback, XmNcalendarDateChangedCallback,
        XmNdatePickerDateChangedCallback, XmNtimePickerValueChangedCallback,
        XmNratingValueChangedCallback, XmNcolorPickerValueChangedCallback,
        XmNbreadcrumbActivateCallback, XmNautoSuggestCommittedCallback,
        XmNsearchQueryChangedCallback, XmNchipRemoveCallback,
        XmNinfoActionCallback, XmNdefaultActionCallback, XmNhelpCallback
    };
    unsigned int i;
    for (i = 0; i < XtNumber(names); ++i)
        if (XtHasCallbacks(example, names[i]) != XtCallbackNoList) {
            XtAddCallback(example, names[i], callback, client_data);
            return;
        }
}

Widget
gallery_create_component_example(Widget parent, WidgetClass class,
                                 const char *name, XtCallbackProc callback,
                                 XtPointer client_data)
{
    XmString label = NULL;
    Widget example;

    /* Compound controls need a small, useful model to render anything.  Keep
     * these values in the same public construction function used by the live
     * gallery; a bare class instantiation is not a meaningful example. */
    if (class == xmChipWidgetClass) {
        label = XmStringCreateLocalized((char *)"Filter: active");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNchipLabel, label,
                                          XmNchipRemovable, True, XmNwidth, 160, XmNheight, 36, NULL);
    } else if (class == xmBadgeWidgetClass) {
        label = XmStringCreateLocalized((char *)"Notifications");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNbadgeLabel, label,
                                          XmNbadgeCount, 3, XmNwidth, 36, XmNheight, 28, NULL);
    } else if (class == xmIconLabelWidgetClass) {
        label = XmStringCreateLocalized((char *)"Recent document");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNiconLabelLabel, label, NULL);
    } else if (class == xmDividerWidgetClass) {
        label = XmStringCreateLocalized((char *)"Presentation");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNdividerLabel, label,
                                          XmNwidth, 360, XmNheight, 40, NULL);
    } else if (class == xmAvatarWidgetClass) {
        label = XmStringCreateLocalized((char *)"CK");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNavatarInitials, label, XmNwidth, 48, XmNheight, 48, NULL);
    } else if (class == xmLinkWidgetClass) {
        label = XmStringCreateLocalized((char *)"Open documentation");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNlinkLabel, label,
                                          XmNlinkTarget, (char *)"docs", NULL);
    } else if (class == xmPromptWidgetClass) {
        label = XmStringCreateLocalized((char *)"Choose a component to inspect");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNpromptText, label, NULL);
    } else if (class == xmEmptyStateWidgetClass) {
        label = XmStringCreateLocalized((char *)"Nothing selected yet");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNemptyStateMessage, label, NULL);
    } else if (class == xmToggleSwitchWidgetClass) {
        XmString switch_label = XmStringCreateLocalized((char *)"Off");
        XmString title_label = XmStringCreateLocalized((char *)"Kitchen lights");

        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNlabelString, title_label,
                                          XmNset, False,
                                          XmNwidth, 220, XmNheight, 44, NULL);
        XmStringFree(title_label);
        XmStringFree(switch_label);
    } else if (class == xmToggleButtonWidgetClass ||
               class == xmToggleButtonGadgetClass) {
        /* Keep the familiar check-box indicator visible. */
        label = XmStringCreateLocalized((char *)"Enable notifications");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNlabelString, label,
                                          XmNindicatorOn, True,
                                          XmNwidth, 220, XmNheight, 44, NULL);
    } else if (class == xmCascadeButtonWidgetClass || class == xmCascadeButtonGadgetClass) {
        Widget menu = XmCreatePulldownMenu(parent, "fileMenu", NULL, 0);
        const char *actions[] = {"Open document", "Save changes", "Close document"};
        unsigned int i;
        for (i = 0; i < XtNumber(actions); ++i) {
            XmString value = XmStringCreateLocalized((char *)actions[i]);
            Widget item = XtVaCreateManagedWidget("action", xmPushButtonWidgetClass,
                menu, XmNlabelString, value, NULL);
            XmStringFree(value);
            if (callback) XtAddCallback(item, XmNactivateCallback, callback, client_data);
        }
        label = XmStringCreateLocalized((char *)"File");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNlabelString, label, XmNsubMenuId, menu, NULL);
    } else if (class == xmPushButtonWidgetClass ||
               class == xmPushButtonGadgetClass ||
               class == xmLabelWidgetClass ||
               class == xmLabelGadgetClass) {
        label = XmStringCreateLocalized((char *)"Apply changes");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNlabelString, label,
                                          XmNwidth, 220, XmNheight, 44, NULL);
    } else if (class == xmArrowButtonWidgetClass ||
               class == xmArrowButtonGadgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNarrowDirection, XmARROW_RIGHT,
                                          XmNwidth, 56, XmNheight, 44, NULL);
    } else if (class == xmTextWidgetClass || class == xmTextFieldWidgetClass ||
               class == xmDataFieldWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNvalue, (char *)"Editable sample text",
                                          XmNcolumns, 28, XmNwidth, 360,
                                          XmNheight, 44, NULL);
    } else if (class == xmListWidgetClass) {
        XmString rows[3];
        rows[0] = XmStringCreateLocalized((char *)"First item");
        rows[1] = XmStringCreateLocalized((char *)"Second item");
        rows[2] = XmStringCreateLocalized((char *)"Third item");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNitems, rows, XmNitemCount, 3,
                                          XmNwidth, 300, XmNheight, 110, NULL);
        XmStringFree(rows[0]); XmStringFree(rows[1]); XmStringFree(rows[2]);
    } else if (class == xmComboBoxWidgetClass) {
        XmString rows[3];
        rows[0] = XmStringCreateLocalized((char *)"Alpha");
        rows[1] = XmStringCreateLocalized((char *)"Beta");
        rows[2] = XmStringCreateLocalized((char *)"Gamma");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNitems, rows, XmNitemCount, 3,
                                          XmNwidth, 300, NULL);
        XmStringFree(rows[0]); XmStringFree(rows[1]); XmStringFree(rows[2]);
    } else if (class == xmScaleWidgetClass || class == xmScrollBarWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNorientation, XmHORIZONTAL, XmNminimum, 0, XmNmaximum, 100,
                                          XmNvalue, 65, XmNwidth, 320,
                                          XmNheight, 48, NULL);
    } else if (class == xmNumberBoxWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 220, XmNheight, 48, NULL);
        XmNumberBoxSetRange(example, 0, 100, False);
        XmNumberBoxSetValue(example, 42, False);
    } else if (class == xmRatingControlWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 180, XmNheight, 36, NULL);
        XmRatingControlSetValue(example, 4, False);
    } else if (class == xmProgressBarWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, XmNheight, 40, NULL);
        XmProgressBarSetRange(example, 0, 100, False);
        XmProgressBarSetValue(example, 68, False);
    } else if (class == xmProgressRingWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 96, XmNheight, 96, NULL);
        XmProgressRingSetRange(example, 0, 100, False);
        XmProgressRingSetValue(example, 68, False);
    } else if (class == xmSpinnerWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 72, XmNheight, 72, NULL);
        XmSpinnerSetActive(example, True, False);
    } else if (class == xmStepperWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 180, XmNheight, 48, NULL);
        XmStepperSetRange(example, 0, 100, False);
        XmStepperSetValue(example, 7, False);
    } else if (class == xmColorPickerWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 180, XmNheight, 48, NULL);
        XmColorPickerSetValue(example, 0x3478d4ffUL, False);
    } else if (class == xmBreadcrumbBarWidgetClass) {
        XmBreadcrumbSegment segments[3] = {
            {"Home", (XtPointer)"home"},
            {"Components", (XtPointer)"components"},
            {"Current", (XtPointer)"current"}
        };
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 420, XmNheight, 44, NULL);
        XmBreadcrumbBarSetSegments(example, segments, 3);
        XmBreadcrumbBarSetCurrentIndex(example, 2, False);
    } else if (class == xmAutoSuggestBoxWidgetClass) {
        XmString suggestions_text[3];
        XmAutoSuggestItem suggestions[3];
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, XmNheight, 48, NULL);
        suggestions_text[0] = XmStringCreateLocalized((char *)"Open document");
        suggestions_text[1] = XmStringCreateLocalized((char *)"Open folder");
        suggestions_text[2] = XmStringCreateLocalized((char *)"Open recent");
        suggestions[0] = (XmAutoSuggestItem){"document", suggestions_text[0], NULL};
        suggestions[1] = (XmAutoSuggestItem){"folder", suggestions_text[1], NULL};
        suggestions[2] = (XmAutoSuggestItem){"recent", suggestions_text[2], NULL};
        XmAutoSuggestBoxSetSuggestions(example, suggestions, 3, 1);
        XmStringFree(suggestions_text[0]); XmStringFree(suggestions_text[1]);
        XmStringFree(suggestions_text[2]);
    } else if (class == xmInfoBarWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNinfoMessage, (char *)"Changes saved",
                                          XmNinfoActionLabel, (char *)"Undo",
                                          XmNwidth, 420, XmNheight, 48, NULL);
    } else if (class == xmToastWidgetClass) {
        label = XmStringCreateLocalized((char *)"Document saved successfully");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, XmNheight, 64, XmNtoastMessage, label,
                                          XmNtoastTimeout, 0, NULL);
        XmToastShow(example);
    } else if (class == xmSegmentedControlWidgetClass) {
        /* This branch is retained for callers outside the gallery's
         * specialized constructor.  It still renders real segments. */
        XmString segments[3];
        XmSegmentedControlItem items[3];
        segments[0] = XmStringCreateLocalized((char *)"Overview");
        segments[1] = XmStringCreateLocalized((char *)"Details");
        segments[2] = XmStringCreateLocalized((char *)"Activity");
        items[0] = (XmSegmentedControlItem){"overview", segments[0], XmUNSPECIFIED_PIXMAP, True};
        items[1] = (XmSegmentedControlItem){"details", segments[1], XmUNSPECIFIED_PIXMAP, True};
        items[2] = (XmSegmentedControlItem){"activity", segments[2], XmUNSPECIFIED_PIXMAP, True};
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, XmNheight, 42, NULL);
        (void)XmSegmentedControlSetItems(example, items, 3);
        XmSegmentedControlSetSelectedIndex(example, 0, False);
        XmStringFree(segments[0]); XmStringFree(segments[1]); XmStringFree(segments[2]);
    } else if (class == xmTabStackWidgetClass) {
        /* TabStack is a manager: an empty instance is only a frame.  Build
         * three real tab pages so the gallery shows the widget's defining
         * interaction rather than metadata or a placeholder label. */
        XmString tab_labels[3];
        XmString page_text;
        Widget page;
        tab_labels[0] = XmStringCreateLocalized((char *)"Overview");
        tab_labels[1] = XmStringCreateLocalized((char *)"Details");
        tab_labels[2] = XmStringCreateLocalized((char *)"Activity");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 420, XmNheight, 180, NULL);
        page_text = XmStringCreateLocalized((char *)"Overview content");
        page = XtVaCreateManagedWidget("overviewPage", xmLabelWidgetClass,
                                      example, XmNtabLabelString, tab_labels[0],
                                      XmNlabelString, page_text, NULL);
        XmStringFree(page_text);
        (void)page;
        page_text = XmStringCreateLocalized((char *)"Details content");
        page = XtVaCreateManagedWidget("detailsPage", xmLabelWidgetClass,
                                      example, XmNtabLabelString, tab_labels[1],
                                      XmNlabelString, page_text, NULL);
        XmStringFree(page_text);
        (void)page;
        page_text = XmStringCreateLocalized((char *)"Activity content");
        page = XtVaCreateManagedWidget("activityPage", xmLabelWidgetClass,
                                      example, XmNtabLabelString, tab_labels[2],
                                      XmNlabelString, page_text, NULL);
        XmStringFree(page_text);
        (void)page;
        XmStringFree(tab_labels[0]); XmStringFree(tab_labels[1]);
        XmStringFree(tab_labels[2]);
    } else if (class == xmToolbarWidgetClass) {
        XmToolbarAction actions[4];
        XmString labels[4];
        XmString shortcuts[4];
        labels[0] = XmStringCreateLocalized((char *)"Open");
        labels[1] = XmStringCreateLocalized((char *)"Save");
        labels[2] = XmStringCreateLocalized((char *)"Print");
        labels[3] = XmStringCreateLocalized((char *)"Undo");
        shortcuts[0] = XmStringCreateLocalized((char *)"Ctrl+O");
        shortcuts[1] = XmStringCreateLocalized((char *)"Ctrl+S");
        shortcuts[2] = XmStringCreateLocalized((char *)"Ctrl+P");
        shortcuts[3] = XmStringCreateLocalized((char *)"Ctrl+Z");
        actions[0] = (XmToolbarAction){"open", labels[0], shortcuts[0], False, 0, NULL};
        actions[1] = (XmToolbarAction){"save", labels[1], shortcuts[1], False, 0, NULL};
        actions[2] = (XmToolbarAction){"print", labels[2], shortcuts[2], True, 5, NULL};
        actions[3] = (XmToolbarAction){"undo", labels[3], shortcuts[3], True, 4, NULL};
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 420, XmNheight, 48, NULL);
        XmToolbarSetActions(example, actions, 4);
        {
            Pixmap *icons = (Pixmap *)XtCalloc(4, sizeof(Pixmap));
            gallery_toolbar_icons(example, icons);
            XmToolbarSetActionIcons(example, icons, 4);
            XtAddCallback(example, XmNdestroyCallback, free_toolbar_icons, icons);
        }
        if (callback)
            XtAddCallback(example, XmNtoolbarActivateCallback, callback, client_data);
        XmStringFree(labels[0]); XmStringFree(labels[1]);
        XmStringFree(labels[2]); XmStringFree(labels[3]);
        XmStringFree(shortcuts[0]); XmStringFree(shortcuts[1]);
        XmStringFree(shortcuts[2]); XmStringFree(shortcuts[3]);
    } else if (class == xmTreeViewWidgetClass) {
        static const XmTreeViewNode nodes[] = {
            {(XtPointer)"root", NULL, "Project", True, True},
            {(XtPointer)"src", (XtPointer)"root", "Source", True, True},
            {(XtPointer)"gallery", (XtPointer)"src", "Gallery", False, False},
            {(XtPointer)"docs", (XtPointer)"root", "Documentation", False, False}
        };
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, XmNheight, 140,
                                          XmNtreeViewVisibleRows, 5, NULL);
        XmTreeViewSetNodes(example, nodes, XtNumber(nodes));
        XmTreeViewSetSelectedId(example, (XtPointer)"gallery", False);
    } else if (class == xmDataGridWidgetClass) {
        static const char *const values0[] = {"Button", "Enabled", "Ready"};
        static const char *const values1[] = {"Slider", "Enabled", "65%"};
        static const char *const values2[] = {"Dialog", "Preview", "Open"};
        static const XmDataGridColumn columns[] = {
            {(XtPointer)"component", "Component", False, 0},
            {(XtPointer)"state", "State", False, 0},
            {(XtPointer)"value", "Value", False, 0}
        };
        static const XmDataGridRow rows[] = {
            {(XtPointer)"button", values0},
            {(XtPointer)"slider", values1},
            {(XtPointer)"dialog", values2}
        };
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 420, XmNheight, 140,
                                          XmNdataGridVisibleRows, 4, NULL);
        XmDataGridSetModel(example, columns, XtNumber(columns), rows,
                           XtNumber(rows));
        XmDataGridSetSelectedCell(example, (XtPointer)"slider",
                                  (XtPointer)"value", False);
    } else if (class == xmCalendarWidgetClass) {
        XmCalendarDate date = {2026, 10, 25};
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 300, XmNheight, 220, NULL);
        XmCalendarSetDate(example, &date, False);
    } else if (class == xmMultiListWidgetClass) {
        GalleryListData *data = (GalleryListData *)XtCalloc(1, sizeof(*data));
        const char *columns[] = {"Document", "Status", "Pages"};
        const char *values[3][3] = {{"Quarterly report", "Saved", "12"},
            {"Meeting notes", "Draft", "3"}, {"Project brief", "Reviewed", "8"}};
        XmString titles[3];
        unsigned int row, column;
        if (!data) return NULL;
        for (column = 0; column < 3; ++column) titles[column] = XmStringCreateLocalized((char *)columns[column]);
        for (row = 0; row < 3; ++row) {
            data->rows[row].values = data->values[row];
            for (column = 0; column < 3; ++column)
                data->values[row][column] = XmStringCreateLocalized((char *)values[row][column]);
        }
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 540, XmNheight, 240, XmNnumRows, 3, XmNnumColumns, 3,
            XmNentryData, data->rows, XmNcolumnTitles, titles,
            XmNfirstColumnPixmaps, False, NULL);
        XtAddCallback(example, XmNdestroyCallback, free_list_data, data);
        for (column = 0; column < 3; ++column) XmStringFree(titles[column]);
    } else if (class == xmDropDownWidgetClass) {
        XmString values[3];
        values[0] = XmStringCreateLocalized((char *)"Draft");
        values[1] = XmStringCreateLocalized((char *)"Reviewed");
        values[2] = XmStringCreateLocalized((char *)"Published");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 320, XmNheight, 44, NULL);
        XmListAddItems(XmDropDownGetList(example), values, 3, 1);
        XmTextFieldSetString(XmDropDownGetText(example), "Draft");
        label = XmStringCreateLocalized((char *)"Status");
        XtVaSetValues(XmDropDownGetLabel(example), XmNlabelString, label, NULL);
        XmStringFree(values[0]); XmStringFree(values[1]); XmStringFree(values[2]);
    } else if (class == xmTabBoxWidgetClass) {
        XmTabbedStackList tabs = XmTabbedStackListCreate();
        const char *names[] = {"Overview", "Details", "Activity"};
        unsigned int i;
        for (i = 0; i < 3; ++i) {
            XmString value = XmStringCreateLocalized((char *)names[i]);
            XmTabbedStackListSimpleAppend(tabs, value);
            XmStringFree(value);
        }

        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNtabList, tabs, XmNwidth, 480, XmNheight, 48, NULL);
        XmTabbedStackListFree(tabs);
    } else if (class == xmOutlineWidgetClass || class == xmTreeWidgetClass) {
        Widget root;
        XmString caption = XmStringCreateLocalized((char *)"Documents");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 480, XmNheight, 220, NULL);
        root = XtVaCreateManagedWidget("documents", xmPushButtonWidgetClass, example,
            XmNlabelString, caption, XmNnodeState, XmAlwaysOpen, NULL);
        XmStringFree(caption);
        caption = XmStringCreateLocalized((char *)"Quarterly report");
        XtVaCreateManagedWidget("report", xmPushButtonWidgetClass, example,
            XmNlabelString, caption, XmNparentNode, root, XmNnodeState, XmAlwaysOpen, NULL);
        XmStringFree(caption);
        caption = XmStringCreateLocalized((char *)"Meeting notes");
        XtVaCreateManagedWidget("notes", xmPushButtonWidgetClass, example,
            XmNlabelString, caption, XmNparentNode, root, XmNnodeState, XmAlwaysOpen, NULL);
        XmStringFree(caption);
    } else if (class == xmIconBoxWidgetClass) {
        const char *titles[] = {"Documents", "Pictures", "Archive"};
        Pixmap *icons = (Pixmap *)XtCalloc(4, sizeof(Pixmap));
        unsigned int i;
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 480, XmNheight, 200, NULL);
        gallery_toolbar_icons(example, icons);
        XtAddCallback(example, XmNdestroyCallback, free_toolbar_icons, icons);
        for (i = 0; i < 3; ++i) {
            XmString caption = XmStringCreateLocalized((char *)titles[i]);
            XtVaCreateManagedWidget(titles[i], xmIconButtonWidgetClass, example,
                XmNlabelString, caption, XmNpixmap, icons[i],
                XmNcellX, i, XmNcellY, 0, XmNwidth, 140, XmNheight, 72, NULL);
            XmStringFree(caption);
        }
    } else if (class == xmIconButtonWidgetClass || class == xmIconGadgetClass ||
               class == xmIconHeaderClass || class == xmDrawnButtonWidgetClass) {
        label = XmStringCreateLocalized((char *)"Open document");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNlabelString, label, XmNwidth, 180, XmNheight, 48, NULL);
    } else if (class == xmDatePickerWidgetClass) {
        XmDatePickerDate date = {2026, 10, 25};
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 260, XmNheight, 44, NULL);
        XmDatePickerSetDate(example, &date, False);
    } else if (class == xmTimePickerWidgetClass) {
        XmTimePickerValue value = {14, 30, 0};
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 260, XmNheight, 44, NULL);
        XmTimePickerSetValue(example, &value, False);
    } else if (class == xmStatusBarWidgetClass) {
        XmStatusBarItem item = {0};
        label = XmStringCreateLocalized((char *)"All changes saved");
        item.id = "save"; item.label = label; item.kind = XmSTATUS_BAR_ITEM_TEXT;
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 480, XmNheight, 40, NULL);
        XmStatusBarSetItems(example, &item, 1);
    } else if (class == xmDisclosureWidgetClass || class == xmSidebarWidgetClass) {
        Widget content;
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 360, XmNheight, 180, NULL);
        if (class == xmDisclosureWidgetClass) {
            label = XmStringCreateLocalized((char *)"Document options");
            XtVaSetValues(example, XmNlabelString, label, NULL);
        }
        content = class == xmDisclosureWidgetClass ? XmDisclosureGetContent(example) : XmSidebarGetContent(example);
        sample_label(content, "sectionTitle", class == xmDisclosureWidgetClass ?
            "Backup settings" : "Document options");
        sample_label(content, "explanation", "Expand or collapse\nthis group of settings.");
        XmString caption = XmStringCreateLocalized((char *)"Keep a backup copy");
        XtVaCreateManagedWidget("backup", xmToggleButtonWidgetClass, content,
            XmNlabelString, caption, XmNset, True, NULL);
        XmStringFree(caption);
        if (class == xmDisclosureWidgetClass) XmDisclosureSetExpanded(example, True, False);
    } else if (class == xmCommandPaletteWidgetClass || class == xmRowActionsWidgetClass) {
        XmString captions[3], shortcuts[3];
        const char *names[] = {"Open document", "Save changes", "Export PDF"};
        const char *keys[] = {"Ctrl+O", "Ctrl+S", "Ctrl+E"};
        XmCommandPaletteItem commands[3];
        XmRowActionItem actions[3];
        unsigned int i;
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 480, XmNheight, class == xmCommandPaletteWidgetClass ? 240 : 48, NULL);
        for (i = 0; i < 3; ++i) {
            captions[i] = XmStringCreateLocalized((char *)names[i]);
            shortcuts[i] = XmStringCreateLocalized((char *)keys[i]);
            commands[i] = (XmCommandPaletteItem){names[i], captions[i], shortcuts[i], NULL};
            actions[i] = (XmRowActionItem){names[i], captions[i], shortcuts[i], NULL};
        }
        if (class == xmCommandPaletteWidgetClass) {
            XmCommandPaletteSetItems(example, commands, 3); XmCommandPaletteShow(example);
        } else {
            XmRowActionsSetItems(example, actions, 3); XmRowActionsReveal(example, True);
        }
        for (i = 0; i < 3; ++i) { XmStringFree(captions[i]); XmStringFree(shortcuts[i]); }
    } else if (class == xmFlyoutWidgetClass || class == xmTeachingTipWidgetClass) {
        XmString caption = XmStringCreateLocalized((char *)(class == xmFlyoutWidgetClass ? "Show details" : "Show keyboard tip"));
        Widget trigger = XtVaCreateManagedWidget("showPopup", xmPushButtonWidgetClass, parent,
            XmNlabelString, caption, NULL);
        XmStringFree(caption);
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNy, 60, XmNwidth, 280, XmNheight, 40, NULL);
        if (class == xmFlyoutWidgetClass)
            XtVaSetValues(example, XmNflyoutText, "Document details: saved locally and ready to export.", NULL);
        if (class == xmTeachingTipWidgetClass) {
            XmString title = XmStringCreateLocalized((char *)"Move with the keyboard");
            XmString body = XmStringCreateLocalized((char *)"Press Tab to move forward and Shift+Tab to move back.");
            XtVaSetValues(example, XmNteachingTipTitle, title, XmNteachingTipText, body,
                XmNteachingTipDelay, 0, NULL);
            XmStringFree(title); XmStringFree(body);
        }
        XtAddCallback(trigger, XmNactivateCallback, show_popup_example, example);
    } else if (class == xmDropHighlightWidgetClass) {
        label = XmStringCreateLocalized((char *)"Drop a document here");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNdropHighlightLabel, label,
            XmNwidth, 360, XmNheight, 120, NULL);
        XmDropHighlightSetState(example, XmDROP_HIGHLIGHT_VALID);
    } else if (class == xmMessageBoxWidgetClass) {
        label = XmStringCreateLocalized((char *)"Your changes have been saved.");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNmessageString, label, XmNdialogType, XmDIALOG_INFORMATION,
            XmNwidth, 480, XmNheight, 180, NULL);
    } else if (class == xmCommandWidgetClass || class == xmSelectionBoxWidgetClass) {
        XmString values[3];
        values[0] = XmStringCreateLocalized((char *)"Open document");
        values[1] = XmStringCreateLocalized((char *)"Save changes");
        values[2] = XmStringCreateLocalized((char *)"Export report");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 480, XmNheight, 260, XmNlistItems, values, XmNlistItemCount, 3,
            XmNlistVisibleItemCount, 5, NULL);
        XmStringFree(values[0]); XmStringFree(values[1]); XmStringFree(values[2]);
    } else if (class == xmSeparatorWidgetClass || class == xmSeparatorGadgetClass) {
        sample_label(parent, "above", "Document settings");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNy, 40, XmNwidth, 360, XmNheight, 12, NULL);
        Widget below = sample_label(parent, "below", "Export options");
        XtVaSetValues(below, XmNy, 68, NULL);
    } else if (class == xmNotebookWidgetClass) {
        unsigned int i;
        const char *names[] = {"Overview", "Details", "Activity"};
        const char *contents[] = {"Project overview\n\nA classic notebook groups related pages.",
            "Document details\n\nChoose a tab to switch between these live pages.",
            "Recent activity\n\nCreated document\nSaved changes\nExported report"};
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 520, XmNheight, 260, NULL);
        for (i = 0; i < 3; ++i) {
            XmString value = XmStringCreateLocalized((char *)contents[i]);
            XtVaCreateManagedWidget("page", xmLabelWidgetClass, example,
                XmNnotebookChildType, XmPAGE, XmNpageNumber, (int)i + 1,
                XmNlabelString, value, XmNalignment, XmALIGNMENT_BEGINNING,
                XmNmarginWidth, 20, XmNmarginHeight, 20, NULL);
            XmStringFree(value);
            value = XmStringCreateLocalized((char *)names[i]);
            XtVaCreateManagedWidget("tab", xmPushButtonWidgetClass, example,
                XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, (int)i + 1,
                XmNlabelString, value, NULL);
            XmStringFree(value);
        }
        XtVaSetValues(example, XmNcurrentPageNumber, 1, NULL);
    } else if (class == xmFileSelectionBoxWidgetClass) {
        XmString directory = XmStringCreateLocalized((char *)"./demos/programs/ckmotif-gallery/");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNdirectory, directory, XmNpathMode, XmPATH_MODE_RELATIVE,
            XmNwidth, 620, XmNheight, 350,
            XmNlistVisibleItemCount, 7, NULL);
        XmStringFree(directory);
    } else if (class == xmSearchBoxWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 340, XmNheight, 40, NULL);
        XmSearchBoxSetString(example, "Find a document");
    } else if (class == xmSpinBoxWidgetClass || class == xmSimpleSpinBoxWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 200, XmNheight, 48, NULL);
        if (class == xmSimpleSpinBoxWidgetClass)
            XtVaSetValues(example, XmNspinBoxChildType, XmNUMERIC,
                XmNminimumValue, 0, XmNmaximumValue, 100, XmNposition, 7,
                XmNcolumns, 6, NULL);
        if (class == xmSpinBoxWidgetClass)
            XtVaCreateManagedWidget("quantity", xmTextFieldWidgetClass, example,
                XmNspinBoxChildType, XmNUMERIC, XmNminimumValue, 0,
                XmNmaximumValue, 100, XmNposition, 7, XmNcolumns, 6, NULL);
    } else if (class == xmColumnWidgetClass) {
        const char *captions[] = {"Document", "Status", "Owner"};
        const char *values[] = {"Quarterly report", "Draft", "Design team"};
        unsigned int i;
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 520, XmNheight, 200, NULL);
        for (i = 0; i < XtNumber(captions); ++i) {
            XmString caption = XmStringCreateLocalized((char *)captions[i]);
            XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, example,
                XmNentryLabelString, caption, XmNvalue, values[i], XmNcolumns, 28, NULL);
            XmStringFree(caption);
        }
    } else if (class == xmContainerWidgetClass) {
        Pixmap *icons = (Pixmap *)XtCalloc(4, sizeof(Pixmap));
        Widget folder;
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 520, XmNheight, 240, XmNlayoutType, XmOUTLINE,
            XmNentryViewType, XmSMALL_ICON, NULL);
        gallery_toolbar_icons(example, icons);
        XtAddCallback(example, XmNdestroyCallback, free_toolbar_icons, icons);
        label = XmStringCreateLocalized((char *)"Documents");
        folder = XtVaCreateManagedWidget("folder", xmIconGadgetClass, example,
            XmNlabelString, label, XmNsmallIconPixmap, icons[0],
            XmNoutlineState, XmEXPANDED, NULL);
        const char *names[] = {"Quarterly report", "Meeting notes"};
        unsigned int i;
        for (i = 0; i < XtNumber(names); ++i) {
            XmString caption = XmStringCreateLocalized((char *)names[i]);
            XtVaCreateManagedWidget("document", xmIconGadgetClass, example,
                XmNlabelString, caption, XmNsmallIconPixmap, icons[i + 1],
                XmNentryParent, folder, NULL);
            XmStringFree(caption);
        }
    } else if (class == xmFormWidgetClass || class == xmFrameWidgetClass ||
               class == xmBulletinBoardWidgetClass || class == xmRowColumnWidgetClass ||
               class == xmButtonBoxWidgetClass ||
               class == xmPanedWindowWidgetClass || class == xmPanedWidgetClass ||
               class == xmMainWindowWidgetClass ||
               class == xmScrolledWindowWidgetClass || class == xmFormSectionWidgetClass) {
        Widget panels;
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 520, XmNheight, 260, NULL);
        panels = XtVaCreateManagedWidget("settings", xmRowColumnWidgetClass,
            class == xmFormSectionWidgetClass ? XmFormSectionGetContent(example) : example,
            XmNorientation, XmVERTICAL, XmNpacking, XmPACK_TIGHT,
            XmNisAligned, False, XmNspacing, 12, XmNmarginWidth, 16,
            XmNmarginHeight, 16, NULL);
        {
            XmString value = XmStringCreateLocalized((char *)"Document settings");
            XtVaCreateManagedWidget("sectionTitle", xmLabelWidgetClass, panels,
                XmNlabelString, value, XmNalignment, XmALIGNMENT_BEGINNING, NULL);
            XmStringFree(value);
            value = XmStringCreateLocalized((char *)"Document name");
            XtVaCreateManagedWidget("nameLabel", xmLabelWidgetClass, panels,
                XmNlabelString, value, XmNalignment, XmALIGNMENT_BEGINNING, NULL);
            XmStringFree(value);
            XtVaCreateManagedWidget("documentName", xmTextFieldWidgetClass, panels,
                XmNvalue, "Quarterly report", XmNcolumns, 30, NULL);
            value = XmStringCreateLocalized((char *)"Save a backup copy");
            XtVaCreateManagedWidget("backup", xmToggleButtonWidgetClass, panels,
                XmNlabelString, value, XmNset, True, NULL);
            XmStringFree(value);
            value = XmStringCreateLocalized((char *)"Apply settings");
            Widget apply = XtVaCreateManagedWidget("apply", xmPushButtonWidgetClass, panels,
                XmNlabelString, value, NULL);
            if (callback) XtAddCallback(apply, XmNactivateCallback, callback, client_data);
            XmStringFree(value);
        }
        if (class == xmPanedWindowWidgetClass || class == xmPanedWidgetClass) {
            XmString preview = XmStringCreateLocalized((char *)"Preview\nQuarterly report — draft document");
            XtVaCreateManagedWidget("previewPane", xmLabelWidgetClass, example,
                XmNlabelString, preview, XmNalignment, XmALIGNMENT_BEGINNING,
                XmNmarginWidth, 16, XmNmarginHeight, 16, XmNheight, 90, NULL);
            XmStringFree(preview);
            XtVaSetValues(example, XmNheight, 360, NULL);
        }
    } else if (class == xmDrawingAreaWidgetClass) {
        const char *days[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};
        unsigned int i;
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 360, XmNheight, 210, NULL);
        Widget title = sample_label(example, "chartTitle", "Weekly activity");
        XtVaSetValues(title, XmNx, 24, XmNy, 10, NULL);
        for (i = 0; i < XtNumber(days); ++i) {
            Widget day = sample_label(example, "day", days[i]);
            XtVaSetValues(day, XmNx, 28 + i * 64, XmNy, 160, NULL);
        }
        XtAddCallback(example, XmNexposeCallback, draw_activity, NULL);
        if (callback) XtAddCallback(example, XmNinputCallback, callback, client_data);
    } else if (class == xmFontSelectorWidgetClass || class == xmColorSelectorWidgetClass) {
        /* These legacy composites require an initial allocation before their
         * internal panes can negotiate usable child geometry. */
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 720, XmNheight, 340, NULL);
    } else {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, XmNheight, 64, NULL);

    }
    {
        Dimension width, height;
        XtWidgetGeometry preferred = {0};
        XtVaGetValues(example, XmNwidth, &width, XmNheight, &height, NULL);
        if (class == xmLinkWidgetClass || class == xmPromptWidgetClass ||
            class == xmIconLabelWidgetClass || class == xmIconBoxWidgetClass ||
            class == xmColumnWidgetClass || class == xmDisclosureWidgetClass ||
            class == xmFormSectionWidgetClass ||
            width <= 1 || height <= 1) {
            XtQueryGeometry(example, NULL, &preferred);
            if ((preferred.request_mode & CWWidth) && preferred.width)
                XtVaSetValues(example, XmNwidth, preferred.width, NULL);
            if ((preferred.request_mode & CWHeight) && preferred.height)
                XtVaSetValues(example, XmNheight, preferred.height, NULL);
        }
    }
    if (class == xmPushButtonWidgetClass || class == xmPushButtonGadgetClass ||
        class == xmToggleButtonWidgetClass || class == xmToggleButtonGadgetClass) {
        XmString alternate = XmStringCreateLocalized((char *)"Unavailable");
        XtVaCreateManagedWidget("disabledState", class, parent,
            XmNlabelString, alternate, XmNsensitive, False,
            XmNy, 64, XmNwidth, 220, XmNheight, 44, NULL);
        XmStringFree(alternate);
        if (class == xmToggleButtonWidgetClass || class == xmToggleButtonGadgetClass) {
            alternate = XmStringCreateLocalized((char *)"Selected option");
            Widget selected = XtVaCreateManagedWidget("selectedState", class, parent,
                XmNlabelString, alternate, XmNset, True,
                XmNy, 128, XmNwidth, 220, XmNheight, 44, NULL);
            if (callback) XtAddCallback(selected, XmNvalueChangedCallback, callback, client_data);
            XmStringFree(alternate);
        }
    }
    if (class == xmTextFieldWidgetClass || class == xmDataFieldWidgetClass ||
        class == xmTextWidgetClass || class == xmNumberBoxWidgetClass) {
        sample_label(parent, "fieldLabel", class == xmNumberBoxWidgetClass ? "Quantity" : "Document title");
        XtVaSetValues(example, XmNy, 32, NULL);
        Widget help = sample_label(parent, "fieldHelp", "Edit the value to see its change notification.");
        Dimension height;
        XtVaGetValues(example, XmNheight, &height, NULL);
        XtVaSetValues(help, XmNy, height + 48, NULL);
    }
    if (label)
        XmStringFree(label);
    if (callback)
        gallery_attach_public_callback(example, callback, client_data);
    return example;
}
/* GALLERY-EXAMPLE-END: shared-public-construction */

/* GALLERY-EXAMPLE-BEGIN: segmented-control-construction */
Widget
gallery_create_segmented_control_example(Widget parent, const char *name,
                                         XtCallbackProc callback,
                                         XtPointer client_data)
{
    XmString labels[3];
    XmSegmentedControlItem items[3];
    Widget example;
    labels[0] = XmStringCreateLocalized((char *)"Overview");
    labels[1] = XmStringCreateLocalized((char *)"Details");
    labels[2] = XmStringCreateLocalized((char *)"Activity");
    items[0] = (XmSegmentedControlItem){"overview", labels[0], XmUNSPECIFIED_PIXMAP, True};
    items[1] = (XmSegmentedControlItem){"details", labels[1], XmUNSPECIFIED_PIXMAP, True};
    items[2] = (XmSegmentedControlItem){"activity", labels[2], XmUNSPECIFIED_PIXMAP, True};
    example = XtVaCreateManagedWidget((char *)name,
                                      xmSegmentedControlWidgetClass, parent,
                                      XmNwidth, 360, XmNheight, 42, NULL);
    if (!XmSegmentedControlSetItems(example, items, 3)) {
        XtDestroyWidget(example);
        example = NULL;
    } else {
        XmSegmentedControlSetSelectedIndex(example, 0, False);
        if (callback)
            XtAddCallback(example, XmNSegmentedSelectionCallback,
                          callback, client_data);
    }
    XmStringFree(labels[0]);
    XmStringFree(labels[1]);
    XmStringFree(labels[2]);
    return example;
}
/* GALLERY-EXAMPLE-END: segmented-control-construction */

/* GALLERY-EXAMPLE-BEGIN: public-teardown */
void
gallery_destroy_component_example(Widget *example)
{
    if (example && *example) {
        XtDestroyWidget(*example);
        *example = NULL;
    }
}
/* GALLERY-EXAMPLE-END: public-teardown */

/* GALLERY-EXAMPLE-BEGIN: drag-public-construction */
Widget
gallery_create_drag_source_example(Widget parent, XtCallbackProc callback,
                                   XtPointer client_data)
{
    Widget source = XtVaCreateManagedWidget(
        "dragSource", xmDrawingAreaWidgetClass, parent,
        XmNwidth, 320, XmNheight, 80, NULL);
    sample_label(source, "dragHint", "Press and drag from this area");
    if (callback)
        XtAddCallback(source, XmNinputCallback, callback, client_data);
    return source;
}
/* GALLERY-EXAMPLE-END: drag-public-construction */

/* GALLERY-EXAMPLE-BEGIN: shell-public-construction */
Widget
gallery_create_shell_example(Widget parent, WidgetClass class,
                             const char *name)
{
    Arg args[2];
    XtSetArg(args[0], XmNwidth, 240);
    XtSetArg(args[1], XmNheight, 96);
    Widget shell = XtCreatePopupShell((char *)name, class, parent, args, 2);
    XmString label = XmStringCreateLocalized((char *)"Popup shell example");
    if (class == xmMenuShellWidgetClass) {
        Widget menu = XtVaCreateManagedWidget(
            "popupContent", xmRowColumnWidgetClass, shell,
            XmNrowColumnType, XmMENU_POPUP, NULL);
        XtVaCreateManagedWidget("popupAction", xmPushButtonWidgetClass, menu,
                                XmNlabelString, label, NULL);
    } else {
        XtVaCreateManagedWidget("popupContent", xmLabelWidgetClass, shell,
                                XmNlabelString, label, NULL);
    }
    XmStringFree(label);
    return shell;
}
/* GALLERY-EXAMPLE-END: shell-public-construction */
