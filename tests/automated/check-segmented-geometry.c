#include <Xm/XmAll.h>
#include <Xm/SegmentedControl.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <Xm/ToggleBP.h>
#include <X11/IntrinsicP.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void drain(XtAppContext app,Display *display) {
    XSync(display,False);
    while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);
    XSync(display,False);
}
static unsigned long pixel(Widget widget,int x,int y) {
    XWindowAttributes attributes;
    CHECK(XGetWindowAttributes(XtDisplay(widget),XtWindow(widget),&attributes));
    if(x<0 || y<0 || x>=attributes.width || y>=attributes.height)
        fprintf(stderr,"pixel outside %s: %d,%d in %dx%d\n",XtName(widget),x,y,attributes.width,attributes.height);
    CHECK(x>=0 && y>=0 && x<attributes.width && y<attributes.height);
    XImage *image=XGetImage(XtDisplay(widget),XtWindow(widget),x,y,1,1,AllPlanes,ZPixmap);
    unsigned long result;
    CHECK(image);result=XGetPixel(image,0,0);XDestroyImage(image);return result;
}
static int surface_y(Display *display) {
    double scale=1.0;
    (void)XmGetDisplayScaleOverride(display,&scale);
    return (int)(scale+0.5)+1;
}
static void appearance(Widget control,Widget *children,Boolean rtl,XtAppContext app,Display *display) {
    Pixel border,selected,unselected;Dimension width,height;
    CHECK(XmSegmentedControlSetSelectedIndex(control,1,False));drain(app,display);
    for(unsigned i=0;i<3;++i) {
        XtVaGetValues(children[i],XmNbottomShadowColor,&border,XmNselectColor,&selected,
            XmNunselectColor,&unselected,XmNwidth,&width,XmNheight,&height,NULL);
        /* Continuous top/bottom stroke, full interior fill, one shared divider. */
        CHECK(pixel(children[i],width/2,0)==border);
        CHECK(pixel(children[i],width/2,height-1)==border);
        CHECK(pixel(children[i],width/2,surface_y(display))==(i==1?selected:unselected));
        /* The label's first scanline is blank for these strings; it must retain
         * the selected surface instead of XClearArea's window background. */
        {
            XmToggleButtonWidget button=(XmToggleButtonWidget)children[i];
            CHECK(pixel(children[i],button->label.TextRect.x+button->label.StringRect.x,button->label.TextRect.y+button->label.StringRect.y)==
                (i==1?selected:unselected));
            if(button->label.label_type==XmPIXMAP_AND_STRING) {
                XImage *image=XGetImage(display,button->label.pixmap,6,6,1,1,AllPlanes,ZPixmap);
                CHECK(image);
                CHECK(pixel(children[i],button->label.TextRect.x+button->label.PixmapRect.x+6,
                    button->label.TextRect.y+button->label.PixmapRect.y+6)==XGetPixel(image,0,0));
                XDestroyImage(image);
            }
        }
        if(i!=(rtl?2:0))CHECK(pixel(children[i],0,height/2)==border);
    }
    /* Focus loss must repaint the surface without erasing the shared outline. */
    CHECK(XmProcessTraversal(children[2],XmTRAVERSE_CURRENT));drain(app,display);
    CHECK(XmProcessTraversal(children[0],XmTRAVERSE_CURRENT));drain(app,display);
    XtVaGetValues(children[2],XmNwidth,&width,XmNbottomShadowColor,&border,NULL);
    CHECK(pixel(children[2],width/2,0)==border);
}
static void bounds(Widget parent,Widget *children,Boolean rtl,unsigned spacing) {
    Dimension width;unsigned edge=0;
    XtVaGetValues(parent,XmNwidth,&width,NULL);
    for(unsigned i=0;i<3;++i) {
        Position x;Dimension child_width;
        unsigned index=rtl?2-i:i;
        XtVaGetValues(children[index],XmNx,&x,XmNwidth,&child_width,NULL);
        CHECK(x>=0 && (unsigned)x==edge);CHECK(child_width>0);
        edge+=child_width;CHECK(edge<=width);
        if(i<2)edge+=spacing;
    }
    CHECK(edge==width);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *display;unsigned cases=0;
    XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,
        XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_HIGH_CONTRAST};
    const char *text[]={"Day","Working week","Entire month"};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"segmentGeometry","SegmentGeometry",NULL,0,&argc,argv);CHECK(display);
    CHECK(XmSetAppearanceProfile(display,XmAPPEARANCE_CLASSIC));
    for(unsigned profile=0;profile<4;++profile)for(unsigned rtl=0;rtl<2;++rtl) {
        Widget shell,form,control,children[3];XmSegmentedControlItem items[3];
        XtWidgetGeometry natural={0},after={0},query={0},reply={0};
        Position saved_x[3];Dimension saved_width[3];
        CHECK(XmApplyAppearanceProfile(display,profiles[profile]));
        shell=XtAppCreateShell("segmentGeometry","SegmentGeometry",applicationShellWidgetClass,display,NULL,0);
        form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,XmNwidth,700,XmNheight,100,NULL);
        control=XmVaCreateManagedSegmentedControl(form,"segments",XmNwidth,600,XmNheight,50,
            XmNSegmentedRtl,rtl,NULL);
        for(unsigned i=0;i<3;++i) {
            items[i].id=text[i];items[i].label=XmStringCreateLocalized((char *)text[i]);
            items[i].pixmap=XmUNSPECIFIED_PIXMAP;items[i].enabled=True;
        }
        CHECK(XmSegmentedControlSetItems(control,items,3));
        for(unsigned i=0;i<3;++i) {
            char name[32];snprintf(name,sizeof(name),"segment%u",i);
            children[i]=XtNameToWidget(control,name);CHECK(children[i]);XmStringFree(items[i].label);
        }
        XtRealizeWidget(shell);drain(app,display);
        if(profile==1 || profile==2)appearance(control,children,rtl,app,display);
        XtQueryGeometry(control,NULL,&natural);
        XtVaSetValues(control,XmNwidth,30,NULL);drain(app,display);
        XtQueryGeometry(control,NULL,&after);
        CHECK(natural.width==after.width && natural.height==after.height);
        XtVaSetValues(control,XmNwidth,600,NULL);drain(app,display);bounds(control,children,rtl,0);
        for(unsigned i=0;i<3;++i)XtVaGetValues(children[i],XmNx,&saved_x[i],XmNwidth,&saved_width[i],NULL);
        query.request_mode=CWWidth|CWHeight|XtCWQueryOnly;query.width=13;query.height=17;
        CHECK(XtMakeGeometryRequest(children[1],&query,&reply)!=XtGeometryYes);
        for(unsigned i=0;i<3;++i) {
            Position x;Dimension width;
            XtVaGetValues(children[i],XmNx,&x,XmNwidth,&width,NULL);
            CHECK(x==saved_x[i] && width==saved_width[i]);
        }
        XtVaSetValues(control,XmNwidth,1,NULL);drain(app,display);
        for(unsigned i=0;i<3;++i) {
            XWindowAttributes attributes;
            CHECK(XGetWindowAttributes(display,XtWindow(children[i]),&attributes));
            CHECK(attributes.width>0);
            CHECK((attributes.map_state!=IsUnmapped)==(i==0));
        }
        XtVaSetValues(control,XmNwidth,600,NULL);drain(app,display);bounds(control,children,rtl,0);
        for(unsigned i=0;i<3;++i) {
            XWindowAttributes attributes;
            CHECK(XGetWindowAttributes(display,XtWindow(children[i]),&attributes));
            CHECK(attributes.map_state==IsViewable);
        }
        /* Explicit gaps belong to the preferred width and survive expansion. */
        XtVaSetValues(control,XmNSegmentedSpacing,7,NULL);drain(app,display);
        XtQueryGeometry(control,NULL,&after);
        CHECK(after.width==natural.width+14);
        bounds(control,children,rtl,7);
        XtVaSetValues(control,XmNwidth,30,NULL);drain(app,display);
        bounds(control,children,rtl,0);
        XtQueryGeometry(control,NULL,&after);CHECK(after.width==natural.width+14);
        XtVaSetValues(control,XmNwidth,600,NULL);drain(app,display);
        bounds(control,children,rtl,7);
        /* A matching query must also be side-effect free. */
        XtVaGetValues(children[1],XmNwidth,&query.width,XmNheight,&query.height,NULL);
        CHECK(XtMakeGeometryRequest(children[1],&query,&reply)==XtGeometryYes);
        bounds(control,children,rtl,7);
        /* Recompute preferred sizes from labels, never the old allocation. */
        {
            XmString longer=XmStringCreateLocalized("An extended segment label for the complete working month");
            XtVaSetValues(children[1],XmNlabelString,longer,NULL);XmStringFree(longer);
            XtQueryGeometry(control,NULL,&after);CHECK(after.width>natural.width+14);
            natural=after;
            XtVaSetValues(control,XmNwidth,30,NULL);drain(app,display);
            XtQueryGeometry(control,NULL,&after);CHECK(after.width==natural.width);
        }
        /* Excessive resource values must not wrap signed child positions. */
        XtVaSetValues(control,XmNSegmentedSpacing,USHRT_MAX,XmNwidth,600,NULL);
        drain(app,display);bounds(control,children,rtl,0);
        XtQueryGeometry(control,NULL,&after);CHECK(after.width==SHRT_MAX);
        XtDestroyWidget(shell);drain(app,display);++cases;
    }
    /* Visual state matrix uses generous allocation to isolate appearance from
     * overflow: scale/density, live profiles, explicit colors and sensitivity. */
    for(unsigned pixmap_mode=0;pixmap_mode<2;++pixmap_mode)
    for(unsigned scale_index=0;scale_index<4;++scale_index)
    for(unsigned density=0;density<2;++density)
    for(unsigned rtl=0;rtl<2;++rtl) {
        const double scales[]={1.0,1.5,2.0,3.0};
        Widget shell,form,control,children[3];XmSegmentedControlItem items[3];
        Pixmap icon=XmUNSPECIFIED_PIXMAP;
        fprintf(stderr,"STATE scale=%g density=%u rtl=%u\n",scales[scale_index],density,rtl);
        CHECK(XmSetDisplayScaleOverride(display,scales[scale_index]));
        CHECK(XmApplyAppearanceDensity(display,density?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("segmentStates","SegmentStates",applicationShellWidgetClass,display,NULL,0);
        form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,XmNwidth,1500,XmNheight,180,NULL);
        control=XmVaCreateManagedSegmentedControl(form,"segments",XmNwidth,1400,XmNheight,150,XmNSegmentedRtl,rtl,NULL);
        if(pixmap_mode) {
            GC gc;XColor color,exact;
            CHECK(XAllocNamedColor(display,DefaultColormap(display,DefaultScreen(display)),"#448844",&color,&exact));
            icon=XCreatePixmap(display,DefaultRootWindow(display),12,12,DefaultDepth(display,DefaultScreen(display)));
            gc=XCreateGC(display,icon,0,NULL);XSetForeground(display,gc,color.pixel);
            XFillRectangle(display,icon,gc,0,0,12,12);XFreeGC(display,gc);
        }
        for(unsigned i=0;i<3;++i) {
            items[i].id=text[i];items[i].label=XmStringCreateLocalized((char *)text[i]);
            items[i].pixmap=icon;items[i].enabled=True;
        }
        CHECK(XmSegmentedControlSetItems(control,items,3));
        for(unsigned i=0;i<3;++i) {
            char name[32];snprintf(name,sizeof(name),"segment%u",i);
            children[i]=XtNameToWidget(control,name);CHECK(children[i]);XmStringFree(items[i].label);
        }
        XtRealizeWidget(shell);drain(app,display);
        appearance(control,children,rtl,app,display);
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_DARK));drain(app,display);
        appearance(control,children,rtl,app,display);
        {
            Pixel selected;Dimension width;
            XtVaGetValues(children[1],XmNselectColor,&selected,XmNwidth,&width,NULL);
            XtSetSensitive(control,False);drain(app,display);
            CHECK(XmSegmentedControlGetSelectedIndex(control)==1);
            CHECK(pixel(children[1],width/2,surface_y(display))==selected);
            XtSetSensitive(control,True);drain(app,display);
        }
        /* Native pointer actions preview a press, cancel it on leave, and
         * commit exactly the item released inside its own allocation. */
        {
            XEvent event={0};Pixel selected,unselected;Dimension width;
            event.xbutton.type=ButtonPress;event.xbutton.display=display;
            event.xbutton.window=XtWindow(children[2]);event.xbutton.button=Button1;
            event.xbutton.x=10;event.xbutton.y=10;event.xbutton.same_screen=True;
            XtVaGetValues(children[2],XmNselectColor,&selected,XmNunselectColor,&unselected,XmNwidth,&width,NULL);
            XtCallActionProc(children[2],"Arm",&event,NULL,0);drain(app,display);
            CHECK(XmSegmentedControlGetSelectedIndex(control)==1);
            CHECK(pixel(children[2],width/2,surface_y(display))==selected);
            CHECK(pixel(children[2],width-20,75)==selected);
            event.xcrossing.type=LeaveNotify;event.xcrossing.mode=NotifyNormal;
            XtCallActionProc(children[2],"Leave",&event,NULL,0);drain(app,display);
            CHECK(pixel(children[2],width/2,surface_y(display))==unselected);
            XtCallActionProc(children[2],"Disarm",&event,NULL,0);
            event.xbutton.type=ButtonPress;event.xbutton.x=10;event.xbutton.y=10;
            XtCallActionProc(children[2],"Arm",&event,NULL,0);
            event.xbutton.type=ButtonRelease;
            {
                Position root_x,root_y;
                XtTranslateCoords(children[2],10,10,&root_x,&root_y);
                event.xbutton.x_root=root_x;event.xbutton.y_root=root_y;
            }
            XtCallActionProc(children[2],"Select",&event,NULL,0);
            XtCallActionProc(children[2],"Disarm",&event,NULL,0);drain(app,display);
            CHECK(XmSegmentedControlGetSelectedIndex(control)==2);
            CHECK(pixel(children[2],width/2,surface_y(display))==selected);
            CHECK(pixel(children[2],width-20,75)==selected);
            /* Clicking the required selection again must restore both native
             * toggle state and painted selection after ToggleButton clears it. */
            event.xbutton.type=ButtonPress;
            XtCallActionProc(children[2],"Arm",&event,NULL,0);
            event.xbutton.type=ButtonRelease;
            XtCallActionProc(children[2],"Select",&event,NULL,0);
            XtCallActionProc(children[2],"Disarm",&event,NULL,0);drain(app,display);
            CHECK(XmSegmentedControlGetSelectedIndex(control)==2);
            CHECK(XmToggleButtonGetState(children[2]));
            CHECK(pixel(children[2],width/2,surface_y(display))==selected);
            CHECK(XmSegmentedControlSetSelectedIndex(control,1,False));drain(app,display);
        }
        /* Explicit application palette choices survive a live profile change. */
        {
            XColor exact,color;Pixel actual;Dimension width;
            CHECK(XAllocNamedColor(display,DefaultColormap(display,DefaultScreen(display)),"#884422",&color,&exact));
            XtVaSetValues(children[1],XmNselectColor,color.pixel,NULL);
            CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_LIGHT));drain(app,display);
            XtVaGetValues(children[1],XmNselectColor,&actual,XmNwidth,&width,NULL);
            CHECK(actual==color.pixel);CHECK(pixel(children[1],width/2,surface_y(display))==color.pixel);
        }
        XtDestroyWidget(shell);drain(app,display);
        if(icon!=XmUNSPECIFIED_PIXMAP)XFreePixmap(display,icon);
        ++cases;
    }
    for(unsigned scale=1;scale<=3;++scale) {
        Widget shell,form,automatic,fixed;XmString label;
        XmSegmentedControlItem item;XtWidgetGeometry preferred={0};Dimension w,h;
        CHECK(XmSetDisplayScaleOverride(display,scale));
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("segmentNatural","SegmentNatural",applicationShellWidgetClass,display,NULL,0);
        form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,XmNwidth,1500,XmNheight,500,NULL);
        automatic=XmVaCreateManagedSegmentedControl(form,"automatic",NULL);
        fixed=XmVaCreateManagedSegmentedControl(form,"fixed",XmNy,250,XmNwidth,350,XmNheight,120,NULL);
        label=XmStringCreateLocalized("Natural label size");
        item=(XmSegmentedControlItem){"natural",label,XmUNSPECIFIED_PIXMAP,True};
        CHECK(XmSegmentedControlSetItems(automatic,&item,1));
        CHECK(XmSegmentedControlSetItems(fixed,&item,1));XmStringFree(label);
        XtRealizeWidget(shell);drain(app,display);
        XtQueryGeometry(automatic,NULL,&preferred);
        XtVaGetValues(automatic,XmNwidth,&w,XmNheight,&h,NULL);
        CHECK(w==preferred.width && h==preferred.height);
        XtVaGetValues(fixed,XmNwidth,&w,XmNheight,&h,NULL);CHECK(w==350 && h==120);
        CHECK(XmSetDisplayScaleOverride(display,scale==3?1:scale+1));drain(app,display);
        XtQueryGeometry(automatic,NULL,&preferred);
        XtVaGetValues(automatic,XmNwidth,&w,XmNheight,&h,NULL);CHECK(w==preferred.width && h==preferred.height);
        XtVaGetValues(fixed,XmNwidth,&w,XmNheight,&h,NULL);CHECK(w==350 && h==120);
        label=XmStringCreateLocalized("A longer label that changes the natural size");
        XtVaSetValues(XtNameToWidget(automatic,"segment0"),XmNlabelString,label,NULL);
        XtVaSetValues(XtNameToWidget(fixed,"segment0"),XmNlabelString,label,NULL);XmStringFree(label);
        drain(app,display);XtQueryGeometry(automatic,NULL,&preferred);
        XtVaGetValues(automatic,XmNwidth,&w,XmNheight,&h,NULL);CHECK(w==preferred.width && h==preferred.height);
        XtVaGetValues(fixed,XmNwidth,&w,XmNheight,&h,NULL);CHECK(w==350 && h==120);
        XtVaSetValues(fixed,XmNwidth,0,NULL);drain(app,display);
        XtQueryGeometry(fixed,NULL,&preferred);XtVaGetValues(fixed,XmNwidth,&w,XmNheight,&h,NULL);
        CHECK(w==preferred.width && h==120);
        XtVaSetValues(fixed,XmNheight,0,NULL);drain(app,display);
        XtQueryGeometry(fixed,NULL,&preferred);XtVaGetValues(fixed,XmNwidth,&w,XmNheight,&h,NULL);
        CHECK(w==preferred.width && h==preferred.height);
        XtDestroyWidget(shell);drain(app,display);++cases;
    }
    XtCloseDisplay(display);XtDestroyApplicationContext(app);
    printf("PASS segmented natural geometry: %u cases\n",cases);return 0;
}
