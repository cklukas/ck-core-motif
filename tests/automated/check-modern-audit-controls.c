/* Modern control sizing, Xft selection, contrast, and pointer behavior. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <Xm/DataFP.h>
#include <Xm/CalendarP.h>
#include <Xm/TimePickerP.h>
#include <X11/Xutil.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static void drain(XtAppContext app,Display *d) {XSync(d,False);while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);XSync(d,False);}
static void click(Widget w,int x,int y,XtAppContext app) {XEvent e={0};e.xbutton.type=ButtonPress;e.xbutton.display=XtDisplay(w);e.xbutton.window=XtWindow(w);e.xbutton.button=Button1;e.xbutton.x=x;e.xbutton.y=y;XtDispatchEvent(&e);drain(app,XtDisplay(w));}
static void xft(Widget w) {XmRenderTable t=NULL;XmStringTag *tags=NULL;XtVaGetValues(w,XmNrenderTable,&t,NULL);int n=XmRenderTableGetTags(t,&tags);CHECK(n>0);for(int i=0;i<n;i++){XmRendition r=XmRenderTableGetRendition(t,tags[i]);XmFontType type;Arg a;XtSetArg(a,XmNfontType,&type);XmRenditionRetrieve(r,&a,1);CHECK(type==XmFONT_IS_XFT);XmRenditionFree(r);XtFree(tags[i]);}XtFree((char*)tags);}
static void progress_contrast(Widget w,int scale,XtAppContext app) {
    Dimension width,height,spacing;Pixel fill;
    XmProgressBarSetValue(w,75,False);drain(app,XtDisplay(w));
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,XmNspacing,&spacing,XmNhighlightColor,&fill,NULL);
    XImage *image=XGetImage(XtDisplay(w),XtWindow(w),0,0,width,height,AllPlanes,ZPixmap);CHECK(image);
    unsigned black=0,white=0;
    for(int y=spacing+4*scale;y<(int)height-spacing;y++)
        for(int x=(int)width/2-18*scale;x<(int)width/2+18*scale;x++) {
            Pixel p=XGetPixel(image,x,y);black+=p==BlackPixelOfScreen(XtScreen(w));white+=p==WhitePixelOfScreen(XtScreen(w));
        }
    XColor color={0};color.pixel=fill;XQueryColor(XtDisplay(w),DefaultColormapOfScreen(XtScreen(w)),&color);
    double rgb[]={color.red/65535.,color.green/65535.,color.blue/65535.};
    for(unsigned i=0;i<3;i++)rgb[i]=rgb[i]<=.04045?rgb[i]/12.92:pow((rgb[i]+.055)/1.055,2.4);
    double luminance=.2126*rgb[0]+.7152*rgb[1]+.0722*rgb[2];
    CHECK((black&&(luminance+.05)/.05>=4.5)||(white&&1.05/(luminance+.05)>=4.5));
    XDestroyImage(image);
}
int main(int argc,char **argv) {
    CHECK(setlocale(LC_ALL,"")!=NULL);XtSetLanguageProc(NULL,NULL,NULL);
    XtAppContext app;Widget seed=XtVaAppInitialize(&app,"ModernAudit",NULL,0,&argc,argv,NULL,NULL);Display *d=XtDisplay(seed);
    unsigned cases=0;
    for(int dark=0;dark<2;dark++)for(int scale=1;scale<=2;scale++) {
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));CHECK(XmSetDisplayScaleOverride(d,scale));
        Widget shell=XtAppCreateShell("audit","ModernAudit",applicationShellWidgetClass,d,NULL,0);
        Widget root=XtVaCreateManagedWidget("root",xmDrawingAreaWidgetClass,shell,XmNwidth,900,XmNheight,1100,XmNresizePolicy,XmRESIZE_NONE,NULL);
        Widget data=XtVaCreateManagedWidget("data",xmDataFieldWidgetClass,root,XmNcolumns,20,XmNy,0,NULL);
        Widget progress=XtVaCreateManagedWidget("progress",xmProgressBarWidgetClass,root,XmNprogressTextVisible,True,XmNy,100,NULL);
        Widget date=XtVaCreateManagedWidget("date",xmDatePickerWidgetClass,root,XmNy,200,NULL);
        Widget time=XtVaCreateManagedWidget("time",xmTimePickerWidgetClass,root,XmNy,300,NULL);
        Widget calendar=XtVaCreateManagedWidget("calendar",xmCalendarWidgetClass,root,XmNy,400,NULL);
        XtRealizeWidget(shell);drain(app,d);
        xft(data);xft(progress);xft(date);xft(time);xft(calendar);
        Dimension ph;XtVaGetValues(progress,XmNheight,&ph,NULL);CHECK(ph>=40*scale);
        progress_contrast(progress,scale,app);
#ifdef USE_XFT
        CHECK(XmTextF_use_xft(data));
#endif
        XmDataFieldSetString(data,"Grüße Ελληνικά");char *value=XmDataFieldGetString(data);CHECK(value&&!strcmp(value,"Grüße Ελληνικά"));XtFree(value);
        Dimension width,height;XtVaGetValues(date,XmNwidth,&width,XmNheight,&height,NULL);CHECK(width>=160*scale&&height>=36*scale);
        XtVaGetValues(time,XmNwidth,&width,XmNheight,&height,NULL);CHECK(width>=160*scale&&height>=36*scale);
        CHECK(XmTimePickerSetValue(time,&(XmTimePickerValue){12,34,56},False));click(time,width-12*scale,height/4,app);XmTimePickerValue tv;CHECK(XmTimePickerGetValue(time,&tv)&&tv.hour==13&&tv.minute==34);
        CHECK(XmCalendarSetVisibleMonth(calendar,2024,2));XtVaGetValues(calendar,XmNwidth,&width,XmNheight,&height,NULL);CHECK(width>=280*scale&&height>=256*scale);
        click(calendar,width-20*scale,12*scale,app);int year,month;CHECK(XmCalendarGetVisibleMonth(calendar,&year,&month)&&year==2024&&month==3);
        CHECK(XmCalendarSetVisibleMonth(calendar,2024,2));CHECK(XmCalendarSetDate(calendar,&(XmCalendarDate){2024,2,15},False));
        for(int percent=0;percent<=100;percent+=50){XmProgressBarSetValue(progress,percent,False);drain(app,d);CHECK(XmProgressBarGetValue(progress)==percent);}
        XmDatePickerOpen(date);drain(app,d);CHECK(XmDatePickerIsOpen(date));XmDatePickerClose(date);drain(app,d);CHECK(!XmDatePickerIsOpen(date));
        Widget section=XtVaCreateManagedWidget("section",xmFormSectionWidgetClass,root,XmNy,950,NULL);
        Widget footer=XmFormSectionGetFooter(section);Boolean mapped=True;CHECK(footer);XtVaGetValues(footer,XmNmappedWhenManaged,&mapped,NULL);CHECK(!mapped);
        XmString footer_text=XmStringCreateLocalized("Footer text");XtVaSetValues(section,XmNformSectionFooter,footer_text,NULL);XmStringFree(footer_text);XtVaGetValues(footer,XmNmappedWhenManaged,&mapped,NULL);CHECK(mapped);
        footer_text=XmStringCreateLocalized("");XtVaSetValues(section,XmNformSectionFooter,footer_text,NULL);XmStringFree(footer_text);XtVaGetValues(footer,XmNmappedWhenManaged,&mapped,NULL);CHECK(!mapped);
        XtSetSensitive(root,False);drain(app,d);XtSetSensitive(root,True);drain(app,d);XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XtDestroyWidget(seed);puts("PASS modern audit controls: light/dark, 1x/2x, Xft UTF-8, picker sizing, month navigation and time spinner");CHECK(cases==4);return 0;
}
