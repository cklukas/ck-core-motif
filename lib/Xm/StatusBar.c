#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Xm/Label.h>
#include <Xm/StatusBarP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Resize(Widget);
static void ChangeManaged(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *,
                                        XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);

#define Offset(field) XtOffsetOf(XmStatusBarRec, status_bar.field)
static XtResource resources[] = {
    {XmNstatusBarSpacing, XmCStatusBarSpacing, XmRHorizontalDimension,
     sizeof(Dimension), Offset(spacing), XmRImmediate, (XtPointer)6}
};

externaldef(xmstatusbarclassrec) XmStatusBarClassRec xmStatusBarClassRec = {
    {.superclass = (WidgetClass)&xmManagerClassRec,
     .class_name = "XmStatusBar", .widget_size = sizeof(XmStatusBarRec),
     .initialize = Initialize, .realize = XtInheritRealize,
     .resources = resources, .num_resources = XtNumber(resources),
     .compress_motion = True, .compress_exposure = XtExposeCompressMultiple,
     .compress_enterleave = True, .destroy = Destroy, .resize = Resize,
     .set_values = SetValues, .set_values_almost = XtInheritSetValuesAlmost,
     .version = XtVersion, .tm_table = XtInheritTranslations,
     .query_geometry = QueryGeometry},
    {.geometry_manager = GeometryManager, .change_managed = ChangeManaged,
     .insert_child = XtInheritInsertChild, .delete_child = XtInheritDeleteChild},
    {.constraint_size = sizeof(XmManagerConstraintRec)},
    {.translations = XtInheritTranslations,
     .parent_process = XmInheritParentProcess},
    {NULL}};

externaldef(xmstatusbarwidgetclass) WidgetClass xmStatusBarWidgetClass =
    (WidgetClass)&xmStatusBarClassRec;

static char *text_copy(XmString value)
{
    char *text = value ? XmStringUnparse(value, NULL, XmCHARSET_TEXT,
                                          XmCHARSET_TEXT, NULL, 0,
                                          XmOUTPUT_ALL) : NULL;
    return text ? text : XtNewString("");
}

static XmString string_copy(XmString value)
{
    char *text = text_copy(value);
    XmString result = XmStringCreateLocalized(text);
    XtFree(text);
    return result;
}

static XmString display_string(const XmStatusBarItem *item)
{
    char *label = text_copy(item->label);
    char progress[64];
    size_t size;
    char *text;
    XmString result;

    if (item->kind != XmSTATUS_BAR_ITEM_PROGRESS) {
        result = XmStringCreateLocalized(label);
        XtFree(label);
        return result;
    }
    if (item->maximum > 0)
        snprintf(progress, sizeof(progress), " %d/%d", item->value,
                 item->maximum);
    else
        snprintf(progress, sizeof(progress), " %d", item->value);
    size = strlen(label) + strlen(progress) + 1;
    text = XtMalloc(size);
    strcpy(text, label);
    strcat(text, progress);
    result = XmStringCreateLocalized(text);
    XtFree(text);
    XtFree(label);
    return result;
}

static void free_items(XmStatusBarWidget bar, Boolean destroy_children)
{
    XmStatusBarStoredItem *items = bar->status_bar.items;
    Cardinal count = bar->status_bar.item_count;
    Cardinal i;

    bar->status_bar.items = NULL;
    bar->status_bar.item_count = 0;
    bar->status_bar.visible_count = 0;
    bar->status_bar.overflow_count = 0;
    for (i = 0; i < count; ++i) {
        if (destroy_children && items[i].child)
            XtDestroyWidget(items[i].child);
        XtFree(items[i].id);
        XmStringFree(items[i].label);
        XmStringFree(items[i].display);
    }
    XtFree((char *)items);
}

static void preferred(Widget child, Dimension *width, Dimension *height)
{
    XtWidgetGeometry wanted;

    wanted.request_mode = 0;
    XtQueryGeometry(child, NULL, &wanted);
    *width = wanted.request_mode & CWWidth ? wanted.width : child->core.width;
    *height = wanted.request_mode & CWHeight ? wanted.height : child->core.height;
}

static void natural(XmStatusBarWidget bar, Dimension *width, Dimension *height)
{
    Cardinal i;
    Dimension child_width, child_height;
    unsigned long total = 0;

    *height = 1;
    for (i = 0; i < bar->status_bar.item_count; ++i) {
        preferred(bar->status_bar.items[i].child, &child_width, &child_height);
        total += child_width;
        if (i)
            total += bar->status_bar.spacing;
        if (child_height > *height)
            *height = child_height;
    }
    *width = (Dimension)(total > USHRT_MAX ? USHRT_MAX : total ? total : 1);
}

static void layout(XmStatusBarWidget bar)
{
    Cardinal i, selected, visible = 0;
    Dimension width, height;
    unsigned long total = 0;
    Boolean *chosen;
    Position x = 0;

    if (bar->status_bar.initializing || bar->status_bar.laying_out)
        return;
    bar->status_bar.laying_out = True;
    chosen = bar->status_bar.item_count
                 ? XtCalloc(bar->status_bar.item_count, sizeof(Boolean))
                 : NULL;
    for (selected = 0; selected < bar->status_bar.item_count; ++selected) {
        Cardinal candidate = bar->status_bar.item_count;
        unsigned char priority = 0;
        for (i = 0; i < bar->status_bar.item_count; ++i) {
            if (chosen[i] || (candidate < bar->status_bar.item_count &&
                              bar->status_bar.items[i].priority <= priority))
                continue;
            candidate = i;
            priority = bar->status_bar.items[i].priority;
        }
        if (candidate == bar->status_bar.item_count)
            break;
        preferred(bar->status_bar.items[candidate].child, &width, &height);
        if (visible && total + bar->status_bar.spacing + width > bar->core.width)
            continue;
        chosen[candidate] = True;
        total += width + (visible ? bar->status_bar.spacing : 0);
        ++visible;
    }
    if (!visible && bar->status_bar.item_count) {
        preferred(bar->status_bar.items[0].child, &width, &height);
        chosen[0] = True;
        visible = 1;
    }
    for (i = 0; i < bar->status_bar.item_count; ++i) {
        XmStatusBarStoredItem *item = &bar->status_bar.items[i];
        preferred(item->child, &width, &height);
        item->visible = chosen && chosen[i];
        XtSetMappedWhenManaged(item->child, item->visible);
        if (item->visible) {
            XmeConfigureObject(item->child, x, 0, width, height, 0);
            x += width + bar->status_bar.spacing;
        }
    }
    bar->status_bar.visible_count = visible;
    bar->status_bar.overflow_count = bar->status_bar.item_count - visible;
    XtFree((char *)chosen);
    bar->status_bar.laying_out = False;
}

static void negotiate(XmStatusBarWidget bar)
{
    Dimension width, height, reply_width, reply_height;
    XtGeometryResult result;

    if (bar->status_bar.initializing || bar->status_bar.laying_out)
        return;
    natural(bar, &width, &height);
    result = XtMakeResizeRequest((Widget)bar, width, height, &reply_width,
                                 &reply_height);
    if (result == XtGeometryAlmost)
        XtMakeResizeRequest((Widget)bar, reply_width, reply_height, NULL, NULL);
    layout(bar);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count)
{
    XmStatusBarWidget bar = (XmStatusBarWidget)widget;
    (void)request;
    (void)args;
    (void)count;
    bar->status_bar.initializing = False;
    bar->status_bar.laying_out = False;
}

static void Destroy(Widget widget)
{
    free_items((XmStatusBarWidget)widget, False);
}

static void Resize(Widget widget)
{
    layout((XmStatusBarWidget)widget);
}

static void ChangeManaged(Widget widget)
{
    negotiate((XmStatusBarWidget)widget);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count)
{
    XmStatusBarWidget old = (XmStatusBarWidget)old_widget;
    XmStatusBarWidget bar = (XmStatusBarWidget)widget;
    (void)request;
    (void)args;
    (void)count;
    if (old->status_bar.spacing != bar->status_bar.spacing) {
        layout(bar);
        return True;
    }
    return False;
}

static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request,
                                        XtWidgetGeometry *reply)
{
    XmStatusBarWidget bar = (XmStatusBarWidget)XtParent(child);
    if (request->request_mode &
        (CWX | CWY | CWBorderWidth | CWSibling | CWStackMode))
        return XtGeometryNo;
    if (!bar->status_bar.laying_out && !(request->request_mode & XtCWQueryOnly))
        negotiate(bar);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = child->core.width;
    reply->height = child->core.height;
    return XtGeometryAlmost;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply)
{
    XmStatusBarWidget bar = (XmStatusBarWidget)widget;
    natural(bar, &reply->width, &reply->height);
    reply->request_mode = CWWidth | CWHeight;
    if (request && request->width == reply->width &&
        request->height == reply->height)
        return XtGeometryYes;
    return XtGeometryAlmost;
}

Widget XmCreateStatusBar(Widget parent, char *name, ArgList args, Cardinal count)
{
    return XtCreateWidget(name, xmStatusBarWidgetClass, parent, args, count);
}

Widget XmVaCreateStatusBar(Widget parent, char *name, ...)
{
    Widget widget;
    va_list values;
    int count;
    Va_start(values, name);
    count = XmeCountVaListSimple(values);
    va_end(values);
    Va_start(values, name);
    widget = XmeVLCreateWidget(name, xmStatusBarWidgetClass, parent, False,
                               values, count);
    va_end(values);
    return widget;
}

Widget XmVaCreateManagedStatusBar(Widget parent, char *name, ...)
{
    Widget widget;
    va_list values;
    int count;
    Va_start(values, name);
    count = XmeCountVaListSimple(values);
    va_end(values);
    Va_start(values, name);
    widget = XmeVLCreateWidget(name, xmStatusBarWidgetClass, parent, True,
                               values, count);
    va_end(values);
    return widget;
}

void XmStatusBarSetItems(Widget widget, const XmStatusBarItem *items,
                         Cardinal count)
{
    XmStatusBarWidget bar;
    Cardinal i;

    if (!widget || !XmIsStatusBar(widget) || (count && !items))
        return;
    bar = (XmStatusBarWidget)widget;
    bar->status_bar.laying_out = True;
    free_items(bar, True);
    if (count)
        bar->status_bar.items = (XmStatusBarStoredItem *)XtCalloc(
            count, sizeof(*bar->status_bar.items));
    for (i = 0; i < count; ++i) {
        XmString display;
        XmStatusBarStoredItem *stored = &bar->status_bar.items[i];
        stored->id = XtNewString(items[i].id ? items[i].id : "");
        stored->label = string_copy(items[i].label);
        stored->display = NULL;
        stored->kind = items[i].kind;
        stored->priority = items[i].priority;
        stored->value = items[i].value;
        stored->maximum = items[i].maximum;
        stored->user_data = items[i].user_data;
        display = display_string(&items[i]);
        stored->child = XtVaCreateManagedWidget(
            "item", xmLabelWidgetClass, widget, XmNlabelString, display,
            XmNborderWidth, 0, XmNrecomputeSize, True, NULL);
        XmStringFree(display);
    }
    bar->status_bar.item_count = count;
    bar->status_bar.laying_out = False;
    layout(bar);
    negotiate(bar);
}

Cardinal XmStatusBarGetItemCount(Widget widget)
{
    return widget && XmIsStatusBar(widget)
               ? ((XmStatusBarWidget)widget)->status_bar.item_count : 0;
}

Cardinal XmStatusBarGetVisibleItemCount(Widget widget)
{
    return widget && XmIsStatusBar(widget)
               ? ((XmStatusBarWidget)widget)->status_bar.visible_count : 0;
}

Cardinal XmStatusBarGetOverflowCount(Widget widget)
{
    return widget && XmIsStatusBar(widget)
               ? ((XmStatusBarWidget)widget)->status_bar.overflow_count : 0;
}
