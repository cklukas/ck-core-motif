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

/* GALLERY-EXAMPLE-BEGIN: shared-public-construction */
static void
gallery_attach_public_callback(Widget example, XtCallbackProc callback,
                               XtPointer client_data)
{
    /* Prefer the component's semantic action, then its value notification.
     * Every Motif widget/gadget still has the public help callback as a
     * keyboard-accessible interaction fallback. */
    if (XtHasCallbacks(example, XmNactivateCallback) == XtCallbackHasSome)
        XtAddCallback(example, XmNactivateCallback, callback, client_data);
    else if (XtHasCallbacks(example, XmNvalueChangedCallback) == XtCallbackHasSome)
        XtAddCallback(example, XmNvalueChangedCallback, callback, client_data);
    else if (XtHasCallbacks(example, XmNhelpCallback) == XtCallbackHasSome)
        XtAddCallback(example, XmNhelpCallback, callback, client_data);
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
                                          XmNchipRemovable, True, NULL);
    } else if (class == xmBadgeWidgetClass) {
        label = XmStringCreateLocalized((char *)"Notifications");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNbadgeLabel, label,
                                          XmNbadgeCount, 3, NULL);
    } else if (class == xmIconLabelWidgetClass) {
        label = XmStringCreateLocalized((char *)"Recent document");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNiconLabelLabel, label, NULL);
    } else if (class == xmDividerWidgetClass) {
        label = XmStringCreateLocalized((char *)"Presentation");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNdividerLabel, label, NULL);
    } else if (class == xmAvatarWidgetClass) {
        label = XmStringCreateLocalized((char *)"CK");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNavatarInitials, label, NULL);
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
        XtVaCreateManagedWidget("switchTitle", xmLabelWidgetClass, parent,
                                XmNlabelString, title_label, NULL);
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNlabelString, switch_label,
                                          XmNset, False,
                                          XmNwidth, 150, XmNheight, 44, NULL);
        XmStringFree(title_label);
        XmStringFree(switch_label);
    } else if (class == xmToggleButtonWidgetClass ||
               class == xmToggleButtonGadgetClass) {
        /* Show the toggle as a button, not as a check-box indicator.  The
         * resource still preserves toggle semantics; only its presentation
         * matches the gallery's component example. */
        label = XmStringCreateLocalized((char *)"Toggle example");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNlabelString, label,
                                          XmNindicatorOn, False,
                                          XmNwidth, 220, XmNheight, 44, NULL);
    } else if (class == xmPushButtonWidgetClass ||
               class == xmPushButtonGadgetClass ||
               class == xmCascadeButtonWidgetClass ||
               class == xmCascadeButtonGadgetClass ||
               class == xmLabelWidgetClass ||
               class == xmLabelGadgetClass) {
        label = XmStringCreateLocalized((char *)"Run example");
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
                                          XmNminimum, 0, XmNmaximum, 100,
                                          XmNvalue, 65, XmNwidth, 320,
                                          XmNheight, 48, NULL);
    } else if (class == xmNumberBoxWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 220, XmNheight, 48, NULL);
        XmNumberBoxSetRange(example, 0, 100, False);
        XmNumberBoxSetValue(example, 42, False);
    } else if (class == xmRatingControlWidgetClass) {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 320, XmNheight, 64, NULL);
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
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, XmNheight, 64, NULL);
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
    } else {
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, XmNheight, 64, NULL);
        /* Managers and presentation shells are still useful only when they
         * contain something.  Give every remaining constructible entry a
         * visible child while preserving the selected class as the owner of
         * the example surface. */
        if (XtIsComposite(example)) {
            XmString child_label = XmStringCreateLocalized((char *)"Live component content");
            XtVaCreateManagedWidget("content", xmLabelWidgetClass, example,
                                    XmNlabelString, child_label, NULL);
            XmStringFree(child_label);
        }
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
        XmNwidth, 180, XmNheight, 64, NULL);
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
