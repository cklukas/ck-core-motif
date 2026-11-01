#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/TreeViewP.h>
#include <stdio.h>
#include <X11/keysym.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);} }while(0)
static void drain(XtAppContext app,Display *display) {
    XSync(display,False);while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);XSync(display,False);
}
static void verify(Widget widget,XtAppContext app) {
    XmTreeViewWidget view=(XmTreeViewWidget)widget;
    Pixel fg,bg,selection;XmAppearanceProfile profile;XGCValues gc;XImage *image;unsigned text_pixels=0;
    drain(app,XtDisplay(widget));
    XtVaGetValues(widget,XmNforeground,&fg,XmNbackground,&bg,NULL);
    CHECK(XGetGCValues(XtDisplay(widget),view->tree_view.gc,GCForeground|GCBackground,&gc));
    CHECK(gc.foreground==fg && gc.background==bg);
    image=XGetImage(XtDisplay(widget),XtWindow(widget),0,0,280,96,AllPlanes,ZPixmap);CHECK(image);
    CHECK(XmGetAppearanceProfile(XtDisplay(widget),&profile));
    XtVaGetValues(widget,XmNselectColor,&selection,NULL);
    CHECK(XGetPixel(image,20,1)==
        ((profile==XmAPPEARANCE_MODERN_LIGHT || profile==XmAPPEARANCE_MODERN_DARK) ? selection : fg));
    CHECK(XGetPixel(image,275,90)==bg);
    for(int y=5;y<23;++y)for(int x=22;x<120;++x)
        if(XGetPixel(image,x,y)==fg)++text_pixels;
    CHECK(text_pixels>5);
    if (profile==XmAPPEARANCE_MODERN_LIGHT || profile==XmAPPEARANCE_MODERN_DARK)
        for(int y=8;y<22;++y)for(int x=26;x<80;++x)
            CHECK(XGetPixel(image,x,y)!=bg); /* text must preserve the selection surface */
    XDestroyImage(image);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *display;Widget shell,board,tree,next;
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
        next=XtVaCreateManagedWidget("next",xmPushButtonWidgetClass,board,XmNx,300,NULL);
        XmTreeViewSetNodes(tree,&node,1);XmTreeViewSetSelectedId(tree,&id,False);
        XtRealizeWidget(shell);verify(tree,app);
        CHECK(XmApplyAppearanceProfile(display,profiles[(i+1)%4]));verify(tree,app);
        if (profiles[(i+1)%4]==XmAPPEARANCE_MODERN_LIGHT ||
            profiles[(i+1)%4]==XmAPPEARANCE_MODERN_DARK) {
            Pixel highlight; Dimension thickness; XImage *focus;
            XSetInputFocus(display,XtWindow(shell),RevertToParent,CurrentTime); drain(app,display);
            CHECK(XmProcessTraversal(tree,XmTRAVERSE_CURRENT)); drain(app,display);
            XtVaGetValues(tree,XmNhighlightColor,&highlight,XmNhighlightThickness,&thickness,NULL);
            CHECK(thickness>0);
            focus=XGetImage(display,XtWindow(tree),thickness-1,20,1,1,AllPlanes,ZPixmap); CHECK(focus);
            if(XGetPixel(focus,0,0)!=highlight) fprintf(stderr,"focus profile=%d thickness=%u actual=%lu expected=%lu highlighted=%d focus=%p tree=%p\n",profiles[(i+1)%4],thickness,XGetPixel(focus,0,0),highlight,((XmTreeViewWidget)tree)->primitive.highlighted,(void*)XmGetFocusWidget(shell),(void*)tree);
            CHECK(XGetPixel(focus,0,0)==highlight); XDestroyImage(focus);
            {
                XEvent key={0}; key.xkey.type=KeyPress; key.xkey.display=display;
                key.xkey.window=XtWindow(tree); key.xkey.keycode=XKeysymToKeycode(display,XK_Right);
                XtDispatchEvent(&key); drain(app,display); CHECK(XmGetFocusWidget(shell)==tree);
            }
            CHECK(XmProcessTraversal(next,XmTRAVERSE_CURRENT)); drain(app,display);
            CHECK(!((XmTreeViewWidget)tree)->primitive.highlighted);
        }
        {
            XColor fg,bg,selected,exact;Colormap map=DefaultColormap(display,DefaultScreen(display));
            CHECK(XAllocNamedColor(display,map,"#eedd99",&fg,&exact));
            CHECK(XAllocNamedColor(display,map,"#223344",&bg,&exact));
            CHECK(XAllocNamedColor(display,map,"#334466",&selected,&exact));
            XtVaSetValues(tree,XmNforeground,fg.pixel,XmNbackground,bg.pixel,
                XmNselectColor,selected.pixel,NULL);verify(tree,app);
            CHECK(XmApplyAppearanceProfile(display,profiles[(i+2)%4]));verify(tree,app);
            { Pixel actual; XtVaGetValues(tree,XmNselectColor,&actual,NULL); CHECK(actual==selected.pixel); }
        }
        XtDestroyWidget(shell);drain(app,display);
    }
    XtCloseDisplay(display);XtDestroyApplicationContext(app);
    puts("PASS TreeView colors: 4 initial profiles, live transitions and explicit palettes");return 0;
}
