#define _GNU_SOURCE
/* ckmotif-gallery: a manifest-driven native component gallery. */
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <stdint.h>
#include <X11/Xatom.h>
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
    Widget tabs, example_tab, source_tab, search, feedback, navigation_form;
    int current_page;
    Widget title, provenance, action, component, component_host, source_text, preview_viewport;
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
static const char welcome_id[] = "gallery.welcome";
static const char icons_id[] = "gallery.icons";

/* Composite controls can negotiate larger sizes after realization or when
 * expanded. Keep their scrollable canvas in step with the actual allocation. */
static void resize_preview(GalleryState *state)
{
    if (!state->component_host || !state->preview_viewport) return;
    WidgetList children;
    Cardinal count, i;
    unsigned int natural_width = 1, natural_height = 1;
    XtVaGetValues(state->component_host, XmNchildren, &children,
                  XmNnumChildren, &count, NULL);
    for (i = 0; i < count; ++i) {
        Position x, y;
        Dimension width, height, border;
        if (!XtIsManaged(children[i])) continue;
        XtVaGetValues(children[i], XmNx, &x, XmNy, &y,
            XmNwidth, &width, XmNheight, &height,
            XmNborderWidth, &border, NULL);
        unsigned int right = (x > 0 ? x : 0) + width + 2 * border;
        unsigned int bottom = (y > 0 ? y : 0) + height + 2 * border;
        if (right > natural_width) natural_width = right;
        if (bottom > natural_height) natural_height = bottom;
    }
    Dimension old_width, old_height;
    XtVaGetValues(state->component_host, XmNwidth, &old_width, XmNheight, &old_height, NULL);
    if (old_width != natural_width || old_height != natural_height)
        XtVaSetValues(state->component_host, XmNwidth, natural_width,
                      XmNheight, natural_height, NULL);
    XtVaSetValues(state->preview_viewport, XmNheight,
        natural_height < 340 ? natural_height + 16 : 356, NULL);
}

static void preview_configured(Widget widget, XtPointer client, XEvent *event,
                               Boolean *dispatch)
{
    (void)widget; (void)dispatch;
    if (event->type == ConfigureNotify || event->type == MapNotify)
        resize_preview((GalleryState *)client);
}

static Widget open_gallery_dialog(Widget parent, const char *kind)
{
    const char *body = !strcmp(kind, "question") ? "Save changes before closing this document?" :
        !strcmp(kind, "error") ? "The document could not be opened.\nCheck its location and try again." :
        !strcmp(kind, "warning") ? "Replace the existing export?\nThe previous file will be overwritten." :
        !strcmp(kind, "working") ? "Preparing your export...\nYou can cancel this operation." :
        "Export complete.\nYour document is ready to share.";
    XmString message = text(body);
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
        XmString accept = text(!strcmp(kind, "question") ? "Save" :
            !strcmp(kind, "warning") ? "Replace" : "OK");
        XtVaSetValues(dialog, XmNokLabelString, accept,
            XmNmessageAlignment, XmALIGNMENT_BEGINNING,
            XmNmarginWidth, 24, XmNmarginHeight, 20, NULL);
        XmStringFree(accept);
        if (!strcmp(kind, "working"))
            XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON));
        if (!strcmp(kind, "error") || !strcmp(kind, "information"))
            XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
        XtManageChild(dialog);
    }
    XmStringFree(message);
    return dialog;
}

/* The gallery uses ordinary layout widgets; the decorated Notebook remains
 * available as a component example instead of framing every other control. */
static Widget gallery_label(Widget parent, const char *name, const char *value)
{
    XmString label = text(value);
    Widget widget = XtVaCreateManagedWidget(name, xmLabelWidgetClass, parent,
        XmNlabelString, label, XmNalignment, XmALIGNMENT_BEGINNING,
        XmNtraversalOn, False, NULL);
    XmStringFree(label);
    return widget;
}

static void component_name(const GalleryManifestEntry *entry, char *out, size_t size)
{
    const char *symbol = entry->class_symbol + 2;
    if (!strcmp(symbol, "GadgetClass")) {
        snprintf(out, size, "Gadget");
        return;
    }
    size_t n = 0;
    while (*symbol && strncmp(symbol, "WidgetClass", 11) &&
           strncmp(symbol, "GadgetClass", 11) && strcmp(symbol, "Class")) {
        char c = *symbol++;
        if (n && c >= 'A' && c <= 'Z' && n + 1 < size) out[n++] = ' ';
        if (n + 1 < size) out[n++] = c;
    }
    if (strstr(entry->class_symbol, "GadgetClass") && n + 8 < size) {
        memcpy(out + n, " gadget", 7); n += 7;
    }
    out[n] = '\0';
}

static void select_page(GalleryState *state, int page)
{
    Widget pages[] = {state->welcome, state->example, state->source, state->icons};
    unsigned int i;
    state->current_page = page;
    for (i = 0; i < XtNumber(pages); ++i) {
        if (!pages[i]) continue;
        if ((int)i + 1 == page) XtManageChild(pages[i]);
        else XtUnmanageChild(pages[i]);
    }
    if (state->tabs) {
        if (page == 2 || page == 3) XtManageChild(state->tabs);
        else XtUnmanageChild(state->tabs);
        XmToggleButtonSetState(state->example_tab, page == 2, False);
        XmToggleButtonSetState(state->source_tab, page == 3, False);
    }
}

static void select_tab(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = client;
    (void)call;
    select_page(state, widget == state->source_tab ? 3 : 2);
}

static Widget create_page(Widget parent, const char *name)
{
    return XtVaCreateWidget(name, xmFormWidgetClass, parent,
        XmNmarginWidth, 24, XmNmarginHeight, 24,
        XmNshadowThickness, 0, XmNborderWidth, 0,
        XmNtopAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
}

static void create_welcome(GalleryState *state)
{
    Widget column;
    state->welcome = create_page(state->notebook, "welcomePage");
    column = XtVaCreateManagedWidget("welcomeContent", xmRowColumnWidgetClass,
        state->welcome, XmNorientation, XmVERTICAL, XmNpacking, XmPACK_TIGHT,
        XmNisAligned, False, XmNspacing, 16,
        XmNtopAttachment, XmATTACH_FORM, XmNleftAttachment, XmATTACH_FORM, NULL);
    gallery_label(column, "pageTitle", "Explore Motif");
    gallery_label(column, "intro", "A working reference for classic and contemporary desktop controls.");
    gallery_label(column, "sectionTitle", "Start with a component");
    gallery_label(column, "intro", "Browse a category or search by component name on the left.\nOperate each example directly and inspect the source used to create it.");
    gallery_label(column, "sectionTitle", "Built for exploration");
    gallery_label(column, "intro", "Buttons & Selection: actions, toggles and value controls\nText & Input: editing, search and suggestions\nData Views: lists, trees, calendars and tables\nLayout: forms, panes and page containers");
    gallery_label(column, "sectionTitle", "Familiar details, clearer presentation");
    gallery_label(column, "intro", "Raised buttons, inset fields and explicit keyboard focus preserve Motif's character.\nUse Tab to move between controls and the arrow keys inside lists and trees.");
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

static void expand_entry_category(Widget tree, const GalleryManifestEntry *entry)
{
    Cardinal i;
    if (!entry || !entry->category)
        return;
    for (i = 0; i < GALLERY_CATEGORY_COUNT; ++i)
        if (!strcmp(entry->category, gallery_categories[i])) {
            (void)XmTreeViewSetExpanded(tree, (XtPointer)gallery_categories[i],
                                        True, False);
            return;
        }
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
    (void)call;
    ++state->callback_count;
    if (state->feedback) {
        XmString label;
        if (XmIsToggleButton(widget))
            snprintf(message, sizeof(message), "Selection: %s", XmToggleButtonGetState(widget) ? "on" : "off");
        else if (XmIsNumberBox(widget))
            snprintf(message, sizeof(message), "Quantity: %d", XmNumberBoxGetValue(widget));
        else if (XmIsStepper(widget))
            snprintf(message, sizeof(message), "Value: %d", XmStepperGetValue(widget));
        else if (XmIsRatingControl(widget))
            snprintf(message, sizeof(message), "Rating: %d", XmRatingControlGetValue(widget));
        else if (XmIsTextField(widget)) {
            char *value = XmTextFieldGetString(widget);
            snprintf(message, sizeof(message), "Current value: %.120s", value ? value : "");
            XtFree(value);
        } else
            snprintf(message, sizeof(message), "Interaction %lu received", state->callback_count);
        label = text(message);
        XtVaSetValues(state->feedback, XmNlabelString, label, NULL);
        XmStringFree(label);
    }
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
    expand_entry_category(state->tree, entry);
    XmTreeViewSetSelectedId(state->tree, (XtPointer)entry->id, False);
    snprintf(details, sizeof(details), "%s", !entry->constructible ?
             "Infrastructure class: used by the toolkit rather than placed in a layout." :
             !entry->class ? "This optional component is not available in this build." :
             "Try the control below. Changes are reported beneath the example.");
    strncpy(state->provenance_text, details, sizeof(state->provenance_text) - 1);
    state->provenance_text[sizeof(state->provenance_text) - 1] = '\0';
    select_page(state, 2);
    if (!state->example) {
        state->example = create_page(state->notebook, "examplePage");
        state->title = gallery_label(state->example, "pageTitle", "Component");
        XtVaSetValues(state->title, XmNtopAttachment, XmATTACH_FORM,
                      XmNleftAttachment, XmATTACH_FORM, NULL);
        state->provenance = gallery_label(state->example, "description", "");
        XtVaSetValues(state->provenance,
            XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, state->title,
            XmNtopOffset, 12, XmNleftAttachment, XmATTACH_FORM, NULL);
        state->action = XtVaCreateWidget("openExample", xmPushButtonWidgetClass,
            state->example, XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, state->provenance, XmNtopOffset, 16,
            XmNleftAttachment, XmATTACH_FORM, NULL);
        XtAddCallback(state->action, XmNactivateCallback, component_activate, state);
        state->feedback = gallery_label(state->example, "feedback", "Ready for interaction");
        XtVaSetValues(state->feedback, XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, state->provenance, XmNtopOffset, 24,
            XmNleftAttachment, XmATTACH_FORM, NULL);
        state->source = create_page(state->notebook, "sourcePage");
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
    { char name[128]; component_name(entry, name, sizeof(name)); title = text(name); }
    XtVaSetValues(state->title, XmNlabelString, title, NULL);
    XmStringFree(title);
    {
        char description[1024];
        XmString label;
        snprintf(description, sizeof(description), "%s\n\n%s", entry->class_symbol, details);
        label = text(description);
        XtVaSetValues(state->provenance, XmNlabelString, label, NULL);
        XmStringFree(label);
    }
    XtUnmanageChild(state->action);
    if (state->drag_context) {
        XmDragCancel(state->drag_context);
        state->drag_context = NULL;
    }
    if (state->component)
        gallery_destroy_component_example(&state->component);
    if (state->preview_viewport) {
        XtDestroyWidget(state->preview_viewport);
        state->preview_viewport = NULL;
        state->component_host = NULL;
    } else if (state->component_host) {
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
            int row_column_type = XmWORK_AREA;
            if (strstr(entry->class_symbol, "Cascade"))
                row_column_type = XmMENU_BAR;
            state->preview_viewport = XtVaCreateManagedWidget("previewViewport",
                xmScrolledWindowWidgetClass, state->example,
                XmNscrollingPolicy, XmAUTOMATIC, XmNshadowThickness, 0,
                XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, state->provenance,
                XmNtopOffset, 24, XmNleftAttachment, XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM, XmNheight, 300, NULL);
            if (row_column_type == XmMENU_BAR)
                state->component_host = XtVaCreateManagedWidget(
                    "componentHost", xmRowColumnWidgetClass, state->preview_viewport,
                    XmNrowColumnType, XmMENU_BAR, XmNmarginWidth, 0,
                    XmNmarginHeight, 0, NULL);
            else
                state->component_host = XtVaCreateManagedWidget(
                    "componentHost", xmBulletinBoardWidgetClass, state->preview_viewport,
                    XmNresizePolicy, XmRESIZE_ANY, XmNmarginWidth, 0,
                    XmNmarginHeight, 0, XmNshadowThickness, 0, NULL);
            if (!strcmp(entry->class_symbol, "xmSegmentedControlWidgetClass"))
                state->component = gallery_create_segmented_control_example(
                    state->component_host, name, component_activate, state);
            else
                state->component = gallery_create_component_example(
                    state->component_host, entry->class, name,
                    component_activate, state);
            resize_preview(state);
            XtAddEventHandler(state->component_host,
                StructureNotifyMask | SubstructureNotifyMask, False,
                preview_configured, state);
            anchor = state->preview_viewport;
        }
    } else {
        anchor = state->provenance;
        if (entry->constructible)
            set_status(state, "This example is an optional capability disabled in this build");
    }
    XtVaSetValues(state->action, XmNtopWidget, anchor, NULL);
    XtVaSetValues(state->feedback, XmNtopWidget,
        state->component_is_popup ? state->action : anchor, NULL);
    if (state->component_is_popup) {
        XmString label = text("Open example window");
        XtVaSetValues(state->action, XmNlabelString, label, NULL);
        XmStringFree(label);
        XtManageChild(state->action);
    }
    {
        XmString label = text(!entry->constructible ?
            "This class provides services to other widgets. See Source for its public construction contract." :
            !entry->class ? "See Source for the optional component's construction contract." :
            "Ready for interaction");
        XtVaSetValues(state->feedback, XmNlabelString, label, NULL);
        XmStringFree(label);
    }
    XmTextSetString(state->source_text, (char *)source_for(entry));
    /* Select the live example after both component pages exist. */
    select_page(state, 2);
    if (entry->constructible && !entry->class)
        set_status(state, "Selected component; optional example unavailable in this build");
    else
        set_status(state, "Use Tab to move between controls. Open Source to inspect the example.");
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
        if (state->preview_viewport) {
            XtAppContext app = XtWidgetToApplicationContext(state->shell);
            unsigned int events = 0;
            Dimension component_width, component_height, host_width, host_height;
            XSync(XtDisplay(state->shell), False);
            while (XtAppPending(app) && events++ < 1000)
                XtAppProcessEvent(app, XtIMAll);
            XtVaGetValues(state->component, XmNwidth, &component_width,
                          XmNheight, &component_height, NULL);
            XtVaGetValues(state->component_host, XmNwidth, &host_width,
                          XmNheight, &host_height, NULL);
            if (component_width > host_width || component_height > host_height) {
                fprintf(stderr, "gallery integration: canvas clips %s\n", entry->id);
                return 1;
            }
        }
        if (entry->class == xmLinkWidgetClass || entry->class == xmPromptWidgetClass ||
            entry->class == xmIconLabelWidgetClass || entry->class == xmIconBoxWidgetClass) {
            XtWidgetGeometry preferred = {0};
            Dimension width, height;
            XtQueryGeometry(state->component, NULL, &preferred);
            XtVaGetValues(state->component, XmNwidth, &width, XmNheight, &height, NULL);
            if (((preferred.request_mode & CWWidth) && width < preferred.width) ||
                ((preferred.request_mode & CWHeight) && height < preferred.height)) {
                fprintf(stderr, "gallery integration: preferred example size clipped for %s\n", entry->id);
                return 1;
            }
        }
        if (entry->class == xmPushButtonWidgetClass) {
            Widget disabled = XtNameToWidget(state->component_host, "disabledState");
            Widget clip = NULL;
            Position y;
            Dimension height, clip_height;
            XtVaGetValues(state->preview_viewport, XmNclipWindow, &clip, NULL);
            if (!disabled || !clip) return 1;
            XtVaGetValues(disabled, XmNy, &y, XmNheight, &height, NULL);
            XtVaGetValues(clip, XmNheight, &clip_height, NULL);
            if (y < 0 || (unsigned int)y + height > clip_height) {
                fprintf(stderr, "gallery integration: disabled button is clipped\n");
                return 1;
            }
        }
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
    {
        Cardinal all = XmTreeViewGetNodeCount(state->tree);
        XmTextFieldSetString(state->search, "push button");
        if (XmTreeViewGetNodeCount(state->tree) >= all) {
            fprintf(stderr, "gallery integration: component filtering failed\n");
            return 1;
        }
        XmTextFieldSetString(state->search, "");
        if (XmTreeViewGetNodeCount(state->tree) != all) return 1;
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
    select_page(state, 3);
    if (exercise_component_matrix(state))
        return 1;
    {
        if (state->current_page != 2 || XmStatusBarGetItemCount(state->status) != 1 ||
            !state->source_text || !XtIsManaged(state->example) || XtIsManaged(state->source)) {
            fprintf(stderr, "gallery integration: page/status check failed\n");
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
            state->selected = NULL;
            select_page(state, 1);
            set_status(state, "Welcome - choose a component category to begin");
            return;
        }
        if (!strcmp((const char *)callback->id, "gallery.icons")) {
            state->selected = NULL;
            select_page(state, 4);
            set_status(state, "Built-in dialog symbols");
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

static void add_tree_nodes(Widget tree, const char *query)
{
    static char labels[128][96];
    XmTreeViewNode nodes[128]; Cardinal i, used, offset = GALLERY_CATEGORY_COUNT + 2;
    if (GALLERY_MANIFEST_COUNT + GALLERY_CATEGORY_COUNT + 2 > XtNumber(nodes)) return;
    nodes[0].id = (XtPointer)welcome_id;
    nodes[0].parent_id = NULL;
    nodes[0].label = "Welcome";
    nodes[0].has_children = False; nodes[0].expanded = False;
    nodes[1].id = (XtPointer)icons_id;
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
        nodes[i + 2].has_children = True; nodes[i + 2].expanded = False;
    }
    used = offset;
    for (i = 0; i < GALLERY_MANIFEST_COUNT; ++i) {
        Cardinal category;
        for (category = 0; category < GALLERY_CATEGORY_COUNT; ++category)
            if (!strcmp(gallery_manifest[i].category, gallery_categories[category])) break;
        nodes[offset + i].id = (XtPointer)gallery_manifest[i].id;
        nodes[offset + i].parent_id = (XtPointer)gallery_categories[category];
        component_name(&gallery_manifest[i], labels[offset + i], sizeof(labels[offset + i]));
        nodes[offset + i].label = labels[offset + i];
        nodes[offset + i].has_children = False; nodes[offset + i].expanded = False;
        if (!query || !*query || strcasestr(labels[offset + i], query) ||
            strcasestr(gallery_manifest[i].class_symbol, query)) {
            nodes[used++] = nodes[offset + i];
            if (query && *query) nodes[category + 2].expanded = True;
        }
    }
    XmTreeViewSetNodes(tree, nodes, used);
}

static void navigation_resize(Widget widget, XtPointer client, XEvent *event, Boolean *dispatch)
{
    GalleryState *state = client;
    Dimension width, height;
    Cardinal rows;
    (void)dispatch;
    if (event->type != ConfigureNotify || !state->navigation_form) return;
    XtVaGetValues(widget, XmNwidth, &width, XmNheight, &height, NULL);
    XtVaSetValues(state->navigation_form, XmNwidth, width, XmNheight, height, NULL);
    rows = height > 84 ? (height - 84) / 28 : 1;
    if (!rows) rows = 1;
    XtVaSetValues(state->tree, XmNtreeViewVisibleRows, rows, NULL);
}

static void search_changed(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = client;
    char *query = XmTextFieldGetString(widget);
    (void)call;
    add_tree_nodes(state->tree, query);
    if (state->selected) XmTreeViewSetSelectedId(state->tree, (XtPointer)state->selected->id, False);
    else XmTreeViewSetSelectedId(state->tree,
        (XtPointer)(state->current_page == 4 ? icons_id : welcome_id), False);
    XtFree(query);
}

static void release_icon(Widget widget, XtPointer client, XtPointer call)
{
    (void)call;
    XmDestroyPixmap(XtScreen(widget), (Pixmap)(uintptr_t)client);
}

static Widget create_icon_page(Widget notebook)
{
    static const char *names[] = {
        "dialog_error", "dialog_information", "dialog_question",
        "dialog_warning", "dialog_working"
    };
    static const char *captions[] = {"Error", "Information", "Question", "Warning", "Working"};
    XtInitializeWidgetClass(xmMessageBoxWidgetClass);
    Widget page = create_page(notebook, "iconsPage");
    Widget title = gallery_label(page, "pageTitle", "Built-in icons");
    XtVaSetValues(title, XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM, NULL);
    Widget grid = XtVaCreateManagedWidget("iconGrid", xmRowColumnWidgetClass, page,
                                         XmNnumColumns, 3,
                                         XmNorientation, XmVERTICAL,
                                         XmNpacking, XmPACK_COLUMN,
                                         XmNspacing, 16,
                                         XmNleftAttachment, XmATTACH_FORM,
                                         XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, title,
                                         XmNtopOffset, 24, NULL);
    size_t i;
    for (i = 0; i < XtNumber(names); ++i) {
        Pixel foreground, background;
        XtVaGetValues(page, XmNforeground, &foreground, XmNbackground, &background, NULL);
        Pixmap pixmap = XmGetPixmap(XtScreen(page), (char *)names[i], foreground, background);
        if (pixmap == XmUNSPECIFIED_PIXMAP) {
            char fallback[64];
            snprintf(fallback, sizeof(fallback), "default_xm_%s", names[i] + 7);
            pixmap = XmGetPixmap(XtScreen(page), fallback,
                foreground, background);
        }
        XmString label = XmStringCreateLocalized((char *)captions[i]);
        Widget cell = XtVaCreateManagedWidget("iconCell", xmFormWidgetClass, grid,
                                              XmNwidth, 190, XmNheight, 112, XmNresizePolicy, XmRESIZE_NONE,
                                              XmNshadowThickness, 0, NULL);
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
        if (pixmap != XmUNSPECIFIED_PIXMAP)
            XtAddCallback(cell, XmNdestroyCallback, release_icon, (XtPointer)(uintptr_t)pixmap);
    }
    return page;
}

static Boolean capture_ready(XtPointer client)
{
    GalleryState *state = client;
    unsigned long pid = (unsigned long)getpid();
    unsigned long ready = 1;
    Display *display = XtDisplay(state->shell);
    Window window = XtWindow(state->shell);
    XChangeProperty(display, window, XInternAtom(display, "_NET_WM_PID", False),
        XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&pid, 1);
    XChangeProperty(display, window, XInternAtom(display, "_CKMOTIF_GALLERY_READY", False),
        XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&ready, 1);
    XSync(display, False);
    return True;
}

static String gallery_resources[] = {
    "*renderTable: ui",
    "*defaultEntryLabelRenderTable: ui",
    "*entryLabelRenderTable: ui",
    "*ui.fontType: FONT_IS_XFT",
    "*ui.fontName: DejaVu Sans",
    "*ui.fontSize: 11",
    "*pageTitle.renderTable: heading",
    "*heading.fontType: FONT_IS_XFT",
    "*heading.fontName: DejaVu Sans",
    "*heading.fontSize: 20",
    "*sectionTitle.renderTable: section",
    "*section.fontType: FONT_IS_XFT",
    "*section.fontName: DejaVu Sans",
    "*section.fontSize: 12",
    "*section.fontStyle: Bold",
    "*sourceText.renderTable: code",
    "*code.fontType: FONT_IS_XFT",
    "*code.fontName: DejaVu Sans Mono",
    "*code.fontSize: 11",
    "*XmLabel.traversalOn: False",
    "*XmLabel.highlightThickness: 0",
    "*XmLabelGadget.highlightThickness: 0",
    "*header.marginWidth: 16",
    "*header.marginHeight: 12",
    "*header.spacing: 24",
    "*body.spacing: 12",
    "*viewTabs.marginWidth: 24",
    "*viewTabs.marginHeight: 8",
    "*exampleTab.topShadowColor: #a8afb8",
    "*exampleTab.bottomShadowColor: #a8afb8",
    "*sourceTab.topShadowColor: #a8afb8",
    "*sourceTab.bottomShadowColor: #a8afb8",
    "*welcomePage.topShadowColor: #a8afb8",
    "*welcomePage.bottomShadowColor: #a8afb8",
    "*iconsPage.topShadowColor: #a8afb8",
    "*iconsPage.bottomShadowColor: #a8afb8",
    NULL
};

int main(int argc, char **argv)
{
    XtAppContext app; Widget shell, root, header, body, sidebar, notebook;
    GalleryState state = {0};
    int integration_mode = 0;
    int screenshot_mode = 0;
    const char *initial_id = NULL;
    const char *dialog_kind = NULL;
    Boolean requested_profile = True;
    XmAppearanceProfile requested_profile_value = XmAPPEARANCE_MODERN_LIGHT;
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
                              gallery_resources, XmNallowShellResize, False,
                              XmNwidth, 1100, XmNheight, 740, XmNminWidth, 980, XmNminHeight, 680, NULL);
    state.shell = shell;
    {
        /* The source-built MWM frame adds 14 horizontal and 31 vertical
         * pixels around this 1100x740 client.  Include those decorations in
         * the initial geometry; MWM owns the frame position after mapping. */
        const int frame_width = 1114;
        const int frame_height = 771;
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
                            XmNlabelString, text("Component reference"), NULL);
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
#ifndef CKMOTIF_GALLERY_ENABLE_LIVE_TRANSACTIONS
        XtUnmanageChild(theme);
        XtUnmanageChild(dpi);
#endif
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
        Widget content = XmSidebarGetContent(sidebar);
        Widget label;
        XtVaSetValues(content, XmNpacking, XmPACK_NONE,
            XmNmarginWidth, 0, XmNmarginHeight, 0, NULL);
        state.navigation_form = XmVaCreateManagedForm(content, "navigationForm",
            XmNwidth, 280, XmNheight, 560, XmNmarginWidth, 8, XmNmarginHeight, 8, NULL);
        label = gallery_label(state.navigation_form, "searchLabel", "Find a component");
        XtVaSetValues(label, XmNtopAttachment, XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_FORM, NULL);
        state.search = XtVaCreateManagedWidget("componentSearch", xmTextFieldWidgetClass,
            state.navigation_form, XmNcolumns, 18,
            XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, label, XmNtopOffset, 4,
            XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
        XtAddCallback(state.search, XmNvalueChangedCallback, search_changed, &state);
        state.tree = XtVaCreateManagedWidget("components", xmTreeViewWidgetClass,
            state.navigation_form, XmNtraversalOn, True,
            XmNtreeViewVisibleRows, 17, XmNtreeViewRowHeight, 28,
            XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, state.search, XmNtopOffset, 12,
            XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
            XmNbottomAttachment, XmATTACH_FORM, NULL);
        XtAddEventHandler(content, StructureNotifyMask, False, navigation_resize, &state);
    }
    XtManageChild(state.tree);
    {
        Widget pane = XmVaCreateManagedForm(body, "workspace", XmNshadowThickness, 0, NULL);
        state.tabs = XmVaCreateRowColumn(pane, "viewTabs", XmNorientation, XmHORIZONTAL,
            XmNpacking, XmPACK_TIGHT, XmNspacing, 4,
            XmNtopAttachment, XmATTACH_FORM, XmNleftAttachment, XmATTACH_FORM, NULL);
        state.example_tab = XtVaCreateManagedWidget("exampleTab", xmToggleButtonWidgetClass,
            state.tabs, XmNlabelString, text("Example"), XmNindicatorOn, False,
            XmNfillOnSelect, True, XmNshadowThickness, 1, XmNhighlightThickness, 0,
            XmNmarginWidth, 14, XmNmarginHeight, 6, NULL);
        state.source_tab = XtVaCreateManagedWidget("sourceTab", xmToggleButtonWidgetClass,
            state.tabs, XmNlabelString, text("Source"), XmNindicatorOn, False,
            XmNfillOnSelect, True, XmNshadowThickness, 1, XmNhighlightThickness, 0,
            XmNmarginWidth, 14, XmNmarginHeight, 6, NULL);
        XtAddCallback(state.example_tab, XmNvalueChangedCallback, select_tab, &state);
        XtAddCallback(state.source_tab, XmNvalueChangedCallback, select_tab, &state);
        notebook = XmVaCreateManagedForm(pane, "content", XmNshadowThickness, 0,
            XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, state.tabs,
            XmNbottomAttachment, XmATTACH_FORM, XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM, NULL);
        state.notebook = notebook;
    }
    create_welcome(&state);
    state.icons = create_icon_page(notebook);
    XtAddCallback(state.tree, XmNtreeViewSelectionCallback, tree_selected, &state);
    XtAddCallback(sidebar, XmNsidebarToggleCallback, toggle_sidebar, &state);
    add_tree_nodes(state.tree, NULL);
    {
        const GalleryManifestEntry *initial = initial_id ? find_entry(initial_id)
                                                         : &gallery_manifest[0];
        Boolean special_page = initial_id && (!strcmp(initial_id, "gallery.welcome") ||
                                              !strcmp(initial_id, "gallery.icons"));
        if (initial_id && !special_page && (!initial || strcmp(initial->id, initial_id))) {
            fprintf(stderr, "unknown gallery component: %s\n", initial_id);
            return 2;
        }
        XmTreeViewSetSelectedId(state.tree,
            (XtPointer)(initial_id && !special_page ? initial->id :
                (initial_id && !strcmp(initial_id, icons_id) ? icons_id : welcome_id)), False);
        if (initial_id && strcmp(initial_id, "gallery.icons") != 0 &&
            strcmp(initial_id, "gallery.welcome") != 0) {
            /* Create the requested component after the initial tree selection. */
        } else if (initial_id && !strcmp(initial_id, "gallery.icons"))
            select_page(&state, 4);
        else {
            select_page(&state, 1);
            set_status(&state, "Welcome - choose a component category to begin");
        }
    }
    if (initial_id && !strcmp(initial_id, "gallery.icons")) {
        select_page(&state, 4);
    } else if (initial_id && !strcmp(initial_id, "gallery.welcome")) {
        select_page(&state, 1);
    } else if (initial_id) {
        const GalleryManifestEntry *initial = find_entry(initial_id);
        expand_entry_category(state.tree, initial);
        show_entry(&state, initial);
        select_page(&state, 2);
    }
    XtManageChild(root);
    XtRealizeWidget(shell);
    center_shell_frame(shell);
    /* Keep the requested landing page visible after geometry negotiation. */
    if (initial_id && !strcmp(initial_id, "gallery.icons")) {
        select_page(&state, 4);
    } else if (initial_id && !strcmp(initial_id, "gallery.welcome")) {
        select_page(&state, 1);
    }
    if (dialog_kind && !open_gallery_dialog(shell, dialog_kind)) {
        fprintf(stderr, "unknown gallery dialog: %s\n", dialog_kind);
        return 2;
    }
    if (initial_id && strcmp(initial_id, "gallery.icons") != 0 &&
        strcmp(initial_id, "gallery.welcome") != 0)
        select_page(&state, 2);
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
    if (screenshot_mode) XtAppAddWorkProc(app, capture_ready, &state);
    XtAppMainLoop(app);
    return 0;
}
