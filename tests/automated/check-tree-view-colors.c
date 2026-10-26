#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/TreeViewP.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);} }while(0)
static void drain(XtAppContext app,Display *display) {
    XSync(display,False);while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);XSync(display,False);
}
static void verify(Widget widget,XtAppContext app) {
    XmTreeViewWidget view=(XmTreeViewWidget)widget;
    Pixel fg,bg;XGCValues gc;XImage *image;unsigned text_pixels=0;
    drain(app,XtDisplay(widget));
    XtVaGetValues(widget,XmNforeground,&fg,XmNbackground,&bg,NULL);
    CHECK(XGetGCValues(XtDisplay(widget),view->tree_view.gc,GCForeground|GCBackground,&gc));
    CHECK(gc.foreground==fg && gc.background==bg);
    image=XGetImage(XtDisplay(widget),XtWindow(widget),0,0,280,96,AllPlanes,ZPixmap);CHECK(image);
    CHECK(XGetPixel(image,20,1)==fg); /* selected row outline */
    CHECK(XGetPixel(image,275,90)==bg);
    for(int y=5;y<23;++y)for(int x=22;x<120;++x)
        if(XGetPixel(image,x,y)==fg)++text_pixels;
    CHECK(text_pixels>5);XDestroyImage(image);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *display;Widget shell,board,tree;
    int id=1;XmTreeViewNode node={&id,NULL,"Readable label",False,False};
    XmAppearanceProfile profiles[]={XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_HIGH_CONTRAST,XmAPPEARANCE_CLASSIC};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"treeColors","TreeColors",NULL,0,&argc,argv);CHECK(display);
    for(unsigned i=0;i<4;++i) {
        CHECK(XmApplyAppearanceProfile(display,profiles[i]));
        shell=XtAppCreateShell("treeColors","TreeColors",applicationShellWidgetClass,display,NULL,0);
        board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,NULL);
        tree=XtVaCreateManagedWidget("tree",xmTreeViewWidgetClass,board,XmNwidth,280,XmNheight,96,
            XmNtreeViewRowHeight,28,XmNtreeViewVisibleRows,3,NULL);
        XmTreeViewSetNodes(tree,&node,1);XmTreeViewSetSelectedId(tree,&id,False);
        XtRealizeWidget(shell);verify(tree,app);
        CHECK(XmApplyAppearanceProfile(display,profiles[(i+1)%4]));verify(tree,app);
        {
            XColor fg,bg,exact;Colormap map=DefaultColormap(display,DefaultScreen(display));
            CHECK(XAllocNamedColor(display,map,"#eedd99",&fg,&exact));
            CHECK(XAllocNamedColor(display,map,"#223344",&bg,&exact));
            XtVaSetValues(tree,XmNforeground,fg.pixel,XmNbackground,bg.pixel,NULL);verify(tree,app);
            CHECK(XmApplyAppearanceProfile(display,profiles[(i+2)%4]));verify(tree,app);
        }
        XtDestroyWidget(shell);drain(app,display);
    }
    XtCloseDisplay(display);XtDestroyApplicationContext(app);
    puts("PASS TreeView colors: 4 initial profiles, live transitions and explicit palettes");return 0;
}
