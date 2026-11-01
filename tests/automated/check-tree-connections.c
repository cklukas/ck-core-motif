/* Tree connections must reach node faces through the focus gutter. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/TreeP.h>
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
static void connected(Widget child, Boolean vertical, Boolean outgoing, Pixel color) {
    if(!outgoing) {
        TreeConstraints node=(TreeConstraints)child->core.constraints;
        if(XmHierarchyC_open_close_button(node)) child=XmHierarchyC_open_close_button(node);
    }
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
        if(!found)fprintf(stderr,"missing connector in %s gutter pixel %u\n",XtName(child),i);
        CHECK(found);
    }
    XDestroyImage(im);
}
static unsigned long gutter_pixels(Widget child, Pixel color) {
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
    Pixel line=BlackPixel(d,DefaultScreen(d));unsigned cases=0;
    XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK};
    const double scales[]={1.0,1.5,2.0};
    for(unsigned scale=0;scale<3;++scale)for(unsigned profile=0;profile<3;++profile)for(unsigned vertical=0;vertical<2;++vertical)
    for(unsigned ladder=0;ladder<2;++ladder)for(unsigned expandable=0;expandable<2;++expandable) {
        fprintf(stderr,"CASE scale=%g profile=%u vertical=%u ladder=%u expandable=%u\n",scales[scale],profile,vertical,ladder,expandable);
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,profiles[profile]));
        Widget shell=XtAppCreateShell("connections","Connections",applicationShellWidgetClass,d,NULL,0);
        Widget tree=XtVaCreateManagedWidget("tree",xmTreeWidgetClass,shell,
            XmNorientation,vertical?XmVERTICAL:XmHORIZONTAL,
            XmNconnectStyle,ladder?XmTreeLadder:XmTreeDirect,
            XmNwidth,500,XmNheight,350,NULL);
        Widget parent=XtVaCreateManagedWidget("parent",xmPushButtonWidgetClass,tree,
            XmNnodeState,expandable?XmOpen:XmAlwaysOpen,XmNtraversalOn,False,XmNlineColor,line,NULL);
        Widget children[2];
        for(unsigned i=0;i<2;++i)children[i]=XtVaCreateManagedWidget(i?"second":"first",xmPushButtonWidgetClass,tree,
            XmNparentNode,parent,XmNnodeState,expandable?XmOpen:XmAlwaysOpen,XmNtraversalOn,False,
            XmNlineColor,line,NULL);
        XtRealizeWidget(shell);drain(app,d);
        for(unsigned repaint=0;repaint<3;++repaint) {
            fprintf(stderr,"repaint=%u\n",repaint);
            for(unsigned i=0;i<2;++i)connected(children[i],vertical,False,line);
            connected(parent,vertical,True,line);
            ++cases;
            if(repaint==0)for(unsigned i=0;i<2;++i)XClearArea(d,XtWindow(children[i]),0,0,0,0,True);
            else XtVaSetValues(children[0],XmNwidth,XtWidth(children[0])+17,XmNheight,XtHeight(children[0])+9,NULL);
            drain(app,d);
        }
        if(expandable) {
            XtVaSetValues(parent,XmNnodeState,XmClosed,NULL);drain(app,d);
            XtVaSetValues(parent,XmNnodeState,XmOpen,NULL);drain(app,d);
            for(unsigned i=0;i<2;++i)connected(children[i],vertical,False,line);
            connected(parent,vertical,True,line);
        }
        /* Resize must not retain connector fragments from the old slope. */
        unsigned long before[3];Widget nodes[]={parent,children[0],children[1]};
        for(unsigned i=0;i<3;++i)before[i]=gutter_pixels(nodes[i],line);
        for(unsigned i=0;i<3;++i)XClearArea(d,XtWindow(nodes[i]),0,0,0,0,True);
        drain(app,d);
        for(unsigned i=0;i<3;++i)CHECK(before[i]==gutter_pixels(nodes[i],line));
        /* Focus unhighlight clears the gutter without a child expose. */
        XmPrimitiveWidgetClass klass=(XmPrimitiveWidgetClass)XtClass(parent);
        klass->primitive_class.border_highlight(parent);
        drain(app,d);
        CHECK(((XmPrimitiveWidget)parent)->primitive.highlighted);
        klass->primitive_class.border_unhighlight(parent);
        XEvent focus={0};focus.xfocus.type=FocusOut;focus.xfocus.display=d;
        focus.xfocus.window=XtWindow(parent);focus.xfocus.mode=NotifyNormal;
        focus.xfocus.detail=NotifyNonlinear;
        XSendEvent(d,XtWindow(parent),False,FocusChangeMask,&focus);
        drain(app,d);connected(parent,vertical,True,line);++cases;
        /* Destroy with a coalesced child repaint pending. */
        XClearArea(d,XtWindow(children[0]),0,0,0,0,True);XSync(d,False);
        while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);
        XtDestroyWidget(shell);drain(app,d);
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS tree connections: %u layout/repaint cases\n",cases);return 0;
}
