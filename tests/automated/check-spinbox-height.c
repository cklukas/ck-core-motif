/* Vertical arrow spacing must depend on height, never on unrelated width. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/SpinBP.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static void drain(XtAppContext a,Display*d){XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);XSync(d,False);}
int main(int argc,char**argv){
    XtAppContext a;Display*d;unsigned cases=0;double scales[]={1,1.5,2};
    XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_HIGH_CONTRAST};
    unsigned char layouts[]={XmARROWS_BEGINNING,XmARROWS_END};
    XtToolkitInitialize();a=XtCreateApplicationContext();d=XtOpenDisplay(a,NULL,"spinHeight","SpinHeight",NULL,0,&argc,argv);CHECK(d);
    for(unsigned p=0;p<4;++p)for(unsigned scale=0;scale<3;++scale)for(unsigned l=0;l<2;++l)for(unsigned rtl=0;rtl<2;++rtl)for(unsigned extra=0;extra<4;++extra){
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceProfile(d,profiles[p]));
        Widget shell=XtAppCreateShell("spinHeight","SpinHeight",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,900,XmNheight,700,NULL);
        Widget spin=XtVaCreateManagedWidget("spin",xmSimpleSpinBoxWidgetClass,board,
            XmNspinBoxChildType,XmNUMERIC,XmNarrowLayout,layouts[l],XmNspacing,6,
            XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        XtRealizeWidget(shell);drain(a,d);XmSpinBoxWidget s=(XmSpinBoxWidget)spin;
        Dimension arrow=s->spinBox.arrow_size;
        /* Fit both arrows exactly. Width has ample space in each case. */
        Dimension height=2*arrow+extra*4;
        XtResizeWidget(spin,400,height,0);drain(a,d);
        int up=s->spinBox.up_arrow_rect.y,down=s->spinBox.down_arrow_rect.y;
        if(down+s->spinBox.down_arrow_rect.height>height)
            fprintf(stderr,"overflow profile=%u scale=%g layout=%u rtl=%u height=%u up=%d down=%d arrow=%u\n",p,scales[scale],l,rtl,height,up,down,arrow);
        CHECK(up>=0 && down>=up+arrow && down+arrow<=height);
        XtResizeWidget(spin,600,height,0);drain(a,d);
        CHECK(s->spinBox.up_arrow_rect.y==up && s->spinBox.down_arrow_rect.y==down);++cases;
        XtDestroyWidget(shell);drain(a,d);
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(a);printf("PASS spinbox height: %u constrained cases\n",cases);return 0;
}
