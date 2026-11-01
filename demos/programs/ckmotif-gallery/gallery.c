#define _GNU_SOURCE
/* ckmotif-gallery: a manifest-driven native component gallery. */
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <signal.h>
#include "gallery_demos.h"
#include "demo_launch.h"
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <Xm/XmAll.h>
#include <Xm/Sidebar.h>
#include <Xm/SegmentedControl.h>
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
extern void gallery_refresh_icon_resources(void);
extern Boolean gallery_validate_icon_resources(void);
extern Boolean gallery_bind_named_icon(Widget, const char *, const char *, const char *);
extern Widget gallery_create_drag_source_example(Widget, XtCallbackProc, XtPointer);

typedef struct {
    Widget viewport, canvas, clip;
    Widget items[9];
    const char *paragraphs[9];
    Cardinal count;
    Dimension measured_width;
    Boolean grid, laying_out;
} GalleryFlowPage;

typedef struct {
    GalleryFlowPage welcome_flow, icons_flow;
    Widget shell, sidebar, tree, notebook, welcome, example, source, icons, demos, status;
    Widget tabs, search, feedback, navigation_form;
    Widget body, workspace, app_title, theme_note;
    Dimension navigation_width;
    Boolean compact_layout, desktop_navigation_expanded, updating_layout;
    int current_page;
    Widget title, provenance, action, component, component_host, source_text, preview_viewport;
    unsigned int example_serial;
    unsigned long callback_count;
    char provenance_text[768];
    Widget theme_control, scale_control;
    Boolean component_is_popup, component_is_drag_source;
    Widget drag_context;
    XmAccessibility *accessibility;
    const GalleryManifestEntry *selected;
} GalleryState;

static void layout_gallery(GalleryState *state);
static Boolean settle_gallery(GalleryState *state);
static void close_compact_navigation(GalleryState *state);

static XmString text(const char *value) { return XmStringCreateLocalized((char *)value); }
static const char welcome_id[] = "gallery.welcome";
static const char icons_id[] = "gallery.icons";
static const char demos_id[] = "gallery.demos";

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
    /* RowColumn owns spacing and frame insets around its children. Their
     * bounding box alone is not a legal preferred size for a menu bar: asking
     * for it on each ConfigureNotify causes repeated geometry negotiation. */
    if (XmIsRowColumn(state->component_host)) {
        XtWidgetGeometry preferred = {0};
        XtQueryGeometry(state->component_host, NULL, &preferred);
        if (preferred.request_mode & CWWidth) natural_width = preferred.width;
        if (preferred.request_mode & CWHeight) natural_height = preferred.height;
    }
    Dimension old_width, old_height;
    XtVaGetValues(state->component_host, XmNwidth, &old_width, XmNheight, &old_height, NULL);
    if (old_width != natural_width || old_height != natural_height)
        XtVaSetValues(state->component_host, XmNwidth, natural_width,
                      XmNheight, natural_height, NULL);
    /* Keep short examples compact; use the remaining page as the maximum
     * viewport for large examples. The scroll frame owns its chrome metrics. */
    Widget clip = NULL;
    Position top;
    Dimension page_height, feedback_height, viewport_height, clip_height;
    int gap;
    XtVaGetValues(state->preview_viewport, XmNy, &top, XmNheight, &viewport_height,
                  XmNclipWindow, &clip, NULL);
    XtVaGetValues(state->example, XmNheight, &page_height, NULL);
    XtVaGetValues(state->feedback, XmNheight, &feedback_height, XmNtopOffset, &gap, NULL);
    if (!XtIsManaged(state->feedback)) { feedback_height = 0; gap = 0; }
    clip_height = viewport_height;
    if (clip) XtVaGetValues(clip, XmNheight, &clip_height, NULL);
    int available = (int)page_height - top - feedback_height - gap;
    int chrome = (int)viewport_height - clip_height;
    int wanted = (int)natural_height + (chrome > 0 ? chrome : 0);
    if (wanted > available) wanted = available;
    if (wanted < 1) wanted = 1;
    if (viewport_height != wanted)
        XtVaSetValues(state->preview_viewport, XmNheight, wanted, NULL);
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
    Widget pages[] = {state->welcome, state->example, state->source, state->icons, state->demos};
    unsigned int i;
    state->current_page = page;
    for (i = 0; i < XtNumber(pages); ++i) {
        if (!pages[i]) continue;
        if ((int)i + 1 == page) XtManageChild(pages[i]);
        else XtUnmanageChild(pages[i]);
    }
    if (state->tabs) {
        if (page == 2 || page == 3 || page == 4) XtManageChild(state->tabs);
        else XtUnmanageChild(state->tabs);
        if (page >= 2 && page <= 4)
            XmSegmentedControlSetSelectedIndex(state->tabs, page - 2, False);
    }
}

static void select_tab(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = client;
    XmSegmentedControlCallbackStruct *selection = call;
    (void)widget;
    if (selection && selection->selected_index >= 0)
        select_page(state, selection->selected_index + 2);
}

static int verify_view_navigation(GalleryState *state)
{
    const int indices[] = {1, 2, 0};
    Widget pages[] = {state->example, state->source, state->icons};
    if (!state->example || !state->source) return 0;
    for (unsigned i = 0; i < XtNumber(indices); ++i) {
        int index = indices[i];
        if (!XmSegmentedControlSetSelectedIndex(state->tabs, index, True) ||
            state->current_page != index + 2 || !XtIsManaged(pages[index])) {
            fprintf(stderr, "gallery: view navigation did not select page %d\n", index + 2);
            return 1;
        }
        for (unsigned j = 0; j < XtNumber(pages); ++j)
            if ((int)j != index && XtIsManaged(pages[j])) return 1;
        if (index == 1) {
            int mode;
            Widget vertical = NULL;
            Position x, y;
            Dimension height;
            if (!settle_gallery(state)) return 1;
            XtVaGetValues(state->source_text, XmNeditMode, &mode, XmNheight, &height, NULL);
            XtVaGetValues(XtParent(state->source_text), XmNverticalScrollBar, &vertical, NULL);
            if (mode != XmMULTI_LINE_EDIT || !vertical || !XtIsManaged(vertical)) {
                fprintf(stderr, "gallery: source view has no multiline vertical scrolling\n");
                return 1;
            }
            XmTextPosition last = XmTextGetLastPosition(state->source_text);
            if (last > 0) {
                XmTextShowPosition(state->source_text, last-1);
                if (!settle_gallery(state) ||
                    !XmTextPosToXY(state->source_text, last-1, &x, &y) || y < 0 || y >= height) {
                    fprintf(stderr, "gallery: final source line is unreachable\n");
                    return 1;
                }
                XmTextShowPosition(state->source_text, 0);
            }
        }
    }
    return 0;
}

static Widget create_page(Widget parent, const char *name)
{
    return XtVaCreateWidget(name, xmFormWidgetClass, parent,
        XmNmarginWidth, 24, XmNmarginHeight, 24,
        XmNtopAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
}

/* XmLabel accepts explicit line breaks. Keep the source prose immutable and
 * derive its lines from the actual render table, preserving UTF-8 word bytes.
 * A word wider than the viewport stays intact on the scrollable canvas. */
static XmString flow_text(Widget label, const char *source, Dimension available)
{
    char *copy = XtNewString(source), *line = copy, *word = copy;
    XmRenderTable table = NULL;
    XtVaGetValues(label, XmNrenderTable, &table, NULL);
    while (*word) {
        while (*word == ' ' || *word == '\t') ++word;
        if (*word == '\n') { line = ++word; continue; }
        if (!*word) break;
        char *end = word;
        while (*end && *end != ' ' && *end != '\t' && *end != '\n') ++end;
        char saved = *end;
        *end = '\0';
        XmString candidate = text(line);
        Dimension width = 0, height = 0;
        if (table) XmStringExtent(table, candidate, &width, &height);
        XmStringFree(candidate);
        *end = saved;
        if (width > available && word > line) {
            word[-1] = '\n';
            line = word;
        }
        word = end;
    }
    XmString result = text(copy);
    XtFree(copy);
    return result;
}

static void layout_flow_page(GalleryFlowPage *flow)
{
    Dimension available, gap = 0;
    unsigned int width = 1, height = 0;
    if (!flow->clip || !flow->count || flow->laying_out) return;
    XtVaGetValues(flow->clip, XmNwidth, &available, NULL);
    if (available == flow->measured_width) return;
    flow->laying_out = True;
    flow->measured_width = available;
    (void)XmGetAppearanceSpacing(XtDisplay(flow->canvas), XmAPPEARANCE_SPACING_MEDIUM, &gap);
    if (flow->grid) {
        unsigned int cell_width = 1, image_height = 1, caption_height = 1;
        Dimension image_gap = 0;
        (void)XmGetAppearanceSpacing(XtDisplay(flow->canvas), XmAPPEARANCE_SPACING_XS, &image_gap);
        for (Cardinal i = 0; i < flow->count; ++i) {
            Widget picture = XtNameToWidget(flow->items[i],"bitmap");
            if (!picture) picture = XtNameToWidget(flow->items[i],"description");
            Widget caption = XtNameToWidget(flow->items[i],"name");
            XtWidgetGeometry preferred = {0};
            XtQueryGeometry(picture, NULL, &preferred);
            if (preferred.width > cell_width) cell_width = preferred.width;
            if (preferred.height > image_height) image_height = preferred.height;
            XtQueryGeometry(caption, NULL, &preferred);
            if (preferred.width > cell_width) cell_width = preferred.width;
            if (preferred.height > caption_height) caption_height = preferred.height;
        }
        unsigned int cell_height = image_height + image_gap + caption_height;
        for (Cardinal i = 0; i < flow->count; ++i) {
            Widget picture = XtNameToWidget(flow->items[i],"bitmap");
            if (!picture) picture = XtNameToWidget(flow->items[i],"description");
            Widget caption = XtNameToWidget(flow->items[i],"name");
            XtVaSetValues(caption, XmNtopOffset, image_height + image_gap, NULL);
            XtVaSetValues(picture, XmNbottomOffset, image_gap, NULL);
        }
        unsigned int columns = (available + gap) / (cell_width + gap);
        if (!columns) columns = 1;
        if (columns > 3) columns = 3;
        unsigned int rows = (flow->count + columns - 1) / columns;
        for (Cardinal i = 0; i < flow->count; ++i)
            XtVaSetValues(flow->items[i], XmNx, (i % columns) * (cell_width + gap),
                XmNy, (i / columns) * (cell_height + gap),
                XmNwidth, cell_width, XmNheight, cell_height, NULL);
        width = columns * cell_width + (columns - 1) * gap;
        height = rows * cell_height + (rows - 1) * gap;
    } else {
        for (Cardinal i = 0; i < flow->count; ++i) {
            Dimension margin, left, right, shadow, highlight;
            XtVaGetValues(flow->items[i], XmNmarginWidth, &margin,
                XmNmarginLeft, &left, XmNmarginRight, &right,
                XmNshadowThickness, &shadow, XmNhighlightThickness, &highlight, NULL);
            unsigned int inset = 2U * (margin + shadow + highlight) + left + right;
            XmString lines = flow_text(flow->items[i], flow->paragraphs[i],
                                      available > inset ? available - inset : 1);
            XtVaSetValues(flow->items[i], XmNlabelString, lines, NULL);
            XmStringFree(lines);
            XtWidgetGeometry preferred = {0};
            XtQueryGeometry(flow->items[i], NULL, &preferred);
            XtVaSetValues(flow->items[i], XmNx, 0, XmNy, height,
                XmNwidth, preferred.width, XmNheight, preferred.height, NULL);
            if (preferred.width > width) width = preferred.width;
            height += preferred.height + (i + 1 < flow->count ? gap : 0);
        }
    }
    XtVaSetValues(flow->canvas, XmNwidth, width, XmNheight, height ? height : 1, NULL);
    flow->laying_out = False;
}

static void flow_configured(Widget widget, XtPointer client, XEvent *event,
                            Boolean *dispatch)
{
    GalleryFlowPage *flow = client;
    (void)dispatch;
    if (event->type == ConfigureNotify || event->type == MapNotify) {
        if (widget != flow->clip) flow->measured_width = 0;
        layout_flow_page(flow);
    }
}

static void create_flow_page(Widget page, Widget heading, GalleryFlowPage *flow)
{
    flow->viewport = XtVaCreateManagedWidget("pageViewport", xmScrolledWindowWidgetClass,
        page, XmNscrollingPolicy, XmAUTOMATIC,
        XmNtopAttachment, heading ? XmATTACH_WIDGET : XmATTACH_FORM,
        XmNtopWidget, heading, XmNtopOffset, heading ? 16 : 0,
        XmNbottomAttachment, XmATTACH_FORM, XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM, NULL);
    flow->canvas = XtVaCreateManagedWidget("pageCanvas", xmBulletinBoardWidgetClass,
        flow->viewport, XmNresizePolicy, XmRESIZE_ANY,
        XmNmarginWidth, 0, XmNmarginHeight, 0, NULL);
    XtVaGetValues(flow->viewport, XmNclipWindow, &flow->clip, NULL);
    XtAddEventHandler(flow->clip, StructureNotifyMask, False, flow_configured, flow);
}

static void create_welcome(GalleryState *state)
{
    static const struct { const char *role, *value; } copy[] = {
        {"pageTitle", "Explore Motif"},
        {"intro", "A working reference for classic and contemporary desktop controls."},
        {"sectionTitle", "Start with a component"},
        {"intro", "Open navigation to browse a category or search by component name. Try each example and inspect its source."},
        {"sectionTitle", "Built for exploration"},
        {"intro", "Buttons & Selection: actions, toggles and value controls\nText & Input: editing, search and suggestions\nData Views: lists, trees, calendars and tables\nLayout: forms, panes and page containers"},
        {"sectionTitle", "Familiar details, clearer presentation"},
        {"intro", "Raised buttons, inset fields and explicit keyboard focus preserve Motif's character. Use Tab to move between controls and the arrow keys inside lists and trees."}
    };
    state->welcome = create_page(state->notebook, "welcomePage");
    GalleryFlowPage *flow = &state->welcome_flow;
    create_flow_page(state->welcome, NULL, flow);
    for (Cardinal i = 0; i < XtNumber(copy); ++i) {
        flow->items[i] = gallery_label(flow->canvas, copy[i].role, copy[i].value);
        flow->paragraphs[i] = copy[i].value;
        XtAddEventHandler(flow->items[i], StructureNotifyMask, False, flow_configured, flow);
    }
    flow->count = XtNumber(copy);
    layout_flow_page(flow);
}

/* Ask the manager to position our client; its frame/base windows are private. */
static void center_shell_frame(Widget shell)
{
    Display *display = XtDisplay(shell);
    Window window = XtWindow(shell), frame = window, root, parent, *children = NULL;
    unsigned int child_count, width, height, border, depth;
    int x, y, screen = XScreenNumberOfScreen(XtScreen(shell));
    String geometry = NULL;
    XtVaGetValues(shell, XtNgeometry, &geometry, NULL);
    if (geometry && (XParseGeometry(geometry, &x, &y, &width, &height) & (XValue | YValue)))
        return;
    if (!XtIsRealized(shell)) return;
    /* MWM reparents through both a base and a frame. The direct parent is
     * consequently not necessarily the decorated frame. */
    for (unsigned level = 0; level < 64; ++level) {
        if (!XQueryTree(display, frame, &root, &parent, &children, &child_count)) return;
        if (children) { XFree(children); children = NULL; }
        if (parent == root || parent == None) break;
        frame = parent;
        if (level == 63) return;
    }
    if (!XGetGeometry(display, frame, &root, &x, &y, &width, &height, &border, &depth)) return;
    XWindowChanges changes;
    changes.x = DisplayWidth(display,screen) > (int)width ? (DisplayWidth(display,screen)-(int)width)/2 : 0;
    changes.y = DisplayHeight(display,screen) > (int)height ? (DisplayHeight(display,screen)-(int)height)/2 : 0;
    (void)XReconfigureWMWindow(display, window, screen, CWX | CWY, &changes);
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
            snprintf(message, sizeof(message), XmNumberBoxIsInvalid(widget) ?
                "Last event: input rejected; committed quantity: %d" :
                "Last event: committed quantity: %d", XmNumberBoxGetValue(widget));
        else if (XmIsStepper(widget))
            snprintf(message, sizeof(message), "Value: %d", XmStepperGetValue(widget));
        else if (XmIsRatingControl(widget))
            snprintf(message, sizeof(message), "Rating: %d", XmRatingControlGetValue(widget));
        else if (XmIsTabStack(widget)) {
            Widget selected = XmTabStackGetSelectedTab(widget);
            XmString caption = NULL;
            char *value = NULL;
            if (selected) XtVaGetValues(selected, XmNtabLabelString, &caption, NULL);
            if (caption) XmStringGetLtoR(caption, XmFONTLIST_DEFAULT_TAG, &value);
            snprintf(message, sizeof(message), "Current tab: %.120s",
                     value ? value : selected ? XtName(selected) : "none");
            XtFree(value);
            if (caption) XmStringFree(caption);
        } else if (XmIsNotebook(widget)) {
            int page;
            XtVaGetValues(widget, XmNcurrentPageNumber, &page, NULL);
            snprintf(message, sizeof(message), "Current page: %d", page);
        } else if (XmIsTextField(widget)) {
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
    if (XmIsTabStack(component)) {
        Widget selected = XmTabStackGetSelectedTab(component);
        Widget first = XmTabStackIndexToWidget(component, 0);
        Widget next = XmTabStackIndexToWidget(component, 1);
        if (!first || !next) return False;
        XmTabStackSelectTab(selected == first ? next : first, True);
    } else if (XmIsNotebook(component)) {
        int current, first, last;
        XtVaGetValues(component, XmNcurrentPageNumber, &current,
                      XmNfirstPageNumber, &first, XmNlastPageNumber, &last, NULL);
        XtVaSetValues(component, XmNcurrentPageNumber,
                      current < last ? current + 1 : first, NULL);
    } else if (XtHasCallbacks(component, XmNactivateCallback) == XtCallbackHasSome)
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

/* These examples present data without a user action. This describes the
 * constructed example, not whether a class can expose optional actions. */
static Boolean passive_example(const GalleryManifestEntry *entry)
{
    WidgetClass class = entry->class;
    return class && (class == xmAvatarWidgetClass || class == xmBadgeWidgetClass ||
        class == xmDividerWidgetClass || class == xmIconLabelWidgetClass ||
        class == xmPromptWidgetClass || class == xmEmptyStateWidgetClass ||
        class == xmLabelWidgetClass || class == xmLabelGadgetClass ||
        class == xmSeparatorWidgetClass || class == xmSeparatorGadgetClass ||
        class == xmProgressBarWidgetClass || class == xmProgressRingWidgetClass ||
        class == xmSpinnerWidgetClass || class == xmSkeletonWidgetClass ||
        class == xmStatusBarWidgetClass);
}

static const char *example_description(const GalleryManifestEntry *entry, Boolean compact)
{
    if (!entry->constructible)
        return compact ? "Toolkit service. See Source." :
            "Infrastructure class: used by the toolkit rather than placed in a layout.";
    if (!entry->class)
        return compact ? "Unavailable in this build. See Source." :
            "This optional component is not available in this build.";
    if (passive_example(entry))
        return compact ? "Preview: Theme / Scale." :
            "A display-only example. Use Theme and Scale to compare its appearance.";
    if (entry->popup_shell)
        return compact ? "Open the example window below." :
            "Open the example window to explore this component.";
    return compact ? "Explore the example below." :
        "Explore the example below. Select Source to see how it is constructed.";
}

static void show_entry(GalleryState *state, const GalleryManifestEntry *entry)
{
    Widget anchor;
    XmString title;
    char details[768];
    state->selected = entry;
    expand_entry_category(state->tree, entry);
    XmTreeViewSetSelectedId(state->tree, (XtPointer)entry->id, False);
    snprintf(details, sizeof(details), "%s", example_description(entry, False));
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
        state->feedback = gallery_label(state->example, "feedback", "");
        XtVaSetValues(state->feedback, XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, state->provenance, XmNtopOffset, 16,
            XmNleftAttachment, XmATTACH_FORM, NULL);
        XtAddEventHandler(state->example, StructureNotifyMask, False,
                          preview_configured, state);
        state->source = create_page(state->notebook, "sourcePage");
        {
            Arg args[2];
            XtSetArg(args[0], XmNeditable, False);
            XtSetArg(args[1], XmNeditMode, XmMULTI_LINE_EDIT);
            state->source_text = XmCreateScrolledText(
                state->source, (char *)"sourceText", args, 2);
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
        XmString current = NULL;
        XtVaGetValues(state->provenance, XmNlabelString, &current, NULL);
        if (!XmStringCompare(current, label))
            XtVaSetValues(state->provenance, XmNlabelString, label, NULL);
        XmStringFree(current);
        XmStringFree(label);
    }
    XtUnmanageChild(state->action);
    if (state->drag_context) {
        XmDragCancel(state->drag_context);
        state->drag_context = NULL;
    }
    /* Detach dependents before removing their anchor: Form otherwise preserves
     * the old absolute position by rewriting the attachment offset. */
    XtVaSetValues(state->feedback, XmNtopWidget, state->provenance,
                  XmNtopOffset, 16, NULL);
    XtVaSetValues(state->action, XmNtopWidget, state->provenance, NULL);
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
                XmNscrollingPolicy, XmAUTOMATIC,                 XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, state->provenance,
                XmNtopOffset, 24, XmNleftAttachment, XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNbottomAttachment, XmATTACH_NONE, NULL);
            XtVaSetValues(state->feedback, XmNtopWidget, state->preview_viewport, NULL);
            XtAddEventHandler(state->preview_viewport, StructureNotifyMask, False,
                              preview_configured, state);
            if (row_column_type == XmMENU_BAR)
                state->component_host = XtVaCreateManagedWidget(
                    "componentHost", xmRowColumnWidgetClass, state->preview_viewport,
                    XmNrowColumnType, XmMENU_BAR, XmNmarginWidth, 0,
                    XmNmarginHeight, 0, NULL);
            else
                state->component_host = XtVaCreateManagedWidget(
                    "componentHost", xmBulletinBoardWidgetClass, state->preview_viewport,
                    XmNresizePolicy, XmRESIZE_ANY, XmNmarginWidth, 0,
                    XmNmarginHeight, 0, NULL);
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
                  state->component_is_popup ? state->action : anchor, XmNtopOffset, 16, NULL);
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
            passive_example(entry) ? "" : "Ready for interaction");
        XtVaSetValues(state->feedback, XmNlabelString, label, NULL);
        XmStringFree(label);
        if (passive_example(entry)) XtUnmanageChild(state->feedback);
        else XtManageChild(state->feedback);
    }
    XmTextSetString(state->source_text, (char *)source_for(entry));
    /* Select the live example after both component pages exist. */
    select_page(state, 2);
    if (entry->constructible && !entry->class)
        set_status(state, "Selected component; optional example unavailable in this build");
    else
        set_status(state, "Tab: move focus | Source: view code");
    layout_gallery(state);
}

static void theme_control(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    XmAppearanceProfile current;
    (void)widget; (void)call;
    if (!XmGetAppearanceProfile(XtDisplay(state->shell), &current)) return;
    XmAppearanceProfile next = current == XmAPPEARANCE_CLASSIC ?
        XmAPPEARANCE_MODERN_LIGHT : current == XmAPPEARANCE_MODERN_LIGHT ?
        XmAPPEARANCE_MODERN_DARK : XmAPPEARANCE_CLASSIC;
    if (XmApplyAppearanceProfile(XtDisplay(state->shell), next)) {
        set_status(state, next == XmAPPEARANCE_CLASSIC ? "Theme: Classic" :
            next == XmAPPEARANCE_MODERN_LIGHT ? "Theme: Modern light" : "Theme: Modern dark");
        layout_gallery(state);
    } else set_status(state, "Theme change failed; previous settings retained");
}

static void dpi_control(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    static const double scales[] = {1.0, 1.5, 2.0, 3.0};
    double current = 0.0;
    Display *display = XtDisplay(state->shell);
    (void)widget; (void)call;
    Boolean overridden = XmGetDisplayScaleOverride(display, &current);
    unsigned next = 0;
    if (overridden)
        while (next < XtNumber(scales) && scales[next] <= current + 0.001) ++next;
    Boolean changed = next == XtNumber(scales) ? XmClearDisplayScaleOverride(display) :
        XmSetDisplayScaleOverride(display, scales[next]);
    if (changed) {
        char status[80];
        if (next == XtNumber(scales)) strcpy(status, "Scale: Automatic");
        else snprintf(status, sizeof(status), "Scale: %.0f%%", scales[next] * 100);
        set_status(state, status);
        layout_gallery(state);
    } else set_status(state, "Scale change failed; previous settings retained");
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
        if (XtIsManaged(state->feedback) == passive_example(entry)) {
            fprintf(stderr, "gallery integration: inappropriate feedback for %s\n", entry->id);
            return 1;
        }
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
        if (entry->class == xmTreeViewWidgetClass) {
            Dimension row, height;
            XtVaGetValues(state->component, XmNtreeViewRowHeight, &row,
                XmNheight, &height, NULL);
            if (row == 0 || (unsigned long)row * 4 > height) {
                fprintf(stderr, "gallery integration: TreeView sample rows clipped row=%u height=%u\n", row, height);
                return 1;
            }
        }
        if (entry->class == xmListWidgetClass || entry->class == xmComboBoxWidgetClass) {
            Widget list = state->component;
            int visible;
            Position x, y;
            Dimension row_width, row_height, list_height;
            if (entry->class == xmComboBoxWidgetClass)
                XtVaGetValues(state->component, XmNlist, &list, NULL);
            if (!list) return 1;
            XtVaGetValues(list, XmNvisibleItemCount, &visible, XmNheight, &list_height, NULL);
            if (visible != 3 || !XmListPosToBounds(list, 3, &x, &y, &row_width, &row_height) ||
                y < 0 || (unsigned)y + row_height > list_height) {
                fprintf(stderr, "gallery integration: three-row sample geometry invalid for %s\n", entry->id);
                return 1;
            }
        }
        if (entry->class == xmNumberBoxWidgetClass) {
            Widget number=state->component, layout=XtParent(number);
            Widget help=XtNameToWidget(layout,"fieldHelp");
            if (!help) return 1;
            unsigned long before=state->callback_count;
            XmTextFieldSetString(XmNumberBoxGetTextField(number),"99999999");
            XtCallActionProc(XmNumberBoxGetTextField(number),"activate",NULL,NULL,0);
            if (!settle_gallery(state) || !XmNumberBoxIsInvalid(number) ||
                state->callback_count!=before+1) return 1;
            XmString feedback;char *feedback_text=NULL;
            XtVaGetValues(state->feedback,XmNlabelString,&feedback,NULL);
            Boolean rejected=feedback && XmStringGetLtoR(feedback,XmFONTLIST_DEFAULT_TAG,&feedback_text) &&
                !strcmp(feedback_text,"Last event: input rejected; committed quantity: 42");
            XtFree(feedback_text);if(feedback)XmStringFree(feedback);
            if (!rejected) return 1;
            Position number_y,help_y;Dimension number_height;
            XtVaGetValues(number,XmNy,&number_y,XmNheight,&number_height,NULL);
            XtVaGetValues(help,XmNy,&help_y,NULL);
            if (help_y < number_y+number_height) {
                fprintf(stderr,"gallery integration: NumberBox validation overlaps helper text\n");
                return 1;
            }
            XmTextFieldSetString(XmNumberBoxGetTextField(number),"43");
            XtCallActionProc(XmNumberBoxGetTextField(number),"activate",NULL,NULL,0);
            if (!settle_gallery(state) || XmNumberBoxIsInvalid(number) ||
                XmNumberBoxGetValue(number)!=43 || state->callback_count!=before+2) return 1;
            feedback_text=NULL;
            XtVaGetValues(state->feedback,XmNlabelString,&feedback,NULL);
            Boolean committed=feedback && XmStringGetLtoR(feedback,XmFONTLIST_DEFAULT_TAG,&feedback_text) &&
                !strcmp(feedback_text,"Last event: committed quantity: 43");
            XtFree(feedback_text);if(feedback)XmStringFree(feedback);
            if (!committed) return 1;
        }
        if (entry->class == xmSelectionBoxWidgetClass || entry->class == xmCommandWidgetClass) {
            Widget list = XmSelectionBoxGetChild(state->component, XmDIALOG_LIST);
            Position x, y;
            Dimension width, height, list_height;
            if (!list || !XmListPosToBounds(list, 3, &x, &y, &width, &height)) {
                fprintf(stderr, "gallery integration: third sample item is hidden for %s\n", entry->id);
                return 1;
            }
            XtVaGetValues(list, XmNheight, &list_height, NULL);
            if (y < 0 || (unsigned int)y + height > list_height) {
                fprintf(stderr, "gallery integration: third sample item is clipped for %s\n", entry->id);
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
                Position py; Dimension ph, vh, fh;
                XtVaGetValues(state->preview_viewport, XmNy, &py, XmNheight, &vh, NULL);
                XtVaGetValues(state->example, XmNheight, &ph, NULL);
                XtVaGetValues(state->feedback, XmNheight, &fh, NULL);
                fprintf(stderr, "gallery integration: disabled button clipped y=%d height=%u clip=%u preview=%d/%u page=%u feedback=%u\n", y,height,clip_height,py,vh,ph,fh);
                return 1;
            }
        }
        {
            /* Compare the displayed buffer with the selected generated excerpt. */
            char *source = XmTextGetString(state->source_text);
            Boolean source_matches = source && *source &&
                !strcmp(source, source_for(entry));
            if (!source_matches)
                fprintf(stderr, "gallery source mismatch: expected %zu bytes, received %zu bytes\n",
                        strlen(source_for(entry)), source ? strlen(source) : 0);
            XtFree(source);
            if (state->selected != entry || !source_matches) {
                fprintf(stderr, "gallery integration: source/selection failed for %s\n", entry->id);
                return 1;
            }
        }
        if (XmIsTabStack(state->component)) {
            unsigned long before = state->callback_count;
            XmString label = NULL;
            char *value = NULL;
            invoke_component_callback(state->component, state);
            Widget selected = XmTabStackGetSelectedTab(state->component);
            XtVaGetValues(state->feedback, XmNlabelString, &label, NULL);
            if (label) XmStringGetLtoR(label, XmFONTLIST_DEFAULT_TAG, &value);
            if (label) XmStringFree(label);
            Boolean valid = selected == XmTabStackIndexToWidget(state->component, 1) &&
                state->callback_count == before + 1 &&
                value && !strcmp(value, "Current tab: Details");
            XtFree(value);
            if (!valid) {
                fprintf(stderr, "gallery integration: TabStack selection feedback failed\n");
                return 1;
            }
            puts("gallery integration: TabStack selection callback and feedback PASS");
        } else if (XmIsNotebook(state->component)) {
            unsigned long before = state->callback_count;
            XmString label = NULL;
            char *value = NULL;
            int page;
            invoke_component_callback(state->component, state);
            XtVaGetValues(state->component, XmNcurrentPageNumber, &page, NULL);
            XtVaGetValues(state->feedback, XmNlabelString, &label, NULL);
            if (label) XmStringGetLtoR(label, XmFONTLIST_DEFAULT_TAG, &value);
            if (label) XmStringFree(label);
            Boolean valid = page == 2 && state->callback_count == before + 1 &&
                value && !strcmp(value, "Current page: 2");
            XtFree(value);
            if (!valid) {
                fprintf(stderr, "gallery integration: Notebook page feedback failed\n");
                return 1;
            }
            puts("gallery integration: Notebook page callback and feedback PASS");
        } else if (state->component_is_drag_source) {
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
    {
        XEvent click = {0}, tab = {0};
        if (!XmProcessTraversal(state->search, XmTRAVERSE_CURRENT)) return 1;
        click.xbutton.type = ButtonPress;
        click.xbutton.display = XtDisplay(state->tree);
        click.xbutton.window = XtWindow(state->tree);
        click.xbutton.button = Button1;
        click.xbutton.x = 60; click.xbutton.y = 8;
        XtDispatchEvent(&click);
        if (XmGetFocusWidget(state->shell) != state->tree) {
            fprintf(stderr, "gallery integration: clicking navigation did not focus tree\n");
            return 1;
        }
        tab.xkey.type = KeyPress;
        tab.xkey.display = XtDisplay(state->tree);
        tab.xkey.window = XtWindow(state->tree);
        tab.xkey.keycode = XKeysymToKeycode(XtDisplay(state->tree), XK_Tab);
        XtDispatchEvent(&tab);
        if (XmGetFocusWidget(state->shell) == state->tree) {
            fprintf(stderr, "gallery integration: Tab did not leave navigation tree\n");
            return 1;
        }
    }
    XmTreeViewSetSelectedId(state->tree, selected, True);
        if (!XmProcessTraversal(state->tree, XmTRAVERSE_CURRENT)) {
            fprintf(stderr, "gallery integration: tree traversal failed\n");
        return 1;
        }
    {
        Widget focus_before = XmGetFocusWidget(state->shell);
        Widget workspace = XtParent(state->notebook);
        Dimension expanded, collapsed, restored, content_before, content_after;
        XtVaGetValues(state->sidebar, XmNwidth, &expanded, NULL);
        XtVaGetValues(workspace, XmNwidth, &content_before, NULL);
        XmSidebarSetExpanded(state->sidebar, False);
        if (XmGetFocusWidget(state->shell) != XtNameToWidget(state->sidebar, "toggle")) {
            fprintf(stderr, "gallery integration: collapsed sidebar toggle lost focus\n");
            return 1;
        }
        XtVaGetValues(state->sidebar, XmNwidth, &collapsed, NULL);
        XtVaGetValues(workspace, XmNwidth, &content_after, NULL);
        if (collapsed >= expanded || content_after <= content_before ||
            expanded - collapsed != content_after - content_before) {
            fprintf(stderr, "gallery integration: sidebar collapse did not release space (%u -> %u; workspace %u -> %u)\n",
                expanded, collapsed, content_before, content_after);
            return 1;
        }
        XmSidebarSetExpanded(state->sidebar, True);
        XtVaGetValues(state->sidebar, XmNwidth, &restored, NULL);
        if (restored != expanded) {
            fprintf(stderr, "gallery integration: sidebar width was not restored\n");
            return 1;
        }
        if (focus_before && XmGetFocusWidget(state->shell) != focus_before) {
            fprintf(stderr, "gallery integration: focus was not restored\n");
            return 1;
        }
    }
    select_page(state, 3);
    if (verify_view_navigation(state) || exercise_component_matrix(state))
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
    XmTreeViewSetSelectedId(state->tree, (XtPointer)demos_id, True);
    if (state->current_page != 5 || !XtIsManaged(state->demos) ||
        XtIsManaged(state->example) || XtIsManaged(state->tabs)) {
        fprintf(stderr, "gallery integration: demo navigation failed\n");
        return 1;
    }
    puts("gallery integration: demo applications navigation PASS");
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
    puts("gallery integration: live catalogue transactions: use --integration-live");
    return 0;
}

/* Keep child ownership local: never reap another library's children. */
static pid_t demo_children[XtNumber(gallery_demos)];
static GalleryState *demo_state;

static void reap_demos(XtPointer client, XtIntervalId *timer)
{
    GalleryState *state = client;
    (void)timer;
    for (Cardinal i = 0; i < XtNumber(gallery_demos); ++i) {
        int status;
        if (!demo_children[i]) continue;
        pid_t result = waitpid(demo_children[i], &status, WNOHANG);
        if (result < 0 && errno == ECHILD) { demo_children[i] = 0; continue; }
        if (result > 0) {
            char message[512];
            snprintf(message, sizeof(message), "%s closed%s", gallery_demos[i].title,
                WIFEXITED(status) && WEXITSTATUS(status) == 0 ? "" : " with an error; see terminal output");
            set_status(state, message);
            demo_children[i] = 0;
        }
    }
    XtAppAddTimeOut(XtWidgetToApplicationContext(state->shell), 500, reap_demos, state);
}

static void close_demo_notice(Widget widget, XtPointer client, XtPointer call)
{
    (void)client; (void)call;
    XtDestroyWidget(widget);
}

static void open_demo(Widget widget, XtPointer client, XtPointer call)
{
    const GalleryDemo *demo = client;
    size_t index = (size_t)(demo - gallery_demos);
    char path[4096], message[9216];
    (void)widget; (void)call;
    if (demo_children[index]) {
        set_status(demo_state, "This demo is already running. Close its window before opening it again.");
        return;
    }
    int error = gallery_demo_path(GALLERY_BUILD_DEMOS, GALLERY_INSTALL_DEMOS,
        demo->build_dir, demo->install_dir, demo->binary, path, sizeof(path));
    if (error) {
        snprintf(message, sizeof(message),
            "App not found at %s\nBuild location: %s/%s/%s\n"
            "Build the library with ./configure --enable-demos, then make -j6 && make install.",
            path, GALLERY_BUILD_DEMOS, demo->build_dir, demo->binary);
    } else {
        error = gallery_demo_launch(path, &demo_children[index]);
        snprintf(message, sizeof(message), "%s: %s%s", demo->title,
            error ? strerror(error) : "Opened ", error ? "" : path);
    }
    set_status(demo_state, message);
    if (error) {
        XmString value = text(message);
        Arg argument;
        XtSetArg(argument, XmNmessageString, value);
        Widget dialog = XmCreateInformationDialog(demo_state->shell, "demoLaunchNotice", &argument, 1);
        XmStringFree(value);
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON));
        XtAddCallback(dialog, XmNokCallback, close_demo_notice, NULL);
        XtManageChild(dialog);
    }
}

static void demo_page_configured(Widget clip, XtPointer client, XEvent *event, Boolean *dispatch)
{
    Widget list = client;
    static Boolean busy;
    Dimension width;
    WidgetList rows;
    Cardinal count;
    (void)dispatch;
    if (busy || (event->type != ConfigureNotify && event->type != MapNotify)) return;
    busy = True;
    XtVaGetValues(clip, XmNwidth, &width, NULL);
    XtVaGetValues(list, XmNchildren, &rows, XmNnumChildren, &count, NULL);
    if (count > 1) {
        XmString intro = flow_text(rows[1],
            "Open a standalone example.\nMissing demos show paths and build instructions.",
            width > 32 ? width - 32 : 1);
        XtVaSetValues(rows[1], XmNlabelString, intro, NULL);
        XmStringFree(intro);
    }
    for (Cardinal i = 2; i < count; ++i) {
        Widget label = XtNameToWidget(rows[i], "description");
        if (label) {
            XmString wrapped = flow_text(label, gallery_demos[i - 2].description,
                                          width > 64 ? width - 64 : 1);
            XtVaSetValues(label, XmNlabelString, wrapped, NULL);
            XmStringFree(wrapped);
        }
    }
    XtVaSetValues(list, XmNwidth, width, NULL);
    busy = False;
}

static void create_demos(GalleryState *state)
{
    demo_state = state;
    state->demos = create_page(state->notebook, "demoAppsPage");
    Widget viewport = XtVaCreateManagedWidget("demoViewport", xmScrolledWindowWidgetClass,
        state->demos, XmNscrollingPolicy, XmAUTOMATIC,
        XmNtopAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
    Widget list = XtVaCreateManagedWidget("demoApps", xmRowColumnWidgetClass, viewport,
        XmNorientation, XmVERTICAL, XmNpacking, XmPACK_TIGHT, NULL);
    gallery_label(list, "pageTitle", "Demo Applications");
    gallery_label(list, "intro", "Open a standalone example.\nMissing demos show paths and build instructions.");
    for (Cardinal i = 0; i < XtNumber(gallery_demos); ++i) {
        Widget row = XtVaCreateManagedWidget(gallery_demos[i].binary, xmRowColumnWidgetClass,
            list, XmNorientation, XmVERTICAL, XmNpacking, XmPACK_TIGHT, NULL);
        gallery_label(row, "title", gallery_demos[i].title);
        gallery_label(row, "description", gallery_demos[i].description);
        Widget button = XtVaCreateManagedWidget("Open", xmPushButtonWidgetClass, row, NULL);
        XtAddCallback(button, XmNactivateCallback, open_demo, (XtPointer)&gallery_demos[i]);
    }
    Widget clip = NULL;
    XtVaGetValues(viewport, XmNclipWindow, &clip, NULL);
    XtAddEventHandler(clip, StructureNotifyMask, False, demo_page_configured, list);
    XtAppAddTimeOut(XtWidgetToApplicationContext(state->shell), 500, reap_demos, state);
}

static int verify_demo_apps(GalleryState *state)
{
    XmTreeViewSetSelectedId(state->tree, (XtPointer)demos_id, True);
    if (!settle_gallery(state) || state->current_page != 5 || !XtIsManaged(state->demos)) return 1;
    Widget list = XtNameToWidget(state->demos, "*demoApps");
    if (!list) return 1;
    XtVaSetValues(state->shell, XmNwidth, 450, XmNheight, 500, NULL);
    if (!settle_gallery(state)) return 1;
    Widget viewport = XtNameToWidget(state->demos, "demoViewport"), clip = NULL;
    Dimension list_width, clip_width;
    XtVaGetValues(viewport, XmNclipWindow, &clip, NULL);
    XtVaGetValues(list, XmNwidth, &list_width, NULL);
    XtVaGetValues(clip, XmNwidth, &clip_width, NULL);
    if (list_width > clip_width) {
        fprintf(stderr,"FAIL demo page needs horizontal scrolling at compact size: %u > %u\n",list_width,clip_width);
        return 1;
    }
    for (Cardinal i = 0; i < XtNumber(gallery_demos); ++i) {
        char name[256];
        snprintf(name, sizeof(name), "%s.Open", gallery_demos[i].binary);
        if (!XtNameToWidget(list, name) || !*gallery_demos[i].description) return 1;
    }
    for (Cardinal i = 0; i < XtNumber(gallery_demos); ++i) {
        if (strcmp(gallery_demos[i].binary, "hellomotif")) continue;
        char path[4096];
        int available = gallery_demo_path(GALLERY_BUILD_DEMOS, GALLERY_INSTALL_DEMOS,
            gallery_demos[i].build_dir, gallery_demos[i].install_dir,
            gallery_demos[i].binary, path, sizeof(path)) == 0;
        Widget button = XtNameToWidget(list, "hellomotif.Open");
        XtCallCallbacks(button, XmNactivateCallback, NULL);
        if (available) {
            pid_t child = demo_children[i];
            if (child <= 0) return 1;
            XtCallCallbacks(button, XmNactivateCallback, NULL);
            if (demo_children[i] != child) return 1;
            kill(child, SIGTERM);
            while (waitpid(child, NULL, 0) < 0 && errno == EINTR) {}
            demo_children[i] = 0;
            puts("PASS gallery demo catalogue, Open callback, execution and duplicate-instance guard");
        } else {
            Widget notice = XtNameToWidget(state->shell, "*demoLaunchNotice");
            XmString label = NULL;
            char *message = NULL;
            if (!notice || !XtIsManaged(notice)) return 1;
            XtVaGetValues(notice, XmNmessageString, &label, NULL);
            if (label) message = XmStringUnparse(label, NULL, XmCHARSET_TEXT,
                XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
            Boolean valid = message && strstr(message, path) && strstr(message, "--enable-demos");
            if (message) XtFree(message);
            XtDestroyWidget(notice);
            if (!valid) return 1;
            puts("PASS gallery missing demo notice includes exact path and --enable-demos");
        }
        return 0;
    }
    return 1;
}

static int verify_all_demo_apps(GalleryState *state)
{
    XmTreeViewSetSelectedId(state->tree, (XtPointer)demos_id, True);
    if (!settle_gallery(state)) return 1;
    Widget list = XtNameToWidget(state->demos, "*demoApps");
    unsigned checked = 0;
    for (Cardinal i = 0; i < XtNumber(gallery_demos); ++i) {
        if (!strcmp(gallery_demos[i].binary, "wsm")) continue; /* qualified by its own native suite */
        char name[256];
        snprintf(name,sizeof(name),"%s.Open",gallery_demos[i].binary);
        Widget button = XtNameToWidget(list,name);
        if (!button) return 1;
        XtCallCallbacks(button,XmNactivateCallback,NULL);
        pid_t child = demo_children[i];
        if (child <= 0) { fprintf(stderr,"FAIL demo Open: %s\n",gallery_demos[i].title); return 1; }
        for (int tick = 0; tick < 20; ++tick) {
            usleep(50000);
            while (XtAppPending(XtWidgetToApplicationContext(state->shell)))
                XtAppProcessEvent(XtWidgetToApplicationContext(state->shell),XtIMAll);
        }
        int status;
        pid_t result = waitpid(child,&status,WNOHANG);
        Boolean running = demo_children[i] == child && result == 0;
        if (result == 0) { kill(child,SIGTERM); while(waitpid(child,NULL,0)<0 && errno==EINTR) {} }
        demo_children[i] = 0;
        if (!running) { fprintf(stderr,"FAIL demo exited during Open: %s\n",gallery_demos[i].title); return 1; }
        printf("PASS demo Open: %s\n",gallery_demos[i].title); fflush(stdout);
        ++checked;
    }
    printf("PASS gallery Open startup for %u standalone apps; Wsm qualified separately\n",checked);
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
        if (!strcmp((const char *)callback->id, demos_id)) {
            state->selected = NULL;
            select_page(state, 5);
            set_status(state, "Demo applications - choose Open to launch an example");
            return;
        }
        if (!entry) {
            set_status(state, "Category expanded; choose a component example");
            return;
        }
        if (!strcmp(entry->id, (const char *)callback->id)) {
            show_entry(state, entry);
        }
        else
            set_status(state, "Category expanded; choose a component example");
    }
}

static void activate_navigation(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = client;
    (void)widget; (void)call;
    close_compact_navigation(state);
}

static void toggle_sidebar(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = (GalleryState *)client;
    XmSidebarCallbackStruct *callback = (XmSidebarCallbackStruct *)call;
    (void)widget;
    if (!state->updating_layout && !state->compact_layout && callback)
        state->desktop_navigation_expanded = callback->expanded;
    if (!state->updating_layout) layout_gallery(state);
    set_status(state, callback && callback->expanded ? "Navigation expanded" : "Navigation collapsed");
}

/* Narrow windows use a navigation page, not two competing narrow columns.
 * Automatic mode changes never replace the user's desktop sidebar preference. */
static void layout_gallery(GalleryState *state)
{
    Dimension width;
    double scale = 1.0;
    Boolean compact, expanded;
    if (!state->body || !state->workspace || !state->navigation_width ||
        state->updating_layout) return;
    state->updating_layout = True;
    XtVaGetValues(state->body, XmNwidth, &width, NULL);
    (void)XmGetWidgetScale(state->body, &scale);
    compact = width < state->navigation_width + 640.0 * scale;
    if (compact != state->compact_layout) {
        state->compact_layout = compact;
        XmSidebarSetExpanded(state->sidebar,
            compact ? False : state->desktop_navigation_expanded);
    }
    expanded = XmSidebarGetExpanded(state->sidebar);
    {
        unsigned char attachment, wanted_attachment = compact && expanded ?
            XmATTACH_FORM : XmATTACH_NONE;
        Dimension expanded_width, wanted_width = compact && expanded ?
            width : state->navigation_width;
        /* The navigation page occupies the complete Form width. Remove the
         * sibling pane before attaching that page to both edges: while both
         * are managed, the pane also demands space to the right of it. */
        if (compact && expanded) XtUnmanageChild(state->workspace);
        XtVaGetValues(state->sidebar, XmNrightAttachment, &attachment,
            XmNsidebarExpandedWidth, &expanded_width, NULL);
        if (attachment != wanted_attachment || expanded_width != wanted_width)
            XtVaSetValues(state->sidebar, XmNrightAttachment, wanted_attachment,
                XmNsidebarExpandedWidth, wanted_width, NULL);
        if (!(compact && expanded)) XtManageChild(state->workspace);
    }
    {
        XmString title = text(compact ? "Motif Gallery" : "CK Core Motif / Component Gallery");
        XmString current = NULL;
        XtVaGetValues(state->app_title, XmNlabelString, &current, NULL);
        if (!XmStringCompare(current, title))
            XtVaSetValues(state->app_title, XmNlabelString, title, NULL);
        XmStringFree(current);
        XmStringFree(title);
        if (compact) XtUnmanageChild(state->theme_note);
        else XtManageChild(state->theme_note);
    }
    if (state->provenance && state->selected) {
        char description[1024];
        if (compact)
            snprintf(description, sizeof(description), "%s",
                     example_description(state->selected, True));
        else
            snprintf(description, sizeof(description), "%s\n\n%s",
                     state->selected->class_symbol, state->provenance_text);
        XmString label = text(description);
        XmString current = NULL;
        XtVaGetValues(state->provenance, XmNlabelString, &current, NULL);
        if (!XmStringCompare(current, label))
            XtVaSetValues(state->provenance, XmNlabelString, label, NULL);
        XmStringFree(current);
        XmStringFree(label);
    }
    resize_preview(state);
    state->updating_layout = False;
}

static void gallery_configured(Widget widget, XtPointer client, XEvent *event,
                               Boolean *dispatch)
{
    (void)widget; (void)dispatch;
    if (event->type == ConfigureNotify) layout_gallery((GalleryState *)client);
}

static void close_compact_navigation(GalleryState *state)
{
    if (state->compact_layout) XmSidebarSetExpanded(state->sidebar, False);
}

static void add_tree_nodes(Widget tree, const char *query)
{
    static char labels[128][96];
    XmTreeViewNode nodes[128]; Cardinal i, used, offset = GALLERY_CATEGORY_COUNT + 3;
    if (GALLERY_MANIFEST_COUNT + GALLERY_CATEGORY_COUNT + 3 > XtNumber(nodes)) return;
    nodes[0].id = (XtPointer)welcome_id;
    nodes[0].parent_id = NULL;
    nodes[0].label = "Welcome";
    nodes[0].has_children = False; nodes[0].expanded = False;
    nodes[1].id = (XtPointer)icons_id;
    nodes[1].parent_id = NULL;
    nodes[1].label = "Built-in Icons";
    nodes[1].has_children = False; nodes[1].expanded = False;
    nodes[2].id = (XtPointer)demos_id;
    nodes[2].parent_id = NULL;
    nodes[2].label = "Demo Applications";
    nodes[2].has_children = False; nodes[2].expanded = False;
    for (i = 0; i < GALLERY_CATEGORY_COUNT; ++i) {
        nodes[i + 3].id = (XtPointer)gallery_categories[i];
        nodes[i + 3].parent_id = NULL;
        const char *title = gallery_categories[i];
        if (!strcmp(title, "buttons-selection")) title = "Buttons & Selection";
        else if (!strcmp(title, "commands-navigation")) title = "Commands & Navigation";
        else if (!strcmp(title, "text-input")) title = "Text & Input";
        else if (!strcmp(title, "data-views")) title = "Data Views";
        else if (!strcmp(title, "feedback-dialogs")) title = "Feedback & Dialogs";
        else if (!strcmp(title, "layout")) title = "Layout";
        else if (!strcmp(title, "media")) title = "Media";
        snprintf(labels[i + 3], sizeof(labels[i + 3]), "%s", title);
        nodes[i + 3].label = labels[i + 3];
        nodes[i + 3].has_children = True; nodes[i + 3].expanded = False;
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
            if (query && *query) nodes[category + 3].expanded = True;
        }
    }
    XmTreeViewSetNodes(tree, nodes, used);
}

static void layout_navigation(GalleryState *state)
{
    Widget content = XmSidebarGetContent(state->sidebar);
    Dimension width, height, current_width, current_height;
    if (!state->navigation_form || !state->tree) return;
    XtVaGetValues(content, XmNwidth, &width, XmNheight, &height, NULL);
    XtVaGetValues(state->navigation_form, XmNwidth, &current_width,
        XmNheight, &current_height, NULL);
    if (width != current_width || height != current_height)
        XtVaSetValues(state->navigation_form, XmNwidth, width, XmNheight, height, NULL);

}

static void navigation_resize(Widget widget, XtPointer client, XEvent *event, Boolean *dispatch)
{
    (void)widget; (void)dispatch;
    if (event->type == ConfigureNotify) layout_navigation((GalleryState *)client);
}

static void search_changed(Widget widget, XtPointer client, XtPointer call)
{
    GalleryState *state = client;
    char *query = XmTextFieldGetString(widget);
    (void)call;
    add_tree_nodes(state->tree, query);
    if (state->selected) XmTreeViewSetSelectedId(state->tree, (XtPointer)state->selected->id, False);
    else XmTreeViewSetSelectedId(state->tree,
        (XtPointer)(state->current_page == 5 ? demos_id : state->current_page == 4 ? icons_id : welcome_id), False);
    XtFree(query);
}

static Widget create_icon_page(GalleryState *state)
{
    static const struct { const char *name, *caption; } icons[] = {
        {"error", "Error"}, {"information", "Information"}, {"question", "Question"},
        {"warning", "Warning"}, {"working", "Working"}, {"template", "Template"},
        {"file", "File"}, {"selection", "Selection"}, {"prompt", "Prompt"}
    };
    XtInitializeWidgetClass(xmMessageBoxWidgetClass);
    Widget page = create_page(state->notebook, "iconsPage");
    Widget title = gallery_label(page, "pageTitle", "Built-in icons");
    XtVaSetValues(title, XmNtopAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM, NULL);
    GalleryFlowPage *flow = &state->icons_flow;
    flow->grid = True;
    create_flow_page(page, title, flow);
    for (Cardinal i = 0; i < XtNumber(icons); ++i) {
        char name[64], fallback[64];
        snprintf(name, sizeof(name), "dialog_%s", icons[i].name);
        snprintf(fallback, sizeof(fallback), "default_xm_%s", icons[i].name);
        Widget cell = XtVaCreateManagedWidget("iconCell", xmFormWidgetClass,
            flow->canvas, XmNmarginWidth, 0, XmNmarginHeight, 0, NULL);
        Widget picture = XtVaCreateManagedWidget("bitmap", xmLabelWidgetClass, cell,
            XmNlabelType, XmPIXMAP, NULL);
        if (!gallery_bind_named_icon(picture, XmNlabelPixmap, name, fallback)) {
            XtDestroyWidget(picture);
            gallery_label(cell, "description", "Requires SVG");
        }
        XmString label = text(icons[i].caption);
        Widget caption = XtVaCreateManagedWidget("name", xmLabelWidgetClass, cell,
            XmNlabelString, label, XmNtopAttachment, XmATTACH_FORM,
            XmNbottomAttachment, XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
        picture = XtNameToWidget(cell,"bitmap");
        if (!picture) picture = XtNameToWidget(cell,"description");
        XtVaSetValues(picture, XmNtopAttachment, XmATTACH_FORM,
            XmNbottomAttachment, XmATTACH_WIDGET, XmNbottomWidget, caption,
            XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
        XmStringFree(label);
        flow->items[i] = cell;
        XtAddEventHandler(cell, StructureNotifyMask, False, flow_configured, flow);
    }
    flow->count = XtNumber(icons);
    layout_flow_page(flow);
    return page;
}

static Boolean capture_ready(XtPointer client)
{
    GalleryState *state = client;
    unsigned long pid = (unsigned long)getpid();
    unsigned long ready = 1;
    Display *display = XtDisplay(state->shell);
    Window window = XtWindow(state->shell);
    XWindowAttributes attributes;
    if (!XGetWindowAttributes(display, window, &attributes) ||
        attributes.map_state != IsViewable) return False;
    XSync(display, False);
    /* Reparenting can map the shell after our first idle callback. Drain
     * those exposure/configuration events before declaring pixels ready. */
    if (XtAppPending(XtWidgetToApplicationContext(state->shell))) return False;
    XChangeProperty(display, window, XInternAtom(display, "_NET_WM_PID", False),
        XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&pid, 1);
    XChangeProperty(display, window, XInternAtom(display, "_CKMOTIF_GALLERY_READY", False),
        XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&ready, 1);
    XSync(display, False);
    return True;
}

/* Semantic roles only: palette, typography and control chrome belong to Motif. */
static String gallery_resources[] = {
    "*pageTitle.appearanceRole: page_title",
    "*sectionTitle.appearanceRole: section",
    "*description.appearanceRole: secondary",
    "*themeNote.appearanceRole: secondary",
    "*sourceText.appearanceRole: code",
    NULL
};

static void settle_tick(XtPointer closure, XtIntervalId *id)
{
    (void)id;
    *(Boolean *)closure = True;
}

static Boolean settle_gallery(GalleryState *state)
{
    unsigned idle_samples = 0;
    XtAppContext app = XtWidgetToApplicationContext(state->shell);
    for (unsigned int i = 0; i < 10000; ++i) {
        gallery_refresh_icon_resources();
        XSync(XtDisplay(state->shell), False);
        XtInputMask pending = XtAppPending(app);
        if (!pending) {
            /* A separate window manager may still be processing our configure
             * requests after XSync returns. Require several quiet event-loop
             * samples before asserting the resulting layout. */
            if (++idle_samples == 3) return gallery_validate_icon_resources();
            Boolean tick = False;
            XtAppAddTimeOut(app, 20, settle_tick, &tick);
            while (!tick) XtAppProcessEvent(app, XtIMAll);
            continue;
        }
        idle_samples = 0;
        if (i >= 9990 && (pending & XtIMXEvent)) {
            XEvent event;
            XPeekEvent(XtDisplay(state->shell), &event);
            Widget owner = XtWindowToWidget(XtDisplay(state->shell), event.xany.window);
            fprintf(stderr, "gallery settling: event=%d widget=%s window=%lu size=%dx%d\n",
                event.type, owner ? XtName(owner) : "unknown", event.xany.window,
                event.type == ConfigureNotify ? event.xconfigure.width : 0,
                event.type == ConfigureNotify ? event.xconfigure.height : 0);
        }
        XtAppProcessEvent(app, XtIMAll);
    }
    return False;
}

/* Exercise transactions on the real gallery hierarchy without recreating
 * the selected example. Startup-only captures cannot prove this contract. */
static int verify_live_appearance(GalleryState *state)
{
    static const XmAppearanceProfile profiles[] = {
        XmAPPEARANCE_MODERN_LIGHT, XmAPPEARANCE_MODERN_DARK,
        XmAPPEARANCE_CLASSIC, XmAPPEARANCE_MODERN_LIGHT
    };
    static const double scales[] = {1.0, 1.5, 2.0, 1.0};
    Display *display = XtDisplay(state->shell);
    unsigned cases = 0;
    for (Cardinal i = 0; i < GALLERY_MANIFEST_COUNT; ++i) {
        const GalleryManifestEntry *entry = &gallery_manifest[i];
        if (!entry->constructible || !entry->class) continue;
        XmTreeViewSetSelectedId(state->tree, (XtPointer)entry->id, True);
        if (!settle_gallery(state)) return 1;
        Widget component = state->component;
        unsigned long callbacks = state->callback_count;
        printf("gallery live: %s\n", entry->id);
        fflush(stdout);
        for (unsigned step = 0; step < XtNumber(profiles); ++step) {
            XmAppearanceDensity density = step % 2 ?
                XmAPPEARANCE_DENSITY_COMPACT : XmAPPEARANCE_DENSITY_COMFORTABLE;
            XmAppearanceProfile actual_profile;
            XmAppearanceDensity actual_density;
            double actual_scale;
            Dimension width, height, host_width, host_height;
            const char *failed = NULL;
            if (!XmApplyAppearanceProfile(display, profiles[step])) failed = "profile";
            else if (!XmApplyAppearanceDensity(display, density)) failed = "density";
            else if (!XmSetDisplayScaleOverride(display, scales[step])) failed = "scale";
            else if (!settle_gallery(state)) failed = "event settling";
            if (failed) {
                fprintf(stderr, "gallery live: transaction failed %s step=%u operation=%s\n", entry->id, step, failed);
                return 1;
            }
            if (!XmGetAppearanceProfile(display, &actual_profile) ||
                !XmGetAppearanceDensity(display, &actual_density) ||
                !XmGetWidgetScale(component, &actual_scale) ||
                actual_profile != profiles[step] || actual_density != density ||
                fabs(actual_scale - scales[step]) > 0.001 ||
                state->component != component || state->selected != entry ||
                XmTreeViewGetSelectedId(state->tree) != (XtPointer)entry->id ||
                state->callback_count != callbacks || state->current_page != 2) {
                fprintf(stderr, "gallery live: state changed %s step=%u\n", entry->id, step);
                return 1;
            }
            XtVaGetValues(component, XmNwidth, &width, XmNheight, &height, NULL);
            host_width = width;
            host_height = height;
            if (state->preview_viewport)
                XtVaGetValues(state->component_host, XmNwidth, &host_width,
                              XmNheight, &host_height, NULL);
            if (!width || !height || (state->preview_viewport &&
                (width > host_width || height > host_height))) {
                fprintf(stderr, "gallery live: canvas clips %s step=%u component=%ux%u host=%ux%u\n",
                    entry->id, step, width, height, host_width, host_height);
                return 1;
            }
            ++cases;
        }
    }
    printf("PASS gallery live appearance: %u transitions\n", cases);
    return 0;
}

static int appearance_control_failure(int line)
{
    fprintf(stderr, "FAIL gallery appearance controls at line %d\n", line);
    return 1;
}

static int verify_appearance_controls(GalleryState *state)
{
    Display *display = XtDisplay(state->shell);
    const GalleryManifestEntry *entry = find_entry("xm.pushbuttonwidgetclass");
    XmTreeViewSetSelectedId(state->tree, (XtPointer)entry->id, True);
    Widget component = state->component;
    unsigned long callbacks = state->callback_count;
    const XmAppearanceProfile themes[] = {XmAPPEARANCE_MODERN_LIGHT,
        XmAPPEARANCE_MODERN_DARK, XmAPPEARANCE_CLASSIC};
    const double scales[] = {1.0, 1.5, 2.0, 3.0, 0.0};
    double existing_scale;
    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_CLASSIC) ||
        (XmGetDisplayScaleOverride(display, &existing_scale) &&
         !XmClearDisplayScaleOverride(display)) || !settle_gallery(state)) return appearance_control_failure(__LINE__);
    XSetInputFocus(display, XtWindow(state->shell), RevertToParent, CurrentTime);
    for (unsigned group = 0; group < 2; ++group) {
        Widget control = group ? state->scale_control : state->theme_control;
        unsigned count = group ? XtNumber(scales) : XtNumber(themes);
        if (!XtIsManaged(control) || !XtIsSensitive(control)) return appearance_control_failure(__LINE__);
        for (unsigned i = 0; i < count; ++i) {
            if (!XmProcessTraversal(control, XmTRAVERSE_CURRENT)) return appearance_control_failure(__LINE__);
            XtCallActionProc(control, "ArmAndActivate", NULL, NULL, 0);
            if (!settle_gallery(state)) return appearance_control_failure(__LINE__);
            if (group) {
                double actual = 0.0;
                Boolean overridden = XmGetDisplayScaleOverride(display, &actual);
                if ((scales[i] == 0.0 && overridden) ||
                    (scales[i] != 0.0 && (!overridden || fabs(actual-scales[i]) > 0.001))) return appearance_control_failure(__LINE__);
            } else {
                XmAppearanceProfile actual;
                if (!XmGetAppearanceProfile(display, &actual) || actual != themes[i]) return appearance_control_failure(__LINE__);
            }
            if (XmGetFocusWidget(state->shell) != control ||
                state->component != component || state->selected != entry ||
                state->callback_count != callbacks || state->current_page != 2) {
                fprintf(stderr, "gallery controls: state/focus changed group=%u step=%u\n", group,i);
                return appearance_control_failure(__LINE__);
            }
        }
    }
    puts("PASS gallery appearance controls: activation, focus and state");
    return 0;
}

static Boolean child_fits(Widget child, Widget parent)
{
    Position x, y;
    Dimension width, height, parent_width, parent_height;
    XtVaGetValues(child, XmNx, &x, XmNy, &y, XmNwidth, &width, XmNheight, &height, NULL);
    XtVaGetValues(parent, XmNwidth, &parent_width, XmNheight, &parent_height, NULL);
    return x >= 0 && y >= 0 && x + width <= parent_width && y + height <= parent_height;
}

static int verify_responsive_layout(GalleryState *state)
{
    double scale = 1.0;
    (void)XmGetWidgetScale(state->shell, &scale);
    Dimension wide = (Dimension)(1100 * scale), high = (Dimension)(740 * scale);
    Dimension narrow = (Dimension)(450 * scale), low = (Dimension)(325 * scale);
    const GalleryManifestEntry *entry = find_entry("xm.selectionboxwidgetclass");
    if (!entry) return 1;
    show_entry(state, entry);
#define RESPONSIVE_CHECK(condition, message) do { \
    if (!(condition)) { fprintf(stderr, "gallery responsive: %s\n", message); return 1; } \
} while (0)
    XtVaSetValues(state->shell, XmNwidth, wide, XmNheight, high, NULL);
    RESPONSIVE_CHECK(settle_gallery(state), "wide layout did not settle");
    RESPONSIVE_CHECK(!state->compact_layout && XmSidebarGetExpanded(state->sidebar), "wide navigation missing");
    XtVaSetValues(state->shell, XmNwidth, narrow, XmNheight, low, NULL);
    RESPONSIVE_CHECK(settle_gallery(state), "narrow layout did not settle");
    RESPONSIVE_CHECK(state->compact_layout && !XmSidebarGetExpanded(state->sidebar) &&
        XtIsManaged(state->workspace), "narrow mode did not release workspace");
    RESPONSIVE_CHECK(child_fits(state->tabs,state->workspace) &&
        child_fits(state->title,state->example) && child_fits(state->provenance,state->example) &&
        child_fits(state->preview_viewport,state->example) && child_fits(state->feedback,state->example),
        "narrow content clipped");
    XmSidebarToggle(state->sidebar, NULL);
    RESPONSIVE_CHECK(settle_gallery(state), "navigation page did not settle");
    RESPONSIVE_CHECK(XmSidebarGetExpanded(state->sidebar) && !XtIsManaged(state->workspace) &&
        child_fits(state->sidebar,state->body), "navigation page did not replace workspace");
    const GalleryManifestEntry *next = find_entry("xm.messageboxwidgetclass");
    RESPONSIVE_CHECK(next != NULL, "missing message box entry");
    XmTreeViewSetSelectedId(state->tree, (XtPointer)next->id, True);
    RESPONSIVE_CHECK(settle_gallery(state) && XmSidebarGetExpanded(state->sidebar),
        "selection prematurely closed navigation");
    RESPONSIVE_CHECK(XmProcessTraversal(state->tree, XmTRAVERSE_CURRENT), "tree could not receive focus");
    XEvent activation = {0};
    activation.xkey.type = KeyPress;
    activation.xkey.display = XtDisplay(state->tree);
    activation.xkey.window = XtWindow(state->tree);
    activation.xkey.keycode = XKeysymToKeycode(XtDisplay(state->tree), XK_Return);
    XtDispatchEvent(&activation);
    RESPONSIVE_CHECK(settle_gallery(state), "selected page did not settle");
    RESPONSIVE_CHECK(!XmSidebarGetExpanded(state->sidebar) && XtIsManaged(state->workspace),
        "selection did not return to example");
    XmSidebarToggle(state->sidebar, NULL);
    RESPONSIVE_CHECK(settle_gallery(state) && XmSidebarGetExpanded(state->sidebar),
        "semantic navigation page did not open");
    RESPONSIVE_CHECK(state->accessibility != NULL, "missing accessibility bridge");
    XmAccessibleId tree_id = XmAccessibilityWidget(state->accessibility, state->tree);
    RESPONSIVE_CHECK(tree_id && XmAccessibilityAction(state->accessibility, tree_id, 0),
        "semantic activation rejected selected page");
    RESPONSIVE_CHECK(settle_gallery(state) && !XmSidebarGetExpanded(state->sidebar) &&
        XtIsManaged(state->workspace), "semantic activation did not return to example");
    XtVaSetValues(state->shell, XmNwidth, wide, XmNheight, high, NULL);
    RESPONSIVE_CHECK(settle_gallery(state) && XmSidebarGetExpanded(state->sidebar), "desktop preference was lost");
    XmSidebarSetExpanded(state->sidebar, False);
    XtVaSetValues(state->shell, XmNwidth, narrow, XmNheight, low, NULL);
    RESPONSIVE_CHECK(settle_gallery(state), "second narrow layout did not settle");
    XtVaSetValues(state->shell, XmNwidth, wide, XmNheight, high, NULL);
    RESPONSIVE_CHECK(settle_gallery(state) && !XmSidebarGetExpanded(state->sidebar),
        "manual desktop collapse was overwritten");
#undef RESPONSIVE_CHECK
    puts("PASS gallery responsive layout, navigation selection and preference restoration");
    return 0;
}

static Boolean same_words(const char *left, const char *right)
{
    while (*left || *right) {
        while (isspace((unsigned char)*left)) ++left;
        while (isspace((unsigned char)*right)) ++right;
        const char *a = left, *b = right;
        while (*left && !isspace((unsigned char)*left)) ++left;
        while (*right && !isspace((unsigned char)*right)) ++right;
        if (left-a != right-b || strncmp(a,b,(size_t)(left-a))) return False;
    }
    return True;
}

static char *flow_plain_text(XmString label)
{
    XmStringContext context;
    char *result = XtNewString("");
    size_t used = 0;
    if (!XmStringInitContext(&context,label)) return result;
    char *segment, *charset; XmStringDirection direction; Boolean separator;
    while (XmStringGetNextSegment(context,&segment,&charset,&direction,&separator)) {
        size_t length = strlen(segment);
        result = XtRealloc(result,used+length+(separator ? 1 : 0)+1);
        memcpy(result+used,segment,length); used += length;
        if (separator) result[used++] = '\n';
        result[used] = '\0';
        XtFree(segment); XtFree(charset);
    }
    XmStringFreeContext(context);
    return result;
}

static int verify_flow_pages(GalleryState *state)
{
    double scale = 1.0;
    (void)XmGetWidgetScale(state->shell, &scale);
    Boolean svg = getenv("XM_EXPECT_SVG_UNAVAILABLE") == NULL;
    unsigned cases = 0;
    Dimension first_height[2] = {0,0};
    for (unsigned page = 0; page < 2; ++page) {
        GalleryFlowPage *flow = page ? &state->icons_flow : &state->welcome_flow;
        select_page(state, page ? 4 : 1);
        for (unsigned size = 0; size < 3; ++size) {
            Boolean narrow = size == 1;
            XtVaSetValues(state->shell, XmNwidth, (Dimension)((narrow ? 450 : 1100) * scale),
                XmNheight, (Dimension)((narrow ? 325 : 740) * scale), NULL);
            if (!settle_gallery(state)) {
                fprintf(stderr,"gallery pages: layout did not settle\n"); return 1;
            }
            Dimension canvas_width, canvas_height, clip_width;
            XtVaGetValues(flow->canvas, XmNwidth, &canvas_width, XmNheight, &canvas_height, NULL);
            XtVaGetValues(flow->clip, XmNwidth, &clip_width, NULL);
            if (canvas_width > clip_width) {
                fprintf(stderr,"gallery pages: page %u horizontal overflow %u > %u\n",page,canvas_width,clip_width); return 1;
            }
            if (size == 0) first_height[page] = canvas_height;
            if (!page && narrow && canvas_height <= first_height[page]) {
                fprintf(stderr,"gallery pages: narrow prose did not reflow\n"); return 1;
            }
            if (size == 2 && canvas_height != first_height[page]) {
                fprintf(stderr,"gallery pages: wide layout did not restore\n"); return 1;
            }
            for (Cardinal i = 0; i < flow->count; ++i) {
                if (!child_fits(flow->items[i],flow->canvas)) {
                    fprintf(stderr,"gallery pages: item outside canvas\n"); return 1;
                }
                Position item_x,item_y; Dimension item_width,item_height;
                XtVaGetValues(flow->items[i],XmNx,&item_x,XmNy,&item_y,
                    XmNwidth,&item_width,XmNheight,&item_height,NULL);
                for (Cardinal j=0;j<i;++j) {
                    Position x,y; Dimension width,height;
                    XtVaGetValues(flow->items[j],XmNx,&x,XmNy,&y,XmNwidth,&width,XmNheight,&height,NULL);
                    if (item_x < x+width && x < item_x+item_width &&
                        item_y < y+height && y < item_y+item_height) {
                        fprintf(stderr,"gallery pages: page %u size %u items %u and %u overlap (%d,%d %ux%u / %d,%d %ux%u)\n",page,size,i,j,item_x,item_y,item_width,item_height,x,y,width,height); return 1;
                    }
                }
                if (!page) {
                    XmString label = NULL; char *value = NULL;
                    XtVaGetValues(flow->items[i],XmNlabelString,&label,NULL);
                    if (label) value = flow_plain_text(label);
                    Boolean valid = value && same_words(value,flow->paragraphs[i]);
                    XmStringFree(label); XtFree(value);
                    if (!valid) { fprintf(stderr,"gallery pages: prose lost during wrapping\n"); return 1; }
                } else {
                    Widget bitmap = XtNameToWidget(flow->items[i],"bitmap");
                    Widget picture = bitmap ? bitmap : XtNameToWidget(flow->items[i],"description");
                    Widget caption = XtNameToWidget(flow->items[i],"name");
                    Position caption_y, first_y;
                    XtVaGetValues(caption,XmNy,&caption_y,NULL);
                    XtVaGetValues(XtNameToWidget(flow->items[0],"name"),XmNy,&first_y,NULL);
                    if (!child_fits(picture,flow->items[i]) || !child_fits(caption,flow->items[i]) ||
                        caption_y != first_y) {
                        fprintf(stderr,"gallery pages: icon slots or caption alignment invalid\n"); return 1;
                    }
                    if ((svg || i < 5) != (bitmap != NULL)) {
                        fprintf(stderr,"gallery pages: incorrect SVG capability fallback\n"); return 1;
                    }
                    if (bitmap) {
                        Pixmap pixmap; Window root; int x,y; unsigned width,height,border,depth;
                        XtVaGetValues(bitmap,XmNlabelPixmap,&pixmap,NULL);
                        if (!XGetGeometry(XtDisplay(bitmap),pixmap,&root,&x,&y,&width,&height,&border,&depth) ||
                            (svg && (width != (unsigned)(40*scale+0.5) || height != width))) {
                            fprintf(stderr,"gallery pages: icon scale mismatch\n"); return 1;
                        }
                    }
                }
            }
            if (page && flow->count != 9) return 1;
            ++cases;
        }
    }
    /* Keep the same icon widgets alive through palette and DPI changes. The
     * settling check validates their assigned pixmaps, dimensions and pixels. */
    const XmAppearanceProfile profiles[]={XmAPPEARANCE_MODERN_LIGHT,
        XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT};
    const double scales[]={1.0,1.5,2.0,1.0};
    for(unsigned step=0;step<XtNumber(profiles);++step) {
        if(!XmApplyAppearanceProfile(XtDisplay(state->shell),profiles[step]) ||
           !XmSetDisplayScaleOverride(XtDisplay(state->shell),scales[step]) ||
           !settle_gallery(state)) {
            fprintf(stderr,"gallery pages: live icon transition failed at step %u\n",step);
            return 1;
        }
        for(Cardinal i=0;i<state->icons_flow.count;++i)
            if(!child_fits(state->icons_flow.items[i],state->icons_flow.canvas)) return 1;
        ++cases;
    }
    printf("PASS gallery flow pages: %u resize/content/icon scenarios\n",cases);
    return 0;
}

/* Read bounded EWMH cardinal data without trusting its format or length. */
static Boolean root_cardinals(Display *display, Window root, const char *name,
                             long offset, unsigned long count, unsigned long *values)
{
    Atom type;
    int format;
    unsigned long items, remaining;
    unsigned char *data = NULL;
    int result = XGetWindowProperty(display, root, XInternAtom(display, name, False),
        offset, (long)count, False, XA_CARDINAL, &type, &format, &items,
        &remaining, &data);
    Boolean valid = result == Success && type == XA_CARDINAL && format == 32 &&
                    items == count && data != NULL;
    if (valid) memcpy(values, data, count * sizeof(unsigned long));
    if (data) XFree(data);
    return valid;
}

static void configure_startup_geometry(Widget shell)
{
    Display *display = XtDisplay(shell);
    Screen *screen = XtScreen(shell);
    int left = 0, top = 0, width = WidthOfScreen(screen), height = HeightOfScreen(screen);
    unsigned long desktop = 0, area[4];
    double scale = 1.0;
    String geometry = NULL;
    Dimension resource_width = 0, resource_height = 0;
    int geometry_x, geometry_y;
    unsigned int geometry_width, geometry_height;
    int specified = 0;
    (void)XmGetWidgetScale(shell, &scale);
    (void)root_cardinals(display, RootWindowOfScreen(screen), "_NET_CURRENT_DESKTOP", 0, 1, &desktop);
    if (desktop < 4096 && root_cardinals(display, RootWindowOfScreen(screen),
            "_NET_WORKAREA", (long)(desktop * 4), 4, area) &&
        area[0] < (unsigned long)width && area[1] < (unsigned long)height &&
        area[2] > 0 && area[2] <= (unsigned long)width - area[0] &&
        area[3] > 0 && area[3] <= (unsigned long)height - area[1]) {
        left = (int)area[0]; top = (int)area[1];
        width = (int)area[2]; height = (int)area[3];
    }
    XtVaGetValues(shell, XtNgeometry, &geometry, XmNwidth, &resource_width,
                  XmNheight, &resource_height, NULL);
    if (geometry) specified = XParseGeometry(geometry, &geometry_x, &geometry_y,
                                             &geometry_width, &geometry_height);
    /* Leave five percent for decorations; do not assume a particular WM. */
    int available_width = width - width / 20;
    int available_height = height - height / 20;
    int target_width = (1100.0 * scale >= available_width ? available_width : (int)(1100.0 * scale + 0.5));
    int target_height = (740.0 * scale >= available_height ? available_height : (int)(740.0 * scale + 0.5));
    if (resource_width) target_width = resource_width;
    if (resource_height) target_height = resource_height;
    if (specified & WidthValue) target_width = (int)geometry_width;
    if (specified & HeightValue) target_height = (int)geometry_height;
    XtVaSetValues(shell, XmNwidth, target_width, XmNheight, target_height, NULL);
    /* Xt applies explicit positions, including negative offsets, at realize. */
    if (!(specified & XValue))
        XtVaSetValues(shell, XmNx, left + (width > target_width ? (width-target_width)/2 : 0), NULL);
    if (!(specified & YValue))
        XtVaSetValues(shell, XmNy, top + (height > target_height ? (height-target_height)/2 : 0), NULL);
}

int main(int argc, char **argv)
{
    XtAppContext app; Widget shell, root, header, body, sidebar, notebook;
    GalleryState state = {0};
    state.desktop_navigation_expanded = True;
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
        if (!strcmp(argv[argument], "--integration-all-demos")) {
            integration_mode = 8;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--integration-demos")) {
            integration_mode = 7;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--integration-test")) {
            integration_mode = 1;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--integration-controls")) {
            integration_mode = 6;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--integration-live")) {
            integration_mode = 5;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--integration-pages")) {
            integration_mode = 4;
            remove_count = 1;
        } else if (!strcmp(argv[argument], "--integration-responsive")) {
            integration_mode = 3;
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
                              gallery_resources, XmNallowShellResize, False, NULL);
    state.shell = shell;
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
    configure_startup_geometry(shell);
    root = XmVaCreateManagedForm(shell, "root", NULL);
    header = XmVaCreateManagedRowColumn(root, "header", XmNorientation, XmHORIZONTAL,
                                        XmNtopAttachment, XmATTACH_FORM,
                                        XmNleftAttachment, XmATTACH_FORM,
                                        XmNrightAttachment, XmATTACH_FORM, NULL);
    state.app_title = gallery_label(header, "title", "CK Core Motif / Component Gallery");
    state.theme_note = gallery_label(header, "themeNote", "Component reference");
    {
        XmString theme_label = text("Theme"), scale_label = text("Scale");
        state.theme_control = XtVaCreateManagedWidget("themeControl", xmPushButtonWidgetClass, header,
            XmNlabelString, theme_label, NULL);
        state.scale_control = XtVaCreateManagedWidget("dpiControl", xmPushButtonWidgetClass, header,
            XmNlabelString, scale_label, NULL);
        XmStringFree(theme_label);
        XmStringFree(scale_label);
        XtAddCallback(state.theme_control, XmNactivateCallback, theme_control, &state);
        XtAddCallback(state.scale_control, XmNactivateCallback, dpi_control, &state);
    }
    state.status = XmVaCreateManagedStatusBar(root, "status", XmNbottomAttachment, XmATTACH_FORM,
                                              XmNleftAttachment, XmATTACH_FORM,
                                              XmNrightAttachment, XmATTACH_FORM, NULL);
    body = XmVaCreateManagedForm(root, "body",
                                        XmNtopAttachment, XmATTACH_WIDGET,
                                        XmNtopWidget, header, XmNleftAttachment, XmATTACH_FORM,
                                        XmNrightAttachment, XmATTACH_FORM,
                                        XmNbottomAttachment, XmATTACH_WIDGET,
                                        XmNbottomWidget, state.status, NULL);
    state.body = body;
    {
        XmString title = XmStringCreateLocalized("Components");
        sidebar = XmVaCreateManagedSidebar(body, "navigation",
            XmNlabelString, title, XmNsidebarExpandedWidth, 280,
            XmNtopAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_FORM, XmNtraversalOn, True, NULL);
        XmStringFree(title);
    }
    state.sidebar = sidebar;
    {
        Widget content = XmSidebarGetContent(sidebar);
        Widget label;
        XtVaSetValues(content, XmNpacking, XmPACK_NONE,
            XmNmarginWidth, 0, XmNmarginHeight, 0,
            XmNresizeWidth, False, XmNresizeHeight, False, NULL);
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
        {
            Widget viewport = XtVaCreateManagedWidget("navigationViewport", xmScrolledWindowWidgetClass,
                state.navigation_form, XmNscrollingPolicy, XmAPPLICATION_DEFINED,
                XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, state.search, XmNtopOffset, 12,
                XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
                XmNbottomAttachment, XmATTACH_FORM, NULL);
            state.tree = XtVaCreateManagedWidget("components", xmTreeViewWidgetClass,
                viewport, XmNtraversalOn, True, XmNtreeViewVisibleRows, 17, NULL);
        }
        XtAddEventHandler(content, StructureNotifyMask, False, navigation_resize, &state);
        XtAddEventHandler(state.tree, StructureNotifyMask, False, navigation_resize, &state);
    }
    XtManageChild(state.tree);
    {
        Widget pane = XmVaCreateManagedForm(body, "workspace",
            XmNtopAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_WIDGET, XmNleftWidget, sidebar,
            XmNrightAttachment, XmATTACH_FORM, NULL);
        state.workspace = pane;
        XmSegmentedControlItem views[3];
        XmString view_labels[3] = {text("Example"), text("Source"), text("Icons")};
        const char *view_ids[3] = {"example", "source", "icons"};
        state.tabs = XmVaCreateSegmentedControl(pane, "viewTabs",
            XmNtopAttachment, XmATTACH_FORM, XmNleftAttachment, XmATTACH_FORM,
            XmNtopOffset, 12, XmNleftOffset, 24, NULL);
        for (unsigned i = 0; i < 3; ++i)
            views[i] = (XmSegmentedControlItem){view_ids[i], view_labels[i], XmUNSPECIFIED_PIXMAP, True};
        XmSegmentedControlSetItems(state.tabs, views, 3);
        for (unsigned i = 0; i < 3; ++i) XmStringFree(view_labels[i]);
        XtAddCallback(state.tabs, XmNSegmentedSelectionCallback, select_tab, &state);
        notebook = XmVaCreateManagedForm(pane, "content",             XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, state.tabs,
            XmNbottomAttachment, XmATTACH_FORM, XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM, NULL);
        state.notebook = notebook;
    }
    create_welcome(&state);
    create_demos(&state);
    state.icons = create_icon_page(&state);
    XtAddCallback(state.tree, XmNtreeViewSelectionCallback, tree_selected, &state);
    XtAddCallback(state.tree, XmNactivateCallback, activate_navigation, &state);
    XtAddCallback(sidebar, XmNsidebarToggleCallback, toggle_sidebar, &state);
    add_tree_nodes(state.tree, NULL);
    {
        XtWidgetGeometry preferred = {0};
        Dimension padding = 0;
        XmGetAppearanceSpacing(XtDisplay(shell), XmAPPEARANCE_SPACING_SMALL, &padding);
        XtQueryGeometry(state.tree, NULL, &preferred);
        state.navigation_width = preferred.width + 2*padding;
        XtVaSetValues(sidebar, XmNsidebarExpandedWidth, state.navigation_width, NULL);
    }
    {
        const GalleryManifestEntry *initial = initial_id ? find_entry(initial_id)
                                                         : &gallery_manifest[0];
        Boolean special_page = initial_id && (!strcmp(initial_id, "gallery.welcome") ||
                                              !strcmp(initial_id, "gallery.icons") ||
                                              !strcmp(initial_id, demos_id));
        if (initial_id && !special_page && (!initial || strcmp(initial->id, initial_id))) {
            fprintf(stderr, "unknown gallery component: %s\n", initial_id);
            return 2;
        }
        XmTreeViewSetSelectedId(state.tree,
            (XtPointer)(initial_id && !special_page ? initial->id :
                (initial_id && !strcmp(initial_id, demos_id) ? demos_id :
                 initial_id && !strcmp(initial_id, icons_id) ? icons_id : welcome_id)), False);
        if (initial_id && strcmp(initial_id, "gallery.icons") != 0 &&
            strcmp(initial_id, "gallery.welcome") != 0 && strcmp(initial_id, demos_id) != 0) {
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
    } else if (initial_id && !strcmp(initial_id, demos_id)) {
        select_page(&state, 5);
        set_status(&state, "Demo applications - choose Open to launch an example");
    } else if (initial_id) {
        const GalleryManifestEntry *initial = find_entry(initial_id);
        expand_entry_category(state.tree, initial);
        show_entry(&state, initial);
        select_page(&state, 2);
    }
    XtManageChild(root);
    XtRealizeWidget(shell);
    layout_navigation(&state);
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
        strcmp(initial_id, "gallery.welcome") != 0 && strcmp(initial_id, demos_id) != 0)
        select_page(&state, 2);
    if (initial_id && !strcmp(initial_id, demos_id)) select_page(&state, 5);
    layout_gallery(&state);
    XtAddEventHandler(body, StructureNotifyMask, False, gallery_configured, &state);
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
    if (integration_mode == 2) return verify_view_navigation(&state);
    if (integration_mode == 3) return verify_responsive_layout(&state);
    if (integration_mode == 4) return verify_flow_pages(&state);
    if (integration_mode == 5) return verify_live_appearance(&state);
    if (integration_mode == 6) return verify_appearance_controls(&state);
    if (integration_mode == 7) return verify_demo_apps(&state);
    if (integration_mode == 8) return verify_all_demo_apps(&state);
    if (screenshot_mode) XtAppAddWorkProc(app, capture_ready, &state);
    XtAppMainLoop(app);
    return 0;
}
