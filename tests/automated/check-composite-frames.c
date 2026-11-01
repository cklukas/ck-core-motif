/* Composite editor borders follow profile ownership, not paint suppression. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static void drain(XtAppContext a,Display*d){XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);XSync(d,False);}
static Dimension shadow(Widget w){Dimension v;XtVaGetValues(w,XmNshadowThickness,&v,NULL);return v;}
static Widget editor(Widget w){Widget child=NULL;XtVaGetValues(w,XmNtextField,&child,NULL);CHECK(child);return child;}
int main(int argc,char**argv){
    XtAppContext a;Display*d;double scales[]={1,1.5,2};unsigned cases=0;
    XtToolkitInitialize();a=XtCreateApplicationContext();d=XtOpenDisplay(a,NULL,"frames","Frames",NULL,0,&argc,argv);CHECK(d);
    for(unsigned dark=0;dark<2;++dark)for(unsigned scale=0;scale<3;++scale)for(unsigned startup=0;startup<2;++startup)for(unsigned fixed=0;fixed<2;++fixed){
        XmAppearanceProfile profile=dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT;
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceProfile(d,startup?profile:XmAPPEARANCE_CLASSIC));
        Widget shell=XtAppCreateShell("frames","Frames",applicationShellWidgetClass,d,NULL,0);
        /* Keep tested pixels visible across profile growth without shell feedback. */
        XtVaSetValues(shell,XtNwidth,1200,XtNheight,1100,NULL);
        Widget board=XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,NULL);
        Widget controls[4];
        controls[0]=XtVaCreateManagedWidget("spin",xmSimpleSpinBoxWidgetClass,board,XmNspinBoxChildType,XmNUMERIC,XmNwidth,fixed?200:0,XmNheight,fixed?48:0,NULL);
        for(unsigned type=0;type<3;++type)controls[type+1]=XtVaCreateManagedWidget("combo",xmComboBoxWidgetClass,board,XmNcomboBoxType,type,NULL);
        XtRealizeWidget(shell);drain(a,d);CHECK(XmApplyAppearanceProfile(d,profile));drain(a,d);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(a,d);
        for(unsigned i=0;i<4;++i){
            unsigned char type=XmDROP_DOWN_COMBO_BOX;
            if(i)XtVaGetValues(controls[i],XmNcomboBoxType,&type,NULL);
            CHECK(shadow(editor(controls[i]))==(type==XmDROP_DOWN_LIST?0:2));++cases;
        }
        CHECK(XmApplyAppearanceProfile(d,profile));drain(a,d);
        for(unsigned i=0;i<4;++i){
            CHECK(XmApplyAppearanceProfile(d,profile));drain(a,d);
            Widget field=editor(controls[i]);unsigned char type=XmDROP_DOWN_COMBO_BOX;
            if(i)XtVaGetValues(controls[i],XmNcomboBoxType,&type,NULL);
            Dimension expected=type==XmCOMBO_BOX?(Dimension)(scales[scale]+0.5):0;
            if(shadow(field)!=expected)fprintf(stderr,"frame i=%u type=%u startup=%u shadow=%u expected=%u\n",i,type,startup,shadow(field),expected);
            CHECK(shadow(field)==expected);++cases;
            if(i==0) {
                Pixel border;XtVaGetValues(controls[i],XmNbottomShadowColor,&border,NULL);
                CHECK(shadow(controls[i])>0);
                XImage *im=XGetImage(d,XtWindow(controls[i]),0,0,1,1,AllPlanes,ZPixmap);CHECK(im);
                CHECK(XGetPixel(im,0,0)==border);XDestroyImage(im);
                im=XGetImage(d,XtWindow(controls[i]),XtWidth(controls[i])-1,XtHeight(controls[i])-1,1,1,AllPlanes,ZPixmap);CHECK(im);
                CHECK(XGetPixel(im,0,0)==border);XDestroyImage(im);++cases;
            }
            /* An equal-valued setter transfers ownership to the caller. */
            XtVaSetValues(field,XmNshadowThickness,expected,NULL);
            CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(a,d);CHECK(shadow(field)==expected);
            CHECK(XmApplyAppearanceProfile(d,profile));drain(a,d);CHECK(shadow(field)==expected);++cases;
            XtVaSetValues(field,XmNshadowThickness,3,NULL);
            CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));drain(a,d);CHECK(shadow(field)==3);++cases;
        }
        XtDestroyWidget(shell);drain(a,d);
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(a);printf("PASS composite frames: %u ownership cases\n",cases);return 0;
}
