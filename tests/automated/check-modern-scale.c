/* Scale-specific paint and the underlying value/interaction contract. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/ScrollBarP.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static unsigned cases,callbacks;
static int observer_token;
static void observed(Widget w,XtPointer closure,XtPointer call) {
    CHECK(XmIsScrollBar(w));CHECK(closure==&observer_token);CHECK(call);
}

static void changed(Widget w,XtPointer c,XtPointer v) {(void)w;(void)c;CHECK(v);++callbacks;}
static unsigned mutation_first,mutation_second,mutation_third;
static int mutation_tokens[3];
static void mutation_last(Widget w,XtPointer c,XtPointer v) {
    (void)w;CHECK(c==&mutation_tokens[2]);CHECK(v);++mutation_third;
}
static void mutation_next(Widget w,XtPointer c,XtPointer v) {
    (void)w;CHECK(c==&mutation_tokens[1]);CHECK(v);++mutation_second;
}
static void mutation_start(Widget w,XtPointer c,XtPointer v) {
    CHECK(c==&mutation_tokens[0]);CHECK(v);++mutation_first;
    if(mutation_first==1) {
        XtRemoveCallback(w,XmNvalueChangedCallback,mutation_next,&mutation_tokens[1]);
        XtAddCallback(w,XmNvalueChangedCallback,mutation_last,&mutation_tokens[2]);
    }
}
static void drain(XtAppContext a,Display *d) {XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);XSync(d,False);}
static Widget scrollbar(Widget scale) {WidgetList children;Cardinal count;XtVaGetValues(scale,XmNchildren,&children,XmNnumChildren,&count,NULL);for(Cardinal i=0;i<count;++i)if(XmIsScrollBar(children[i]))return children[i];return NULL;}
static void pixels(Widget w,Boolean reverse,Boolean horizontal) {
    XmScrollBarWidget s=(XmScrollBarWidget)w;
    int x=s->scrollBar.slider_area_x,y=s->scrollBar.slider_area_y;
    int width=s->scrollBar.slider_area_width,height=s->scrollBar.slider_area_height;
    int sw=s->scrollBar.slider_width,sh=s->scrollBar.slider_height;
    int cx=s->scrollBar.slider_x+sw/2,cy=s->scrollBar.slider_y+sh/2;
    int first=horizontal?x+sw/2:y+sh/2,last=horizontal?x+width-sw+sw/2:y+height-sh+sh/2;
    XImage *im=XGetImage(XtDisplay(w),XtWindow(w),0,0,XtWidth(w),XtHeight(w),AllPlanes,ZPixmap);CHECK(im);
    Pixel accent=XtIsSensitive(w)?s->primitive.highlight_color:s->primitive.top_shadow_color;
    int mid=horizontal?cx:cy,a=(first+mid)/2,b=(last+mid)/2;
    CHECK(XGetPixel(im,horizontal?a:x+width/2,horizontal?y+height/2:a)==(reverse?s->primitive.top_shadow_color:accent));
    CHECK(XGetPixel(im,horizontal?b:x+width/2,horizontal?y+height/2:b)==(reverse?accent:s->primitive.top_shadow_color));
    CHECK(XGetPixel(im,cx,cy)==s->core.background_pixel);
    CHECK(XGetPixel(im,x,y)==s->scrollBar.trough_color);
    XDestroyImage(im);++cases;
}
static Pixel center_pixel(Widget w) {
    XmScrollBarWidget s=(XmScrollBarWidget)w;
    XSync(XtDisplay(w),False);
    XImage *im=XGetImage(XtDisplay(w),XtWindow(w),
        s->scrollBar.slider_x+s->scrollBar.slider_width/2,
        s->scrollBar.slider_y+s->scrollBar.slider_height/2,1,1,AllPlanes,ZPixmap);
    CHECK(im);Pixel result=XGetPixel(im,0,0);XDestroyImage(im);return result;
}
static void pointer(Widget w,const char *action,int type,int x,int y) {
    XEvent e={0};Position rx,ry;XtTranslateCoords(w,x,y,&rx,&ry);
    e.xbutton.type=type;e.xbutton.display=XtDisplay(w);e.xbutton.window=XtWindow(w);
    e.xbutton.button=Button1;e.xbutton.x=x;e.xbutton.y=y;e.xbutton.x_root=rx;e.xbutton.y_root=ry;e.xbutton.same_screen=True;
    XtCallActionProc(w,action,&e,NULL,0);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;double scales[]={1,1.5,2};
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"scaleVisual","ScaleVisual",NULL,0,&argc,argv);CHECK(d);
    for(int scale=0;scale<3;++scale)for(int dark=0;dark<2;++dark)for(int horizontal=0;horizontal<2;++horizontal)for(int reverse=0;reverse<2;++reverse) {
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));
        Widget shell=XtAppCreateShell("scaleVisual","ScaleVisual",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,600,XmNheight,600,NULL);
        Widget scale_w=XtVaCreateManagedWidget("scale",xmScaleWidgetClass,board,XmNminimum,0,XmNmaximum,100,XmNvalue,50,
            XmNorientation,horizontal?XmHORIZONTAL:XmVERTICAL,
            XmNprocessingDirection,horizontal?(reverse?XmMAX_ON_LEFT:XmMAX_ON_RIGHT):(reverse?XmMAX_ON_TOP:XmMAX_ON_BOTTOM),
            XmNscaleWidth,horizontal?300:40,XmNscaleHeight,horizontal?40:300,NULL);
        Widget natural=XtVaCreateManagedWidget("natural",xmScaleWidgetClass,board,XmNx,320,NULL);
        Widget explicit_w=XtVaCreateManagedWidget("explicit",xmScaleWidgetClass,board,XmNx,350,XmNsliderMark,XmROUND_MARK,XmNshadowThickness,3,NULL);
        Widget thermo=XtVaCreateManagedWidget("thermo",xmScaleWidgetClass,board,XmNx,450,XmNslidingMode,XmTHERMOMETER,XmNeditable,True,NULL);
        unsigned char classic_mark,thermo_mark,mark;Dimension shadow;
        XtVaGetValues(scale_w,XmNsliderMark,&classic_mark,NULL);XtVaGetValues(thermo,XmNsliderMark,&thermo_mark,NULL);
        XtAddCallback(scale_w,XmNvalueChangedCallback,changed,NULL);XtRealizeWidget(shell);drain(app,d);
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));drain(app,d);
        Widget w=scrollbar(scale_w);CHECK(w);XmScrollBarWidget s=(XmScrollBarWidget)w;
        /* Scale already installs its value callback; accessibility adds another
         * observer in real applications. Exercise that compiled-list shape. */
        XtAddCallback(w,XmNvalueChangedCallback,observed,&observer_token);

        XtVaGetValues(scale_w,XmNsliderMark,&mark,XmNshadowThickness,&shadow,NULL);CHECK(mark==XmNONE && shadow==0);
        XtVaGetValues(explicit_w,XmNsliderMark,&mark,XmNshadowThickness,&shadow,NULL);CHECK(mark==XmROUND_MARK && shadow==3);
        XtVaGetValues(thermo,XmNsliderMark,&mark,NULL);CHECK(mark==thermo_mark);
        Widget fresh=XtVaCreateManagedWidget("fresh",xmScaleWidgetClass,board,XmNx,300,NULL);
        drain(app,d);
        CHECK(XtWidth(natural)==XtWidth(fresh) && XtHeight(natural)==XtHeight(fresh));
        Widget natural_bar=scrollbar(natural);CHECK(natural_bar);
        CHECK(XtWidth(natural_bar)>=(Dimension)(24*scales[scale]));
        CHECK(s->scrollBar.slider_mark==XmNONE);pixels(w,reverse,horizontal);
        for(int v=20;v<=80;v+=60){XmScaleSetValue(scale_w,v);drain(app,d);pixels(w,reverse,horizontal);}
        XmScaleSetValue(scale_w,50);drain(app,d);
        int old,value;XmScaleGetValue(scale_w,&old);callbacks=0;
        int cx=s->scrollBar.slider_x+s->scrollBar.slider_width/2,cy=s->scrollBar.slider_y+s->scrollBar.slider_height/2;
        pointer(w,"Select",ButtonPress,cx,cy);
        CHECK(s->scrollBar.sliding_on && center_pixel(w)==s->primitive.highlight_color);
        XClearArea(d,XtWindow(w),0,0,0,0,True);drain(app,d);
        CHECK(center_pixel(w)==s->primitive.highlight_color);
        pointer(w,"Moved",MotionNotify,cx+(horizontal?20:0),cy+(horizontal?0:20));
        CHECK(center_pixel(w)==s->primitive.highlight_color);
        CHECK(horizontal?s->scrollBar.slider_x+s->scrollBar.slider_width/2==cx+20:
                         s->scrollBar.slider_y+s->scrollBar.slider_height/2==cy+20);
        /* Both movement directions must paint before release/expose can repair it. */
        pointer(w,"Moved",MotionNotify,cx-(horizontal?20:0),cy-(horizontal?0:20));
        CHECK(center_pixel(w)==s->primitive.highlight_color);
        CHECK(horizontal?s->scrollBar.slider_x+s->scrollBar.slider_width/2==cx-20:
                         s->scrollBar.slider_y+s->scrollBar.slider_height/2==cy-20);
        pointer(w,"Release",ButtonRelease,cx-(horizontal?20:0),cy-(horizontal?0:20));drain(app,d);
        XmScaleGetValue(scale_w,&value);CHECK(value!=old && callbacks==1);pixels(w,reverse,horizontal);
        XEvent key={0};String parameter=horizontal?"left":"up";key.xkey.type=KeyPress;key.xkey.display=d;key.xkey.window=XtWindow(w);
        old=value;XtCallActionProc(w,"IncrementUpOrLeft",&key,&parameter,1);drain(app,d);XmScaleGetValue(scale_w,&value);CHECK(value!=old);pixels(w,reverse,horizontal);
        XSetInputFocus(d,XtWindow(shell),RevertToParent,CurrentTime);drain(app,d);
        CHECK(XmProcessTraversal(w,XmTRAVERSE_CURRENT));drain(app,d);
        CHECK(s->primitive.highlighted);
        XImage *focused=XGetImage(d,XtWindow(w),0,0,1,1,AllPlanes,ZPixmap);CHECK(focused);
        CHECK(XGetPixel(focused,0,0)==s->primitive.highlight_color);XDestroyImage(focused);
        pixels(w,reverse,horizontal);
        XtSetSensitive(scale_w,False);drain(app,d);pixels(w,reverse,horizontal);
        XtSetSensitive(scale_w,True);drain(app,d);
        cx=s->scrollBar.slider_x+s->scrollBar.slider_width/2;
        cy=s->scrollBar.slider_y+s->scrollBar.slider_height/2;
        pointer(w,"Select",ButtonPress,cx,cy);CHECK(s->scrollBar.sliding_on);
        XtSetSensitive(scale_w,False);drain(app,d);
        CHECK(!s->scrollBar.sliding_on && !(s->scrollBar.flags & KEYBOARD_GRABBED));
        XtSetSensitive(scale_w,True);drain(app,d);pixels(w,reverse,horizontal);
        callbacks=0;pointer(w,"Release",ButtonRelease,cx,cy);CHECK(callbacks==0);
        CHECK(!(s->scrollBar.add_flags & INTERACTION_ACTIVE));
        /* Read-only changes must also end an armed drag without a late callback. */
        pointer(w,"Select",ButtonPress,cx,cy);CHECK(s->scrollBar.sliding_on);
        XtVaSetValues(scale_w,XmNeditable,False,NULL);drain(app,d);
        CHECK(!s->scrollBar.sliding_on && !s->scrollBar.timer);
        CHECK(!(s->scrollBar.add_flags & INTERACTION_ACTIVE));
        XtVaSetValues(scale_w,XmNeditable,True,NULL);drain(app,d);
        callbacks=0;pointer(w,"Release",ButtonRelease,cx,cy);CHECK(callbacks==0);
        /* Escape restores the start value, even if no keyboard grab is held. */
        XmScaleGetValue(scale_w,&old);
        pointer(w,"Select",ButtonPress,cx,cy);
        XtUngrabKeyboard(w,CurrentTime);s->scrollBar.flags &= ~KEYBOARD_GRABBED;
        pointer(w,"Moved",MotionNotify,cx+(horizontal?20:0),cy+(horizontal?0:20));
        pointer(w,"CancelDrag",KeyPress,cx,cy);drain(app,d);
        XmScaleGetValue(scale_w,&value);CHECK(value==old);
        CHECK(!s->scrollBar.sliding_on && !(s->scrollBar.add_flags & INTERACTION_ACTIVE));
        pixels(w,reverse,horizontal);
        callbacks=0;pointer(w,"Release",ButtonRelease,cx,cy);CHECK(callbacks==0);
        /* Native tick gadgets remain within the allocated scale. */
        XmScaleSetTicks(scale_w,25,1,0,8,4,0);drain(app,d);
        WidgetList tick_children;Cardinal tick_count;
        XtVaGetValues(scale_w,XmNchildren,&tick_children,XmNnumChildren,&tick_count,NULL);
        unsigned ticks=0;int tick_first=100000,tick_last=-1;
        for(Cardinal k=0;k<tick_count;++k) if(XmIsSeparatorGadget(tick_children[k])) {
            Widget tick=tick_children[k];++ticks;
            int middle=horizontal?XtX(tick)+XtWidth(tick)/2:XtY(tick)+XtHeight(tick)/2;
            if(middle<tick_first)tick_first=middle;
            if(middle>tick_last)tick_last=middle;

            CHECK(XtX(tick)>=0 && XtY(tick)>=0);
            CHECK(XtX(tick)+XtWidth(tick)<=XtWidth(scale_w));
            CHECK(XtY(tick)+XtHeight(tick)<=XtHeight(scale_w));
        }
        int lead=horizontal?XtX(w)+s->scrollBar.slider_area_x+s->scrollBar.slider_width/2:
                            XtY(w)+s->scrollBar.slider_area_y+s->scrollBar.slider_height/2;
        int trail=horizontal?XtX(w)+s->scrollBar.slider_area_x+s->scrollBar.slider_area_width-s->scrollBar.slider_width+s->scrollBar.slider_width/2:
                             XtY(w)+s->scrollBar.slider_area_y+s->scrollBar.slider_area_height-s->scrollBar.slider_height+s->scrollBar.slider_height/2;
        if(abs(tick_first-lead)>1 || abs(tick_last-trail)>1)
            fprintf(stderr,"ticks scale=%g horizontal=%d actual=%d,%d expected=%d,%d\n",scales[scale],horizontal,tick_first,tick_last,lead,trail);
        CHECK(abs(tick_first-lead)<=1 && abs(tick_last-trail)<=1);
        CHECK(ticks==9);pixels(w,reverse,horizontal);

        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);XtVaGetValues(scale_w,XmNsliderMark,&mark,NULL);CHECK(mark==classic_mark);
        XtDestroyWidget(shell);drain(app,d);
    }
    CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
    CHECK(XmSetDisplayScaleOverride(d,1.5));
    Widget dpi_shell=XtAppCreateShell("scaleDpi","ScaleDpi",applicationShellWidgetClass,d,NULL,0);
    Widget dpi_board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,dpi_shell,XmNwidth,600,XmNheight,600,NULL);
    XFontStruct *font=XLoadQueryFont(d,"fixed");CHECK(font);
    XmFontList fonts=XmFontListCreate(font,XmFONTLIST_DEFAULT_TAG);CHECK(fonts);
    Widget dpi_scale=XtVaCreateManagedWidget("natural",xmScaleWidgetClass,dpi_board,XmNfontList,fonts,NULL);
    Widget fixed_scale=XtVaCreateManagedWidget("fixed",xmScaleWidgetClass,dpi_board,XmNx,300,XmNfontList,fonts,
        XmNwidth,117,XmNheight,231,XmNscaleWidth,27,XmNscaleHeight,173,NULL);
    Widget mutation=XtVaCreateManagedWidget("mutation",xmScrollBarWidgetClass,dpi_board,
        XmNx,450,XmNvalue,0,XmNmaximum,100,XmNsliderSize,10,NULL);
    XtAddCallback(mutation,XmNvalueChangedCallback,mutation_start,&mutation_tokens[0]);
    XtAddCallback(mutation,XmNvalueChangedCallback,mutation_next,&mutation_tokens[1]);
    XmFontListFree(fonts);XtRealizeWidget(dpi_shell);drain(app,d);
    XmScrollBarSetValues(mutation,10,10,1,10,True);
    CHECK(mutation_first==1 && mutation_second==1 && mutation_third==0);
    XmScrollBarSetValues(mutation,20,10,1,10,True);
    CHECK(mutation_first==2 && mutation_second==1 && mutation_third==1);

    CHECK(XmSetDisplayScaleOverride(d,2));drain(app,d);
    CHECK(XtWidth(scrollbar(dpi_scale))>=48);
    CHECK(XtWidth(fixed_scale)==117 && XtHeight(fixed_scale)==231);
    for(unsigned i=0;i<3;++i) {
        double factor=(double[]){1,1.5,2}[i];
        CHECK(XmSetDisplayScaleOverride(d,factor));drain(app,d);
        CHECK(XtWidth(scrollbar(dpi_scale))>=(Dimension)(24*factor));
        CHECK(XtWidth(fixed_scale)==117 && XtHeight(fixed_scale)==231);
    }
    Dimension locked_width=XtWidth(dpi_scale);
    XtVaSetValues(dpi_scale,XmNwidth,locked_width,NULL);
    CHECK(XmSetDisplayScaleOverride(d,1));drain(app,d);CHECK(XtWidth(dpi_scale)==locked_width);
    for(unsigned size=1;size<=16;++size) {
        XtResizeWidget(dpi_scale,size,size,0);drain(app,d);
        Widget tiny_bar=scrollbar(dpi_scale);CHECK(tiny_bar);
        XClearArea(d,XtWindow(tiny_bar),0,0,0,0,True);drain(app,d);
        XmScaleSetValue(dpi_scale,size*5);drain(app,d);
        ++cases;
    }
    XtDestroyWidget(dpi_shell);drain(app,d);XFreeFont(d,font);
    XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS modern scale: %u paint and interaction cases\n",cases);return 0;
}
