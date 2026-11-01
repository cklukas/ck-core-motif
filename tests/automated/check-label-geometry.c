/* Natural labels follow measured content, explicit dimensions retain ownership. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1); } } while(0)
static void drain(XtAppContext app,Display *d) {
    for(unsigned n=0;n<10000;++n) { XSync(d,False);if(!XtAppPending(app))return;XtAppProcessEvent(app,XtIMAll); } CHECK(0);
}
static void natural(Widget w,const char *resource,XmString expected) {
    Dimension width,height,tw,th;XmString text;XmRenderTable font;XtWidgetGeometry preferred={0},request={0};
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,resource,&text,XmNrenderTable,&font,NULL);
    CHECK(text && font && XmStringCompare(text,expected));
    XmStringExtent(font,text,&tw,&th);XtQueryGeometry(w,NULL,&preferred);
    if(width!=preferred.width || height!=preferred.height || width<tw || height<th)
        fprintf(stderr,"%s actual=%ux%u preferred=%ux%u text=%ux%u\n",XtName(w),width,height,preferred.width,preferred.height,tw,th);
    CHECK(width==preferred.width && height==preferred.height && width>=tw && height>=th);
    request.request_mode=CWWidth|CWHeight;request.width=width;request.height=height;
    CHECK(XtQueryGeometry(w,&request,&preferred)==XtGeometryYes);
}
static void icon_position(Widget w,Pixel pixel,Boolean rtl) {
    Dimension width,height;double scale;
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,NULL);CHECK(XmGetWidgetScale(w,&scale));
    int inset=(int)(6*scale+0.5),x=rtl?(int)width-inset-1:inset;
    XImage *image=XGetImage(XtDisplay(w),XtWindow(w),0,0,width,height,AllPlanes,ZPixmap);CHECK(image);
    CHECK(XGetPixel(image,x,height/2)==pixel);XDestroyImage(image);
}
static void badge_pixels(Widget w, Pixel ink) {
    Dimension width,height;Pixel border;
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,XmNbottomShadowColor,&border,NULL);
    XImage *image=XGetImage(XtDisplay(w),XtWindow(w),0,0,width,height,AllPlanes,ZPixmap);CHECK(image);
    unsigned ink_pixels=0;
    for(unsigned y=0;y<height;++y)for(unsigned x=0;x<width;++x) {
        Pixel pixel=XGetPixel(image,x,y);ink_pixels+=pixel==ink;
        if ((pixel==border)!=(XGetPixel(image,x,height-1-y)==border)) {
            fprintf(stderr,"badge border %ux%u at %u,%u: %lx reflected %lx border %lx ink %lx\n",
                width,height,x,y,pixel,XGetPixel(image,x,height-1-y),border,ink);
            FILE *file=fopen("badge-border.ppm","wb");CHECK(file);
            fprintf(file,"P6\n%u %u\n255\n",width,height);
            for(unsigned row=0;row<height;++row)for(unsigned col=0;col<width;++col) {
                unsigned long p=XGetPixel(image,col,row);
                fputc((p>>16)&255,file);fputc((p>>8)&255,file);fputc(p&255,file);
            }
            fclose(file);
        }
        CHECK((pixel==border)==(XGetPixel(image,width-1-x,y)==border));
        CHECK((pixel==border)==(XGetPixel(image,x,height-1-y)==border));
    }
    CHECK(ink_pixels>0);XDestroyImage(image);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;const double scales[]={1,1.5,2,3};
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"labelGeometry","LabelGeometry",NULL,0,&argc,argv);CHECK(d);
    XColor exact,color;CHECK(XAllocNamedColor(d,DefaultColormap(d,DefaultScreen(d)),"magenta",&color,&exact));
    Pixmap pixmap=XCreatePixmap(d,DefaultRootWindow(d),23,31,DefaultDepth(d,DefaultScreen(d)));
    GC gc=XCreateGC(d,pixmap,0,NULL);XSetForeground(d,gc,color.pixel);XFillRectangle(d,pixmap,gc,0,0,23,31);XFreeGC(d,gc);
    for(unsigned kind=0;kind<3;++kind)for(unsigned manager=0;manager<2;++manager)
    for(unsigned dark=0;dark<2;++dark)for(unsigned scale=0;scale<4;++scale)for(unsigned rtl=0;rtl<2;++rtl) {
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        Widget shell=XtAppCreateShell("labelGeometry","LabelGeometry",applicationShellWidgetClass,d,NULL,0);
        XtVaSetValues(shell,XmNallowShellResize,True,NULL);
        Widget board=manager?XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,XmNorientation,XmVERTICAL,XmNpacking,XmPACK_TIGHT,XmNadjustLast,False,NULL):XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,1400,XmNheight,700,XmNresizePolicy,XmRESIZE_NONE,NULL);
        const char *resource=kind==2?XmNbadgeLabel:kind==1?XmNiconLabelLabel:XmNpromptText;
        XmString body=XmStringCreateLocalized("First line\nSecond line");
        XmString direction=XmStringDirectionCreate(rtl?XmSTRING_DIRECTION_R_TO_L:XmSTRING_DIRECTION_L_TO_R);
        XmString text=XmStringConcat(direction,body);XmStringFree(direction);XmStringFree(body);
        Widget label=XtVaCreateManagedWidget("label",kind==2?xmBadgeWidgetClass:kind==1?xmIconLabelWidgetClass:xmPromptWidgetClass,board,resource,text,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        if(kind==1)XtVaSetValues(label,XmNiconLabelPixmap,pixmap,NULL);
        XtRealizeWidget(shell);drain(app,d);natural(label,resource,text);
        if(kind==2) {
            Dimension width,height,tw,th;XmRenderTable font;
            XmString counter=XmStringCreateLocalized("99+");
            XtVaSetValues(label,XmNbadgeCount,100,XmNforeground,color.pixel,NULL);drain(app,d);
            XtVaGetValues(label,XmNwidth,&width,XmNheight,&height,XmNrenderTable,&font,NULL);
            XmStringExtent(font,counter,&tw,&th);CHECK(width>=tw && height>=th);
            CHECK(XmBadgeGetDisplayedCount(label)==99);badge_pixels(label,color.pixel);
            XmStringFree(counter);
            XtVaSetValues(label,XmNbadgeCount,-1,NULL);drain(app,d);natural(label,resource,text);
        }
        if(kind==1) {
            Dimension spacing;XtVaGetValues(label,XmNiconLabelSpacing,&spacing,NULL);
            CHECK(spacing==(Dimension)(6*scales[scale]+0.5));
            icon_position(label,color.pixel,rtl);
            CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));drain(app,d);
            XtVaGetValues(label,XmNiconLabelSpacing,&spacing,NULL);
            CHECK(spacing==(Dimension)(4*scales[scale]+0.5));natural(label,resource,text);
            CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));drain(app,d);
        }
        Dimension width,height,w2,h2;XtVaGetValues(label,XmNwidth,&width,XmNheight,&height,NULL);
        XtVaSetValues(label,XmNwidth,width,XmNheight,height,NULL);XmStringFree(text);
        text=XmStringCreateLocalized("Longer label with details\nSecond line\nThird line");
        XtVaSetValues(label,resource,text,NULL);drain(app,d);
        XtVaGetValues(label,XmNwidth,&w2,XmNheight,&h2,NULL);CHECK(w2==width && h2==height);
        XtVaSetValues(label,XmNwidth,0,XmNheight,0,NULL);drain(app,d);natural(label,resource,text);
        CHECK(XmSetDisplayScaleOverride(d,scales[(scale+1)%4]));drain(app,d);natural(label,resource,text);
        if(kind==1) {
            Dimension spacing;XtVaGetValues(label,XmNiconLabelSpacing,&spacing,NULL);
            CHECK(spacing==(Dimension)(6*scales[(scale+1)%4]+0.5));
            XtVaSetValues(label,XmNiconLabelSpacing,17,NULL);
        }
        XtVaSetValues(label,XmNwidth,777,XmNheight,111,NULL);
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));drain(app,d);
        XtVaGetValues(label,XmNwidth,&w2,XmNheight,&h2,NULL);CHECK(w2==777 && h2==111);
        if(kind==1) { Dimension spacing;XtVaGetValues(label,XmNiconLabelSpacing,&spacing,NULL);CHECK(spacing==17); }
        XtVaSetValues(label,XmNwidth,0,XmNheight,0,NULL);CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);natural(label,resource,text);
        XmStringFree(text);XtDestroyWidget(shell);drain(app,d);++cases;
    }
    {
        Widget shell=XtAppCreateShell("overflow","Overflow",applicationShellWidgetClass,d,NULL,0);
        XmString text=XmStringCreateLocalized("A");
        Widget label=XtVaCreateWidget("overflow",xmIconLabelWidgetClass,shell,
            XmNiconLabelLabel,text,XmNiconLabelPixmap,pixmap,
            XmNiconLabelSpacing,USHRT_MAX,NULL);
        XtWidgetGeometry preferred={0};XtQueryGeometry(label,NULL,&preferred);
        CHECK(preferred.width==USHRT_MAX);
        XmStringFree(text);XtDestroyWidget(shell);drain(app,d);
    }
    XFreePixmap(d,pixmap);XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS label natural geometry: %u configurations\n",cases);return 0;
}
