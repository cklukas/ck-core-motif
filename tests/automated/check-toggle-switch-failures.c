/* Exercise real allocation-failure branches without changing production APIs. */
#define _GNU_SOURCE
#include <Xm/ToggleSwitchP.h>
#include <Xm/DrawingA.h>
#include <X11/Shell.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);} } while(0)
static XmString fail_string;
static XmFontList fail_font;
static int fail_empty,fail_gc,hits;
XmString XmStringCopy(XmString value) {
    XmString (*real)(XmString)=(XmString (*)(XmString))dlsym(RTLD_NEXT,"XmStringCopy");
    CHECK(real);
    if(value && value==fail_string){fail_string=NULL;++hits;return NULL;}
    return real(value);
}
XmString XmStringCreateLocalized(char *value) {
    XmString (*real)(char *)=(XmString (*)(char *))dlsym(RTLD_NEXT,"XmStringCreateLocalized");
    CHECK(real);
    if(fail_empty && value && !*value){fail_empty=0;++hits;return NULL;}
    return real(value);
}
XmFontList XmFontListCopy(XmFontList value) {
    XmFontList (*real)(XmFontList)=(XmFontList (*)(XmFontList))dlsym(RTLD_NEXT,"XmFontListCopy");
    CHECK(real);
    if(value && value==fail_font){fail_font=NULL;++hits;return NULL;}
    return real(value);
}
GC XCreateGC(Display *d,Drawable draw,unsigned long mask,XGCValues *values) {
    GC (*real)(Display *,Drawable,unsigned long,XGCValues *)=
        (GC (*)(Display *,Drawable,unsigned long,XGCValues *))dlsym(RTLD_NEXT,"XCreateGC");
    CHECK(real);
    if(fail_gc && mask==(GCForeground|GCBackground|GCGraphicsExposures)){
        fail_gc=0;++hits;return NULL;
    }
    return real(d,draw,mask,values);
}
int main(int argc,char **argv) {
    XtAppContext app;Widget shell=XtVaAppInitialize(&app,"SwitchFailures",NULL,0,&argc,argv,NULL,NULL);
    Display *d=XtDisplay(shell);Widget host=XtVaCreateManagedWidget("host",xmDrawingAreaWidgetClass,shell,NULL);
    XmString label=XmStringCreateLocalized("Owned original");
    Widget w=XtVaCreateManagedWidget("switch",xmToggleSwitchWidgetClass,host,XmNlabelString,label,NULL);
    XmToggleSwitchWidget p=(XmToggleSwitchWidget)w;
    XtRealizeWidget(shell);
    XmString original=p->toggle_switch.label;
    XmString replacement=XmStringCreateLocalized("Borrowed replacement");
    fail_string=replacement;
    XtVaSetValues(w,XmNlabelString,replacement,NULL);
    CHECK(hits==1 && p->toggle_switch.label==original);
    CHECK(XmStringCompare(p->toggle_switch.label,label));
    XmStringFree(replacement);
    fail_empty=1;
    XtVaSetValues(w,XmNlabelString,NULL,NULL);
    CHECK(hits==2 && p->toggle_switch.label==original);
    XmFontList oldfont=p->toggle_switch.font;
    XmFontList supplied=XmFontListCopy(oldfont);CHECK(supplied);
    fail_font=supplied;
    XtVaSetValues(w,XmNrenderTable,supplied,NULL);
    CHECK(hits==3 && p->toggle_switch.font==oldfont);
    XmFontListFree(supplied);
    XtClass(w)->core_class.expose(w,NULL,NULL);
    XtDestroyWidget(w);
    fail_string=label;
    w=XtVaCreateWidget("fallback",xmToggleSwitchWidgetClass,host,XmNlabelString,label,NULL);
    p=(XmToggleSwitchWidget)w;
    CHECK(hits==4 && p->toggle_switch.label && XmStringEmpty(p->toggle_switch.label));
    XtDestroyWidget(w);
    fail_string=label;fail_empty=1;
    w=XtVaCreateManagedWidget("emptyFailure",xmToggleSwitchWidgetClass,host,XmNlabelString,label,NULL);
    p=(XmToggleSwitchWidget)w;CHECK(hits==6 && !p->toggle_switch.label);
    XtClass(w)->core_class.expose(w,NULL,NULL);XtDestroyWidget(w);
    fail_gc=1;
    w=XtVaCreateManagedWidget("gcFailure",xmToggleSwitchWidgetClass,host,NULL);
    p=(XmToggleSwitchWidget)w;CHECK(hits==7 && !p->toggle_switch.track_gc);
    XtClass(w)->core_class.expose(w,NULL,NULL);XtDestroyWidget(w);
    XmStringFree(label);XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS switch failure injection: %d allocation failures\n",hits);return 0;
}
