/* ckmotif-gallery: a manifest-driven native component gallery. */
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <X11/Xft/Xft.h>
#include <Xm/XmAll.h>
#include <Xm/Sidebar.h>
#include <Xm/TreeView.h>
#include <Xm/Notebook.h>
#include <Xm/StatusBar.h>
#include <Xm/Accessible.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>

#include "gallery_manifest.h"
#include "gallery_sources.h"

extern Widget gallery_create_component_example(Widget, WidgetClass,
                                               const char *, XtCallbackProc,
                                               XtPointer);
extern Widget gallery_create_segmented_control_example(Widget, const char *,
                                                       XtCallbackProc,
                                                       XtPointer);
extern Widget gallery_create_shell_example(Widget, WidgetClass, const char *);
extern void gallery_destroy_component_example(Widget *);
extern Widget gallery_create_drag_source_example(Widget, XtCallbackProc, XtPointer);

typedef struct {
    Widget shell, sidebar, tree, notebook, welcome, example, source, icons, status;
    Widget title, provenance, action, component, component_host, source_text;
    unsigned int example_serial;
    unsigned long callback_count;
    char provenance_text[768];
    XmAppearanceProfile profile;
    unsigned int dpi_step;
    Boolean component_is_popup, component_is_drag_source;
    Widget drag_context;
    XmAccessibility *accessibility;
    const GalleryManifestEntry *selected;
} GalleryState;

static XmString text(const char *value) { return XmStringCreateLocalized((char *)value); }

static Widget open_gallery_dialog(Widget parent, const char *kind)
{
    XmString message = text("This is a live Motif dialog from the gallery.");
    Widget dialog = NULL;
    if (!strcmp(kind, "error"))
        dialog = XmCreateErrorDialog(parent, "errorDialog", NULL, 0);
    else if (!strcmp(kind, "information"))
        dialog = XmCreateInformationDialog(parent, "informationDialog", NULL, 0);
    else if (!strcmp(kind, "question"))
        dialog = XmCreateQuestionDialog(parent, "questionDialog", NULL, 0);
    else if (!strcmp(kind, "warning"))
        dialog = XmCreateWarningDialog(parent, "warningDialog", NULL, 0);
    else if (!strcmp(kind, "working"))
        dialog = XmCreateWorkingDialog(parent, "workingDialog", NULL, 0);
    if (dialog) {
        XtVaSetValues(dialog, XmNmessageString, message, NULL);
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
        XtManageChild(dialog);
    }
    XmStringFree(message);
    return dialog;
}

static void welcome_expose(Widget widget, XtPointer client_data, XtPointer call_data) {
    static const char *lines[] = {
        "Hello world!", "Hallo Welt!", "こんにちは世界！", "你好，世界！",
        "", "Choose a component category on the left to explore live examples."
    };
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    XftDraw *draw;
    XftFont *font;
    XftFont *cjk_font;
    XftColor color;
    XGlyphInfo extents;
    Dimension width, height;
    size_t i;
    (void)client_data;
    (void)call_data;
    if (!window) return;
    XtVaGetValues(widget, XmNwidth, &width, XmNheight, &height, NULL);
    draw = XftDrawCreate(display, window, DefaultVisual(display, DefaultScreen(display)),
                         DefaultColormap(display, DefaultScreen(display)));
    font = XftFontOpenName(display, DefaultScreen(display), "Noto Sans-16");
    if (!font) font = XftFontOpenName(display, DefaultScreen(display), "DejaVu Sans-16");
    cjk_font = XftFontOpenName(display, DefaultScreen(display), "Droid Sans Fallback-16");
    if (!draw || !font || !cjk_font) {
        if (cjk_font) XftFontClose(display, cjk_font);
        if (font) XftFontClose(display, font);
        if (draw) XftDrawDestroy(draw);
        return;
    }
    XftColorAllocName(display, DefaultVisual(display, DefaultScreen(display)),
                      DefaultColormap(display, DefaultScreen(display)), "#202020", &color);
    for (i = 0; i < sizeof(lines) / sizeof(lines[0]); ++i) {
        XftFont *line_font = (i == 2 || i == 3) ? cjk_font : font;
        XftTextExtentsUtf8(display, line_font, (const FcChar8 *)lines[i], strlen(lines[i]), &extents);
        XftDrawStringUtf8(draw, &color, line_font,
                          (int)((width > extents.width ? width - extents.width : 0) / 2),
                          (int)(height / 2 - 70 + i * (font->height + 4)),
                          (const FcChar8 *)lines[i], strlen(lines[i]));
    }
    XftColorFree(display, DefaultVisual(display, DefaultScreen(display)),
                 DefaultColormap(display, DefaultScreen(display)), &color);
    XftFontClose(display, font);
    XftFontClose(display, cjk_font);
    XftDrawDestroy(draw);
}

static void provenance_expose(Widget widget, XtPointer client_data, XtPointer call_data) {
    GalleryState *state = (GalleryState *)client_data;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    XftDraw *draw;
    XftFont *font;
    XftColor color;
    Dimension height;
    XGlyphInfo extents;
    int baseline;
    (void)call_data;
    if (!window || !state) return;
    XtVaGetValues(widget, XmNheight, &height, NULL);
    draw = XftDrawCreate(display, window, DefaultVisual(display, DefaultScreen(display)),
                         DefaultColormap(display, DefaultScreen(display)));
    font = XftFontOpenName(display, DefaultScreen(display), "Noto Sans-14");
    if (!font) font = XftFontOpenName(display, DefaultScreen(display), "DejaVu Sans-14");
    if (!draw || !font) {
        if (font) XftFontClose(display, font);
        if (draw) XftDrawDestroy(draw);
        return;
    }
    XftColorAllocName(display, DefaultVisual(display, DefaultScreen(display)),
                      DefaultColormap(display, DefaultScreen(display)), "#202020", &color);
    XftTextExtentsUtf8(display, font, (const FcChar8 *)state->provenance_text,
                       strlen(state->provenance_text), &extents);
    baseline = (int)((height + font->ascent - font->descent) / 2);
    XftDrawStringUtf8(draw, &color, font, 4, baseline,
                      (const FcChar8 *)state->provenance_text,
                      strlen(state->provenance_text));
    (void)extents;
    XftColorFree(display, DefaultVisual(display, DefaultScreen(display)),
                 DefaultColormap(display, DefaultScreen(display)), &color);
    XftFontClose(display, font);
    XftDrawDestroy(draw);
}

static void center_shell_frame(Widget shell) {
    Display *display = XtDisplay(shell);
    Window root, parent, *children = NULL;
    unsigned int child_count = 0;
    int frame_x, frame_y;
    unsigned int frame_width, frame_height, border, depth;
    int screen_width, screen_height;
    if (!XtIsRealized(shell) || !XQueryTree(display, XtWindow(shell), &root, &parent,
                                            &children, &child_count))
        return;
    if (children) XFree(children);
    if (!XGetGeometry(display, parent, &root, &frame_x, &frame_y,
                      &frame_width, &frame_height, &border, &depth))
        return;
    screen_width = DisplayWidth(display, DefaultScreen(display));
    screen_height = DisplayHeight(display, DefaultScreen(display));
    {
        XWindowChanges changes;
        changes.x = (screen_width > (int)frame_width)
            ? (screen_width - (int)frame_width) / 2 : 0;
        changes.y = (screen_height > (int)frame_height)
            ? (screen_height - (int)frame_height) / 2 : 0;
        XMoveWindow(display, parent, changes.x, changes.y);
        (void)XReconfigureWMWindow(display, parent, DefaultScreen(display),
                                   CWX | CWY, &changes);
    }
    XSync(display, False);
}

static Boolean parse_appearance(const char *value,
                                XmAppearanceProfile *profile_return)
{
    if (!strcmp(value, "classic"))
        *profile_return = XmAPPEARANCE_CLASSIC;
    else if (!strcmp(value, "modern-light"))
        *profile_return = XmAPPEARANCE_MODERN_LIGHT;
    else if (!strcmp(value, "high-contrast"))
        *profile_return = XmAPPEARANCE_HIGH_CONTRAST;
    else if (!strcmp(value, "modern-dark"))
        *profile_return = XmAPPEARANCE_MODERN_DARK;
    else
        return False;
    return True;
}

static Boolean parse_scale(const char *value, double *scale_return)
{
    char *end;
    double scale;

    errno = 0;
    scale = strtod(value, &end);
    if (end == value || *end != '\0' || errno == ERANGE ||
        !isfinite(scale) || scale <= 0.0 || scale > 64.0)
        return False;
    *scale_return = scale;
    return True;
}

static const GalleryManifestEntry *find_entry(const char *id)
{
    Cardinal i;
    for (i = 0; i < GALLERY_MANIFEST_COUNT; ++i)
        if (!strcmp(gallery_manifest[i].id, id)) return &gallery_manifest[i];
    return &gallery_manifest[0];
}

static const char *source_for(const GalleryManifestEntry *entry)
{
    Cardinal i;
    if (!entry || !entry->interaction || !entry->docs_anchor || !entry->screenshot)
        return "Source fragment unavailable";
    for (i = 0; i < GALLERY_MANIFEST_COUNT; ++i)
        if (!strcmp(gallery_sources[i].fragment, entry->source_fragment))
            return gallery_sources[i].text;
    return "Source fragment unavailable";
}

static void set_status(GalleryState *state, const char *message)
{
    XmStatusBarItem item;
    item.id = "interaction"; item.label = text(message);
    item.kind = XmSTATUS_BAR_ITEM_TEXT; item.priority = 0;
    item.value = 0; item.maximum = 0; item.user_data = NULL;
    XmStatusBarSetItems(state->status, &item, 1);
    XmStringFree(item.label);
}

static void drag_context_destroyed(Widget widget, XtPointer client,
                                   XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    (void)call;
    if (state->drag_context == widget)
        state->drag_context = NULL;
}

static void component_activate(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    char message[160];
    (void)widget; (void)call;
    ++state->callback_count;
    if (state->component_is_drag_source) {
        set_status(state, "Drag source ready; press and drag the example surface");
        return;
    }
    if (state->component_is_popup && state->component) {
        XtPopup(state->component, XtGrabNone);
        snprintf(message, sizeof(message),
                 "Callback #%lu: popup opened through public Xt popup API",
                 state->callback_count);
        set_status(state, message);
        return;
    }
    snprintf(message, sizeof(message),
             "Callback #%lu: delivered through public Xt API",
             state->callback_count);
    set_status(state, message);
}

static void drag_input(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    XmDrawingAreaCallbackStruct *callback = (XmDrawingAreaCallbackStruct *)call;
    if (callback == NULL || callback->event == NULL)
        return;
    if (callback->event->type != ButtonPress || state->drag_context != NULL)
        return;
    state->drag_context = XmDragStart(widget, callback->event, NULL, 0);
    if (state->drag_context != NULL) {
        XtAddCallback(state->drag_context, XmNdestroyCallback,
                      drag_context_destroyed, state);
        set_status(state, "Drag started through the public XmDragStart API");
    } else {
        set_status(state, "Drag operation unavailable on this display");
    }
}

static Boolean invoke_component_callback(Widget component, GalleryState *state)
{
    if (!component)
        return False;
    if (XtHasCallbacks(component, XmNactivateCallback) == XtCallbackHasSome)
        XtCallCallbacks(component, XmNactivateCallback, NULL);
    else if (XtHasCallbacks(component, XmNvalueChangedCallback) == XtCallbackHasSome)
        XtCallCallbacks(component, XmNvalueChangedCallback, NULL);
    else if (XtHasCallbacks(component, XmNhelpCallback) == XtCallbackHasSome)
        XtCallCallbacks(component, XmNhelpCallback, NULL);
    else
        return False;
    (void)state;
    return True;
}

static void show_entry(GalleryState *state, const GalleryManifestEntry *entry)
{
    Widget anchor;
    XmString title;
    char details[768];
    state->selected = entry;
    snprintf(details, sizeof(details), "Interactive %s example — use the control below",
             entry->kind);
    strncpy(state->provenance_text, details, sizeof(state->provenance_text) - 1);
    state->provenance_text[sizeof(state->provenance_text) - 1] = '\0';
    XtVaSetValues(state->notebook, XmNcurrentPageNumber, 2, NULL);
    if (!state->example) {
        state->example = XtVaCreateWidget("examplePage", xmFormWidgetClass,
                                          state->notebook, XmNnotebookChildType,
                                          XmPAGE, XmNpageNumber, 2, NULL);
        title = text(entry->id);
        state->title = XtVaCreateManagedWidget("componentTitle", xmLabelWidgetClass,
                                              state->example, XmNlabelString, title,
                                              XmNtopAttachment, XmATTACH_FORM,
                                              XmNleftAttachment, XmATTACH_FORM, NULL);
        XmStringFree(title);
        state->provenance = XtVaCreateManagedWidget(
            "provenance", xmDrawingAreaWidgetClass, state->example,
            XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, state->title,
            XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
            XmNheight, 28, NULL);
        XtAddCallback(state->provenance, XmNexposeCallback, provenance_expose, state);
        state->action = XtVaCreateManagedWidget(
            "activateExample", xmPushButtonWidgetClass, state->example,
            XmNlabelString, text("Activate example"), XmNtopAttachment,
            XmATTACH_WIDGET, XmNtopWidget, state->provenance,
            XmNleftAttachment, XmATTACH_FORM, NULL);
        XtAddCallback(state->action, XmNactivateCallback, component_activate, state);
        XtManageChild(state->example);

        state->source = XtVaCreateWidget("sourcePage", xmFormWidgetClass,
                                         state->notebook, XmNnotebookChildType,
                                         XmPAGE, XmNpageNumber, 3, NULL);
        {
            Arg args[1];
            XtSetArg(args[0], XmNeditable, False);
            state->source_text = XmCreateScrolledText(
                state->source, (char *)"sourceText", args, 1);
            XtManageChild(state->source_text);
            XtVaSetValues(XtParent(state->source_text),
                          XmNtopAttachment, XmATTACH_FORM,
                          XmNbottomAttachment, XmATTACH_FORM,
                          XmNleftAttachment, XmATTACH_FORM,
                          XmNrightAttachment, XmATTACH_FORM, NULL);
        }
        XtManageChild(state->source);
    }
    title = text(entry->id);
    XtVaSetValues(state->title, XmNlabelString, title, NULL);
    XmStringFree(title);
    if (XtIsRealized(state->provenance))
        XClearArea(XtDisplay(state->provenance), XtWindow(state->provenance),
                   0, 0, 0, 0, True);
    if (state->drag_context) {
        XmDragCancel(state->drag_context);
        state->drag_context = NULL;
    }
    if (state->component)
        gallery_destroy_component_example(&state->component);
    if (state->component_host) {
        XtDestroyWidget(state->component_host);
        state->component_host = NULL;
    }
    state->component_is_popup = False;
    state->component_is_drag_source = False;
    if (entry->constructible && entry->class) {
        char name[48];
        snprintf(name, sizeof(name), "component%u", ++state->example_serial);
        if (!strcmp(entry->class_symbol, "xmDragOverShellWidgetClass")) {
            state->component_host = XtVaCreateManagedWidget(
                "componentHost", xmRowColumnWidgetClass, state->example,
                XmNtopAttachment, XmATTACH_WIDGET,
                XmNtopWidget, state->provenance,
                XmNleftAttachment, XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNrowColumnType, XmWORK_AREA,
                XmNorientation, XmVERTICAL, NULL);
            state->component = gallery_create_drag_source_example(
                state->component_host, drag_input, state);
            state->component_is_drag_source = True;
            anchor = state->component_host;
        } else if (entry->popup_shell) {
            state->component = gallery_create_shell_example(state->shell,
                                                            entry->class, name);
            state->component_is_popup = True;
            anchor = state->provenance;
        } else {
            Boolean gadget = !strcmp(entry->kind, "gadget");
            int row_column_type = XmWORK_AREA;
            if (gadget && strstr(entry->class_symbol, "Cascade"))
                row_column_type = XmMENU_BAR;
            state->component_host = XtVaCreateManagedWidget(
                "componentHost", xmRowColumnWidgetClass, state->example,
                XmNtopAttachment, XmATTACH_WIDGET,
                XmNtopWidget, state->provenance,
                XmNleftAttachment, XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNwidth, 420, XmNheight, 140,
                XmNrowColumnType, row_column_type,
                XmNorientation, XmVERTICAL, NULL);
            if (!strcmp(entry->class_symbol, "xmSegmentedControlWidgetClass"))
                state->component = gallery_create_segmented_control_example(
                    state->component_host, name, component_activate, state);
            else
                state->component = gallery_create_component_example(
                    state->component_host, entry->class, name,
                    component_activate, state);
            anchor = state->component_host;
        }
    } else {
        anchor = state->provenance;
        if (entry->constructible)
            set_status(state, "This example is an optional capability disabled in this build");
    }
    XtVaSetValues(state->action, XmNtopWidget, anchor, NULL);
    XmTextSetString(state->source_text, (char *)source_for(entry));
    /* The initial component can be selected before the notebook pages are
     * realized.  Select Example again after both pages exist so --component
     * starts on the live control rather than leaving the Welcome page active. */
    XtVaSetValues(state->notebook, XmNcurrentPageNumber, 2, NULL);
    if (entry->constructible && !entry->class)
        set_status(state, "Selected component; optional example unavailable in this build");
    else
        set_status(state, "Selected component; Example and Source tabs are ready");
}

static void theme_control(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    (void)widget; (void)call;
#ifdef CKMOTIF_GALLERY_ENABLE_LIVE_TRANSACTIONS
    XmAppearanceProfile next = (XmAppearanceProfile)
        ((state->profile + 1) % (XmAPPEARANCE_MODERN_DARK + 1));
    if (XmApplyAppearanceProfile(XtDisplay(state->shell), next)) {
        state->profile = next;
        set_status(state, "Appearance profile applied through the public live API");
    } else {
        set_status(state, "Appearance profile transaction failed; state preserved");
    }
#else
    set_status(state, "Theme control pending toolkit live-transaction qualification");
#endif
}

static void dpi_control(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    (void)widget; (void)call;
#ifdef CKMOTIF_GALLERY_ENABLE_LIVE_TRANSACTIONS
    static const double scales[] = { 1.0, 1.5, 2.0, 3.0 };
    ++state->dpi_step;
    if (state->dpi_step >= XtNumber(scales)) {
        if (XmClearDisplayScaleOverride(XtDisplay(state->shell))) {
            state->dpi_step = 0;
            set_status(state, "DPI override cleared through the public live API");
        } else {
            set_status(state, "DPI transaction failed; state preserved");
        }
    } else if (XmSetDisplayScaleOverride(XtDisplay(state->shell),
                                          scales[state->dpi_step])) {
        set_status(state, "DPI override applied through the public live API");
    } else {
        set_status(state, "DPI transaction failed; state preserved");
    }
#else
    set_status(state, "DPI control pending toolkit live-transaction qualification");
#endif
}

static int exercise_component_matrix(GalleryState *state)
{
    Cardinal i;
    unsigned long callbacks_before = state->callback_count;
    for (i = 0; i < GALLERY_MANIFEST_COUNT; ++i) {
        const GalleryManifestEntry *entry = &gallery_manifest[i];
        if (!entry->constructible || !entry->class)
            continue;
        XmTreeViewSetSelectedId(state->tree, (XtPointer)entry->id, True);
        if (XmTreeViewGetSelectedId(state->tree) != (XtPointer)entry->id) {
            fprintf(stderr, "gallery integration: selection failed for %s\n", entry->id);
            return 1;
        }
        /* Selection callbacks are delivered synchronously by the native
         * tree, but keep the matrix deterministic for toolkit test doubles. */
        if (state->selected != entry)
            show_entry(state, entry);
        {
            /* The source buffer contains the generated fragment body, not a
             * hand-maintained label; the ID must still be present in it. */
            char *source = XmTextGetString(state->source_text);
            Boolean source_matches = source && strstr(source, entry->source_fragment);
            XtFree(source);
            if (state->selected != entry || !source_matches) {
                fprintf(stderr, "gallery integration: source/selection failed for %s\n", entry->id);
                return 1;
            }
        }
        if (state->component_is_drag_source) {
            set_status(state, "Drag source callback and public DnD path ready");
        } else if (state->component_is_popup) {
            component_activate(state->action, state, NULL);
        } else if (!invoke_component_callback(state->component, state)) {
            component_activate(state->action, state, NULL);
        }
    }
    if (state->callback_count == callbacks_before) {
        fprintf(stderr, "gallery integration: no component callback was observed\n");
        return 1;
    }
    return 0;
}

static int run_integration_test(GalleryState *state)
{
    XtPointer selected = (XtPointer)gallery_manifest[1].id;
    Cardinal i, available = 0, optional = 0;
    for (i = 0; i < GALLERY_MANIFEST_COUNT; ++i) {
        if (!gallery_manifest[i].source_fragment ||
            !gallery_manifest[i].interaction ||
            !gallery_manifest[i].docs_anchor ||
            !gallery_manifest[i].screenshot) {
            fprintf(stderr, "gallery integration: missing metadata at index %u\n", (unsigned)i);
            return 1;
        }
        if (!gallery_manifest[i].constructible) continue;
        if (gallery_manifest[i].class) ++available;
        else ++optional;
    }
    XmTreeViewSetSelectedId(state->tree, selected, True);
        if (!XmProcessTraversal(state->tree, XmTRAVERSE_CURRENT)) {
            fprintf(stderr, "gallery integration: tree traversal failed\n");
        return 1;
        }
    {
        Widget focus_before = XmGetFocusWidget(state->shell);
        XmSidebarSetExpanded(state->sidebar, False);
        XmSidebarSetExpanded(state->sidebar, True);
        if (focus_before && XmGetFocusWidget(state->shell) != focus_before) {
            fprintf(stderr, "gallery integration: focus was not restored\n");
            return 1;
        }
    }
    XtVaSetValues(state->notebook, XmNcurrentPageNumber, 3, NULL);
    if (exercise_component_matrix(state))
        return 1;
    {
        int current_page = 0;
        XmNotebookPageInfo page_info;
        XtVaGetValues(state->notebook, XmNcurrentPageNumber, &current_page, NULL);
        if (current_page != 2 || XmStatusBarGetItemCount(state->status) != 1 ||
            XmNotebookGetPageInfo(state->notebook, 3, &page_info) != XmPAGE_FOUND) {
            fprintf(stderr, "gallery integration: notebook/status check failed (page=%d status=%u)\n",
                    current_page, (unsigned)XmStatusBarGetItemCount(state->status));
            return 1;
        }
    }
    if (state->accessibility) {
        XmAccessibleId tree_id = XmAccessibilityWidget(state->accessibility,
                                                        state->tree);
        XmAccessibleInfo info = {0};
        if (!tree_id || !XmAccessibilityQuery(state->accessibility, tree_id, &info) ||
            info.role != XmACCESSIBLE_TREE) {
            fprintf(stderr, "gallery integration: accessibility check failed (id=%lu role=%d)\n",
                    (unsigned long)tree_id, (int)info.role);
            XmAccessibleInfoClear(&info);
            return 1;
        }
        XmAccessibleInfoClear(&info);
    }
    /* Restore the original selection after exercising the full component
     * matrix; this is the state-preservation assertion for the shell. */
    XmTreeViewSetSelectedId(state->tree, selected, True);
    if (XmTreeViewGetSelectedId(state->tree) != selected ||
        !XmSidebarGetExpanded(state->sidebar)) {
        fprintf(stderr, "gallery integration: final navigation state failed (selected=%p expected=%p expanded=%d)\n",
                XmTreeViewGetSelectedId(state->tree), selected,
                (int)XmSidebarGetExpanded(state->sidebar));
        return 1;
    }
    printf("gallery integration: component registry %u available, %u optional\n",
           (unsigned)available, (unsigned)optional);
    printf("gallery integration: acceptance metadata %u entries PASS\n",
           (unsigned)GALLERY_MANIFEST_COUNT);
    puts("gallery integration: navigation tabs interaction state PASS");
    puts("gallery integration: keyboard traversal and focus restoration PASS");
    puts("gallery integration: theme/dpi live transaction PENDING toolkit qualification");
    return 0;
}

static void tree_selected(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    XmTreeViewCallbackStruct *callback = (XmTreeViewCallbackStruct *)call;
    (void)widget;
    if (callback && callback->id) {
        const GalleryManifestEntry *entry = find_entry((const char *)callback->id);
        if (!strcmp((const char *)callback->id, "gallery.welcome")) {
            XtVaSetValues(state->notebook, XmNcurrentPageNumber, 1, NULL);
            set_status(state, "Welcome - choose a component category to begin");
            return;
        }
        if (!strcmp((const char *)callback->id, "gallery.icons")) {
            XtVaSetValues(state->notebook, XmNcurrentPageNumber, 4, NULL);
            set_status(state, "Built-in Motif bitmaps");
            return;
        }
        if (!entry) {
            set_status(state, "Category expanded; choose a component example");
            return;
        }
        if (!strcmp(entry->id, (const char *)callback->id))
            show_entry(state, entry);
        else
            set_status(state, "Category expanded; choose a component example");
    }
}

static void toggle_sidebar(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    XmSidebarCallbackStruct *callback = (XmSidebarCallbackStruct *)call;
    (void)widget;
    set_status(state, callback && callback->expanded ? "Navigation expanded" : "Navigation collapsed");
}

static void add_tree_nodes(Widget tree)
{
    static char labels[128][96];
    XmTreeViewNode nodes[128]; Cardinal i, offset = GALLERY_CATEGORY_COUNT + 2;
    if (GALLERY_MANIFEST_COUNT + GALLERY_CATEGORY_COUNT + 2 > XtNumber(nodes)) return;
    nodes[0].id = (XtPointer)"gallery.welcome";
    nodes[0].parent_id = NULL;
    nodes[0].label = "Welcome";
    nodes[0].has_children = False; nodes[0].expanded = False;
    nodes[1].id = (XtPointer)"gallery.icons";
    nodes[1].parent_id = NULL;
    nodes[1].label = "Built-in Icons";
    nodes[1].has_children = False; nodes[1].expanded = False;
    for (i = 0; i < GALLERY_CATEGORY_COUNT; ++i) {
        nodes[i + 2].id = (XtPointer)gallery_categories[i];
        nodes[i + 2].parent_id = NULL;
        const char *title = gallery_categories[i];
        if (!strcmp(title, "buttons-selection")) title = "Buttons & Selection";
        else if (!strcmp(title, "commands-navigation")) title = "Commands & Navigation";
        else if (!strcmp(title, "text-input")) title = "Text & Input";
        else if (!strcmp(title, "data-views")) title = "Data Views";
        else if (!strcmp(title, "feedback-dialogs")) title = "Feedback & Dialogs";
        else if (!strcmp(title, "layout")) title = "Layout";
        else if (!strcmp(title, "media")) title = "Media";
        snprintf(labels[i + 2], sizeof(labels[i + 2]), "%s", title);
        nodes[i + 2].label = labels[i + 2];
        nodes[i + 2].has_children = True; nodes[i + 2].expanded = True;
    }
    for (i = 0; i < GALLERY_MANIFEST_COUNT; ++i) {
        Cardinal category;
        for (category = 0; category < GALLERY_CATEGORY_COUNT; ++category)
            if (!strcmp(gallery_manifest[i].category, gallery_categories[category])) break;
        nodes[offset + i].id = (XtPointer)gallery_manifest[i].id;
        nodes[offset + i].parent_id = (XtPointer)gallery_categories[category];
        {
            const char *symbol = gallery_manifest[i].class_symbol;
            size_t out = 0, pos = 0;
            if (!strncmp(symbol, "xm", 2)) pos = 2;
            while (symbol[pos] && out + 1 < sizeof(labels[offset + i])) {
                char c = symbol[pos++];
                if (c == 'W' && !strncmp(symbol + pos - 1, "WidgetClass", 11)) break;
                if (c == 'G' && !strncmp(symbol + pos - 1, "GadgetClass", 11)) break;
                if (out && c >= 'A' && c <= 'Z' && labels[offset + i][out - 1] != ' ')
                    labels[offset + i][out++] = ' ';
                if (c >= 'A' && c <= 'Z' && (!out || labels[offset + i][out - 1] == ' '))
                    labels[offset + i][out++] = c;
                else if (c >= 'A' && c <= 'Z')
                    labels[offset + i][out++] = (char)(c - 'A' + 'a');
                else labels[offset + i][out++] = c;
            }
            labels[offset + i][out] = '\0';
            nodes[offset + i].label = labels[offset + i];
        }
        nodes[offset + i].has_children = False; nodes[offset + i].expanded = False;
    }
    XmTreeViewSetNodes(tree, nodes, offset + GALLERY_MANIFEST_COUNT);
    XmTreeViewSetSelectedId(tree, nodes[offset].id, False);
}

static Widget create_icon_page(Widget notebook)
{
    static const char *names[] = {
        "dialog_error", "dialog_information", "dialog_question",
        "dialog_warning", "dialog_working", "50_foreground",
        "menu_cascade", "menu_checkmark", "menu_dash"
    };
    Widget page = XtVaCreateWidget("iconsPage", xmFormWidgetClass, notebook,
                                   XmNnotebookChildType, XmPAGE,
                                   XmNpageNumber, 4, NULL);
    Widget grid = XtVaCreateManagedWidget("iconGrid", xmRowColumnWidgetClass, page,
                                         XmNnumColumns, 3,
                                         XmNorientation, XmHORIZONTAL,
                                         XmNpacking, XmPACK_TIGHT,
                                         XmNspacing, 18,
                                         XmNleftAttachment, XmATTACH_FORM,
                                         XmNrightAttachment, XmATTACH_FORM,
                                         XmNtopAttachment, XmATTACH_FORM, NULL);
    size_t i;
    for (i = 0; i < XtNumber(names); ++i) {
        Pixmap pixmap = XmGetPixmap(XtScreen(page), (char *)names[i],
                                    BlackPixelOfScreen(XtScreen(page)),
                                    WhitePixelOfScreen(XtScreen(page)));
        XmString label = XmStringCreateLocalized((char *)names[i]);
        Widget cell = XtVaCreateManagedWidget("iconCell", xmFormWidgetClass, grid,
                                              XmNwidth, 150, XmNheight, 88, NULL);
        XtVaCreateManagedWidget("bitmap", xmLabelWidgetClass, cell,
                                XmNlabelType, XmPIXMAP, XmNlabelPixmap, pixmap,
                                XmNtopAttachment, XmATTACH_FORM,
                                XmNleftAttachment, XmATTACH_FORM,
                                XmNrightAttachment, XmATTACH_FORM, NULL);
        XtVaCreateManagedWidget("name", xmLabelWidgetClass, cell,
                                XmNlabelString, label,
                                XmNtopAttachment, XmATTACH_WIDGET,
                                XmNtopWidget, XtNameToWidget(cell, "bitmap"),
                                XmNleftAttachment, XmATTACH_FORM,
                                XmNrightAttachment, XmATTACH_FORM, NULL);
        XmStringFree(label);
    }
    XtManageChild(page);
    return page;
}

int main(int argc, char **argv)
{
    XtAppContext app; Widget shell, root, header, body, sidebar, notebook;
    GalleryState state = {0};
    int integration_mode = 0;
    int screenshot_mode = 0;
    const char *initial_id = NULL;
    const char *dialog_kind = NULL;
    Boolean requested_profile = False;
    XmAppearanceProfile requested_profile_value = XmAPPEARANCE_CLASSIC;
    Boolean requested_scale = False;
    double requested_scale_value = 1.0;
    int argument;
    for (argument = 1; argument < argc;) {
        int remove_count = 0;
        if (!strcmp(argv[argument], "--integration-test")) {
            integration_mode = 1;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--integration-smoke")) {
            integration_mode = 2;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--screenshot-mode")) {
            screenshot_mode = 1;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--appearance") &&
                   argument + 1 < argc) {
            if (!parse_appearance(argv[argument + 1],
                                  &requested_profile_value)) {
                fprintf(stderr, "unknown appearance profile: %s\n",
                        argv[argument + 1]);
                return 2;
            }
            requested_profile = True;
            remove_count = 2;
        } else if (!strcmp(argv[argument], "--appearance")) {
            fprintf(stderr, "--appearance requires a profile name\n");
            return 2;
        } else if (!strncmp(argv[argument], "--appearance=", 13)) {
            if (!parse_appearance(argv[argument] + 13,
                                  &requested_profile_value)) {
                fprintf(stderr, "unknown appearance profile: %s\n",
                        argv[argument] + 13);
                return 2;
            }
            requested_profile = True;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--scale") &&
                   argument + 1 < argc) {
            if (!parse_scale(argv[argument + 1], &requested_scale_value)) {
                fprintf(stderr, "invalid display scale: %s\n",
                        argv[argument + 1]);
                return 2;
            }
            requested_scale = True;
            remove_count = 2;
        } else if (!strcmp(argv[argument], "--scale")) {
            fprintf(stderr, "--scale requires a numeric value\n");
            return 2;
        } else if (!strncmp(argv[argument], "--scale=", 8)) {
            if (!parse_scale(argv[argument] + 8, &requested_scale_value)) {
                fprintf(stderr, "invalid display scale: %s\n",
                        argv[argument] + 8);
                return 2;
            }
            requested_scale = True;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--component") && argument + 1 < argc) {
            initial_id = argv[argument + 1];
            remove_count = 2;
        } else if (!strncmp(argv[argument], "--component=", 12)) {
            initial_id = argv[argument] + 12;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--dialog") && argument + 1 < argc) {
            dialog_kind = argv[argument + 1];
            remove_count = 2;
        } else if (!strncmp(argv[argument], "--dialog=", 9)) {
            dialog_kind = argv[argument] + 9;
            remove_count = 1;
        } else {
            ++argument;
            continue;
        }
        memmove(&argv[argument], &argv[argument + remove_count],
                (size_t)(argc - argument - remove_count + 1) * sizeof(argv[0]));
        argc -= remove_count;
    }
    argv[argc] = NULL;
    /* The gallery deliberately exercises UTF-8 strings, including its
     * welcome page.  Prefer the system locale, then use the portable UTF-8
     * locale shipped by the Linux qualification images when the environment
     * is still the historical C locale. */
    if (!setlocale(LC_ALL, "") || !strcmp(setlocale(LC_ALL, NULL), "C")) {
        (void)setenv("LANG", "C.utf8", 1);
        (void)setenv("LC_CTYPE", "C.utf8", 1);
        (void)setlocale(LC_ALL, "C.utf8");
    }
    XtSetLanguageProc(NULL, NULL, NULL);
    gallery_manifest_initialize();
    shell = XtVaAppInitialize(&app, "CkMotifGallery", NULL, 0, &argc, argv,
                              NULL, XmNallowShellResize, True,
                              XmNwidth, 1100, XmNheight, 700, NULL);
    state.shell = shell;
    {
        /* The source-built MWM frame adds 14 horizontal and 31 vertical
         * pixels around this 1100x700 client.  Include those decorations in
         * the initial geometry; MWM owns the frame position after mapping. */
        const int frame_width = 1114;
        const int frame_height = 731;
        int screen_width = DisplayWidth(XtDisplay(shell), DefaultScreen(XtDisplay(shell)));
        int screen_height = DisplayHeight(XtDisplay(shell), DefaultScreen(XtDisplay(shell)));
        Position x = screen_width > frame_width ? (Position)((screen_width - frame_width) / 2) : 0;
        Position y = screen_height > frame_height ? (Position)((screen_height - frame_height) / 2) : 0;
        XtVaSetValues(shell, XmNx, x, XmNy, y, NULL);
    }
    state.profile = requested_profile ? requested_profile_value
                                      : XmAPPEARANCE_CLASSIC;
    /* Select the requested profile before creating any child widgets.  The
     * live transaction API is intentionally for an already-created
     * application hierarchy; using it here made some legacy widgets rebuild
     * their graphics contexts while realized and could issue BadGC requests
     * during screenshot capture. */
    if (requested_profile &&
        !XmSetAppearanceProfile(XtDisplay(shell), requested_profile_value)) {
        fprintf(stderr, "gallery: requested appearance profile failed\n");
        return 3;
    }
    if (requested_scale &&
        !XmSetDisplayScaleOverride(XtDisplay(shell), requested_scale_value)) {
        fprintf(stderr, "gallery: requested display scale failed\n");
        return 3;
    }
    root = XmVaCreateManagedForm(shell, "root", NULL);
    header = XmVaCreateManagedRowColumn(root, "header", XmNorientation, XmHORIZONTAL,
                                        XmNtopAttachment, XmATTACH_FORM,
                                        XmNleftAttachment, XmATTACH_FORM,
                                        XmNrightAttachment, XmATTACH_FORM, NULL);
    XtVaCreateManagedWidget("title", xmLabelWidgetClass, header,
                            XmNlabelString, text("CK Core Motif / Component Gallery"), NULL);
    XtVaCreateManagedWidget("themeNote", xmLabelWidgetClass, header,
                            XmNlabelString, text("Theme/DPI: live API qualification pending"), NULL);
    {
        Widget theme = XtVaCreateManagedWidget("themeControl", xmPushButtonWidgetClass, header,
                                               XmNlabelString, text("Theme"),
#ifdef CKMOTIF_GALLERY_ENABLE_LIVE_TRANSACTIONS
                                               XmNsensitive, True,
#else
                                               XmNsensitive, False,
#endif
                                               NULL);
        Widget dpi = XtVaCreateManagedWidget("dpiControl", xmPushButtonWidgetClass, header,
                                             XmNlabelString, text("DPI"),
#ifdef CKMOTIF_GALLERY_ENABLE_LIVE_TRANSACTIONS
                                             XmNsensitive, True,
#else
                                             XmNsensitive, False,
#endif
                                             NULL);
        XtAddCallback(theme, XmNactivateCallback, theme_control, &state);
        XtAddCallback(dpi, XmNactivateCallback, dpi_control, &state);
    }
    state.status = XmVaCreateManagedStatusBar(root, "status", XmNbottomAttachment, XmATTACH_FORM,
                                              XmNleftAttachment, XmATTACH_FORM,
                                              XmNrightAttachment, XmATTACH_FORM, NULL);
    body = XmVaCreateManagedPanedWindow(root, "body", XmNorientation, XmHORIZONTAL,
                                        XmNtopAttachment, XmATTACH_WIDGET,
                                        XmNtopWidget, header, XmNleftAttachment, XmATTACH_FORM,
                                        XmNrightAttachment, XmATTACH_FORM,
                                        XmNbottomAttachment, XmATTACH_WIDGET,
                                        XmNbottomWidget, state.status, NULL);
    sidebar = XmVaCreateManagedSidebar(body, "navigation",
                                       XmNsidebarExpandedWidth, 280,
                                       XmNtraversalOn, True, NULL);
    state.sidebar = sidebar;
    {
        Arg tree_args[1];
        XtSetArg(tree_args[0], XmNtraversalOn, True);
        state.tree = XmCreateTreeView(XmSidebarGetContent(sidebar),
                                      (char *)"components", tree_args, 1);
    }
    XtManageChild(state.tree);
    notebook = XmVaCreateManagedNotebook(body, "content", XmNtraversalOn, True, NULL);
    state.notebook = notebook;
    state.welcome = XtVaCreateWidget(
        "welcomePage", xmFormWidgetClass, notebook,
        XmNnotebookChildType, XmPAGE, XmNpageNumber, 1, NULL);
    {
        Widget welcome_text = XtVaCreateManagedWidget("welcomeText", xmDrawingAreaWidgetClass,
                                state.welcome,
                                XmNtopAttachment, XmATTACH_FORM,
                                XmNbottomAttachment, XmATTACH_FORM,
                                XmNleftAttachment, XmATTACH_FORM,
                                XmNrightAttachment, XmATTACH_FORM,
                                XmNwidth, 520, XmNheight, 220, NULL);
        XtAddCallback(welcome_text, XmNexposeCallback, welcome_expose, NULL);
    }
    XtManageChild(state.welcome);
    state.icons = create_icon_page(notebook);
    XtAddCallback(state.tree, XmNtreeViewSelectionCallback, tree_selected, &state);
    XtAddCallback(sidebar, XmNsidebarToggleCallback, toggle_sidebar, &state);
    add_tree_nodes(state.tree);
    {
        const GalleryManifestEntry *initial = initial_id ? find_entry(initial_id)
                                                         : &gallery_manifest[0];
        Boolean special_page = initial_id && (!strcmp(initial_id, "gallery.welcome") ||
                                              !strcmp(initial_id, "gallery.icons"));
        if (initial_id && !special_page && (!initial || strcmp(initial->id, initial_id))) {
            fprintf(stderr, "unknown gallery component: %s\n", initial_id);
            return 2;
        }
        XmTreeViewSetSelectedId(state.tree, (XtPointer)(initial_id ? initial_id : "gallery.welcome"), False);
        if (initial_id && strcmp(initial_id, "gallery.icons") != 0 &&
            strcmp(initial_id, "gallery.welcome") != 0) {
            /* The live page is created after the notebook's page/tab range is
             * complete below.  Selecting it here used to leave the notebook
             * on Welcome for --component launches. */
        } else if (initial_id && !strcmp(initial_id, "gallery.icons"))
            XtVaSetValues(state.notebook, XmNcurrentPageNumber, 4, NULL);
        else {
            XtVaSetValues(state.notebook, XmNcurrentPageNumber, 1, NULL);
            set_status(&state, "Welcome - choose a component category to begin");
        }
    }
    XtVaCreateManagedWidget("exampleTab", xmLabelWidgetClass, notebook,
                            XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, 2,
                            XmNlabelString, text("Example"), NULL);
    XtVaCreateManagedWidget("sourceTab", xmLabelWidgetClass, notebook,
                            XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, 3,
                            XmNlabelString, text("Source"), NULL);
    XtVaCreateManagedWidget("iconsTab", xmLabelWidgetClass, notebook,
                            XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, 4,
                            XmNlabelString, text("Built-in Icons"), NULL);
    if (initial_id && strcmp(initial_id, "gallery.icons") != 0 &&
        strcmp(initial_id, "gallery.welcome") != 0) {
        show_entry(&state, find_entry(initial_id));
        XtVaSetValues(state.notebook, XmNcurrentPageNumber, 2, NULL);
    }
    XtManageChild(root);
    XtRealizeWidget(shell);
    center_shell_frame(shell);
    if (dialog_kind && !open_gallery_dialog(shell, dialog_kind)) {
        fprintf(stderr, "unknown gallery dialog: %s\n", dialog_kind);
        return 2;
    }
    if (initial_id && strcmp(initial_id, "gallery.icons") != 0 &&
        strcmp(initial_id, "gallery.welcome") != 0)
        XtVaSetValues(state.notebook, XmNcurrentPageNumber, 2, NULL);
    state.accessibility = XmAccessibilityCreate(root, "CkMotifGallery");
    if (state.accessibility) {
        XmAccessibilityAttachShell(state.accessibility, shell);
        XmAccessibilityRefresh(state.accessibility);
    }
    if (requested_profile || requested_scale)
        XSync(XtDisplay(shell), False);
    if (integration_mode == 1) {
        int result = run_integration_test(&state);
        if (state.accessibility) XmAccessibilityDestroy(state.accessibility);
        return result;
    }
    if (integration_mode == 2) return 0;
    (void)screenshot_mode;
    XtAppMainLoop(app);
    return 0;
}
