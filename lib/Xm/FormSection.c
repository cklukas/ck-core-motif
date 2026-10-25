#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Xm/FormSectionP.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Resize(Widget);
static void ChangeManaged(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
#define Offset(field) XtOffsetOf(XmFormSectionRec, section.field)
static XtResource resources[] = {
    {XmNformSectionHeader, XmCFormSectionHeader, XmRXmString, sizeof(XmString), Offset(header_text), XmRImmediate, NULL},
    {XmNformSectionFooter, XmCFormSectionFooter, XmRXmString, sizeof(XmString), Offset(footer_text), XmRImmediate, NULL},
    {XmNformSectionMarginWidth, XmCFormSectionMarginWidth, XmRHorizontalDimension, sizeof(Dimension), Offset(margin_width), XmRImmediate, (XtPointer)6},
    {XmNformSectionMarginHeight, XmCFormSectionMarginHeight, XmRVerticalDimension, sizeof(Dimension), Offset(margin_height), XmRImmediate, (XtPointer)4},
    {XmNformSectionSpacing, XmCFormSectionSpacing, XmRVerticalDimension, sizeof(Dimension), Offset(spacing), XmRImmediate, (XtPointer)4}
};
static XmSyntheticResource synthetic_resources[] = {
    {XmNformSectionMarginWidth, sizeof(Dimension), Offset(margin_width), XmeFromHorizontalPixels, XmeToHorizontalPixels},
    {XmNformSectionMarginHeight, sizeof(Dimension), Offset(margin_height), XmeFromVerticalPixels, XmeToVerticalPixels},
    {XmNformSectionSpacing, sizeof(Dimension), Offset(spacing), XmeFromVerticalPixels, XmeToVerticalPixels}
};
externaldef(xmformsectionclassrec) XmFormSectionClassRec xmFormSectionClassRec = {
    {.superclass = (WidgetClass)&xmManagerClassRec, .class_name = "XmFormSection",
     .widget_size = sizeof(XmFormSectionRec), .initialize = Initialize,
     .realize = XtInheritRealize, .resources = resources,
     .num_resources = XtNumber(resources), .compress_motion = True,
     .compress_exposure = XtExposeCompressMultiple, .compress_enterleave = True,
     .destroy = Destroy, .resize = Resize, .set_values = SetValues,
     .set_values_almost = XtInheritSetValuesAlmost, .version = XtVersion,
     .tm_table = XtInheritTranslations, .query_geometry = QueryGeometry},
    {.geometry_manager = GeometryManager, .change_managed = ChangeManaged,
     .insert_child = XtInheritInsertChild, .delete_child = XtInheritDeleteChild},
    {.constraint_size = sizeof(XmManagerConstraintRec)},
    {.translations = XtInheritTranslations, .syn_resources = synthetic_resources,
     .num_syn_resources = XtNumber(synthetic_resources),
     .parent_process = XmInheritParentProcess}, {NULL}};
externaldef(xmformsectionwidgetclass) WidgetClass xmFormSectionWidgetClass =
    (WidgetClass)&xmFormSectionClassRec;

static XmString copy_string(XmString source) {
    char *text; XmString result;
    if (!source) return XmStringCreateLocalized("");
    text = XmStringUnparse(source, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                           NULL, 0, XmOUTPUT_ALL);
    if (!text) return XmStringCreateLocalized("");
    result = XmStringCreateLocalized(text); XtFree(text); return result;
}
static Dimension dimension(unsigned long value) {
    return (Dimension)(value > USHRT_MAX ? USHRT_MAX : value ? value : 1);
}
static void preferred(Widget child, Dimension *width, Dimension *height) {
    XtWidgetGeometry wanted; wanted.request_mode = 0; XtQueryGeometry(child, NULL, &wanted);
    *width = wanted.request_mode & CWWidth ? wanted.width : child->core.width;
    *height = wanted.request_mode & CWHeight ? wanted.height : child->core.height;
}
static Boolean footer_visible(XmFormSectionWidget section) {
    return section->section.footer_text && XmStringLength(section->section.footer_text) != 0;
}
static void natural(XmFormSectionWidget section, Dimension *width, Dimension *height) {
    Dimension hw, hh, fw = 0, fh = 0, cw, ch;
    preferred(section->section.header, &hw, &hh); preferred(section->section.content, &cw, &ch);
    if (footer_visible(section)) preferred(section->section.footer, &fw, &fh);
    *width = dimension(2UL * section->section.margin_width + (hw > cw ? hw : cw));
    if (fw > *width - 2 * section->section.margin_width) *width = dimension(2UL * section->section.margin_width + fw);
    *height = dimension(2UL * section->section.margin_height + hh + section->section.spacing + ch +
                        (footer_visible(section) ? section->section.spacing + fh : 0));
}
static void layout(XmFormSectionWidget section) {
    Dimension hw, hh, fw = 0, fh = 0, cw, ch, width, content_height;
    Position x, y; Boolean footer = footer_visible(section);
    if (section->section.initializing || section->section.laying_out) return;
    section->section.laying_out = True;
    preferred(section->section.header, &hw, &hh); preferred(section->section.content, &cw, &ch);
    if (footer) preferred(section->section.footer, &fw, &fh);
    x = (Position)section->section.margin_width;
    width = dimension(section->core.width > 2UL * section->section.margin_width
                         ? section->core.width - 2UL * section->section.margin_width : 1);
    XmeConfigureObject(section->section.header, x, (Position)section->section.margin_height, width, hh, 0);
    y = (Position)(section->section.margin_height + hh + section->section.spacing);
    content_height = dimension(section->core.height > (unsigned long)y + section->section.margin_height +
                                           (footer ? fh + section->section.spacing : 0)
                                       ? section->core.height - y - section->section.margin_height -
                                             (footer ? fh + section->section.spacing : 0)
                                       : ch);
    XmeConfigureObject(section->section.content, x, y, width, content_height, 0);
    if (footer) {
        y = (Position)(y + content_height + section->section.spacing);
        XtSetMappedWhenManaged(section->section.footer, True);
        XmeConfigureObject(section->section.footer, x, y, width, fh, 0);
    } else XtSetMappedWhenManaged(section->section.footer, False);
    section->section.laying_out = False;
}
static void negotiate(XmFormSectionWidget section) {
    Dimension width, height, rw, rh; XtGeometryResult result;
    if (section->section.initializing || section->section.laying_out) return;
    natural(section, &width, &height); result = XtMakeResizeRequest((Widget)section, width, height, &rw, &rh);
    if (result == XtGeometryAlmost) XtMakeResizeRequest((Widget)section, rw, rh, NULL, NULL);
    layout(section);
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmFormSectionWidget section = (XmFormSectionWidget)widget; Dimension width, height;
    (void)request; (void)args; (void)count; section->section.initializing = True; section->section.laying_out = False;
    section->section.header_text = copy_string(section->section.header_text);
    section->section.footer_text = copy_string(section->section.footer_text);
    section->section.header = XtVaCreateManagedWidget("header", xmLabelWidgetClass, widget,
        XmNlabelString, section->section.header_text, XmNalignment, XmALIGNMENT_BEGINNING, XmNborderWidth, 0, NULL);
    section->section.content = XtVaCreateManagedWidget("content", xmRowColumnWidgetClass, widget,
        XmNorientation, XmVERTICAL, XmNpacking, XmPACK_TIGHT, XmNborderWidth, 0, NULL);
    section->section.footer = XtVaCreateManagedWidget("footer", xmLabelWidgetClass, widget,
        XmNlabelString, section->section.footer_text, XmNalignment, XmALIGNMENT_BEGINNING,
        XmNborderWidth, 0, XmNmappedWhenManaged, footer_visible(section), NULL);
    section->section.initializing = False; natural(section, &width, &height);
    if (!section->core.width)
        section->core.width = width;
    if (!section->core.height)
        section->core.height = height;
    layout(section);
}
static void Destroy(Widget widget) {
    XmFormSectionWidget section = (XmFormSectionWidget)widget;
    XmStringFree(section->section.header_text);
    XmStringFree(section->section.footer_text);
}
static void Resize(Widget widget) { layout((XmFormSectionWidget)widget); }
static void ChangeManaged(Widget widget) { negotiate((XmFormSectionWidget)widget); }
static Boolean SetValues(Widget old_widget, Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmFormSectionWidget old = (XmFormSectionWidget)old_widget, section = (XmFormSectionWidget)widget;
    Boolean changed = old->section.header_text != section->section.header_text || old->section.footer_text != section->section.footer_text;
    (void)request; (void)args; (void)count;
    if (old->section.header_text != section->section.header_text) {
        XmStringFree(old->section.header_text);
        section->section.header_text = copy_string(section->section.header_text);
    }
    if (old->section.footer_text != section->section.footer_text) {
        XmStringFree(old->section.footer_text);
        section->section.footer_text = copy_string(section->section.footer_text);
    }
    XtVaSetValues(section->section.header, XmNlabelString, section->section.header_text, NULL);
    XtVaSetValues(section->section.footer, XmNlabelString, section->section.footer_text, XmNmappedWhenManaged, footer_visible(section), NULL);
    if (changed || old->section.margin_width != section->section.margin_width || old->section.margin_height != section->section.margin_height || old->section.spacing != section->section.spacing) negotiate(section);
    return False;
}
static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request, XtWidgetGeometry *reply) {
    XmFormSectionWidget section = (XmFormSectionWidget)XtParent(child);
    if (request->request_mode & (CWX | CWY | CWBorderWidth | CWSibling | CWStackMode)) return XtGeometryNo;
    if (!section->section.laying_out && !(request->request_mode & XtCWQueryOnly)) negotiate(section);
    reply->request_mode = CWWidth | CWHeight; reply->width = child->core.width; reply->height = child->core.height;
    if (((request->request_mode & CWWidth) && request->width != reply->width) || ((request->request_mode & CWHeight) && request->height != reply->height)) return XtGeometryAlmost;
    return (request->request_mode & XtCWQueryOnly) ? XtGeometryYes : XtGeometryDone;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *intended, XtWidgetGeometry *reply) {
    XmFormSectionWidget section = (XmFormSectionWidget)widget; natural(section, &reply->width, &reply->height); reply->request_mode = CWWidth | CWHeight;
    if (intended && (intended->request_mode & (CWWidth | CWHeight)) == (CWWidth | CWHeight) && intended->width == reply->width && intended->height == reply->height) return XtGeometryYes;
    if (section->core.width == reply->width && section->core.height == reply->height) return XtGeometryNo;
    return XtGeometryAlmost;
}
Widget XmCreateFormSection(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmFormSectionWidgetClass, parent, args, count); }
Widget XmVaCreateFormSection(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmFormSectionWidgetClass,parent,False,v,n); va_end(v); return w; }
Widget XmVaCreateManagedFormSection(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmFormSectionWidgetClass,parent,True,v,n); va_end(v); return w; }
Widget XmFormSectionGetContent(Widget widget) { return widget && XmIsFormSection(widget) ? ((XmFormSectionWidget)widget)->section.content : NULL; }
Widget XmFormSectionGetHeader(Widget widget) { return widget && XmIsFormSection(widget) ? ((XmFormSectionWidget)widget)->section.header : NULL; }
Widget XmFormSectionGetFooter(Widget widget) { return widget && XmIsFormSection(widget) ? ((XmFormSectionWidget)widget)->section.footer : NULL; }
