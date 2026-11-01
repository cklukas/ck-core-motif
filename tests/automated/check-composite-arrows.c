/* Inspect actual composite arrow regions across layout, scale and states. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/ComboBoxP.h>
#include <Xm/SpinBP.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static unsigned cases;
static void drain(XtAppContext app,Display *d) {
    XSync(d,False);
    while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll);
    XSync(d,False);
}
static void paint(Widget w) {
    XtClass(w)->core_class.expose(w,NULL,NULL);
    XSync(XtDisplay(w),False);
}
static void inspect(Widget w,XRectangle rect,Boolean pressed,Boolean enabled) {
    Pixel bg,fg,border,fill,glyph;unsigned ink=0;
    CHECK(rect.width && rect.height && rect.x>=0 && rect.y>=0);
    CHECK(rect.x+rect.width<=XtWidth(w) && rect.y+rect.height<=XtHeight(w));
    XtVaGetValues(w,XmNbackground,&bg,XmNforeground,&fg,XmNbottomShadowColor,&border,NULL);
    pressed=pressed&&enabled;fill=pressed?fg:bg;glyph=enabled?(pressed?bg:fg):border;
    XImage *im=XGetImage(XtDisplay(w),XtWindow(w),rect.x,rect.y,
                        rect.width,rect.height,AllPlanes,ZPixmap);CHECK(im);
    for(int y=0;y<rect.height;++y) for(int x=0;x<rect.width;++x) {
        Pixel p=XGetPixel(im,x,y);
        CHECK(p==fill || p==glyph || p==bg);
        if(!pressed && (x==0 || y==0 || x==rect.width-1 || y==rect.height-1)) CHECK(p==bg);
        if(p==glyph) ++ink;
    }
    CHECK(ink>0);
    if(!pressed) CHECK(ink<(unsigned)(rect.width*rect.height/2));
    XDestroyImage(im);++cases;
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;const double scales[]={1,1.5,2};
    const unsigned char layouts[]={XmARROWS_END,XmARROWS_BEGINNING,XmARROWS_SPLIT,XmARROWS_FLAT_END,XmARROWS_FLAT_BEGINNING};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"compositeArrows","CompositeArrows",NULL,0,&argc,argv);CHECK(d);
    for(unsigned dark=0;dark<2;++dark) for(unsigned scale=0;scale<3;++scale) {
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        for(unsigned horizontal=0;horizontal<2;++horizontal) for(unsigned layout=0;layout<5;++layout) {
            Widget shell=XtAppCreateShell("compositeArrows","CompositeArrows",applicationShellWidgetClass,d,NULL,0);
            Widget spin=XtVaCreateManagedWidget("spin",xmSimpleSpinBoxWidgetClass,shell,
                XmNspinBoxChildType,XmNUMERIC,XmNminimumValue,0,XmNmaximumValue,100,XmNposition,7,
                XmNarrowLayout,layouts[layout],XmNarrowOrientation,horizontal?XmARROWS_HORIZONTAL:XmARROWS_VERTICAL,NULL);
            XtRealizeWidget(shell);drain(app,d);
            XmSpinBoxWidget s=(XmSpinBoxWidget)spin;
            for(unsigned enabled=0;enabled<2;++enabled) for(unsigned pressed=0;pressed<2;++pressed) {
                XtSetSensitive(spin,enabled);drain(app,d);
                s->spinBox.up_arrow_pressed=s->spinBox.down_arrow_pressed=pressed;
                paint(spin);
                inspect(spin,s->spinBox.up_arrow_rect,pressed,enabled);
                inspect(spin,s->spinBox.down_arrow_rect,pressed,enabled);
            }
            s->spinBox.up_arrow_pressed=s->spinBox.down_arrow_pressed=False;
            XtSetSensitive(spin,True);drain(app,d);
            Widget field;int before,after;
            XtVaGetValues(spin,XmNtextField,&field,NULL);
            XtVaGetValues(field,XmNposition,&before,NULL);
            XEvent event={0};Position rx,ry;
            event.xbutton.type=ButtonPress;event.xbutton.display=d;
            event.xbutton.window=XtWindow(spin);event.xbutton.button=Button1;
            event.xbutton.x=s->spinBox.up_arrow_rect.x+s->spinBox.up_arrow_rect.width/2;
            event.xbutton.y=s->spinBox.up_arrow_rect.y+s->spinBox.up_arrow_rect.height/2;
            XtTranslateCoords(spin,event.xbutton.x,event.xbutton.y,&rx,&ry);
            event.xbutton.x_root=rx;event.xbutton.y_root=ry;
            event.xbutton.same_screen=True;
            XtCallActionProc(spin,"SpinBArm",&event,NULL,0);
            CHECK(s->spinBox.up_arrow_pressed);
            inspect(spin,s->spinBox.up_arrow_rect,True,True);
            event.xbutton.type=ButtonRelease;
            XtCallActionProc(spin,"SpinBDisarm",&event,NULL,0);
            CHECK(!s->spinBox.up_arrow_pressed);
            XtVaGetValues(field,XmNposition,&after,NULL);CHECK(after==before+1);
            inspect(spin,s->spinBox.up_arrow_rect,False,True);
            XtVaSetValues(field,XmNarrowSensitivity,XmARROWS_INCREMENT_SENSITIVE,NULL);
            paint(spin);
            inspect(spin,s->spinBox.up_arrow_rect,False,True);
            inspect(spin,s->spinBox.down_arrow_rect,False,False);
            XtDestroyWidget(shell);drain(app,d);
        }
        for(unsigned editable=0;editable<2;++editable) for(unsigned rtl=0;rtl<2;++rtl) {
            Widget shell=XtAppCreateShell("compositeArrows","CompositeArrows",applicationShellWidgetClass,d,NULL,0);
            XmString item=XmStringCreateLocalized("Selected item");
            Widget combo=XtVaCreateManagedWidget("combo",xmComboBoxWidgetClass,shell,
                XmNcomboBoxType,editable?XmDROP_DOWN_COMBO_BOX:XmDROP_DOWN_LIST,
                XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,
                XmNitems,&item,XmNitemCount,1,NULL);XmStringFree(item);
            XtRealizeWidget(shell);drain(app,d);
            for(unsigned enabled=0;enabled<2;++enabled) for(unsigned pressed=0;pressed<2;++pressed) {
                XtSetSensitive(combo,enabled);drain(app,d);
                CB_ArrowPressed(combo)=pressed;paint(combo);
                inspect(combo,CB_HitRect(combo),pressed,enabled);
            }
            CB_ArrowPressed(combo)=False;
            XtDestroyWidget(shell);drain(app,d);
        }
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS composite arrows: %u native region/state cases\n",cases);return 0;
}
