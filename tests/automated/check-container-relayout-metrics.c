/* Relayout must use current child metrics, not a previously cached grid. */
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
static void drain(XtAppContext app, Display *d)
{
    XSync(d,False);
    while(XtAppPending(app)) { XtAppProcessEvent(app,XtIMAll);XSync(d,False); }
}
static void bounds(Widget w, Position *x, Position *y, Dimension *width, Dimension *height)
{ XtVaGetValues(w,XmNx,x,XmNy,y,XmNwidth,width,XmNheight,height,NULL); }
static void disjoint(Widget *items)
{
    for(int i=0;i<3;i++)for(int j=i+1;j<3;j++) {
        Position ax,ay,bx,by;Dimension aw,ah,bw,bh;
        bounds(items[i],&ax,&ay,&aw,&ah);bounds(items[j],&bx,&by,&bw,&bh);
        if(ax<bx+bw && bx<ax+aw && ay<by+bh && by<ay+ah) {
            fprintf(stderr,"overlap: %d (%d,%d %ux%u), %d (%d,%d %ux%u)\n",i,ax,ay,aw,ah,j,bx,by,bw,bh);
            abort();
        }
    }
}
int main(int argc,char **argv)
{
    XtAppContext app;Widget shell=XtVaAppInitialize(&app,"ContainerRelayoutTest",NULL,0,&argc,argv,NULL,NULL);
    Display *d=XtDisplay(shell);
    assert(XmSetAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
    for(int small=0;small<2;small++)for(int cells=0;cells<2;cells++)for(int fixed=0;fixed<2;fixed++) {
        Widget popup=XtVaCreatePopupShell("case",topLevelShellWidgetClass,shell,
            XtNwidth,600,XtNheight,400,NULL);
        Widget container=XtVaCreateManagedWidget("grid",xmContainerWidgetClass,popup,
            XmNlayoutType,XmSPATIAL,XmNspatialStyle,cells?XmCELLS:XmGRID,
            XmNentryViewType,small?XmSMALL_ICON:XmLARGE_ICON,
            small?XmNsmallCellWidth:XmNlargeCellWidth,fixed?80:0,
            small?XmNsmallCellHeight:XmNlargeCellHeight,fixed?60:0,NULL);
        Widget items[3];
        for(int i=0;i<3;i++) {
            XmString label=XmStringCreateLocalized("One");
            items[i]=XtVaCreateManagedWidget("item",xmIconGadgetClass,container,XmNlabelString,label,NULL);
            XmStringFree(label);
        }
        XtPopup(popup,XtGrabNone);drain(app,d);
        /* Qualify forced layout after all child initialization has completed. */
        XmContainerRelayout(container);drain(app,d);disjoint(items);
        XmString label=XmStringCreateLocalized(fixed?"Longish":"A substantially wider label\nwith a second line");
        XtVaSetValues(items[0],XmNlabelString,label,NULL);XmStringFree(label);
        drain(app,d);XmContainerRelayout(container);drain(app,d);disjoint(items);
        if(fixed) {
            Dimension width,height;
            XtVaGetValues(container,small?XmNsmallCellWidth:XmNlargeCellWidth,&width,
                small?XmNsmallCellHeight:XmNlargeCellHeight,&height,NULL);
            assert(width==80 && height==60);
        }
        label=XmStringCreateLocalized("One");XtVaSetValues(items[0],XmNlabelString,label,NULL);XmStringFree(label);
        XmContainerRelayout(container);drain(app,d);disjoint(items);
        XtDestroyWidget(popup);drain(app,d);
    }
    XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
    puts("PASS Container relayout metrics: large/small, grid/cells, dynamic/explicit, grow/shrink");
    return 0;
}
