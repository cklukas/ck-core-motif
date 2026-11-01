/* NumberBox native child layout, automatic axes and live appearance. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/NumberBox.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);} } while(0)
static void tick(XtPointer data,XtIntervalId *id){*(Boolean*)data=True;(void)id;}
static void settle(XtAppContext app,Display*d) {
    Boolean done=False;XtAppAddTimeOut(app,10,tick,&done);
    for(unsigned n=0;!done;++n){CHECK(n<10000);XtAppProcessEvent(app,XtIMAll);}
    XSync(d,False);
    for(unsigned n=0;XtAppPending(app);++n){CHECK(n<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}
}
static void contained(Widget number) {
    Widget children[]={XmNumberBoxGetTextField(number),XtNameToWidget(number,"increment"),
        XtNameToWidget(number,"decrement"),XtNameToWidget(number,"validation")};
    for(unsigned i=0;i<XtNumber(children);++i) {
        Widget c=children[i];CHECK(c);
        if(!XtIsManaged(c) || !c->core.mapped_when_managed)continue;
        if(c->core.x<0 || c->core.y<0 || c->core.x+c->core.width>number->core.width || c->core.y+c->core.height>number->core.height)
            fprintf(stderr,"%s %d,%d %ux%u outside %ux%u\n",XtName(c),c->core.x,c->core.y,c->core.width,c->core.height,number->core.width,number->core.height);
        CHECK(c->core.x>=0 && c->core.y>=0);
        CHECK(c->core.x+c->core.width<=number->core.width && c->core.y+c->core.height<=number->core.height);
    }
}
static void natural(Widget number,Boolean rtl) {
    XtWidgetGeometry preferred={0};XtQueryGeometry(number,NULL,&preferred);
    if(number->core.width!=preferred.width || number->core.height!=preferred.height)
        fprintf(stderr,"NumberBox actual %ux%u preferred %ux%u\n",number->core.width,number->core.height,preferred.width,preferred.height);
    CHECK(number->core.width==preferred.width && number->core.height==preferred.height);
    contained(number);
    Widget editor=XmNumberBoxGetTextField(number),up=XtNameToWidget(number,"increment"),down=XtNameToWidget(number,"decrement");
    CHECK(rtl?up->core.x+up->core.width<=editor->core.x:editor->core.x+editor->core.width<=up->core.x);
    CHECK(up->core.y+up->core.height<=down->core.y);
    int minimum,maximum;XmNumberBoxGetRange(number,&minimum,&maximum);
    Dimension mw,hh,sh;XmRenderTable font;XtVaGetValues(editor,XmNmarginWidth,&mw,XmNhighlightThickness,&hh,XmNshadowThickness,&sh,XmNrenderTable,&font,NULL);
    int values[]={minimum,maximum};
    for(unsigned i=0;i<2;++i){char text[32];snprintf(text,sizeof(text),"%d",values[i]);XmString s=XmStringCreateLocalized(text);Dimension w,h;XmStringExtent(font,s,&w,&h);XmStringFree(s);
        CHECK(editor->core.width>=w+2*(mw+hh+sh));CHECK(editor->core.height>=h);}
}
int main(int argc,char **argv) {
    XtAppContext app;Display*d;unsigned cases=0;const double scales[]={1,1.5,2,3};
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"numberGeometry","NumberGeometry",NULL,0,&argc,argv);CHECK(d);
    for(unsigned manager=0;manager<2;++manager)for(unsigned dark=0;dark<2;++dark)
    for(unsigned scale=0;scale<4;++scale)for(unsigned rtl=0;rtl<2;++rtl) {
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        Widget shell=XtAppCreateShell("numberGeometry","NumberGeometry",applicationShellWidgetClass,d,NULL,0);
        XtVaSetValues(shell,XmNallowShellResize,True,NULL);
        Widget board=manager?XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,XmNorientation,XmVERTICAL,XmNadjustLast,False,NULL):
            XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,1400,XmNheight,800,XmNresizePolicy,XmRESIZE_NONE,NULL);
        Widget number=XmVaCreateManagedNumberBox(board,"number",XmNnumberMinimum,-1000,XmNnumberMaximum,1000,
            XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        XtRealizeWidget(shell);settle(app,d);natural(number,rtl);
        XmNumberBoxSetRange(number,INT_MIN,INT_MAX,False);settle(app,d);natural(number,rtl);
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));settle(app,d);natural(number,rtl);
        CHECK(XmSetDisplayScaleOverride(d,scales[(scale+1)%4]));settle(app,d);natural(number,rtl);
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));settle(app,d);natural(number,rtl);
        Widget editor=XmNumberBoxGetTextField(number);
        XFontStruct *fixed=XLoadQueryFont(d,"fixed");CHECK(fixed);
        XmFontList custom=XmFontListCreate(fixed,XmFONTLIST_DEFAULT_TAG);CHECK(custom);
        XtVaSetValues(editor,XmNrenderTable,custom,NULL);settle(app,d);natural(number,rtl);
        XmRenderTable parent_font;XtVaGetValues(number,XmNrenderTable,&parent_font,NULL);
        XmRenderTable copy=XmRenderTableCopy(parent_font,NULL,0);CHECK(copy);
        XtVaSetValues(number,XmNrenderTable,copy,NULL);XmRenderTableFree(copy);settle(app,d);natural(number,rtl);
        XmRenderTable actual;XtVaGetValues(editor,XmNrenderTable,&actual,NULL);
        XmString sample=XmStringCreateLocalized("1234567890");Dimension ew,eh,aw,ah;
        XmStringExtent(custom,sample,&ew,&eh);XmStringExtent(actual,sample,&aw,&ah);
        CHECK(ew==aw && eh==ah);XmStringFree(sample);
        /* Release only after the widget's copied table has been destroyed. */
        XmTextFieldSetString(editor,"not a number");
        XtCallActionProc(editor,"activate",NULL,NULL,0);settle(app,d);
        CHECK(XmNumberBoxIsInvalid(number));contained(number);
        Widget error=XtNameToWidget(number,"validation");CHECK(error && XtIsManaged(error));
        XtWidgetGeometry error_size={0};XtQueryGeometry(error,NULL,&error_size);
        CHECK(error->core.height>=error_size.height && error->core.width>=error_size.width);
        CHECK(error->core.y>=editor->core.y+editor->core.height);
        XmNumberBoxSetValue(number,42,False);settle(app,d);CHECK(!XtIsManaged(error));natural(number,rtl);
        XtVaSetValues(number,XmNwidth,220,NULL);settle(app,d);CHECK(number->core.width==220);contained(number);
        XmNumberBoxSetRange(number,-100,100,False);settle(app,d);CHECK(number->core.width==220);
        XtVaSetValues(number,XmNheight,80,NULL);settle(app,d);CHECK(number->core.width==220 && number->core.height==80);
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));settle(app,d);CHECK(number->core.width==220 && number->core.height==80);
        XtVaSetValues(number,XmNwidth,0,XmNheight,0,NULL);settle(app,d);natural(number,rtl);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));settle(app,d);natural(number,rtl);
        for(unsigned edge=1;edge<9;++edge){XtVaSetValues(number,XmNwidth,edge,XmNheight,edge,NULL);settle(app,d);contained(number);}
        XtDestroyWidget(shell);settle(app,d);XmFontListFree(custom);XFreeFont(d,fixed);++cases;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS numberbox geometry: %u configurations\n",cases);return 0;
}
