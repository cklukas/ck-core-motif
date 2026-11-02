/*
 * The source tab is extracted from this file.  Keep the example body real:
 * it is the same public-API construction path used by gallery.c.
 */
#include <Xm/XmAll.h>
#include <Xm/Label.h>
#include <Xm/DrawingA.h>
#include <Xm/DragC.h>
#include <Xm/MenuShell.h>
#include <Xm/Protocols.h>
#include <Xm/AtomMgr.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/SegmentedControl.h>
#include <Xm/TabStack.h>
#include <Xm/Toolbar.h>
#include <Xm/TreeView.h>
#include <Xm/DataGrid.h>
#include <Xm/Calendar.h>
#include <Xm/Appearance.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <Xm/Display.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

/* Image resources are application content. Keep their cache references alive
 * and refresh them through public APIs when their owner's colors/scale change. */
typedef struct GalleryIcons {
    Widget owner, hooks;
    const char *resource;
    const char *names[4];
    Pixmap pixmaps[4];
    Cardinal count;
    unsigned logical_width, logical_height;
    Pixel foreground, background;
    double scale;
    XtWorkProcId work;
    Boolean updating, initialized;
    struct GalleryIcons *next;
} GalleryIcons;
static GalleryIcons *gallery_icons;

static void gallery_icon_colors(GalleryIcons *icons, Pixel *foreground, Pixel *background)
{
    XtVaGetValues(icons->owner,XmNforeground,foreground,XmNbackground,background,NULL);
    if (icons->resource && !strcmp(icons->resource, XmNarmPixmap)) {
        XtVaGetValues(icons->owner,XmNarmColor,background,NULL);
    } else if (icons->resource && !strcmp(icons->resource, XmNlabelInsensitivePixmap)) {
        XtVaGetValues(icons->owner,XmNinsensitiveForeground,foreground,NULL);
        if (*foreground == XmUNSPECIFIED_PIXEL)
            XtVaGetValues(icons->owner,XmNbottomShadowColor,foreground,NULL);
    } else if (XtIsSubclass(icons->owner, xmIconButtonWidgetClass)) {
        Boolean pressed;
        XtVaGetValues(icons->owner,XmNset,&pressed,NULL);
        if (pressed) XtVaGetValues(icons->owner,XmNarmColor,background,NULL);
    }
}

static Boolean refresh_gallery_icons(XtPointer closure)
{
    GalleryIcons *icons = closure;
    Pixel foreground,background;
    double scale;
    int depth;
    Pixmap next[4] = {0};
    icons->work = 0;
    gallery_icon_colors(icons,&foreground,&background);
    if (!XmGetWidgetScale(icons->owner,&scale)) return True;
    if (icons->initialized && icons->foreground==foreground && icons->background==background &&
        fabs(icons->scale-scale)<0.00001) return True;
    XtVaGetValues(XtIsWidget(icons->owner)?icons->owner:XtParent(icons->owner),XmNdepth,&depth,NULL);
    for (Cardinal i=0;i<icons->count;++i) {
        next[i]=XmGetScaledPixmap(icons->owner,(char *)icons->names[i],foreground,background,depth,0.0);
        if (next[i]==XmUNSPECIFIED_PIXMAP || next[i]==None) {
            for(Cardinal j=0;j<i;++j) XmDestroyPixmap(XtScreenOfObject(icons->owner),next[j]);
            XtAppWarning(XtWidgetToApplicationContext(icons->owner),"Cannot load gallery icon");
            return True;
        }
    }
    icons->updating=True;
    if (icons->resource) XtVaSetValues(icons->owner,icons->resource,next[0],NULL);
    else XmToolbarSetActionIcons(icons->owner,next,icons->count);
    icons->updating=False;
    for (Cardinal i=0;i<icons->count;++i) {
        if(icons->initialized) XmDestroyPixmap(XtScreenOfObject(icons->owner),icons->pixmaps[i]);
        icons->pixmaps[i]=next[i];
    }
    icons->foreground=foreground;icons->background=background;icons->scale=scale;
    icons->initialized=True;
    return True;
}

static void gallery_icon_changed(Widget hooks, XtPointer closure, XtPointer data)
{
    GalleryIcons *icons=closure;
    XtChangeHookData change=data;
    (void)hooks;
    if (!icons->updating && !icons->work && change && change->widget==icons->owner &&
        change->type && strcmp(change->type,XtHsetValues)==0)
        icons->work=XtAppAddWorkProc(XtWidgetToApplicationContext(icons->owner),refresh_gallery_icons,icons);
}

static void destroy_gallery_icons(Widget owner, XtPointer closure, XtPointer data)
{
    GalleryIcons *icons=closure, **link=&gallery_icons;
    (void)data;
    if(icons->work) XtRemoveWorkProc(icons->work);
    XtRemoveCallback(icons->hooks,XtNchangeHook,gallery_icon_changed,icons);
    while(*link && *link!=icons)link=&(*link)->next;
    if(*link) *link=icons->next;
    for(Cardinal i=0;i<icons->count;++i) {
        if(icons->initialized) XmDestroyPixmap(XtScreenOfObject(owner),icons->pixmaps[i]);
        XtFree((char *)icons->names[i]);
    }
    XtFree((char *)icons);
}

static void gallery_bind_sized_icons(Widget owner, const char *resource,
                               const char *const *names, Cardinal count,
                               unsigned width, unsigned height)
{
    GalleryIcons *icons=(GalleryIcons *)XtCalloc(1,sizeof(*icons));
    if(!icons || count==0 || count>4) { XtFree((char *)icons);return; }
    icons->owner=owner;icons->resource=resource;icons->count=count;
    icons->logical_width=width;icons->logical_height=height;
    for(Cardinal i=0;i<count;++i)icons->names[i]=XtNewString(names[i]);
    icons->hooks=XtHooksOfDisplay(XtDisplayOfObject(owner));
    icons->next=gallery_icons;gallery_icons=icons;
    XtAddCallback(owner,XmNdestroyCallback,destroy_gallery_icons,icons);
    XtAddCallback(icons->hooks,XtNchangeHook,gallery_icon_changed,icons);
    refresh_gallery_icons(icons);
}

static void gallery_bind_icons(Widget owner, const char *resource,
                               const char *const *names, Cardinal count)
{
    gallery_bind_sized_icons(owner,resource,names,count,24,24);
}

/* Discover the available representation at logical size, then let the binding
 * own scaled cache references. Classic bitmap symbols need not be square. */
Boolean gallery_bind_named_icon(Widget owner, const char *resource,
                                const char *primary, const char *fallback)
{
    const char *names[]={primary,fallback};
    Pixel foreground,background;
    int depth;
    XtVaGetValues(owner,XmNforeground,&foreground,XmNbackground,&background,NULL);
    XtVaGetValues(XtIsWidget(owner)?owner:XtParent(owner),XmNdepth,&depth,NULL);
    for(Cardinal i=0;i<XtNumber(names);++i) {
        if(!names[i]) continue;
        Pixmap probe=XmGetScaledPixmap(owner,(char *)names[i],foreground,background,depth,1.0);
        if(probe==None || probe==XmUNSPECIFIED_PIXMAP) continue;
        Window root;int x,y;unsigned width,height,border,pixmap_depth;
        Boolean valid=XGetGeometry(XtDisplayOfObject(owner),probe,&root,&x,&y,
                                   &width,&height,&border,&pixmap_depth);
        if(valid) gallery_bind_sized_icons(owner,resource,&names[i],1,width,height);
        XmDestroyPixmap(XtScreenOfObject(owner),probe);
        return valid;
    }
    return False;
}

static void gallery_bind_icon(Widget owner, const char *resource, const char *name)
{
    const char *names[]={name};gallery_bind_icons(owner,resource,names,1);
}

void gallery_refresh_icon_resources(void)
{
    for(GalleryIcons *icons=gallery_icons;icons;icons=icons->next) {
        if(icons->work) { XtRemoveWorkProc(icons->work);icons->work=0; }
        refresh_gallery_icons(icons);
    }
}

Boolean gallery_validate_icon_resources(void)
{
    for(GalleryIcons *icons=gallery_icons;icons;icons=icons->next) {
        Pixel foreground,background;double scale;
        gallery_icon_colors(icons,&foreground,&background);
        if(!icons->initialized || !XmGetWidgetScale(icons->owner,&scale) ||
           foreground!=icons->foreground || background!=icons->background ||
           fabs(scale-icons->scale)>0.00001) return False;
        if(icons->resource) {
            Pixmap assigned=None;
            XtVaGetValues(icons->owner,icons->resource,&assigned,NULL);
            if(assigned!=icons->pixmaps[0]) return False;
        }
        for(Cardinal i=0;i<icons->count;++i) {
            Window root;int x,y;unsigned int width=0,height=0,border,depth;
            if(!XGetGeometry(XtDisplayOfObject(icons->owner),icons->pixmaps[i],&root,&x,&y,
                             &width,&height,&border,&depth) ||
               width!=(unsigned)(icons->logical_width*scale+0.5) ||
               height!=(unsigned)(icons->logical_height*scale+0.5)) {
                fprintf(stderr,"gallery icon size: %s actual=%ux%u logical=%ux%u scale=%g\n",
                    icons->names[i],width,height,icons->logical_width,icons->logical_height,scale);
                return False;
            }
            XImage *image=XGetImage(XtDisplayOfObject(icons->owner),icons->pixmaps[i],0,0,1,1,AllPlanes,ZPixmap);
            if(!image) return False;
            Boolean valid=XGetPixel(image,0,0)==background;
            XDestroyImage(image);
            if(!valid) {
                fprintf(stderr,"gallery icon background mismatch: %s\n",icons->names[i]);
                return False;
            }
        }
    }
    return True;
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
        XmNtoastActionCallback,
        XmNstepperValueChangedCallback, XmNcalendarDateChangedCallback,
        XmNdatePickerDateChangedCallback, XmNtimePickerValueChangedCallback,
        XmNratingValueChangedCallback, XmNcolorPickerValueChangedCallback,
        XmNbreadcrumbActivateCallback, XmNautoSuggestCommittedCallback,
        XmNsearchQueryChangedCallback, XmNchipRemoveCallback,
        XmNinfoActionCallback, XmNdefaultActionCallback, XmNpageChangedCallback, XmNtabSelectedCallback,
        XmNhelpCallback
    };
    if (XmIsNumberBox(example)) {
        XtAddCallback(example, XmNnumberValueChangedCallback, callback, client_data);
        XtAddCallback(example, XmNnumberInvalidCallback, callback, client_data);
        return;
    }
    unsigned int i;
    for (i = 0; i < XtNumber(names); ++i)
        if (XtHasCallbacks(example, names[i]) != XtCallbackNoList) {
            XtAddCallback(example, names[i], callback, client_data);
            return;
        }
}

/* GALLERY-SOURCE-BEGIN: setup xmAutoSuggestBoxWidgetClass */
static Boolean gallery_suggestion_matches(const char *label, const char *query)
{
    if (!*query) return True;
    for (; *label; ++label) {
        size_t i = 0;
        while (query[i] && label[i] &&
               tolower((unsigned char)query[i]) == tolower((unsigned char)label[i])) ++i;
        if (!query[i]) return True;
    }
    return False;
}
static void gallery_suggestions(Widget widget, XtPointer closure, XtPointer data)
{
    static const char *const names[] = {"Open document", "Open folder", "Open recent"};
    static const char *const ids[] = {"document", "folder", "recent"};
    XmAutoSuggestCallbackStruct *query = data;
    XmAutoSuggestItem items[3];
    Cardinal count = 0;
    (void)closure;
    for (Cardinal i = 0; i < XtNumber(names); ++i) {
        if (!gallery_suggestion_matches(names[i], query->query)) continue;
        items[count].id = ids[i];
        items[count].label = XmStringCreateLocalized((char *)names[i]);
        items[count].user_data = NULL;
        ++count;
    }
    XmAutoSuggestBoxSetSuggestions(widget, items, count, query->generation);
    for (Cardinal i = 0; i < count; ++i) XmStringFree(items[i].label);
}
/* GALLERY-SOURCE-END */

Widget
gallery_create_component_example(Widget parent, WidgetClass class,
                                 const char *name, XtCallbackProc callback,
                                 XtPointer client_data)
{
    XmString label = NULL;
    Widget example;

    /* GALLERY-SOURCE-BEGIN: setup xmTextFieldWidgetClass xmDataFieldWidgetClass xmTextWidgetClass xmNumberBoxWidgetClass */
    if (class == xmTextFieldWidgetClass || class == xmDataFieldWidgetClass ||
        class == xmTextWidgetClass || class == xmNumberBoxWidgetClass) {
        parent = XtVaCreateManagedWidget("fieldLayout", xmRowColumnWidgetClass, parent,
            XmNorientation, XmVERTICAL, XmNpacking, XmPACK_TIGHT,
            XmNadjustLast, False, XmNentryAlignment, XmALIGNMENT_BEGINNING, NULL);
        sample_label(parent, "fieldLabel",
            class == xmNumberBoxWidgetClass ? "Quantity" : "Document title");
    }
    /* GALLERY-SOURCE-END */

    /* Compound controls need a small, useful model to render anything.  Keep
     * these values in the same public construction function used by the live
     * gallery; a bare class instantiation is not a meaningful example. */
    if (class == xmChipWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmChipWidgetClass */
        label = XmStringCreateLocalized((char *)"Filter: active");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNchipLabel, label,
                                          XmNchipRemovable, True, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmBadgeWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmBadgeWidgetClass */
        label = XmStringCreateLocalized((char *)"Notifications");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNbadgeLabel, label,
                                          XmNbadgeCount, 3, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmIconLabelWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmIconLabelWidgetClass */
        label = XmStringCreateLocalized((char *)"Recent document");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNiconLabelLabel, label, NULL);
        gallery_bind_icon(example,XmNiconLabelPixmap,"action_document");
        /* GALLERY-SOURCE-END */
    } else if (class == xmDividerWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmDividerWidgetClass */
        label = XmStringCreateLocalized((char *)"Presentation");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNdividerLabel, label, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmAvatarWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmAvatarWidgetClass */
        label = XmStringCreateLocalized((char *)"CK");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNavatarInitials, label, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmLinkWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmLinkWidgetClass */
        label = XmStringCreateLocalized((char *)"Open documentation");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNlinkLabel, label,
                                          XmNlinkTarget, (char *)"docs", NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmPromptWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmPromptWidgetClass */
        label = XmStringCreateLocalized((char *)"Choose a component to inspect");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNpromptText, label, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmEmptyStateWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmEmptyStateWidgetClass */
        label = XmStringCreateLocalized((char *)"Nothing selected yet");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNemptyStateMessage, label, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmToggleSwitchWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmToggleSwitchWidgetClass */
        XmString title_label = XmStringCreateLocalized((char *)"Kitchen lights");

        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNlabelString, title_label,
                                          XmNset, False, NULL);
        XmStringFree(title_label);
        /* GALLERY-SOURCE-END */
    } else if (class == xmToggleButtonWidgetClass ||
               class == xmToggleButtonGadgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmToggleButtonWidgetClass xmToggleButtonGadgetClass */
        /* Keep the familiar check-box indicator visible. */
        label = XmStringCreateLocalized((char *)"Enable notifications");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNlabelString, label,
                                          XmNindicatorOn, True, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmCascadeButtonWidgetClass || class == xmCascadeButtonGadgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmCascadeButtonWidgetClass xmCascadeButtonGadgetClass */
        Widget bar = parent;
        unsigned char type = XmWORK_AREA;
        if (XmIsRowColumn(parent)) XtVaGetValues(parent, XmNrowColumnType, &type, NULL);
        if (type != XmMENU_BAR) {
            bar = XmCreateMenuBar(parent, "fileMenuBar", NULL, 0);
            XtManageChild(bar);
        }
        Widget menu = XmCreatePulldownMenu(bar, "fileMenu", NULL, 0);
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
        example = XtVaCreateManagedWidget((char *)name, class, bar,
            XmNlabelString, label, XmNsubMenuId, menu, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmPushButtonWidgetClass ||
               class == xmPushButtonGadgetClass ||
               class == xmLabelWidgetClass ||
               class == xmLabelGadgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmPushButtonWidgetClass xmPushButtonGadgetClass xmLabelWidgetClass xmLabelGadgetClass */
        label = XmStringCreateLocalized((char *)"Apply changes");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNlabelString, label, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmArrowButtonWidgetClass ||
               class == xmArrowButtonGadgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmArrowButtonWidgetClass xmArrowButtonGadgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNarrowDirection, XmARROW_RIGHT, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmTextWidgetClass || class == xmTextFieldWidgetClass ||
               class == xmDataFieldWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmTextWidgetClass xmTextFieldWidgetClass xmDataFieldWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNvalue, (char *)"Editable sample text",
                                          XmNcolumns, 28, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmListWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmListWidgetClass */
        XmString rows[3];
        rows[0] = XmStringCreateLocalized((char *)"First item");
        rows[1] = XmStringCreateLocalized((char *)"Second item");
        rows[2] = XmStringCreateLocalized((char *)"Third item");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNitems, rows, XmNitemCount, 3,
                                          XmNvisibleItemCount, 3, XmNwidth, 300, NULL);
        XmStringFree(rows[0]); XmStringFree(rows[1]); XmStringFree(rows[2]);
        /* GALLERY-SOURCE-END */
    } else if (class == xmComboBoxWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmComboBoxWidgetClass */
        XmString rows[3];
        rows[0] = XmStringCreateLocalized((char *)"Alpha");
        rows[1] = XmStringCreateLocalized((char *)"Beta");
        rows[2] = XmStringCreateLocalized((char *)"Gamma");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNitems, rows, XmNitemCount, 3,
                                          XmNvisibleItemCount, 3, XmNwidth, 300, NULL);
        XmStringFree(rows[0]); XmStringFree(rows[1]); XmStringFree(rows[2]);
        /* GALLERY-SOURCE-END */
    } else if (class == xmScaleWidgetClass || class == xmScrollBarWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmScaleWidgetClass xmScrollBarWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNorientation, XmHORIZONTAL, XmNminimum, 0, XmNmaximum, 100,
                                          XmNvalue, 65, XmNwidth, 320,
                                          XmNheight, 48, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmNumberBoxWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmNumberBoxWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        XmNumberBoxSetRange(example, 0, 100, False);
        XmNumberBoxSetValue(example, 42, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmRatingControlWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmRatingControlWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        XmRatingControlSetValue(example, 4, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmProgressBarWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmProgressBarWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, NULL);
        XmProgressBarSetRange(example, 0, 100, False);
        XmProgressBarSetValue(example, 68, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmProgressRingWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmProgressRingWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        XmProgressRingSetRange(example, 0, 100, False);
        XmProgressRingSetValue(example, 68, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmSpinnerWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmSpinnerWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        XmSpinnerSetActive(example, True, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmStepperWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmStepperWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        XmStepperSetRange(example, 0, 100, False);
        XmStepperSetValue(example, 7, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmColorPickerWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmColorPickerWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        XmColorPickerSetValue(example, 0x3478d4ffUL, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmBreadcrumbBarWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmBreadcrumbBarWidgetClass */
        XmBreadcrumbSegment segments[3] = {
            {"Home", (XtPointer)"home"},
            {"Components", (XtPointer)"components"},
            {"Current", (XtPointer)"current"}
        };
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 420, XmNheight, 44, NULL);
        XmBreadcrumbBarSetSegments(example, segments, 3);
        XmBreadcrumbBarSetCurrentIndex(example, 2, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmAutoSuggestBoxWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmAutoSuggestBoxWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        XtAddCallback(example, XmNautoSuggestQueryChangedCallback, gallery_suggestions, NULL);
        XmSearchBoxSetString(XmAutoSuggestBoxGetSearchField(example), "Open");
        /* GALLERY-SOURCE-END */
    } else if (class == xmInfoBarWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmInfoBarWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNinfoMessage, (char *)"Changes saved",
                                          XmNinfoActionLabel, (char *)"Undo",
                                          XmNwidth, 420, XmNheight, 48, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmToastWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmToastWidgetClass */
        label = XmStringCreateLocalized((char *)"Document saved successfully");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, XmNheight, 64, XmNtoastMessage, label,
                                          XmNtoastTimeout, 0, NULL);
        XmToastShow(example);
        /* GALLERY-SOURCE-END */
    } else if (class == xmSegmentedControlWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmSegmentedControlWidgetClass */
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
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        (void)XmSegmentedControlSetItems(example, items, 3);
        XmSegmentedControlSetSelectedIndex(example, 0, False);
        XmStringFree(segments[0]); XmStringFree(segments[1]); XmStringFree(segments[2]);
        /* GALLERY-SOURCE-END */
    } else if (class == xmTabStackWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmTabStackWidgetClass */
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
        /* GALLERY-SOURCE-END */
    } else if (class == xmToolbarWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmToolbarWidgetClass */
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
            static const char *const icons[]={"action_open","action_save","action_print","action_undo"};
            for (Cardinal i = 0; i < XtNumber(icons); ++i) {
                for (unsigned overflow = 0; overflow < 2; ++overflow) {
                    Widget button = XmToolbarGetActionButton(example, i, overflow);
                    /* Rasterize transparent image pixels against the actual
                     * normal/pressed surface, not the toolbar's background. */
                    gallery_bind_icon(button, XmNlabelPixmap, icons[i]);
                    gallery_bind_icon(button, XmNarmPixmap, icons[i]);
                    gallery_bind_icon(button, XmNlabelInsensitivePixmap, icons[i]);
                    XtVaSetValues(button, XmNlabelType, XmPIXMAP, NULL);
                }
            }
        }
        if (callback)
            XtAddCallback(example, XmNtoolbarActivateCallback, callback, client_data);
        XmStringFree(labels[0]); XmStringFree(labels[1]);
        XmStringFree(labels[2]); XmStringFree(labels[3]);
        XmStringFree(shortcuts[0]); XmStringFree(shortcuts[1]);
        XmStringFree(shortcuts[2]); XmStringFree(shortcuts[3]);
        /* GALLERY-SOURCE-END */
    } else if (class == xmTreeViewWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmTreeViewWidgetClass */
        static const XmTreeViewNode nodes[] = {
            {(XtPointer)"root", NULL, "Project", True, True},
            {(XtPointer)"src", (XtPointer)"root", "Source", True, True},
            {(XtPointer)"gallery", (XtPointer)"src", "Gallery", False, False},
            {(XtPointer)"docs", (XtPointer)"root", "Documentation", False, False}
        };
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360,
                                          XmNtreeViewVisibleRows, 5, NULL);
        XmTreeViewSetNodes(example, nodes, XtNumber(nodes));
        XmTreeViewSetSelectedId(example, (XtPointer)"gallery", False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmDataGridWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmDataGridWidgetClass */
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
        /* GALLERY-SOURCE-END */
    } else if (class == xmCalendarWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmCalendarWidgetClass */
        XmCalendarDate date = {2026, 10, 25};
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          NULL);
        XmCalendarSetDate(example, &date, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmMultiListWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmMultiListWidgetClass xmExt18ListWidgetClass */
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
        /* GALLERY-SOURCE-END */
    } else if (class == xmDropDownWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmCombinationBox2WidgetClass */
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
        /* GALLERY-SOURCE-END */
    } else if (class == xmTabBoxWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmTabBoxWidgetClass */
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
        /* GALLERY-SOURCE-END */
    } else if (class == xmOutlineWidgetClass || class == xmTreeWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmOutlineWidgetClass xmTreeWidgetClass */
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
        /* GALLERY-SOURCE-END */
    } else if (class == xmIconBoxWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmIconBoxWidgetClass */
        const char *titles[] = {"Documents", "Pictures", "Archive"};
        unsigned int i;
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        for (i = 0; i < 3; ++i) {
            XmString caption = XmStringCreateLocalized((char *)titles[i]);
            static const char *const icons[]={"action_document","action_picture","action_archive"};
            Widget item=XtVaCreateManagedWidget(titles[i], xmIconButtonWidgetClass, example,
                XmNlabelString, caption, XmNcellX, i, XmNcellY, 0, NULL);
            gallery_bind_icon(item,XmNpixmap,icons[i]);
            XmStringFree(caption);
        }
        /* GALLERY-SOURCE-END */
    } else if (class == xmIconButtonWidgetClass || class == xmIconGadgetClass ||
               class == xmIconHeaderClass || class == xmDrawnButtonWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmIconButtonWidgetClass xmIconGadgetClass xmIconHeaderClass xmDrawnButtonWidgetClass */
        label = XmStringCreateLocalized((char *)"Open document");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNlabelString, label, NULL);
        if (class == xmIconButtonWidgetClass) gallery_bind_icon(example,XmNpixmap,"action_open");
        else if (class == xmIconGadgetClass) gallery_bind_icon(example,XmNlargeIconPixmap,"action_document");
        /* GALLERY-SOURCE-END */
    } else if (class == xmDatePickerWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmDatePickerWidgetClass */
        XmDatePickerDate date = {2026, 10, 25};
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            NULL);
        XmDatePickerSetDate(example, &date, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmTimePickerWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmTimePickerWidgetClass */
        XmTimePickerValue value = {14, 30, 0};
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            NULL);
        XmTimePickerSetValue(example, &value, False);
        /* GALLERY-SOURCE-END */
    } else if (class == xmStatusBarWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmStatusBarWidgetClass */
        XmStatusBarItem item = {0};
        label = XmStringCreateLocalized((char *)"All changes saved");
        item.id = "save"; item.label = label; item.kind = XmSTATUS_BAR_ITEM_TEXT;
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 480, XmNheight, 40, NULL);
        XmStatusBarSetItems(example, &item, 1);
        /* GALLERY-SOURCE-END */
    } else if (class == xmDisclosureWidgetClass || class == xmSidebarWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmDisclosureWidgetClass xmSidebarWidgetClass */
        Widget content;
        example = class == xmDisclosureWidgetClass ?
            XtVaCreateManagedWidget((char *)name, class, parent, NULL) :
            XtVaCreateManagedWidget((char *)name, class, parent,
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
        /* GALLERY-SOURCE-END */
    } else if (class == xmCommandPaletteWidgetClass || class == xmRowActionsWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmCommandPaletteWidgetClass xmRowActionsWidgetClass */
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
        /* GALLERY-SOURCE-END */
    } else if (class == xmFlyoutWidgetClass || class == xmTeachingTipWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmFlyoutWidgetClass xmTeachingTipWidgetClass */
        XmString caption = XmStringCreateLocalized((char *)(class == xmFlyoutWidgetClass ? "Show details" : "Show keyboard tip"));
        /* Popup handles stay unmanaged. Only the trigger participates in this
         * compact owner layout, so popup placement follows the button bounds. */
        Widget owner = XtVaCreateManagedWidget("popupAnchor", xmRowColumnWidgetClass, parent,
            XmNorientation, XmVERTICAL, XmNmarginWidth, 0, XmNmarginHeight, 0,
            XmNspacing, 0, NULL);
        Widget trigger = XtVaCreateManagedWidget("showPopup", xmPushButtonWidgetClass, owner,
            XmNlabelString, caption, NULL);
        XmStringFree(caption);
        example = XtVaCreateWidget((char *)name, class, owner, NULL);
        if (class == xmFlyoutWidgetClass)
            XtVaSetValues(example, XmNflyoutText, "Document details\nSaved locally and ready to export.", NULL);
        if (class == xmTeachingTipWidgetClass) {
            XmString title = XmStringCreateLocalized((char *)"Move with the keyboard");
            XmString body = XmStringCreateLocalized((char *)"Press Tab to move forward and Shift+Tab to move back.");
            XtVaSetValues(example, XmNteachingTipTitle, title, XmNteachingTipText, body,
                XmNteachingTipDelay, 0, NULL);
            XmStringFree(title); XmStringFree(body);
        }
        XtAddCallback(trigger, XmNactivateCallback, show_popup_example, example);
        /* GALLERY-SOURCE-END */
    } else if (class == xmDropHighlightWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmDropHighlightWidgetClass */
        label = XmStringCreateLocalized((char *)"Drop a document here");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNdropHighlightLabel, label,
            XmNwidth, 360, XmNheight, 120, NULL);
        XmDropHighlightSetState(example, XmDROP_HIGHLIGHT_VALID);
        /* GALLERY-SOURCE-END */
    } else if (class == xmMessageBoxWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmMessageBoxWidgetClass */
        label = XmStringCreateLocalized((char *)"Your changes have been saved.");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNmessageString, label, XmNdialogType, XmDIALOG_INFORMATION, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmCommandWidgetClass || class == xmSelectionBoxWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmCommandWidgetClass xmSelectionBoxWidgetClass */
        XmString values[3];
        values[0] = XmStringCreateLocalized((char *)"Open document");
        values[1] = XmStringCreateLocalized((char *)"Save changes");
        values[2] = XmStringCreateLocalized((char *)"Export report");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            class == xmCommandWidgetClass ? XmNhistoryItems : XmNlistItems, values,
            class == xmCommandWidgetClass ? XmNhistoryItemCount : XmNlistItemCount, 3,
            class == xmCommandWidgetClass ? XmNhistoryVisibleItemCount : XmNlistVisibleItemCount, 3, NULL);
        XmStringFree(values[0]); XmStringFree(values[1]); XmStringFree(values[2]);
        /* GALLERY-SOURCE-END */
    } else if (class == xmSeparatorWidgetClass || class == xmSeparatorGadgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmSeparatorWidgetClass xmSeparatorGadgetClass */
        sample_label(parent, "above", "Document settings");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNy, 40, XmNwidth, 360, XmNheight, 12, NULL);
        Widget below = sample_label(parent, "below", "Export options");
        XtVaSetValues(below, XmNy, 68, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmNotebookWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmNotebookWidgetClass */
        unsigned int i;
        const char *names[] = {"Overview", "Details", "Activity"};
        const char *contents[] = {"Project overview\n\nRelated pages in one place.",
            "Document details\n\nSwitch pages with the tabs.",
            "Recent activity\n\nCreated document\nSaved changes\nExported report"};
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        for (i = 0; i < 3; ++i) {
            XmString value = XmStringCreateLocalized((char *)contents[i]);
            XtVaCreateManagedWidget("page", xmLabelWidgetClass, example,
                XmNnotebookChildType, XmPAGE, XmNpageNumber, (int)i + 1,
                XmNlabelString, value, XmNalignment, XmALIGNMENT_BEGINNING, NULL);
            XmStringFree(value);
            value = XmStringCreateLocalized((char *)names[i]);
            XtVaCreateManagedWidget("tab", xmPushButtonWidgetClass, example,
                XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, (int)i + 1,
                XmNlabelString, value, NULL);
            XmStringFree(value);
        }
        XtVaSetValues(example, XmNcurrentPageNumber, 1, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmFileSelectionBoxWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmFileSelectionBoxWidgetClass */
        XmString directory = XmStringCreateLocalized((char *)"./demos/programs/ckmotif-gallery/");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNdirectory, directory, XmNpathMode, XmPATH_MODE_RELATIVE,
            XmNlistVisibleItemCount, 5, NULL);
        XmStringFree(directory);
        /* GALLERY-SOURCE-END */
    } else if (class == xmSearchBoxWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmSearchBoxWidgetClass */
        label = XmStringCreateLocalized((char *)"Find a document");
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNsearchPlaceholder, label, XmNcolumns, 28, NULL);
        XmSearchBoxSetString(example, "Motif");
        /* GALLERY-SOURCE-END */
    } else if (class == xmSpinBoxWidgetClass || class == xmSimpleSpinBoxWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmSpinBoxWidgetClass xmSimpleSpinBoxWidgetClass */
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        if (class == xmSimpleSpinBoxWidgetClass)
            XtVaSetValues(example, XmNspinBoxChildType, XmNUMERIC,
                XmNminimumValue, 0, XmNmaximumValue, 100, XmNposition, 7,
                XmNcolumns, 6, NULL);
        if (class == xmSpinBoxWidgetClass)
            XtVaCreateManagedWidget("quantity", xmTextFieldWidgetClass, example,
                XmNspinBoxChildType, XmNUMERIC, XmNminimumValue, 0,
                XmNmaximumValue, 100, XmNposition, 7, XmNcolumns, 6, NULL);
        /* GALLERY-SOURCE-END */
    } else if (class == xmColumnWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmColumnWidgetClass */
        const char *captions[] = {"Document", "Status", "Owner"};
        const char *values[] = {"Quarterly report", "Draft", "Design team"};
        unsigned int i;
        example = XtVaCreateManagedWidget((char *)name, class, parent, NULL);
        for (i = 0; i < XtNumber(captions); ++i) {
            XmString caption = XmStringCreateLocalized((char *)captions[i]);
            XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, example,
                XmNentryLabelString, caption, XmNvalue, values[i], XmNcolumns, 28, NULL);
            XmStringFree(caption);
        }
        /* GALLERY-SOURCE-END */
    } else if (class == xmContainerWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmContainerWidgetClass */
        Widget folder;
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 520, XmNheight, 240, XmNlayoutType, XmOUTLINE,
            XmNentryViewType, XmSMALL_ICON, NULL);
        label = XmStringCreateLocalized((char *)"Documents");
        folder = XtVaCreateManagedWidget("folder", xmIconGadgetClass, example,
            XmNlabelString, label, XmNoutlineState, XmEXPANDED, NULL);
        gallery_bind_icon(folder,XmNsmallIconPixmap,"action_open");
        const char *names[] = {"Quarterly report", "Meeting notes"};
        unsigned int i;
        for (i = 0; i < XtNumber(names); ++i) {
            XmString caption = XmStringCreateLocalized((char *)names[i]);
            Widget item=XtVaCreateManagedWidget("document", xmIconGadgetClass, example,
                XmNlabelString, caption, XmNentryParent, folder, NULL);
            gallery_bind_icon(item,XmNsmallIconPixmap,"action_document");
            XmStringFree(caption);
        }
        /* GALLERY-SOURCE-END */
    } else if (class == xmFormWidgetClass || class == xmFrameWidgetClass ||
               class == xmBulletinBoardWidgetClass || class == xmRowColumnWidgetClass ||
               class == xmButtonBoxWidgetClass ||
               class == xmPanedWindowWidgetClass || class == xmPanedWidgetClass ||
               class == xmMainWindowWidgetClass ||
               class == xmScrolledWindowWidgetClass || class == xmFormSectionWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmFormWidgetClass xmFrameWidgetClass xmBulletinBoardWidgetClass xmRowColumnWidgetClass xmButtonBoxWidgetClass xmPanedWindowWidgetClass xmPanedWidgetClass xmMainWindowWidgetClass xmScrolledWindowWidgetClass xmFormSectionWidgetClass */
        Widget panels;
        example = class == xmFormSectionWidgetClass ?
            XtVaCreateManagedWidget((char *)name, class, parent, NULL) :
            XtVaCreateManagedWidget((char *)name, class, parent,
                XmNwidth, 520, XmNheight, 260, NULL);
        panels = class == xmFormSectionWidgetClass ? XmFormSectionGetContent(example) :
            XtVaCreateManagedWidget("settings", xmRowColumnWidgetClass, example,
            XmNorientation, XmVERTICAL, XmNpacking, XmPACK_TIGHT,
            XmNisAligned, False, XmNspacing, 12, XmNmarginWidth, 16,
            XmNmarginHeight, 16, NULL);
        {
            XmString value = XmStringCreateLocalized((char *)"Document settings");
            if (class == xmFormSectionWidgetClass)
                XtVaSetValues(example, XmNformSectionHeader, value, NULL);
            else
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
        /* GALLERY-SOURCE-END */
    } else if (class == xmDrawingAreaWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmDrawingAreaWidgetClass */
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
        /* GALLERY-SOURCE-END */
    } else if (class == xmFontSelectorWidgetClass || class == xmColorSelectorWidgetClass) {
        /* GALLERY-SOURCE-BEGIN: construction xmFontSelectorWidgetClass xmColorSelectorWidgetClass */
        /* These legacy composites require an initial allocation before their
         * internal panes can negotiate usable child geometry. */
        example = XtVaCreateManagedWidget((char *)name, class, parent,
            XmNwidth, 720, XmNheight, 340, NULL);
        /* GALLERY-SOURCE-END */
    } else {
        /* GALLERY-SOURCE-BEGIN: construction * */
        example = XtVaCreateManagedWidget((char *)name, class, parent,
                                          XmNwidth, 360, XmNheight, 64, NULL);

        /* GALLERY-SOURCE-END */
    }
    /* GALLERY-SOURCE-BEGIN: states xmPushButtonWidgetClass xmPushButtonGadgetClass xmToggleButtonWidgetClass xmToggleButtonGadgetClass */
    if (class == xmPushButtonWidgetClass || class == xmPushButtonGadgetClass ||
        class == xmToggleButtonWidgetClass || class == xmToggleButtonGadgetClass) {
        XmString alternate = XmStringCreateLocalized((char *)"Unavailable");
        Dimension height, disabled_height, gap;
        XmGetAppearanceSpacing(XtDisplay(parent), XmAPPEARANCE_SPACING_MEDIUM, &gap);
        XtVaGetValues(example, XmNheight, &height, NULL);
        Widget disabled = XtVaCreateManagedWidget("disabledState", class, parent,
            XmNlabelString, alternate, XmNsensitive, False,
            XmNy, height + gap, NULL);
        XtVaGetValues(disabled, XmNheight, &disabled_height, NULL);
        XmStringFree(alternate);
        if (class == xmToggleButtonWidgetClass || class == xmToggleButtonGadgetClass) {
            alternate = XmStringCreateLocalized((char *)"Selected option");
            Widget selected = XtVaCreateManagedWidget("selectedState", class, parent,
                XmNlabelString, alternate, XmNset, True,
                XmNy, height + disabled_height + 2 * gap, NULL);
            if (callback) XtAddCallback(selected, XmNvalueChangedCallback, callback, client_data);
            XmStringFree(alternate);
        } else {
            alternate = XmStringCreateLocalized((char *)"Default action");
            Widget primary = XtVaCreateManagedWidget("defaultState", class, parent,
                XmNlabelString, alternate, XmNshowAsDefault, True,
                XmNy, height + disabled_height + 2 * gap, NULL);
            if (callback) XtAddCallback(primary, XmNactivateCallback, callback, client_data);
            XmStringFree(alternate);
        }
    }
    /* GALLERY-SOURCE-END */
    /* GALLERY-SOURCE-BEGIN: labels xmTextFieldWidgetClass xmDataFieldWidgetClass xmTextWidgetClass xmNumberBoxWidgetClass */
    if (class == xmTextFieldWidgetClass || class == xmDataFieldWidgetClass ||
        class == xmTextWidgetClass || class == xmNumberBoxWidgetClass) {
        sample_label(parent, "fieldHelp", "Edit the value to see its change notification.");
    }
    /* GALLERY-SOURCE-END */
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
                                      xmSegmentedControlWidgetClass, parent, NULL);
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

static void close_shell_example(Widget widget, XtPointer client, XtPointer call)
{
    (void)widget; (void)call;
    XtUnmanageChild((Widget)client);
}

/* DialogShell maps and positions its managed dialog child. A generic XtPopup
 * bypasses that lifecycle when the dialog child has not been managed. */
void gallery_open_shell_example(Widget shell)
{
    if (XmIsDialogShell(shell)) {
        Widget content = XtNameToWidget(shell, "dialogContent");
        if (!content) return;
        XtVaSetValues(content, XmNdefaultPosition, True, NULL);
        XtManageChild(content);
        if (XtIsRealized(shell)) XRaiseWindow(XtDisplay(shell), XtWindow(shell));
    } else XtPopup(shell, XtGrabNone);
}

/* GALLERY-EXAMPLE-BEGIN: shell-public-construction */
Widget
gallery_create_shell_example(Widget parent, WidgetClass class,
                             const char *name)
{
    if (class == xmDialogShellWidgetClass) {
        Widget owner = parent;
        while (owner && !XtIsShell(owner)) owner = XtParent(owner);
        if (!owner) return NULL;
        Widget shell = XtVaCreatePopupShell((char *)name, class, owner,
            XmNtitle, "Dialog Shell example", XmNtransientFor, owner,
            XmNallowShellResize, True, XmNdeleteResponse, XmDO_NOTHING, NULL);
        Widget content = XtVaCreateWidget("dialogContent", xmFormWidgetClass, shell,
            XmNdefaultPosition, True, XmNautoUnmanage, False,
            XmNmarginWidth, 24, XmNmarginHeight, 24, NULL);
        XmString title = XmStringCreateLocalized("Dialog Shell example");
        Widget heading = XtVaCreateManagedWidget("dialogHeading", xmLabelWidgetClass, content,
            XmNlabelString, title, XmNappearanceRole, XmAPPEARANCE_ROLE_SECTION,
            XmNalignment, XmALIGNMENT_BEGINNING,
            XmNtopAttachment, XmATTACH_FORM, XmNleftAttachment, XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM, NULL);
        XmStringFree(title);
        XmString message = XmStringCreateLocalized(
            "This window contains a managed dialog.\nClose it and open it again from the gallery.");
        Widget body = XtVaCreateManagedWidget("dialogMessage", xmLabelWidgetClass, content,
            XmNlabelString, message, XmNalignment, XmALIGNMENT_BEGINNING,
            XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, heading, XmNtopOffset, 12,
            XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
        XmStringFree(message);
        XmString label = XmStringCreateLocalized("Close");
        Widget close = XtVaCreateManagedWidget("closeExample", xmPushButtonWidgetClass, content,
            XmNlabelString, label, XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget, body, XmNtopOffset, 16,
            XmNrightAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM, NULL);
        XmStringFree(label);
        XtVaSetValues(content, XmNdefaultButton, close, XmNcancelButton, close,
                      XmNinitialFocus, close, NULL);
        XtAddCallback(close, XmNactivateCallback, close_shell_example, content);
        XmAddWMProtocolCallback(shell, XmInternAtom(XtDisplay(shell), "WM_DELETE_WINDOW", False),
                                close_shell_example, content);
        return shell;
    }
    Arg args[2];
    XtSetArg(args[0], XmNwidth, 240);
    XtSetArg(args[1], XmNheight, 96);
    Widget shell = XtCreatePopupShell((char *)name, class, parent, args, 2);
    XmString label = XmStringCreateLocalized((char *)"Popup shell example");
    if (class == xmMenuShellWidgetClass) {
        Widget menu = XtVaCreateWidget(
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
