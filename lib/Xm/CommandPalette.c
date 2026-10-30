#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Xm/CommandPaletteP.h>
#include <Xm/List.h>
#include <Xm/SearchBox.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <X11/keysym.h>
#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *); static void Destroy(Widget); static void Resize(Widget); static void ChangeManaged(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *); static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *, XtWidgetGeometry *); static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void query_changed(Widget, XtPointer, XtPointer); static void committed(Widget, XtPointer, XtPointer); static void default_action(Widget, XtPointer, XtPointer); static void events(Widget, XtPointer, XEvent *, Boolean *);
#define Offset(field) XtOffsetOf(XmCommandPaletteRec, palette.field)
static XtResource resources[] = {
    {XmNcommandPaletteInvokeCallback, XmCCommandPaletteInvokeCallback, XmRCallback, sizeof(XtCallbackList), Offset(invoke_callback), XmRPointer, NULL},
    {XmNcommandPaletteDismissCallback, XmCCommandPaletteDismissCallback, XmRCallback, sizeof(XtCallbackList), Offset(dismiss_callback), XmRPointer, NULL}
};
externaldef(xmcommandpaletteclassrec) XmCommandPaletteClassRec xmCommandPaletteClassRec = {
    {.superclass = (WidgetClass)&xmManagerClassRec, .class_name = "XmCommandPalette", .widget_size = sizeof(XmCommandPaletteRec), .initialize = Initialize,
     .realize = XtInheritRealize, .resources = resources, .num_resources = XtNumber(resources), .compress_motion = True, .compress_exposure = XtExposeCompressMultiple,
     .compress_enterleave = True, .destroy = Destroy, .resize = Resize, .set_values = SetValues, .set_values_almost = XtInheritSetValuesAlmost, .version = XtVersion,
     .tm_table = XtInheritTranslations, .query_geometry = QueryGeometry},
    {.geometry_manager = GeometryManager, .change_managed = ChangeManaged, .insert_child = XtInheritInsertChild, .delete_child = XtInheritDeleteChild},
    {.constraint_size = sizeof(XmManagerConstraintRec)}, {.translations = XtInheritTranslations, .parent_process = XmInheritParentProcess}, {NULL}};
externaldef(xmcommandpalettewidgetclass) WidgetClass xmCommandPaletteWidgetClass = (WidgetClass)&xmCommandPaletteClassRec;
static char *copy_text(XmString value) { char *text = value ? XmStringUnparse(value, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL) : NULL; return text ? text : XtNewString(""); }
static XmString copy_string(XmString value) { return value ? XmStringCopy(value) : XmStringCreateLocalized(""); }
static Boolean contains_folded(const char *text, const char *needle) { size_t i, n = strlen(needle), length = strlen(text); if (!n) return True; if (n > length) return False; for (i = 0; i + n <= length; ++i) { size_t j; for (j = 0; j < n; ++j) if (tolower((unsigned char)text[i + j]) != tolower((unsigned char)needle[j])) break; if (j == n) return True; } return False; }
static void free_items(XmCommandPaletteWidget palette) { Cardinal i; for (i = 0; i < palette->palette.item_count; ++i) { XtFree(palette->palette.items[i].id); XmStringFree(palette->palette.items[i].label); XmStringFree(palette->palette.items[i].shortcut); } XtFree((char *)palette->palette.items); XtFree((char *)palette->palette.visible); palette->palette.items = NULL; palette->palette.visible = NULL; palette->palette.item_count = palette->palette.visible_count = 0; }
static void free_item_array(XmCommandPaletteStoredItem *items, Cardinal count) { Cardinal i; if (!items) return; for (i = 0; i < count; ++i) { XtFree(items[i].id); XmStringFree(items[i].label); XmStringFree(items[i].shortcut); } XtFree((char *)items); }
static void preferred(Widget child, Dimension *width, Dimension *height) { XtWidgetGeometry wanted; if (width == NULL || height == NULL) return; if (child == NULL) { *width = 0; *height = 0; return; } wanted.request_mode = 0; XtQueryGeometry(child, NULL, &wanted); *width = wanted.request_mode & CWWidth ? wanted.width : child->core.width; *height = wanted.request_mode & CWHeight ? wanted.height : child->core.height; }
static XmString make_display_string(XmString label, XmString shortcut) {
    XmString result = label ? XmStringCopy(label) : XmStringCreateLocalized("");
    XmString separator;
    XmString suffix;
    XmString combined;

    if (!result)
        return NULL;
    if (!shortcut)
        return result;
    separator = XmStringCreateLocalized((char *)"  ");
    if (!separator) {
        XmStringFree(result);
        return NULL;
    }
    suffix = XmStringConcat(separator, shortcut);
    XmStringFree(separator);
    if (!suffix) {
        XmStringFree(result);
        return NULL;
    }
    combined = XmStringConcat(result, suffix);
    XmStringFree(result);
    XmStringFree(suffix);
    return combined;
}

static void rebuild(XmCommandPaletteWidget palette)
{
    char *filter;
    Cardinal i, visible = 0;
    XmString display;

    if (palette->palette.filtering || palette->palette.list == NULL ||
        palette->palette.search == NULL)
        return;
    palette->palette.filtering = True;
    filter = XmSearchBoxGetString(palette->palette.search);
    XtFree((char *)palette->palette.visible);
    if (palette->palette.item_count &&
        ((size_t)palette->palette.item_count > SIZE_MAX / sizeof(Cardinal) ||
         (size_t)palette->palette.item_count > UINT_MAX / sizeof(Cardinal))) {
        XtWarning("XmCommandPalette: visible item index is too large");
        XtFree(filter);
        palette->palette.visible_count = 0;
        palette->palette.filtering = False;
        return;
    }
    palette->palette.visible = palette->palette.item_count
        ? (Cardinal *)XtCalloc(palette->palette.item_count, sizeof(Cardinal))
        : NULL;
    XmListDeleteAllItems(palette->palette.list);
    if (palette->palette.item_count && !palette->palette.visible) {
        XtWarning("XmCommandPalette: unable to allocate visible item index");
        XtFree(filter);
        palette->palette.visible_count = 0;
        palette->palette.filtering = False;
        return;
    }
    for (i = 0; i < palette->palette.item_count; ++i) {
        char *label = copy_text(palette->palette.items[i].label);
        char *shortcut;
        Boolean has_shortcut;

        if (!label)
            continue;
        if (!contains_folded(label, filter ? filter : "")) {
            XtFree(label);
            continue;
        }
        shortcut = copy_text(palette->palette.items[i].shortcut);
        if (!shortcut) {
            XtFree(label);
            continue;
        }
        has_shortcut = shortcut[0] != '\0';
        display = make_display_string(
            palette->palette.items[i].label,
            has_shortcut ? palette->palette.items[i].shortcut : NULL);
        if (display) {
            XmListAddItem(palette->palette.list, display, 0);
            XmStringFree(display);
            palette->palette.visible[visible++] = i;
        }
        XtFree(shortcut);
        XtFree(label);
    }
    XtFree(filter);
    palette->palette.visible_count = visible;
    if (visible)
        XmListSelectPos(palette->palette.list, 1, False);
    palette->palette.filtering = False;
}
static void natural(XmCommandPaletteWidget palette, Dimension *width, Dimension *height) { Dimension sw, sh, lw, lh; preferred(palette->palette.search, &sw, &sh); preferred(palette->palette.list, &lw, &lh); *width = sw > lw ? sw : lw; if (*width < 240) *width = 240; *height = sh + (lh ? lh : 120); }
static void layout(XmCommandPaletteWidget palette) { Dimension sw, sh, width; if (palette->palette.initializing || palette->palette.laying_out) return; palette->palette.laying_out = True; preferred(palette->palette.search, &sw, &sh); width = palette->core.width ? palette->core.width : 240; if (palette->palette.search != NULL) XmeConfigureObject(palette->palette.search, 0, 0, width, sh, 0); if (palette->palette.list != NULL) XmeConfigureObject(palette->palette.list, 0, (Position)sh, width, palette->core.height > sh ? palette->core.height - sh : 1, 0); palette->palette.laying_out = False; }
static void negotiate(XmCommandPaletteWidget palette) { Dimension width, height, rw, rh; XtGeometryResult result; if (palette->palette.initializing || palette->palette.laying_out) return; natural(palette, &width, &height); result = XtMakeResizeRequest((Widget)palette, width, height, &rw, &rh); if (result == XtGeometryAlmost) XtMakeResizeRequest((Widget)palette, rw, rh, NULL, NULL); layout(palette); }
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) { XmCommandPaletteWidget palette = (XmCommandPaletteWidget)widget; Dimension width, height; XmString placeholder; (void)request; (void)args; (void)count; palette->palette.initializing = True; placeholder = XmStringCreateLocalized((char *)"Type a command"); palette->palette.search = XmVaCreateManagedSearchBox(widget, (char *)"search", XmNsearchPlaceholder, placeholder, NULL); XmStringFree(placeholder); palette->palette.list = XmCreateList(widget, (char *)"commands", NULL, 0); if (palette->palette.list != NULL) XtManageChild(palette->palette.list); if (palette->palette.search != NULL) { XtAddCallback(palette->palette.search, XmNsearchQueryChangedCallback, query_changed, palette); XtAddCallback(palette->palette.search, XmNsearchCommittedCallback, committed, palette); } if (palette->palette.list != NULL) XtAddCallback(palette->palette.list, XmNdefaultActionCallback, default_action, palette); XtAddEventHandler(widget, KeyPressMask, True, events, NULL); palette->palette.initializing = False; natural(palette, &width, &height); if (!palette->core.width) palette->core.width = width; if (!palette->core.height) palette->core.height = height; layout(palette); }
static void Destroy(Widget widget) { XmCommandPaletteWidget palette = (XmCommandPaletteWidget)widget; free_items(palette); XtRemoveEventHandler(widget, KeyPressMask, True, events, NULL); }
static void Resize(Widget widget) { layout((XmCommandPaletteWidget)widget); } static void ChangeManaged(Widget widget) { negotiate((XmCommandPaletteWidget)widget); }
static Boolean SetValues(Widget old_widget, Widget request, Widget widget, ArgList args, Cardinal *count) { (void)old_widget; (void)request; (void)widget; (void)args; (void)count; return False; }
static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request, XtWidgetGeometry *reply) { XmCommandPaletteWidget palette = (XmCommandPaletteWidget)XtParent(child); if (request->request_mode & (CWX | CWY | CWBorderWidth | CWSibling | CWStackMode)) return XtGeometryNo; if (!palette->palette.laying_out && !(request->request_mode & XtCWQueryOnly)) negotiate(palette); reply->request_mode = CWWidth | CWHeight; reply->width = child->core.width; reply->height = child->core.height; return XtGeometryAlmost; }
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *intended, XtWidgetGeometry *reply) { XmCommandPaletteWidget palette = (XmCommandPaletteWidget)widget; natural(palette, &reply->width, &reply->height); reply->request_mode = CWWidth | CWHeight; if (intended && intended->width == reply->width && intended->height == reply->height) return XtGeometryYes; return XtGeometryAlmost; }
static void query_changed(Widget widget, XtPointer data, XtPointer call) { (void)widget; (void)call; rebuild((XmCommandPaletteWidget)data); }
static void committed(Widget widget, XtPointer data, XtPointer call) { (void)widget; (void)call; XmCommandPaletteInvokeSelection((Widget)data, NULL); }
static void default_action(Widget widget, XtPointer data, XtPointer call) { (void)widget; (void)call; XmCommandPaletteInvokeSelection((Widget)data, NULL); }
static void events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) { KeySym key; char input[2]; (void)data; if (widget == NULL || event == NULL || !XmIsCommandPalette(widget)) return; if (event->type != KeyPress) return; XLookupString(&event->xkey, input, sizeof(input), &key, NULL); if (key == XK_Escape) { XmCommandPaletteDismiss(widget, event); if (dispatch) *dispatch = False; } }
static void invoke_callback(XmCommandPaletteWidget palette, XEvent *event) { int *positions = NULL; int count = 0; XmCommandPaletteCallbackStruct callback; Cardinal item; if (palette == NULL || palette->core.being_destroyed || !palette->palette.visible_count || palette->palette.list == NULL || palette->palette.visible == NULL || palette->palette.items == NULL || !XmListGetSelectedPos(palette->palette.list, &positions, &count) || count < 1 || positions == NULL || positions[0] < 1 || (Cardinal)positions[0] > palette->palette.visible_count) { if (positions) XtFree((char *)positions); return; } item = palette->palette.visible[(Cardinal)positions[0] - 1]; XtFree((char *)positions); callback.reason = XmCOMMAND_PALETTE_REASON_INVOKED; callback.event = event; callback.id = palette->palette.items[item].id; callback.user_data = palette->palette.items[item].user_data; XtCallCallbacks((Widget)palette, XmNcommandPaletteInvokeCallback, &callback); }
Widget XmCreateCommandPalette(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmCommandPaletteWidgetClass, parent, args, count); }
Widget XmVaCreateCommandPalette(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmCommandPaletteWidgetClass,parent,False,v,n); va_end(v); return w; }
Widget XmVaCreateManagedCommandPalette(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmCommandPaletteWidgetClass,parent,True,v,n); va_end(v); return w; }
void XmCommandPaletteSetItems(Widget widget, const XmCommandPaletteItem *items, Cardinal count) {
    XmCommandPaletteWidget palette;
    XmCommandPaletteStoredItem *replacement;
    Cardinal i;
    if (!widget || !XmIsCommandPalette(widget) || widget->core.being_destroyed)
        return;
    palette = (XmCommandPaletteWidget)widget;
    if (!count) { free_items(palette); rebuild(palette); return; }
    if (!items) { XtWarning("XmCommandPaletteSetItems: NULL item array"); return; }
    if ((size_t)count > SIZE_MAX / sizeof(*palette->palette.items) ||
        (size_t)count > UINT_MAX / sizeof(*palette->palette.items)) {
        XtWarning("XmCommandPaletteSetItems: item array is too large");
        return;
    }
    replacement = (XmCommandPaletteStoredItem *)XtCalloc(
        count, sizeof(*replacement));
    if (!replacement) {
        XtWarning("XmCommandPaletteSetItems: unable to allocate item array");
        return;
    }
    for (i = 0; i < count; ++i) {
        replacement[i].id = XtNewString(items[i].id ? items[i].id : "");
        replacement[i].label = copy_string(items[i].label);
        replacement[i].shortcut = copy_string(items[i].shortcut);
        if (!replacement[i].id || !replacement[i].label ||
            !replacement[i].shortcut) {
            free_item_array(replacement, i + 1);
            XtWarning("XmCommandPaletteSetItems: unable to copy item text");
            return;
        }
        replacement[i].user_data = items[i].user_data;
    }
    free_items(palette);
    palette->palette.items = replacement;
    palette->palette.item_count = count;
    rebuild(palette);
}
Cardinal XmCommandPaletteGetItemCount(Widget widget) { return widget && XmIsCommandPalette(widget) ? ((XmCommandPaletteWidget)widget)->palette.item_count : 0; }
Cardinal XmCommandPaletteGetVisibleCount(Widget widget) { return widget && XmIsCommandPalette(widget) ? ((XmCommandPaletteWidget)widget)->palette.visible_count : 0; }
void XmCommandPaletteSetFilter(Widget widget, const char *filter) { if (widget && XmIsCommandPalette(widget) && ((XmCommandPaletteWidget)widget)->palette.search != NULL) XmSearchBoxSetString(((XmCommandPaletteWidget)widget)->palette.search, filter ? filter : ""); }
char *XmCommandPaletteGetFilter(Widget widget) { return widget && XmIsCommandPalette(widget) ? XmSearchBoxGetString(((XmCommandPaletteWidget)widget)->palette.search) : NULL; }
void XmCommandPaletteShow(Widget widget) { XmCommandPaletteWidget palette; Window window; int revert; if (!widget || !XmIsCommandPalette(widget) || widget->core.being_destroyed) return; palette = (XmCommandPaletteWidget)widget; XGetInputFocus(XtDisplay(widget), &window, &revert); palette->palette.previous_focus = XtWindowToWidget(XtDisplay(widget), window); palette->palette.shown = True; XtManageChild(widget); if (palette->palette.search != NULL) XmProcessTraversal(palette->palette.search, XmTRAVERSE_CURRENT); }
void XmCommandPaletteDismiss(Widget widget, XEvent *event) { XmCommandPaletteWidget palette; XmCommandPaletteCallbackStruct callback; if (!widget || !XmIsCommandPalette(widget) || widget->core.being_destroyed) return; palette = (XmCommandPaletteWidget)widget; if (!palette->palette.shown) return; palette->palette.shown = False; XtUnmanageChild(widget); if (palette->palette.previous_focus && XtIsManaged(palette->palette.previous_focus)) XmProcessTraversal(palette->palette.previous_focus, XmTRAVERSE_CURRENT); callback.reason = XmCR_UNMAP; callback.event = event; callback.id = NULL; callback.user_data = NULL; XtCallCallbacks(widget, XmNcommandPaletteDismissCallback, &callback); }
void XmCommandPaletteInvokeSelection(Widget widget, XEvent *event) { if (widget && XmIsCommandPalette(widget)) invoke_callback((XmCommandPaletteWidget)widget, event); }
Boolean XmCommandPaletteIsShown(Widget widget) { return widget && XmIsCommandPalette(widget) ? ((XmCommandPaletteWidget)widget)->palette.shown : False; }
