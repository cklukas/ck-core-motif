/* Native pixels and public state/callback behavior for modern toggle indicators. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <Xm/LabelP.h>
#include <Xm/LabelGP.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static unsigned callbacks;
static void changed(Widget w, XtPointer data, XtPointer call) {
    XmToggleButtonCallbackStruct *event = call;
    (void)w; (void)data;
    CHECK(event->reason == XmCR_VALUE_CHANGED);
    ++callbacks;
}
static void drain(XtAppContext app, Display *display) {
    XSync(display, False);
    while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
    XSync(display, False);
}
static unsigned long snapshot(Widget w, Pixel accent, unsigned *accent_count) {
    Position x=0,y=0; Dimension width,height;
    XImage *image;
    unsigned long hash=5381;
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,NULL);
    if (!XtIsWidget(w)) XtVaGetValues(w,XmNx,&x,XmNy,&y,NULL);
    image=XGetImage(XtDisplayOfObject(w),XtWindowOfObject(w),x,y,width,height,AllPlanes,ZPixmap);
    CHECK(image); *accent_count=0;
    for (unsigned iy=0;iy<height;++iy) for(unsigned ix=0;ix<width;++ix) {
        unsigned long pixel=XGetPixel(image,ix,iy);
        hash=hash*33+pixel;
        if(pixel==accent)++*accent_count;
    }
    XDestroyImage(image);return hash;
}
int main(int argc,char **argv) {
    XtAppContext app; Display *display; unsigned cases=0;
    const unsigned scales[]={100,150,200,300};
    XtToolkitInitialize(); app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"modernToggleTest","ModernToggleTest",NULL,0,&argc,argv);CHECK(display);
    for(unsigned dark=0;dark<2;++dark)
    for(unsigned compact=0;compact<2;++compact)
    for(unsigned scale_index=0;scale_index<4;++scale_index)
    for(unsigned gadget=0;gadget<2;++gadget)
    for(unsigned radio=0;radio<2;++radio)
    for(unsigned rtl=0;rtl<2;++rtl) {
        const unsigned scale=scales[scale_index];
        Widget shell,form,toggle,fixed; Pixel accent; unsigned off_count,on_count,mixed_count;
        unsigned long off,on,mixed; Dimension indicator, height;
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        CHECK(XmApplyAppearanceDensity(display,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmSetDisplayScaleOverride(display,scale/100.0));
        shell=XtAppCreateShell("modernToggleTest","ModernToggleTest",applicationShellWidgetClass,display,NULL,0);
        form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,XmNwidth,300*scale/100,XmNheight,70*scale/100,NULL);
        toggle=XtVaCreateManagedWidget("Choice",gadget?xmToggleButtonGadgetClass:xmToggleButtonWidgetClass,form,
            XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,
            XmNleftOffset,8*scale/100,XmNrightOffset,8*scale/100,XmNy,8*scale/100,
            XmNindicatorType,radio?XmONE_OF_MANY:XmN_OF_MANY,
            XmNtoggleMode,radio?XmTOGGLE_BOOLEAN:XmTOGGLE_INDETERMINATE,
            XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        fixed=XtVaCreateWidget("explicitCentered",gadget?xmToggleButtonGadgetClass:xmToggleButtonWidgetClass,
            form,XmNalignment,XmALIGNMENT_CENTER,NULL);
        XtAddCallback(toggle,XmNvalueChangedCallback,changed,NULL);
        XtRealizeWidget(shell);drain(app,display);
        XtVaGetValues(toggle,XmNselectColor,&accent,XmNindicatorSize,&indicator,XmNheight,&height,NULL);
        CHECK(indicator==(compact?16:18)*scale/100);
        CHECK(height >= (compact?28:34)*scale/100);
        {
            unsigned char alignment;Dimension width;Position gx=0;
            XtVaGetValues(toggle,XmNalignment,&alignment,XmNwidth,&width,NULL);
            CHECK(alignment==XmALIGNMENT_BEGINNING);
            if(gadget) XtVaGetValues(toggle,XmNx,&gx,NULL);
            XRectangle text=gadget?LabG_TextRect(toggle):((XmLabelWidget)toggle)->label.TextRect;
            int left=text.x-gx;
            if (!(rtl ? left>(int)width/2 : left+text.width<(int)width/2))
                fprintf(stderr,"caption gadget=%u radio=%u rtl=%u scale=%u left=%d text=%u width=%u gx=%d\n",gadget,radio,rtl,scale,left,text.width,width,gx);
            CHECK(rtl ? left>(int)width/2 : left+text.width<(int)width/2);
            XtVaSetValues(toggle,XmNindicatorOn,XmINDICATOR_NONE,NULL);
            XtVaGetValues(toggle,XmNalignment,&alignment,NULL);
            CHECK(alignment==XmALIGNMENT_CENTER);
            XtVaSetValues(toggle,XmNindicatorOn,XmINDICATOR_FILL,NULL);
            XtVaGetValues(toggle,XmNalignment,&alignment,NULL);
            CHECK(alignment==XmALIGNMENT_BEGINNING);
        }

        off=snapshot(toggle,accent,&off_count);callbacks=0;
        if(gadget)XmToggleButtonGadgetSetState(toggle,True,True);
        else XmToggleButtonSetState(toggle,True,True);
        drain(app,display);on=snapshot(toggle,accent,&on_count);
        CHECK(callbacks==1);CHECK(off!=on);CHECK(on_count>off_count+4*scale/100);
        CHECK(gadget?XmToggleButtonGadgetGetState(toggle):XmToggleButtonGetState(toggle));
        if(!radio) {
            XtVaSetValues(toggle,XmNset,XmINDETERMINATE,NULL);drain(app,display);
            mixed=snapshot(toggle,accent,&mixed_count);
            CHECK(mixed!=off && mixed!=on);CHECK(mixed_count>off_count+4*scale/100);
        }
        XtSetSensitive(toggle,False);drain(app,display);
        snapshot(toggle,accent,&on_count);CHECK(on_count==0);
        XtSetSensitive(toggle,True);
        XtVaSetValues(toggle,XmNset,XmUNSET,NULL);
        XSetInputFocus(display,XtWindow(shell),RevertToParent,CurrentTime);
        drain(app,display);
        CHECK(XmProcessTraversal(toggle,XmTRAVERSE_CURRENT));
        drain(app,display);CHECK(XmGetFocusWidget(shell)==toggle);
        callbacks=0;
        {
            XEvent key={0};
            key.xkey.display=display;key.xkey.window=XtWindowOfObject(toggle);
            key.xkey.root=DefaultRootWindow(display);key.xkey.same_screen=True;
            key.xkey.keycode=XKeysymToKeycode(display,XK_space);
            key.xkey.type=KeyPress;
            XSendEvent(display,key.xkey.window,False,KeyPressMask,&key);
            key.xkey.type=KeyRelease;
            XSendEvent(display,key.xkey.window,False,KeyReleaseMask,&key);
        }
        drain(app,display);
        CHECK(callbacks==1);
        CHECK(gadget?XmToggleButtonGadgetGetState(toggle):XmToggleButtonGetState(toggle));
        XtVaSetValues(toggle,XmNunselectColor,BlackPixel(display,DefaultScreen(display)),
            XmNspacing,19,NULL);
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
        drain(app,display);
        {
            Pixel unselect;Dimension spacing;
            XtVaGetValues(toggle,XmNunselectColor,&unselect,XmNspacing,&spacing,NULL);
            CHECK(unselect==BlackPixel(display,DefaultScreen(display)) && spacing==19);
        }
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_CLASSIC));
        {
            unsigned char alignment;
            XtVaGetValues(toggle,XmNalignment,&alignment,NULL);
            CHECK(alignment==XmALIGNMENT_CENTER);
        }
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        {
            unsigned char alignment;
            XtVaGetValues(toggle,XmNalignment,&alignment,NULL);
            CHECK(alignment==XmALIGNMENT_BEGINNING);
            XtVaGetValues(fixed,XmNalignment,&alignment,NULL);
            CHECK(alignment==XmALIGNMENT_CENTER);
        }
        XtVaSetValues(toggle,XmNalignment,XmALIGNMENT_END,NULL);
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_CLASSIC));
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        {
            unsigned char alignment;
            XtVaGetValues(toggle,XmNalignment,&alignment,NULL);
            CHECK(alignment==XmALIGNMENT_END);
        }
        XtVaSetValues(toggle,XmNwidth,1,XmNheight,1,NULL);drain(app,display);
        XtDestroyWidget(shell);drain(app,display);++cases;
    }
    XtCloseDisplay(display);XtDestroyApplicationContext(app);
    printf("PASS modern toggles: %u native state/geometry cases\n",cases);return 0;
}
