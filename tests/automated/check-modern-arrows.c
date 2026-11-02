/* Verify real widget/gadget pixels and their normal interaction paths. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/ArrowBGP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static unsigned activations;
static void activated(Widget w,XtPointer client,XtPointer call) {
    (void)w; (void)client; CHECK(call); ++activations;
}
static void drain(XtAppContext app, Display *display) {
    XSync(display,False);
    while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll);
    XSync(display,False);
}
static XImage *capture(Widget w) {
    XSync(XtDisplayOfObject(w),False);
    return XGetImage(XtDisplayOfObject(w),XtWindowOfObject(w),
        XtIsWidget(w)?0:XtX(w),XtIsWidget(w)?0:XtY(w),
        XtWidth(w),XtHeight(w),AllPlanes,ZPixmap);
}
static void focus_face(Widget w) {
    Pixel parent,accent;
    Dimension highlight;
    XtWidgetProc on,off;
    if (XtIsWidget(w)) {
        on=((XmPrimitiveWidgetClass)XtClass(w))->primitive_class.border_highlight;
        off=((XmPrimitiveWidgetClass)XtClass(w))->primitive_class.border_unhighlight;
    } else {
        on=((XmGadgetClass)XtClass(w))->gadget_class.border_highlight;
        off=((XmGadgetClass)XtClass(w))->gadget_class.border_unhighlight;
    }
    XtVaGetValues(XtParent(w),XmNbackground,&parent,NULL);
    XtVaGetValues(w,XmNhighlightColor,&accent,XmNhighlightThickness,&highlight,NULL);
    on(w);
    XImage *im=capture(w); CHECK(im);
    CHECK(XGetPixel(im,0,0)==parent);
    CHECK(XGetPixel(im,im->width/2,0)==accent);
    for(int y=0;y<im->height;++y) for(int x=0;x<im->width;++x)
        if(x<highlight || y<highlight || x>=im->width-highlight || y>=im->height-highlight) {
            CHECK(XGetPixel(im,x,y)==XGetPixel(im,im->width-1-x,y));
            CHECK(XGetPixel(im,x,y)==XGetPixel(im,x,im->height-1-y));
        }
    /* Rounded corners extend inside the rectangular highlight band.
     * Every focus pixel, including those corner arcs, must belong to one 8-connected outline. Symmetry
     * alone also accepts four disconnected, identically clipped sides. */
    {
        size_t total = (size_t)im->width * im->height, count = 0, reached = 0;
        unsigned char *seen = calloc(total, 1);
        size_t *queue = malloc(total * sizeof(*queue));
        CHECK(seen && queue);
        for (int y = 0; y < im->height; ++y) for (int x = 0; x < im->width; ++x)
            if (XGetPixel(im,x,y) == accent) {
                if (!count) { queue[0] = (size_t)y*im->width+x; seen[queue[0]] = 1; reached = 1; }
                ++count;
            }
        for (size_t head = 0; head < reached; ++head) {
            int x = queue[head] % im->width, y = queue[head] / im->width;
            for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx) {
                int nx = x+dx, ny = y+dy;
                if (nx < 0 || ny < 0 || nx >= im->width || ny >= im->height) continue;
                size_t index = (size_t)ny*im->width+nx;
                if (!seen[index] &&
                    XGetPixel(im,nx,ny) == accent) { seen[index] = 1; queue[reached++] = index; }
            }
        }
        CHECK(count > 0 && reached == count);
        free(queue); free(seen);
    }
    XClearArea(XtDisplayOfObject(w),XtWindowOfObject(w),0,0,0,0,True);
    drain(XtWidgetToApplicationContext(w),XtDisplayOfObject(w));
    XImage *repaint=capture(w); CHECK(repaint);
    for(int y=0;y<im->height;++y) for(int x=0;x<im->width;++x)
        CHECK(XGetPixel(im,x,y)==XGetPixel(repaint,x,y));
    XDestroyImage(repaint);
    XDestroyImage(im);
    off(w);
    im=capture(w); CHECK(im);
    CHECK(XGetPixel(im,im->width/2,0)==parent);
    XDestroyImage(im);
}
/* Observe the ring after state transitions without repainting it first. */
static void focus_connected(Widget w,Boolean install) {
    if(install) {
        XtWidgetProc on=XtIsWidget(w)?((XmPrimitiveWidgetClass)XtClass(w))->primitive_class.border_highlight:
            ((XmGadgetClass)XtClass(w))->gadget_class.border_highlight;
        on(w);
    }
    Pixel accent;XtVaGetValues(w,XmNhighlightColor,&accent,NULL);
    XImage *im=capture(w);CHECK(im);
    size_t total=(size_t)im->width*im->height,count=0,reached=0;
    unsigned char *seen=calloc(total,1);size_t *queue=malloc(total*sizeof(*queue));CHECK(seen && queue);
    for(int y=0;y<im->height;y++)for(int x=0;x<im->width;x++)if(XGetPixel(im,x,y)==accent) {
        if(!count){queue[0]=(size_t)y*im->width+x;seen[queue[0]]=1;reached=1;}count++;
    }
    for(size_t head=0;head<reached;head++) {
        int x=queue[head]%im->width,y=queue[head]/im->width;
        for(int dy=-1;dy<=1;dy++)for(int dx=-1;dx<=1;dx++) {
            int nx=x+dx,ny=y+dy;if(nx<0 || ny<0 || nx>=im->width || ny>=im->height)continue;
            size_t n=(size_t)ny*im->width+nx;
            if(!seen[n] && XGetPixel(im,nx,ny)==accent){seen[n]=1;queue[reached++]=n;}
        }
    }
    CHECK(count>0 && reached==count);free(queue);free(seen);XDestroyImage(im);
}
static void input(Widget w,const char *action,Mask mask) {
    XEvent event={0};
    Position root_x,root_y;
    event.xbutton.type=(mask&(XmACTIVATE_EVENT|XmMULTI_ACTIVATE_EVENT))?ButtonRelease:ButtonPress;
    event.xbutton.display=XtDisplayOfObject(w);
    event.xbutton.window=XtWindowOfObject(w); event.xbutton.button=Button1;
    event.xbutton.x=(XtIsWidget(w)?0:XtX(w))+XtWidth(w)/2;
    event.xbutton.y=(XtIsWidget(w)?0:XtY(w))+XtHeight(w)/2;
    event.xbutton.same_screen=True;
    XtTranslateCoords(XtIsWidget(w)?w:XtParent(w),event.xbutton.x,event.xbutton.y,&root_x,&root_y);
    event.xbutton.x_root=root_x;event.xbutton.y_root=root_y;
    if(mask==XmENTER_EVENT || mask==XmLEAVE_EVENT) {
        event.xcrossing.type=mask==XmENTER_EVENT?EnterNotify:LeaveNotify;
        event.xcrossing.mode=NotifyNormal;event.xcrossing.detail=NotifyNonlinear;
    }
    if(XtIsWidget(w)) XtCallActionProc(w,action,&event,NULL,0);
    else xmArrowButtonGadgetClassRec.gadget_class.input_dispatch(w,&event,mask);
}
static void face(Widget w,Boolean pressed,Boolean disabled,double scale) {
    Dimension highlight,shadow; Pixel foreground,background,border,fill,glyph;
    XImage *image=capture(w); CHECK(image);
    XtVaGetValues(w,XmNhighlightThickness,&highlight,XmNshadowThickness,&shadow,
        XmNforeground,&foreground,XmNbackground,&background,XmNbottomShadowColor,&border,NULL);
    fill=pressed?foreground:background;
    glyph=disabled?border:(pressed?background:foreground);
    Boolean focused=XtIsWidget(w)?((XmPrimitiveWidget)w)->primitive.highlighted:((XmGadget)w)->gadget.highlighted;
    CHECK(focused || XGetPixel(image,highlight,highlight)==background);
    CHECK(XGetPixel(image,XtWidth(w)/2,highlight)==(shadow?(pressed?foreground:border):fill));
    CHECK(XGetPixel(image,XtWidth(w)/2,highlight+shadow+(int)(3*scale))==fill);
    {
        unsigned ink=0;
        int cx=(XtWidth(w)-1)/2,cy=(XtHeight(w)-1)/2,half=(int)(8*scale);
        for(int y=cy-half;y<=cy+half;++y)
        for(int x=cx-half;x<=cx+half;++x) {
            Pixel pixel=XGetPixel(image,x,y);
            CHECK(pixel==fill || pixel==glyph);
            if(pixel==glyph) ++ink;
        }
        CHECK(ink>0 && ink<(unsigned)((2*half+1)*(2*half+1)/2));
        unsigned char direction;
        int xmin=cx+half,xmax=cx-half,ymin=cy+half,ymax=cy-half;
        XtVaGetValues(w,XmNarrowDirection,&direction,NULL);
        for(int y=cy-half;y<=cy+half;++y) for(int x=cx-half;x<=cx+half;++x)
            if(XGetPixel(image,x,y)==glyph) {
                if(x<xmin)xmin=x;
                if(x>xmax)xmax=x;
                if(y<ymin)ymin=y;
                if(y>ymax)ymax=y;
            }
        Boolean vertical=direction==XmARROW_UP || direction==XmARROW_DOWN;
        for(int y=ymin;y<=ymax;++y) for(int x=xmin;x<=xmax;++x)
            CHECK((XGetPixel(image,x,y)==glyph)==
                (XGetPixel(image,vertical?xmin+xmax-x:x,vertical?y:ymin+ymax-y)==glyph));
    }
    XDestroyImage(image);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *display;unsigned cases=0;
    const double scales[]={1,1.5,2};
    const unsigned char directions[]={XmARROW_UP,XmARROW_DOWN,XmARROW_LEFT,XmARROW_RIGHT};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"modernArrows","ModernArrows",NULL,0,&argc,argv);CHECK(display);
    for(unsigned scale=0;scale<3;++scale) for(unsigned dark=0;dark<2;++dark)
    for(unsigned compact=0;compact<2;++compact) for(unsigned gadget=0;gadget<2;++gadget)
    for(unsigned direction=0;direction<4;++direction) for(unsigned flat=0;flat<2;++flat) {
        Widget shell,board,arrow;Dimension shadow;
        CHECK(XmSetDisplayScaleOverride(display,scales[scale]));
        CHECK(XmApplyAppearanceDensity(display,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("modernArrows","ModernArrows",applicationShellWidgetClass,display,NULL,0);
        board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,
            XmNwidth,400,XmNheight,300,XmNresizePolicy,XmRESIZE_NONE,NULL);
        arrow=XtVaCreateManagedWidget("arrow",gadget?xmArrowButtonGadgetClass:xmArrowButtonWidgetClass,
            board,XmNx,20,XmNy,20,XmNwidth,(int)(80*scales[scale]),XmNheight,(int)(56*scales[scale]),
            XmNarrowDirection,directions[direction],NULL);
        if(flat)XtVaSetValues(arrow,XmNshadowThickness,0,NULL);
        XtAddCallback(arrow,XmNactivateCallback,activated,NULL);
        XtRealizeWidget(shell);drain(app,display);
        XtVaGetValues(arrow,XmNshadowThickness,&shadow,NULL);
        CHECK(shadow==(flat?0:(Dimension)(scales[scale]+0.5)));
        face(arrow,False,False,scales[scale]);
        focus_face(arrow);
        focus_connected(arrow,True);
        input(arrow,"Arm",XmARM_EVENT);drain(app,display);
        focus_connected(arrow,False);
        face(arrow,True,False,scales[scale]);
        input(arrow,"Leave",XmLEAVE_EVENT);
        focus_connected(arrow,False);
        face(arrow,False,False,scales[scale]);
        XClearArea(display,XtWindowOfObject(arrow),0,0,0,0,True);drain(app,display);
        face(arrow,False,False,scales[scale]);
        input(arrow,"Enter",XmENTER_EVENT);
        focus_connected(arrow,False);
        face(arrow,True,False,scales[scale]);
        activations=0;input(arrow,"Activate",XmACTIVATE_EVENT);drain(app,display);
        CHECK(activations==1);focus_connected(arrow,False);face(arrow,False,False,scales[scale]);
        XtSetSensitive(arrow,False);drain(app,display);
        face(arrow,False,True,scales[scale]);
        XtSetSensitive(arrow,True);
        /* Both local and ancestor sensitivity cancel an armed gesture. */
        for(unsigned parent=0;parent<2;++parent) {
            Widget owner=parent?board:arrow;
            unsigned before=activations;
            input(arrow,"Arm",XmARM_EVENT);
            XtSetSensitive(owner,False);drain(app,display);
            face(arrow,False,True,scales[scale]);
            XtSetSensitive(owner,True);drain(app,display);
            face(arrow,False,False,scales[scale]);
            input(arrow,"Activate",XmACTIVATE_EVENT);
            CHECK(activations==before);
            input(arrow,"MultiActivate",XmMULTI_ACTIVATE_EVENT);
            CHECK(activations==before);
            input(arrow,"Arm",XmARM_EVENT);
            input(arrow,"Activate",XmACTIVATE_EVENT);
            CHECK(activations==before+1);
        }
        XtVaSetValues(arrow,XmNshadowThickness,3,NULL);drain(app,display);
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_CLASSIC));drain(app,display);
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));drain(app,display);
        XtVaGetValues(arrow,XmNshadowThickness,&shadow,NULL);CHECK(shadow==3);
        face(arrow,False,False,scales[scale]);
        for(unsigned legacy=0;legacy<2;++legacy) {
            XImage *before,*after;Dimension highlight;
            CHECK(XmApplyAppearanceProfile(display,legacy?XmAPPEARANCE_HIGH_CONTRAST:XmAPPEARANCE_CLASSIC));
            drain(app,display);before=capture(arrow);CHECK(before);
            XtVaGetValues(arrow,XmNhighlightThickness,&highlight,NULL);
            input(arrow,"Arm",XmARM_EVENT);input(arrow,"Leave",XmLEAVE_EVENT);
            XClearArea(display,XtWindowOfObject(arrow),0,0,0,0,True);drain(app,display);
            after=capture(arrow);CHECK(after);
            for(int y=highlight;y<(int)XtHeight(arrow)-highlight;++y)
            for(int x=highlight;x<(int)XtWidth(arrow)-highlight;++x)
                CHECK(XGetPixel(before,x,y)==XGetPixel(after,x,y));
            XDestroyImage(before);XDestroyImage(after);
            input(arrow,"Enter",XmENTER_EVENT);input(arrow,"Activate",XmACTIVATE_EVENT);
            drain(app,display);
        }
        XtDestroyWidget(shell);drain(app,display);++cases;
    }
    XtCloseDisplay(display);XtDestroyApplicationContext(app);
    printf("PASS modern arrows: %u cases\n",cases);return 0;
}
