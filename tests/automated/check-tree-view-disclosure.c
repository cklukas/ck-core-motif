/* Modern disclosure pixels and immediate mouse activation at every scale. */
#include "native-window.h"
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static unsigned changes;
static void changed(Widget w, XtPointer client, XtPointer call) {
    (void)w; (void)client; CHECK(call); ++changes;
}
static void drain(XtAppContext app, Display *display) {
    XSync(display,False);
    while (XtAppPending(app)) XtAppProcessEvent(app,XtIMAll);
    XSync(display,False);
}
static unsigned long pixels(Widget tree, int left, int size, int row, Boolean open) {
    Pixel fg; unsigned count=0; unsigned long hash=5381;
    XtVaGetValues(tree,XmNforeground,&fg,NULL);
    XImage *image=XGetImage(XtDisplay(tree),XtWindow(tree),left,0,size,row,AllPlanes,ZPixmap);
    CHECK(image);
    for (int y=0;y<row;++y) for (int x=0;x<size;++x) {
        unsigned long pixel=XGetPixel(image,x,y); hash=hash*33+pixel;
        if (pixel==fg) { ++count; CHECK(y >= (row-size)/2 && y < (row+size)/2); }
    }
    CHECK(count >= 3);
    int xmin=size,xmax=-1,ymin=row,ymax=-1;
    for(int y=0;y<row;++y) for(int x=0;x<size;++x) if(XGetPixel(image,x,y)==fg) {
        if(x<xmin)xmin=x;
        if(x>xmax)xmax=x;
        if(y<ymin)ymin=y;
        if(y>ymax)ymax=y;
    }
    for(int y=ymin;y<=ymax;++y) for(int x=xmin;x<=xmax;++x) {
        int mirror_x=open?xmin+xmax-x:x;
        int mirror_y=open?y:ymin+ymax-y;
        CHECK((XGetPixel(image,x,y)==fg)==(XGetPixel(image,mirror_x,mirror_y)==fg));
    }
    XDestroyImage(image); return hash;
}
int main(int argc,char **argv) {
    XtAppContext app; Display *display; unsigned cases=0; int root=1,child=2;
    const double scales[]={1,1.5,2,3};
    XmTreeViewNode nodes[]={{&root,NULL,"Root",True,False},{&child,&root,"Child",False,False}};
    XtToolkitInitialize(); app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"treeDisclosure","TreeDisclosure",NULL,0,&argc,argv); CHECK(display);
    for (unsigned dark=0;dark<2;++dark) for(unsigned rtl=0;rtl<2;++rtl)
    for(unsigned scale=0;scale<XtNumber(scales);++scale) {
        Widget shell,tree; XEvent event={0}; unsigned long closed,opened;
        int width=320*scales[scale],row=40*scales[scale],size=12*scales[scale],inset=8*scales[scale];
        int left=rtl ? width-inset-size : inset;
        CHECK(XmSetDisplayScaleOverride(display,scales[scale]));
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("treeDisclosure","TreeDisclosure",applicationShellWidgetClass,display,NULL,0);
        tree=XtVaCreateManagedWidget("tree",xmTreeViewWidgetClass,shell,XmNwidth,width,
            XmNheight,row*2,XmNtreeViewRowHeight,row,XmNtreeViewVisibleRows,2,XmNtreeViewRtl,rtl,NULL);
        XmTreeViewSetNodes(tree,nodes,2); XtAddCallback(tree,XmNtreeViewExpansionCallback,changed,NULL);
        XtRealizeWidget(shell); NativeWaitForViewable(shell); drain(app,display); closed=pixels(tree,left,size,row,False);
        event.xbutton.type=ButtonPress; event.xbutton.display=display; event.xbutton.window=XtWindow(tree);
        event.xbutton.button=Button1; event.xbutton.x=left+size/2; event.xbutton.y=row/2;
        changes=0; XtDispatchEvent(&event); drain(app,display);
        CHECK(XmTreeViewGetExpanded(tree,&root)); CHECK(changes==1);
        CHECK(XmTreeViewGetSelectedId(tree)==NULL);
        opened=pixels(tree,left,size,row,True); CHECK(opened!=closed);
        XtDispatchEvent(&event); drain(app,display); CHECK(!XmTreeViewGetExpanded(tree,&root)); CHECK(changes==2);
        CHECK(pixels(tree,left,size,row,False)==closed);
        /* Tiny explicit rows must not draw into the following row. */
        XtVaSetValues(tree,XmNtreeViewRowHeight,2,NULL); drain(app,display);
        {
            Pixel fg; XImage *image;
            XtVaGetValues(tree,XmNforeground,&fg,NULL);
            image=XGetImage(display,XtWindow(tree),left,0,size,4,AllPlanes,ZPixmap); CHECK(image);
            for (int y=0;y<4;++y) for(int x=0;x<size;++x)
                CHECK(XGetPixel(image,x,y)!=fg);
            XDestroyImage(image);
        }
        XtDestroyWidget(shell); drain(app,display); ++cases;
    }
    XtCloseDisplay(display); XtDestroyApplicationContext(app);
    printf("PASS TreeView disclosure: %u cases\n",cases); return 0;
}
