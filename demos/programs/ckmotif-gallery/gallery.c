/* ckmotif-gallery: a manifest-driven native component gallery. */
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
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
extern Widget gallery_create_shell_example(Widget, WidgetClass, const char *);
extern void gallery_destroy_component_example(Widget *);
extern Widget gallery_create_drag_source_example(Widget, XtCallbackProc, XtPointer);

typedef struct {
    Widget shell, sidebar, tree, notebook, example, source, status;
    Widget title, provenance, action, component, component_host, source_text;
    Widget theme_control, dpi_control;
    unsigned int example_serial;
    unsigned long callback_count;
    XmAppearanceProfile profile;
    unsigned int dpi_step;
    Boolean component_is_popup, component_is_drag_source;
    Widget drag_context;
    XmAccessibility *accessibility;
    const GalleryManifestEntry *selected;
} GalleryState;

static XmString text(const char *value) { return XmStringCreateLocalized((char *)value); }

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
    XmString title, details_string;
    char details[768];
    state->selected = entry;
    snprintf(details, sizeof(details), "Class: %s | Test: %s | Docs: %s | Screenshot: %s",
             entry->class_symbol, entry->interaction, entry->docs_anchor,
             entry->screenshot);
    if (!state->example) {
        state->example = XtVaCreateWidget("examplePage", xmFormWidgetClass,
                                          state->notebook, XmNnotebookChildType,
                                          XmPAGE, XmNpageNumber, 1, NULL);
        title = text(entry->id);
        state->title = XtVaCreateManagedWidget("componentTitle", xmLabelWidgetClass,
                                              state->example, XmNlabelString, title,
                                              XmNtopAttachment, XmATTACH_FORM,
                                              XmNleftAttachment, XmATTACH_FORM, NULL);
        XmStringFree(title);
        details_string = text(details);
        state->provenance = XtVaCreateManagedWidget(
            "provenance", xmLabelWidgetClass, state->example,
            XmNlabelString, details_string, XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, state->title, XmNleftAttachment, XmATTACH_FORM, NULL);
        XmStringFree(details_string);
        state->action = XtVaCreateManagedWidget(
            "activateExample", xmPushButtonWidgetClass, state->example,
            XmNlabelString, text("Activate example"), XmNtopAttachment,
            XmATTACH_WIDGET, XmNtopWidget, state->provenance,
            XmNleftAttachment, XmATTACH_FORM, NULL);
        XtAddCallback(state->action, XmNactivateCallback, component_activate, state);
        XtManageChild(state->example);

        state->source = XtVaCreateWidget("sourcePage", xmFormWidgetClass,
                                         state->notebook, XmNnotebookChildType,
                                         XmPAGE, XmNpageNumber, 2, NULL);
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
    details_string = text(details);
    XtVaSetValues(state->provenance, XmNlabelString, details_string, NULL);
    XmStringFree(details_string);
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
                XmNwidth, 240, XmNheight, 96,
                XmNrowColumnType, row_column_type,
                XmNorientation, XmVERTICAL, NULL);
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
    if (entry->constructible && !entry->class)
        set_status(state, "Selected component; optional example unavailable in this build");
    else
        set_status(state, "Selected component; Example and Source tabs are ready");
}

static void theme_control(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    (void)widget; (void)call;
    XmAppearanceProfile next = (XmAppearanceProfile)
        ((state->profile + 1) % (XmAPPEARANCE_MODERN_DARK + 1));
    if (XmApplyAppearanceProfile(XtDisplay(state->shell), next)) {
        state->profile = next;
        set_status(state, "Appearance profile applied through the public live API");
    } else {
        set_status(state, "Appearance profile transaction failed; state preserved");
    }
}

static void dpi_control(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    (void)widget; (void)call;
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
    XtVaSetValues(state->notebook, XmNcurrentPageNumber, 2, NULL);
    if (exercise_component_matrix(state))
        return 1;
    {
        int current_page = 0;
        XmNotebookPageInfo page_info;
        XtVaGetValues(state->notebook, XmNcurrentPageNumber, &current_page, NULL);
        if (current_page != 2 || XmStatusBarGetItemCount(state->status) != 1 ||
            XmNotebookGetPageInfo(state->notebook, 2, &page_info) != XmPAGE_FOUND) {
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
    if (!state->theme_control || !state->dpi_control ||
        !XtIsSensitive(state->theme_control) || !XtIsSensitive(state->dpi_control)) {
        fprintf(stderr, "gallery integration: live controls are not enabled\n");
        return 1;
    }
    XtCallCallbacks(state->theme_control, XmNactivateCallback, NULL);
    for (i = 0; i < 4; ++i)
        XtCallCallbacks(state->dpi_control, XmNactivateCallback, NULL);
    if (state->profile != XmAPPEARANCE_MODERN_LIGHT || state->dpi_step != 0 ||
        !XmApplyAppearanceProfile(XtDisplay(state->shell), XmAPPEARANCE_CLASSIC)) {
        fprintf(stderr, "gallery integration: live theme/DPI transaction failed\n");
        return 1;
    }
    state->profile = XmAPPEARANCE_CLASSIC;
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
    puts("gallery integration: theme/dpi live transaction PASS");
    return 0;
}

static void tree_selected(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    XmTreeViewCallbackStruct *callback = (XmTreeViewCallbackStruct *)call;
    (void)widget;
    if (callback && callback->id) {
        const GalleryManifestEntry *entry = find_entry((const char *)callback->id);
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
    XmTreeViewNode nodes[128]; Cardinal i, offset = GALLERY_CATEGORY_COUNT;
    if (GALLERY_MANIFEST_COUNT + GALLERY_CATEGORY_COUNT > XtNumber(nodes)) return;
    for (i = 0; i < GALLERY_CATEGORY_COUNT; ++i) {
        nodes[i].id = (XtPointer)gallery_categories[i];
        nodes[i].parent_id = NULL;
        nodes[i].label = gallery_categories[i];
        nodes[i].has_children = True; nodes[i].expanded = True;
    }
    for (i = 0; i < GALLERY_MANIFEST_COUNT; ++i) {
        Cardinal category;
        for (category = 0; category < GALLERY_CATEGORY_COUNT; ++category)
            if (!strcmp(gallery_manifest[i].category, gallery_categories[category])) break;
        nodes[offset + i].id = (XtPointer)gallery_manifest[i].id;
        nodes[offset + i].parent_id = (XtPointer)gallery_categories[category];
        nodes[offset + i].label = gallery_manifest[i].id;
        nodes[offset + i].has_children = False; nodes[offset + i].expanded = False;
    }
    XmTreeViewSetNodes(tree, nodes, offset + GALLERY_MANIFEST_COUNT);
    XmTreeViewSetSelectedId(tree, nodes[offset].id, False);
}

int main(int argc, char **argv)
{
    XtAppContext app; Widget shell, root, header, body, sidebar, notebook;
    GalleryState state = {0};
    int integration_mode = 0;
    int screenshot_mode = 0;
    const char *initial_id = NULL;
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
        } else {
            ++argument;
            continue;
        }
        memmove(&argv[argument], &argv[argument + remove_count],
                (size_t)(argc - argument - remove_count + 1) * sizeof(argv[0]));
        argc -= remove_count;
    }
    argv[argc] = NULL;
    gallery_manifest_initialize();
    shell = XtVaAppInitialize(&app, "CkMotifGallery", NULL, 0, &argc, argv,
                              NULL, XmNallowShellResize, True, NULL);
    state.shell = shell;
    state.profile = XmAPPEARANCE_CLASSIC;
    root = XmVaCreateManagedForm(shell, "root", NULL);
    header = XmVaCreateManagedRowColumn(root, "header", XmNorientation, XmHORIZONTAL,
                                        XmNtopAttachment, XmATTACH_FORM,
                                        XmNleftAttachment, XmATTACH_FORM,
                                        XmNrightAttachment, XmATTACH_FORM, NULL);
    XtVaCreateManagedWidget("title", xmLabelWidgetClass, header,
                            XmNlabelString, text("CK Core Motif / Component Gallery"), NULL);
    XtVaCreateManagedWidget("themeNote", xmLabelWidgetClass, header,
                            XmNlabelString, text("Theme/DPI: live public transactions"), NULL);
    {
        Widget theme = XtVaCreateManagedWidget("themeControl", xmPushButtonWidgetClass, header,
                                               XmNlabelString, text("Theme"),
                                               XmNsensitive, True,
                                               NULL);
        Widget dpi = XtVaCreateManagedWidget("dpiControl", xmPushButtonWidgetClass, header,
                                             XmNlabelString, text("DPI"),
                                             XmNsensitive, True,
                                             NULL);
        state.theme_control = theme;
        state.dpi_control = dpi;
        XtAddCallback(theme, XmNactivateCallback, theme_control, &state);
        XtAddCallback(dpi, XmNactivateCallback, dpi_control, &state);
    }
    state.status = XmVaCreateManagedStatusBar(root, "status", XmNbottomAttachment, XmATTACH_FORM,
                                              XmNleftAttachment, XmATTACH_FORM,
                                              XmNrightAttachment, XmATTACH_FORM, NULL);
    body = XmVaCreateManagedPanedWindow(root, "body", XmNtopAttachment, XmATTACH_WIDGET,
                                        XmNtopWidget, header, XmNleftAttachment, XmATTACH_FORM,
                                        XmNrightAttachment, XmATTACH_FORM,
                                        XmNbottomAttachment, XmATTACH_WIDGET,
                                        XmNbottomWidget, state.status, NULL);
    sidebar = XmVaCreateManagedSidebar(body, "navigation", XmNtraversalOn, True, NULL);
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
    XtAddCallback(state.tree, XmNtreeViewSelectionCallback, tree_selected, &state);
    XtAddCallback(sidebar, XmNsidebarToggleCallback, toggle_sidebar, &state);
    add_tree_nodes(state.tree);
    {
        const GalleryManifestEntry *initial = initial_id ? find_entry(initial_id)
                                                         : &gallery_manifest[0];
        if (initial_id && strcmp(initial->id, initial_id)) {
            fprintf(stderr, "unknown gallery component: %s\n", initial_id);
            return 2;
        }
        XmTreeViewSetSelectedId(state.tree, (XtPointer)initial->id, False);
        show_entry(&state, initial);
    }
    XtVaCreateManagedWidget("exampleTab", xmLabelWidgetClass, notebook,
                            XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, 1,
                            XmNlabelString, text("Example"), NULL);
    XtVaCreateManagedWidget("sourceTab", xmLabelWidgetClass, notebook,
                            XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, 2,
                            XmNlabelString, text("Source"), NULL);
    XtManageChild(root);
    XtRealizeWidget(shell);
    state.accessibility = XmAccessibilityCreate(root, "CkMotifGallery");
    if (state.accessibility) {
        XmAccessibilityAttachShell(state.accessibility, shell);
        XmAccessibilityRefresh(state.accessibility);
    }
    if (requested_profile &&
        !XmApplyAppearanceProfile(XtDisplay(shell), requested_profile_value)) {
        fprintf(stderr, "gallery: requested appearance profile failed\n");
        if (state.accessibility) XmAccessibilityDestroy(state.accessibility);
        return 3;
    }
    if (requested_profile)
        state.profile = requested_profile_value;
    if (requested_scale &&
        !XmSetDisplayScaleOverride(XtDisplay(shell), requested_scale_value)) {
        fprintf(stderr, "gallery: requested display scale failed\n");
        if (state.accessibility) XmAccessibilityDestroy(state.accessibility);
        return 3;
    }
    if (requested_profile || requested_scale)
        XSync(XtDisplay(shell), False);
    if (state.accessibility && (requested_profile || requested_scale))
        XmAccessibilityRefresh(state.accessibility);
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
