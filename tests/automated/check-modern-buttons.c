/* Native widget/gadget painting, default emphasis and drag feedback. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/PushBGP.h>
#include <Xm/PushBP.h>
#include <Xm/IconButtonP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static unsigned activations;
static void activated(Widget w,XtPointer client,XtPointer call) {
    (void)w;(void)client;CHECK(call);++activations;
}
static void drain(XtAppContext app,Display *d) {
    XSync(d,False);while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll);XSync(d,False);
}
static XImage *capture(Widget w) {
    XSync(XtDisplayOfObject(w),False);
    return XGetImage(XtDisplayOfObject(w),XtWindowOfObject(w),
        XtIsWidget(w)?0:XtX(w),XtIsWidget(w)?0:XtY(w),XtWidth(w),XtHeight(w),AllPlanes,ZPixmap);
}
static void face(Widget w,Boolean pressed,Boolean is_default,double scale) {
    Dimension highlight,shadow;Pixel background,arm,border,accent,parent;
    XImage *im=capture(w);CHECK(im);
    XtVaGetValues(w,XmNhighlightThickness,&highlight,XmNshadowThickness,&shadow,
        XmNbackground,&background,XmNarmColor,&arm,XmNbottomShadowColor,&border,
        XmNhighlightColor,&accent,NULL);
    XtVaGetValues(XtParent(w),XmNbackground,&parent,NULL);
    CHECK(shadow==(Dimension)(scale+0.5));
    CHECK(XGetPixel(im,highlight,highlight)==parent);
    if(XGetPixel(im,XtWidth(w)/2,highlight)!=(is_default?accent:border))
        fprintf(stderr,"button gadget=%d pressed=%d default=%d scale=%g highlight=%u actual=%lu expected=%lu\n",!XtIsWidget(w),pressed,is_default,scale,highlight,XGetPixel(im,XtWidth(w)/2,highlight),is_default?accent:border);
    CHECK(XGetPixel(im,XtWidth(w)/2,highlight)==(is_default?accent:border));
    CHECK(XGetPixel(im,highlight+(int)(8*scale),XtHeight(w)/2)==(pressed?arm:background));
    XDestroyImage(im);
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
    /* Check the curved band against concentric circles at pixel centers.
     * A rectangular inner clip can be symmetric and connected yet leave
     * only one pixel of stroke at the diagonals. */
    {
        double scale = 1; CHECK(XmGetWidgetScale(w, &scale));
        int radius = (int)(4*scale+0.5) + highlight;
        if (radius > im->width/2) radius = im->width/2;
        if (radius > im->height/2) radius = im->height/2;
        int inner_radius = radius > highlight ? radius-highlight : 0;
        for (int y=0;y<radius;++y) for (int x=0;x<radius;++x) {
            double dx=radius-x-0.5, dy=radius-y-0.5;
            double distance=dx*dx+dy*dy;
            if (distance <= inner_radius*inner_radius) continue;
            Pixel expected=distance <= radius*radius ? accent : parent;
            CHECK(XGetPixel(im,x,y)==expected);
            CHECK(XGetPixel(im,im->width-1-x,y)==expected);
            CHECK(XGetPixel(im,x,im->height-1-y)==expected);
            CHECK(XGetPixel(im,im->width-1-x,im->height-1-y)==expected);
        }
    }

    for(int y=0;y<im->height;++y) for(int x=0;x<im->width;++x)
        if(x<highlight || y<highlight || x>=im->width-highlight || y>=im->height-highlight) {
            CHECK(XGetPixel(im,x,y)==XGetPixel(im,im->width-1-x,y));
            CHECK(XGetPixel(im,x,y)==XGetPixel(im,x,im->height-1-y));
        }
    /* Every focus pixel must belong to one 8-connected outline. Symmetry
     * alone also accepts four disconnected, identically clipped sides. */
    {
        size_t total = (size_t)im->width * im->height, count = 0, reached = 0;
        unsigned char *seen = calloc(total, 1);
        size_t *queue = malloc(total * sizeof(*queue));
        CHECK(seen && queue);
        for (int y = 0; y < im->height; ++y) for (int x = 0; x < im->width; ++x)
            if ((x < highlight || y < highlight || x >= im->width-highlight || y >= im->height-highlight) &&
                XGetPixel(im,x,y) == accent) {
                if (!count) { queue[0] = (size_t)y*im->width+x; seen[queue[0]] = 1; reached = 1; }
                ++count;
            }
        for (size_t head = 0; head < reached; ++head) {
            int x = queue[head] % im->width, y = queue[head] / im->width;
            for (int dy = -1; dy <= 1; ++dy) for (int dx = -1; dx <= 1; ++dx) {
                int nx = x+dx, ny = y+dy;
                if (nx < 0 || ny < 0 || nx >= im->width || ny >= im->height) continue;
                size_t index = (size_t)ny*im->width+nx;
                if (!seen[index] && (nx < highlight || ny < highlight || nx >= im->width-highlight || ny >= im->height-highlight) &&
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
static void input(Widget w,const char *action,Mask mask) {
    XEvent ev={0};Position rx,ry;
    ev.xbutton.type=mask==XmACTIVATE_EVENT?ButtonRelease:ButtonPress;
    ev.xbutton.display=XtDisplayOfObject(w);ev.xbutton.window=XtWindowOfObject(w);
    ev.xbutton.button=Button1;ev.xbutton.same_screen=True;
    ev.xbutton.x=(XtIsWidget(w)?0:XtX(w))+XtWidth(w)/2;
    ev.xbutton.y=(XtIsWidget(w)?0:XtY(w))+XtHeight(w)/2;
    XtTranslateCoords(XtIsWidget(w)?w:XtParent(w),ev.xbutton.x,ev.xbutton.y,&rx,&ry);
    ev.xbutton.x_root=rx;ev.xbutton.y_root=ry;
    if(mask==XmENTER_EVENT || mask==XmLEAVE_EVENT) {
        ev.xcrossing.type=mask==XmENTER_EVENT?EnterNotify:LeaveNotify;
        ev.xcrossing.mode=NotifyNormal;ev.xcrossing.detail=NotifyNonlinear;
    }
    if(XtIsWidget(w)) XtCallActionProc(w,action,&ev,NULL,0);
    else xmPushButtonGadgetClassRec.gadget_class.input_dispatch(w,&ev,mask);
}
static void custom_expose(Widget w,XtPointer client,XtPointer call) {
    XGCValues v;GC gc;
    (void)client;CHECK(call);
    XtVaGetValues(w,XmNhighlightColor,&v.foreground,NULL);
    gc=XCreateGC(XtDisplay(w),XtWindow(w),GCForeground,&v);
    XFillRectangle(XtDisplay(w),XtWindow(w),gc,XtWidth(w)/2-2,XtHeight(w)/2-2,5,5);
    XFreeGC(XtDisplay(w),gc);
}
static void drawn_face(Widget w,Boolean pressed) {
    XImage *im=capture(w);Dimension highlight;Pixel border,accent;
    XtVaGetValues(w,XmNhighlightThickness,&highlight,XmNbottomShadowColor,&border,XmNhighlightColor,&accent,NULL);
    CHECK(XGetPixel(im,XtWidth(w)/2,highlight)==(pressed?accent:border));
    if(XGetPixel(im,XtWidth(w)/2,XtHeight(w)/2)!=accent) fprintf(stderr,"drawn pressed=%d pixel=%lu expected=%lu\n",pressed,XGetPixel(im,XtWidth(w)/2,XtHeight(w)/2),accent);
    CHECK(XGetPixel(im,XtWidth(w)/2,XtHeight(w)/2)==accent);
    XDestroyImage(im);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;
    const double scales[]={1,1.5,2};
    const unsigned char roles[]={XmAPPEARANCE_ROLE_NORMAL,XmAPPEARANCE_ROLE_PRIMARY,XmAPPEARANCE_ROLE_DESTRUCTIVE};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"buttonAppearance","ButtonAppearance",NULL,0,&argc,argv);CHECK(d);
    for(int scale=0;scale<3;++scale) for(int dark=0;dark<2;++dark)
    for(int compact=0;compact<2;++compact) for(int gadget=0;gadget<2;++gadget)
    for(int role=0;role<3;++role) {
        Widget shell,board,w;XmString text=XmStringCreateLocalized("Save");
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmSetAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("buttons","ButtonAppearance",applicationShellWidgetClass,d,NULL,0);
        board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,600,XmNheight,300,NULL);
        w=XtVaCreateManagedWidget("button",gadget?xmPushButtonGadgetClass:xmPushButtonWidgetClass,board,
            XmNx,20,XmNy,20,XmNwidth,(int)(160*scales[scale]),XmNheight,(int)(56*scales[scale]),
            XmNlabelString,text,XmNappearanceRole,roles[role],NULL);
        if(!gadget && role==0) {
            Widget natural=XtVaCreateManagedWidget("natural",xmPushButtonWidgetClass,board,
                XmNx,20,XmNy,180,XmNlabelString,text,NULL);
            XmFontList font;Dimension tw,th,mw,mh,highlight,shadow;
            XtVaGetValues(natural,XmNfontList,&font,XmNmarginWidth,&mw,XmNmarginHeight,&mh,
                XmNhighlightThickness,&highlight,XmNshadowThickness,&shadow,NULL);
            XmStringExtent(font,text,&tw,&th);
            CHECK(XtWidth(natural)>=tw+2*(mw+highlight+shadow));
            CHECK(XtHeight(natural)>=th+2*(mh+highlight+shadow));
        }
        CHECK(XtWidth(w)==(Dimension)(160*scales[scale]));
        CHECK(XtHeight(w)==(Dimension)(56*scales[scale]));
        XmStringFree(text);XtAddCallback(w,XmNactivateCallback,activated,NULL);
        XtRealizeWidget(shell);drain(app,d);face(w,False,False,scales[scale]);
        activations=0;input(w,"Arm",XmARM_EVENT);drain(app,d);face(w,True,False,scales[scale]);
        input(w,"Leave",XmLEAVE_EVENT);drain(app,d);face(w,False,False,scales[scale]);
        XClearArea(d,XtWindowOfObject(w),0,0,0,0,True);drain(app,d);face(w,False,False,scales[scale]);
        input(w,"Enter",XmENTER_EVENT);drain(app,d);face(w,True,False,scales[scale]);
        input(w,"Activate",XmACTIVATE_EVENT);
        if(!gadget) input(w,"Disarm",0);
        drain(app,d);CHECK(activations==1);face(w,False,False,scales[scale]);
        for(unsigned ancestor=0;ancestor<2;++ancestor) {
            input(w,"Arm",XmARM_EVENT);
            XtSetSensitive(ancestor?board:w,False);
            XtSetSensitive(ancestor?board:w,True);
            input(w,"Activate",XmACTIVATE_EVENT);
            if(!gadget)input(w,"Disarm",0);
            drain(app,d);CHECK(activations==1);face(w,False,False,scales[scale]);
        }
        focus_face(w);
        XtVaSetValues(w,XmNshowAsDefault,1,NULL);drain(app,d);face(w,False,True,scales[scale]);
        XtVaSetValues(w,XmNshowAsDefault,0,NULL);drain(app,d);face(w,False,False,scales[scale]);
        XtSetSensitive(w,False);drain(app,d);face(w,False,False,scales[scale]);
        XtSetSensitive(w,True);drain(app,d);
        for(int legacy=0;legacy<2;++legacy) {
            XImage *before,*after;
            CHECK(XmApplyAppearanceProfile(d,legacy?XmAPPEARANCE_HIGH_CONTRAST:XmAPPEARANCE_CLASSIC));drain(app,d);
            input(w,"Arm",XmARM_EVENT);input(w,"Leave",XmLEAVE_EVENT);drain(app,d);
            before=capture(w);XClearArea(d,XtWindowOfObject(w),0,0,0,0,True);drain(app,d);after=capture(w);
            CHECK(before->width==after->width && before->height==after->height);
            for(int y=0;y<before->height;++y) for(int x=0;x<before->width;++x)
                CHECK(XGetPixel(before,x,y)==XGetPixel(after,x,y));
            XDestroyImage(before);XDestroyImage(after);
            input(w,"Enter",XmENTER_EVENT);input(w,"Activate",XmACTIVATE_EVENT);
            if(!gadget)input(w,"Disarm",0);
            drain(app,d);
        }

        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));drain(app,d);face(w,False,False,scales[scale]);
        if(!gadget && role==0) for(int enabled=0;enabled<2;++enabled) {
            Widget drawn=XtVaCreateManagedWidget("drawn",xmDrawnButtonWidgetClass,board,
                XmNx,400,XmNy,40,XmNwidth,160,XmNheight,80,XmNpushButtonEnabled,enabled,NULL);
            CHECK(XtWidth(drawn)==160 && XtHeight(drawn)==80);
            XtAddCallback(drawn,XmNexposeCallback,custom_expose,NULL);
            XClearArea(d,XtWindow(drawn),0,0,0,0,True);drain(app,d);drawn_face(drawn,False);focus_face(drawn);
            if(enabled) {
                input(drawn,"Arm",XmARM_EVENT);drain(app,d);drawn_face(drawn,True);
                input(drawn,"Leave",XmLEAVE_EVENT);drain(app,d);drawn_face(drawn,False);
                XClearArea(d,XtWindow(drawn),0,0,0,0,True);drain(app,d);drawn_face(drawn,False);
                input(drawn,"Enter",XmENTER_EVENT);drain(app,d);drawn_face(drawn,True);
                input(drawn,"Activate",XmACTIVATE_EVENT);input(drawn,"Disarm",0);drain(app,d);
                XClearArea(d,XtWindow(drawn),0,0,0,0,True);drain(app,d);drawn_face(drawn,False);
                XtVaSetValues(drawn,XmNshadowThickness,0,NULL);drain(app,d);
                input(drawn,"Arm",XmARM_EVENT);drain(app,d);
                {
                    XImage *im=capture(drawn);Pixel background;Dimension highlight;
                    XtVaGetValues(drawn,XmNbackground,&background,XmNhighlightThickness,&highlight,NULL);
                    CHECK(XGetPixel(im,XtWidth(drawn)/2,highlight)==background);XDestroyImage(im);
                }
                input(drawn,"Disarm",0);
                XtVaSetValues(drawn,XmNshadowThickness,3,NULL);drain(app,d);drawn_face(drawn,False);
            }
            XtDestroyWidget(drawn);drain(app,d);
        }
        if (!gadget && role == 0) {
            XmString caption = XmStringCreateLocalized("Open document");
            const char bits[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
            Pixmap bitmap = XCreateBitmapFromData(d,RootWindowOfScreen(XtScreen(board)),bits,8,8);
            Widget icon = XtVaCreateManagedWidget("icon", xmIconButtonWidgetClass, board,
                XmNx,20, XmNy,130, XmNlabelString,caption,XmNpixmap,bitmap,NULL);
            Dimension horizontal,vertical,gap;
            XtVaGetValues(icon, XmNhorizontalMargin,&horizontal,
                XmNverticalMargin,&vertical, XmNiconTextPadding,&gap,NULL);
            CHECK(horizontal == (Dimension)((compact ? 8 : 12)*scales[scale]+0.5));
            CHECK(vertical == (Dimension)((compact ? 3 : 6)*scales[scale]+0.5));
            CHECK(gap == (Dimension)((compact ? 6 : 8)*scales[scale]+0.5));
            drain(app,d);face(icon,False,False,scales[scale]);focus_face(icon);
            {
                XmIconButtonWidget iw = (XmIconButtonWidget)icon;
                Pixel foreground; XImage *im=capture(icon);
                XtVaGetValues(icon,XmNforeground,&foreground,NULL);
                CHECK(XGetPixel(im,XmIconButton_pix_x(iw)+4,XmIconButton_pix_y(iw)+4)==foreground);
                XDestroyImage(im);
            }
            activations=0;XtAddCallback(icon,XmNactivateCallback,activated,NULL);
            {
                XEvent key = {0};key.xkey.type=KeyPress;key.xkey.display=d;
                key.xkey.window=XtWindow(icon);
                XtCallActionProc(icon,"XiArmAndActivate",&key,NULL,0);
            }
            drain(app,d);
            CHECK(activations==1);
            XtVaSetValues(icon,XmNset,True,NULL);drain(app,d);
            face(icon,True,False,scales[scale]);focus_face(icon);
            XtSetSensitive(icon,False);drain(app,d);
            face(icon,True,False,scales[scale]);
            XtSetSensitive(icon,True);
            XtVaSetValues(icon,XmNset,False,XmNhorizontalMargin,17,NULL);
            CHECK(XmApplyAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMFORTABLE:XmAPPEARANCE_DENSITY_COMPACT));
            XtVaGetValues(icon,XmNhorizontalMargin,&horizontal,NULL);
            CHECK(horizontal==17);
            CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);
            XtVaGetValues(icon,XmNverticalMargin,&vertical,XmNiconTextPadding,&gap,NULL);
            CHECK(vertical==2 && gap==2);
            CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
            drain(app,d);face(icon,False,False,scales[scale]);focus_face(icon);
            /* An oversized image must not overwrite the rounded frame,
             * including when an application removes all content margins. */
            {
                char full[512];memset(full,255,sizeof(full));
                Pixmap oversized=XCreateBitmapFromData(d,RootWindowOfScreen(XtScreen(board)),full,64,64);
                Widget small=XtVaCreateManagedWidget("smallIcon",xmIconButtonWidgetClass,board,
                    XmNx,420,XmNy,210,XmNwidth,31,XmNheight,29,
                    XmNrecomputeSize,False,XmNhorizontalMargin,0,XmNverticalMargin,0,
                    XmNiconPlacement,XmIconOnly,NULL);
                Pixel border;Dimension highlight;
                XtVaGetValues(small,XmNbottomShadowColor,&border,XmNhighlightThickness,&highlight,NULL);
                drain(app,d);XImage *before=capture(small);
                XtVaSetValues(small,XmNpixmap,oversized,XmNpixmapWidth,64,
                    XmNpixmapHeight,64,XmNpixmapDepth,1,NULL);
                drain(app,d);XImage *after=capture(small);
                CHECK(before->width==after->width && before->height==after->height);
                unsigned border_pixels=0;
                for(int y=0;y<before->height;++y)for(int x=0;x<before->width;++x)
                    if(XGetPixel(before,x,y)==border) {
                        CHECK(XGetPixel(after,x,y)==border);++border_pixels;
                    }
                CHECK(border_pixels>0);
                CHECK(XGetPixel(before,highlight,highlight)==XGetPixel(after,highlight,highlight));
                XDestroyImage(before);XDestroyImage(after);
                focus_face(small);
                XtDestroyWidget(small);XFreePixmap(d,oversized);
            }
            XmStringFree(caption);
            XtDestroyWidget(icon);XFreePixmap(d,bitmap);
        }
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    printf("PASS modern buttons: %u cases\n",cases);XtCloseDisplay(d);XtDestroyApplicationContext(app);return 0;
}
