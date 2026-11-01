/* Outline connections must reach node faces through the focus gutter. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/OutlineP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static Boolean ready;
static void done(XtPointer data, XtIntervalId *id) {(void)data;(void)id;ready=True;}
static void drain(XtAppContext app, Display *d) {
    ready=False;XtAppAddTimeOut(app,40,done,NULL);
    while(!ready)XtAppProcessEvent(app,XtIMAll);
    XSync(d,False);
}
static Widget endpoint(Widget child) {
    OutlineConstraints node=(OutlineConstraints)child->core.constraints;
    Widget button=XmHierarchyC_open_close_button(node);
    return button?button:child;
}
static void connected(Widget child, Boolean vertical, Boolean outgoing, Pixel color) {
    Widget original=child;child=endpoint(child);
    Dimension focus;XtVaGetValues(child,XmNhighlightThickness,&focus,NULL);
    CHECK(focus>0);
    XImage *im=XGetImage(XtDisplay(child),XtWindow(child),0,0,
                         XtWidth(child),XtHeight(child),AllPlanes,ZPixmap);
    CHECK(im);
    for(unsigned i=0;i<focus;++i) {
        Boolean found=False;
        /* Direct connections can be steep: inspect the full gutter strip,
         * not a narrow neighborhood around the endpoint's center. */
        int extent=vertical?XtWidth(child):XtHeight(child);
        for(int across=0;across<extent;++across) {
            int x=vertical?across:(int)i;
            int y=vertical?(int)i:across;
            if(outgoing) {if(vertical)y=im->height-1-y;else x=im->width-1-x;}
            if(x>=0 && y>=0 && x<im->width && y<im->height && XGetPixel(im,x,y)==color)
                found=True;
        }
        if(!found)fprintf(stderr,"missing connector in %s gutter pixel %u geom=%d,%d %ux%u status=%u\n",XtName(child),i,child->core.x,child->core.y,child->core.width,child->core.height,XmHierarchyC_status((OutlineConstraints)original->core.constraints));
        CHECK(found);
    }
    XDestroyImage(im);
}
static unsigned long gutter_pixels(Widget child, Pixel color) {
    child=endpoint(child);
    Dimension focus;XtVaGetValues(child,XmNhighlightThickness,&focus,NULL);
    XImage *im=XGetImage(XtDisplay(child),XtWindow(child),0,0,
                         XtWidth(child),XtHeight(child),AllPlanes,ZPixmap);
    unsigned long count=0;CHECK(im);
    for(int y=0;y<im->height;++y)for(int x=0;x<im->width;++x)
        if(x<(int)focus || y<(int)focus || x>=im->width-focus || y>=im->height-focus)
            count += XGetPixel(im,x,y)==color;
    XDestroyImage(im);return count;
}
int main(int argc,char **argv) {
    XtAppContext app;XtToolkitInitialize();app=XtCreateApplicationContext();
    Display *d=XtOpenDisplay(app,NULL,"connections","Connections",NULL,0,&argc,argv);CHECK(d);
    /* Distinguish connectors from classic bevel/shadow pixels. */
    Pixel line=0x137f39;unsigned cases=0;
    XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK};
    const double scales[]={1.0,1.5,2.0};
    for(unsigned scale=0;scale<3;++scale)for(unsigned profile=0;profile<3;++profile)for(unsigned expandable=0;expandable<2;++expandable)for(unsigned constrained=0;constrained<2;++constrained) {
        fprintf(stderr,"CASE scale=%g profile=%u expandable=%u constrained=%u\n",scales[scale],profile,expandable,constrained);
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,profiles[profile]));
        Widget shell=XtAppCreateShell("connections","Connections",applicationShellWidgetClass,d,NULL,0);
        Widget host=XtVaCreateManagedWidget("host",xmFormWidgetClass,shell,XmNwidth,900,XmNheight,650,NULL);
        Widget tree=XtVaCreateManagedWidget("tree",xmOutlineWidgetClass,host,
            XmNtopAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_FORM,
            XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,
            XmNconnectNodes,True,XmNconstrainWidth,constrained,XmNforeground,line,
            XmNwidth,500,XmNheight,350,NULL);
        Widget parent=XtVaCreateManagedWidget("parent",xmPushButtonWidgetClass,tree,
            XmNnodeState,expandable?XmOpen:XmAlwaysOpen,XmNtraversalOn,False,NULL);
        Widget children[2];
        for(unsigned i=0;i<2;++i)children[i]=XtVaCreateManagedWidget(i?"second":"first",xmPushButtonWidgetClass,tree,
            XmNparentNode,parent,XmNnodeState,expandable?XmOpen:XmAlwaysOpen,XmNtraversalOn,False,
            NULL);
        /* Keep the initial pixel checks unfocused. Generated disclosure
         * buttons otherwise receive automatic Form traversal focus. */
        XtVaSetValues(endpoint(parent),XmNtraversalOn,False,NULL);
        for(unsigned i=0;i<2;++i)XtVaSetValues(endpoint(children[i]),XmNtraversalOn,False,NULL);
        XtRealizeWidget(shell);drain(app,d);
        for(unsigned repaint=0;repaint<3;++repaint) {
            fprintf(stderr,"repaint=%u\n",repaint);
            for(unsigned i=0;i<2;++i)connected(children[i],False,False,line);
            connected(parent,True,True,line);
            ++cases;
            if(repaint==0)for(unsigned i=0;i<2;++i)XClearArea(d,XtWindow(children[i]),0,0,0,0,True);
            else XtVaSetValues(shell,XmNwidth,XtWidth(shell)+17,XmNheight,XtHeight(shell)+9,NULL);
            drain(app,d);
        }
        if(expandable) {
            XtVaSetValues(parent,XmNnodeState,XmClosed,NULL);drain(app,d);
            XtVaSetValues(parent,XmNnodeState,XmOpen,NULL);drain(app,d);
            for(unsigned i=0;i<2;++i)connected(children[i],False,False,line);
            connected(parent,True,True,line);
        }
        CHECK(children[1]->core.y>=children[0]->core.y+children[0]->core.height);
        XtVaSetValues(shell,XmNwidth,650,XmNheight,450,NULL);drain(app,d);
        /* Parent-only partial exposes must preserve every endpoint. */
        XClearArea(d,XtWindow(tree),1,1,2,2,True);drain(app,d);
        for(unsigned i=0;i<2;++i)connected(children[i],False,False,line);
        connected(parent,True,True,line);
        /* Resize must not retain connector fragments from the old layout. */
        unsigned long before[3];Widget nodes[]={parent,children[0],children[1]};
        for(unsigned i=0;i<3;++i)before[i]=gutter_pixels(nodes[i],line);
        for(unsigned i=0;i<3;++i)XClearArea(d,XtWindow(nodes[i]),0,0,0,0,True);
        drain(app,d);
        for(unsigned i=0;i<3;++i)CHECK(before[i]==gutter_pixels(nodes[i],line));
        /* Focus unhighlight clears the gutter without a child expose. */
        Widget focus_node=endpoint(parent);
        Pixel focus_color=0xff00ff;
        XtVaSetValues(focus_node,XmNhighlightColor,focus_color,NULL);
        XmPrimitiveWidgetClass klass=(XmPrimitiveWidgetClass)XtClass(focus_node);
        klass->primitive_class.border_highlight(focus_node);
        drain(app,d);
        CHECK(((XmPrimitiveWidget)focus_node)->primitive.highlighted);
        unsigned long focused=gutter_pixels(parent,focus_color);CHECK(focused>0);
        XClearArea(d,XtWindow(tree),0,0,0,0,True);drain(app,d);
        CHECK(gutter_pixels(parent,focus_color)==focused);
        klass->primitive_class.border_unhighlight(focus_node);
        XEvent focus={0};focus.xfocus.type=FocusOut;focus.xfocus.display=d;
        focus.xfocus.window=XtWindow(focus_node);focus.xfocus.mode=NotifyNormal;
        focus.xfocus.detail=NotifyNonlinear;
        XSendEvent(d,XtWindow(focus_node),False,FocusChangeMask,&focus);
        drain(app,d);connected(parent,True,True,line);++cases;
        XtVaSetValues(tree,XmNconnectNodes,False,NULL);drain(app,d);
        for(unsigned i=0;i<3;++i)CHECK(gutter_pixels(nodes[i],line)==0);
        ++cases;
        XtVaSetValues(tree,XmNconnectNodes,True,NULL);drain(app,d);
        for(unsigned i=0;i<2;++i)connected(children[i],False,False,line);
        connected(parent,True,True,line);++cases;
        /* Destroy with a child repaint pending. */
        XClearArea(d,XtWindow(children[0]),0,0,0,0,True);XSync(d,False);
        while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);
        XtDestroyWidget(shell);drain(app,d);
    }
    unsigned metrics=0;
    for(unsigned compact=0;compact<2;++compact)for(unsigned scale=0;scale<3;++scale){
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
        CHECK(XmApplyAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        Widget shell=XtAppCreateShell("indent","Connections",applicationShellWidgetClass,d,NULL,0);
        Widget host=XtVaCreateManagedWidget("host",xmFormWidgetClass,shell,NULL);
        Widget normal=XtVaCreateWidget("normal",xmOutlineWidgetClass,host,NULL);
        Widget custom=XtVaCreateWidget("custom",xmOutlineWidgetClass,host,XmNindentSpace,7,NULL);
        Dimension indent,owned;XtVaGetValues(normal,XmNindentSpace,&indent,NULL);XtVaGetValues(custom,XmNindentSpace,&owned,NULL);
        CHECK(indent==(Dimension)((compact?24:30)*scales[scale]+.5)&&owned==7);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));XtVaGetValues(normal,XmNindentSpace,&indent,NULL);CHECK(indent==30);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_DARK));XtVaGetValues(normal,XmNindentSpace,&indent,NULL);XtVaGetValues(custom,XmNindentSpace,&owned,NULL);
        CHECK(indent==(Dimension)((compact?24:30)*scales[scale]+.5)&&owned==7);
        XtDestroyWidget(shell);++metrics;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS outline connections: %u layout/repaint and %u density/ownership cases\n",cases,metrics);return 0;
}
