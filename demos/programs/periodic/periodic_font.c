/* App-scoped font choices. Retain original renditions so repeated choices do
 * not accumulate scaling, and use the same families, weights and text tags. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "periodic_font.h"
#include <Xm/XmAll.h>
#include <Xm/Display.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <string.h>
#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif

typedef struct FontMenu FontMenu;
typedef struct FontTarget {
    FontMenu *menu;
    Widget widget;
    const char *resource;
    XmRenderTable original;
    struct FontTarget *next;
} FontTarget;
typedef struct { FontMenu *menu; double factor; } FontChoice;
struct FontMenu {
    FontTarget *targets;
    Widget *visited;
    Cardinal count;
    FontChoice choices[3];
    PeriodicFontLayoutProc layout;
    XtPointer closure;
    Widget root;
    Boolean closing;
};

static Boolean HasResource(Widget widget, const char *name)
{
    XtResourceList resources=NULL; Cardinal count=0;
    XmSecondaryResourceData *secondary=NULL; Cardinal blocks;
    Boolean found=False;
    XtGetResourceList(XtClass(widget), &resources, &count);
    for(Cardinal i=0;i<count;i++)
        if(!strcmp(resources[i].resource_name,name)) found=True;
    XtFree((char *)resources);
    if(found)return True;
    blocks=XmGetSecondaryResourceData(XtClass(widget),&secondary);
    for(Cardinal i=0;i<blocks;i++) {
        for(Cardinal j=0;j<secondary[i]->num_resources;j++)
            if(!strcmp(secondary[i]->resources[j].resource_name,name))found=True;
        XtFree((char *)secondary[i]->resources);XtFree((char *)secondary[i]);
    }
    XtFree((char *)secondary);return found;
}

static void TargetDestroyed(Widget widget, XtPointer client, XtPointer call)
{
    FontTarget *target=client;FontMenu *menu=target->menu;FontTarget **link=&menu->targets;
    (void)widget;(void)call;
    while(*link && *link!=target)link=&(*link)->next;
    if(*link)*link=target->next;
    XmRenderTableFree(target->original);XtFree((char *)target);
    if(menu->closing && !menu->targets)XtFree((char *)menu);
}

static void Collect(FontMenu *menu, Widget widget)
{
    static const char *resources[][2]={{XmNrenderTable,XmNfontList},{XmNlabelRenderTable,XmNlabelFontList},
        {XmNbuttonRenderTable,XmNbuttonFontList},{XmNtextRenderTable,XmNtextFontList}};
    if(!widget || !XtIsRectObj(widget))return;
    for(Cardinal i=0;i<menu->count;i++)if(menu->visited[i]==widget)return;
    menu->visited=(Widget *)XtRealloc((char *)menu->visited,(menu->count+1)*sizeof(Widget));
    menu->visited[menu->count++]=widget;
    for(Cardinal i=0;i<XtNumber(resources);i++) {
        const char *resource=HasResource(widget,resources[i][0])?resources[i][0]:
            HasResource(widget,resources[i][1])?resources[i][1]:NULL;
        if(!resource)continue;
        XmRenderTable table=NULL;
        XtVaGetValues(widget,resource,&table,NULL);
        if(table) {
            FontTarget *target=(FontTarget *)XtCalloc(1,sizeof(*target));
            target->menu=menu;target->widget=widget;target->resource=resource;
            target->original=XmRenderTableCopy(table,NULL,0);
            target->next=menu->targets;menu->targets=target;
            XtAddCallback(widget,XmNdestroyCallback,TargetDestroyed,target);
        }
    }
    if(XtIsComposite(widget)) {
        WidgetList children;Cardinal count;
        XtVaGetValues(widget,XtNchildren,&children,XtNnumChildren,&count,NULL);
        for(Cardinal i=0;i<count;i++)Collect(menu,children[i]);
    }
    if(XmIsCascadeButton(widget) || XmIsCascadeButtonGadget(widget)) {
        Widget pane=NULL;XtVaGetValues(widget,XmNsubMenuId,&pane,NULL);Collect(menu,pane);
    }
}

static Boolean ResizeRendition(Widget widget, XmRendition rendition, double factor)
{
    XmFontType type; XtPointer font=NULL;
    Arg args[4];Cardinal count=0;
    XtSetArg(args[count],XmNfontType,&type);count++;
    XtSetArg(args[count],XmNfont,&font);count++;
    XmRenditionRetrieve(rendition,args,count);
#ifdef USE_XFT
    {
        XftFont *base=NULL,*scaled;FcPattern *pattern=NULL;double pixels=0;
        Arg query;
        XtSetArg(query,XmNxftFont,&base);XmRenditionRetrieve(rendition,&query,1);
        if(type==XmFONT_IS_XFT && base && (unsigned long)base!=XmAS_IS) {
            pattern=FcPatternDuplicate(base->pattern);
            FcPatternGetDouble(base->pattern,FC_PIXEL_SIZE,0,&pixels);
        } else if(type==XmFONT_IS_FONT && font && (unsigned long)font!=XmAS_IS) {
            unsigned long atom;char *name=NULL;
            if(XGetFontProperty(font,XA_FONT,&atom))name=XGetAtomName(XtDisplayOfObject(widget),atom);
            if(name) { pattern=XftXlfdParse(name,False,False);XFree(name); }
            if(!pattern) { pattern=FcPatternCreate();FcPatternAddString(pattern,FC_FAMILY,(FcChar8 *)"monospace"); }
            pixels=((XFontStruct *)font)->ascent+((XFontStruct *)font)->descent;
            FcPatternDel(pattern,FC_SCALABLE);FcPatternAddBool(pattern,FC_SCALABLE,FcTrue);
            FcPatternDel(pattern,FC_OUTLINE);FcPatternAddBool(pattern,FC_OUTLINE,FcTrue);
        } else return True; /* An attributes-only rendition has no font to resize. */
        if(!pattern)return False;
        if(pixels<=0)pixels=14;
        FcPatternDel(pattern,FC_SIZE);FcPatternDel(pattern,FC_PIXEL_SIZE);
        FcPatternAddDouble(pattern,FC_PIXEL_SIZE,pixels*factor);
        FcConfigSubstitute(NULL,pattern,FcMatchPattern);XftDefaultSubstitute(XtDisplayOfObject(widget),XScreenNumberOfScreen(XtScreenOfObject(widget)),pattern);
        FcResult result;FcPattern *matched=XftFontMatch(XtDisplayOfObject(widget),XScreenNumberOfScreen(XtScreenOfObject(widget)),pattern,&result);
        FcPatternDestroy(pattern);
        scaled=matched?XftFontOpenPattern(XtDisplayOfObject(widget),matched):NULL;
        if(!scaled)return False;
        if(type==XmFONT_IS_FONT && scaled->ascent+scaled->descent>0) {
            /* Core bitmap pixels and outline em sizes have different line
             * metrics. Normalize the replacement to the original line height. */
            double actual_pixels=pixels*factor;
            FcPatternGetDouble(scaled->pattern,FC_PIXEL_SIZE,0,&actual_pixels);
            FcPattern *adjusted=FcPatternDuplicate(scaled->pattern);
            FcPatternDel(adjusted,FC_SIZE);FcPatternDel(adjusted,FC_PIXEL_SIZE);
            FcPatternAddDouble(adjusted,FC_PIXEL_SIZE,
                actual_pixels*pixels*factor/(scaled->ascent+scaled->descent));
            XftFont *normalized=XftFontOpenPattern(XtDisplayOfObject(widget),adjusted);
            if(normalized) { XftFontClose(XtDisplayOfObject(widget),scaled);scaled=normalized; }
        }
        count=0;XtSetArg(args[count],XmNfontType,XmFONT_IS_XFT);count++;
        XtSetArg(args[count],XmNxftFont,scaled);count++;
        XmRenditionUpdate(rendition,args,count);
        XftFontClose(XtDisplayOfObject(widget),scaled);
    }
#else
    {
        char name[160];XFontStruct *probe;
        if(type!=XmFONT_IS_FONT || !font || (unsigned long)font==XmAS_IS)return True;
        int pixels=(int)((((XFontStruct *)font)->ascent+((XFontStruct *)font)->descent)*factor+0.5);
        snprintf(name,sizeof(name),"-*-helvetica-medium-r-normal--%d-*-*-*-*-*-*-*",pixels);
        probe=XLoadQueryFont(XtDisplayOfObject(widget),name);
        if(!probe) { snprintf(name,sizeof(name),"-*-fixed-medium-r-normal--%d-*-*-*-*-*-*-*",pixels);probe=XLoadQueryFont(XtDisplayOfObject(widget),name); }
        if(!probe)return False;
        XFreeFont(XtDisplayOfObject(widget),probe);
        count=0;XtSetArg(args[count],XmNfont,NULL);count++;
        XtSetArg(args[count],XmNfontName,name);count++;
        XtSetArg(args[count],XmNloadModel,XmLOAD_IMMEDIATE);count++;
        XmRenditionUpdate(rendition,args,count);
    }
#endif
    return True;
}

static void ChooseFont(Widget widget, XtPointer client, XtPointer call)
{
    FontChoice *choice=client;FontMenu *menu=choice->menu;
    (void)widget;(void)call;
    for(FontTarget *target=menu->targets;target;target=target->next) {
        XmRenderTable table=XmRenderTableCopy(target->original,NULL,0);
        XmStringTag *tags=NULL;int count=XmRenderTableGetTags(table,&tags);
        for(int i=0;i<count;i++) {
            XmRendition rendition=XmRenderTableGetRendition(table,tags[i]);
            if(choice->factor!=1.0 && rendition && ResizeRendition(target->widget,rendition,choice->factor))
                table=XmRenderTableAddRenditions(table,&rendition,1,XmMERGE_REPLACE);
            XmRenditionFree(rendition);XtFree(tags[i]);
        }
        XtFree((char *)tags);
        XtVaSetValues(target->widget,target->resource,table,NULL);XmRenderTableFree(table);
    }
    if(menu->layout)menu->layout(menu->closure,choice->factor);
    static const char *containers[]={"*dialogContainer","*widgetContainer"};
    for(Cardinal i=0;i<XtNumber(containers);i++) { Widget container=XtNameToWidget(menu->root,(char *)containers[i]);
        if(container && XmIsContainer(container))XmContainerRelayout(container); }
}

static void MenuDestroyed(Widget widget, XtPointer client, XtPointer call)
{
    FontMenu *menu=client;(void)widget;(void)call;
    menu->closing=True;
    if(!menu->targets)XtFree((char *)menu);
}

Boolean PeriodicInstallFontMenu(Widget root, PeriodicFontLayoutProc layout, XtPointer closure)
{
    static const char *buttons[]={"*smallFontButton","*mediumFontButton","*largeFontButton"};
    static const double factors[]={0.85,1.0,1.25};
    static const char *popups[]={"*fileDialog","*selectDialog","*templateDialog","*infoDialog",
        "*warningDialog","*errorDialog","*questionDialog","*workingDialog","*promptDialog","*showDialog","*popupWindow","*popupMenu"};
    #ifdef USE_XFT
    if(!FcInit())return False;
    #endif
    FontMenu *menu=(FontMenu *)XtCalloc(1,sizeof(*menu));
    menu->root=root;menu->layout=layout;menu->closure=closure;
    Collect(menu,root);
    for(Cardinal i=0;i<XtNumber(popups);i++)Collect(menu,XtNameToWidget(root,(char *)popups[i]));
    XtFree((char *)menu->visited);menu->visited=NULL;
    /* Providers first, then each child's own original role/font. */
    FontTarget *forward=NULL;
    while(menu->targets) { FontTarget *target=menu->targets;menu->targets=target->next;target->next=forward;forward=target; }
    menu->targets=forward;
    for(Cardinal i=0;i<XtNumber(buttons);i++) {
        Widget button=XtNameToWidget(root,(char *)buttons[i]);
        if(!button)continue;
        menu->choices[i]=(FontChoice){menu,factors[i]};
        XtAddCallback(button,XmNactivateCallback,ChooseFont,&menu->choices[i]);
    }
    XtAddCallback(root,XmNdestroyCallback,MenuDestroyed,menu);
    return True;
}
