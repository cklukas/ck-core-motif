/* Shared modern viewport framing and explicit-resource ownership. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL %d: %s\n", __LINE__, #c); exit(1); } } while (0)
int main(int argc, char **argv)
{
    XtAppContext app;
    Display *display;
    unsigned cases = 0;
    const XmAppearanceProfile profiles[] = {XmAPPEARANCE_CLASSIC,
        XmAPPEARANCE_MODERN_LIGHT, XmAPPEARANCE_MODERN_DARK, XmAPPEARANCE_HIGH_CONTRAST};
    XtToolkitInitialize(); app = XtCreateApplicationContext();
    display = XtOpenDisplay(app, NULL, "scrolledAppearance", "ScrolledAppearance", NULL, 0, &argc, argv);
    CHECK(display);
    const unsigned char placements[]={XmBOTTOM_RIGHT,XmBOTTOM_LEFT,XmTOP_RIGHT,XmTOP_LEFT};
    for (unsigned place=0;place<XtNumber(placements);++place)
    for (unsigned rtl=0;rtl<2;++rtl)
    for (unsigned scale = 1; scale <= 2; ++scale)
    for (unsigned profile = 0; profile < XtNumber(profiles); ++profile) {
        Widget shell, form, automatic, explicit_frame, work;
        Dimension shadow;
        CHECK(XmSetDisplayScaleOverride(display, scale));
        CHECK(XmApplyAppearanceProfile(display, profiles[profile]));
        shell = XtAppCreateShell("scrolledAppearance", "ScrolledAppearance", applicationShellWidgetClass, display, NULL, 0);
        form = XtVaCreateManagedWidget("form", xmFormWidgetClass, shell, NULL);
        automatic = XtVaCreateManagedWidget("automatic", xmScrolledWindowWidgetClass, form,
            XmNscrollingPolicy, XmAUTOMATIC, XmNwidth, 160 * scale, XmNheight, 100 * scale,
            XmNscrollBarPlacement,placements[place],XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        work = XtVaCreateManagedWidget("work", xmDrawingAreaWidgetClass, automatic,
            XmNwidth, 320 * scale, XmNheight, 200 * scale, NULL);
        explicit_frame = XtVaCreateManagedWidget("explicit", xmScrolledWindowWidgetClass, form,
            XmNscrollingPolicy, XmAUTOMATIC, XmNshadowThickness, 5,
            XmNx, 180 * scale, XmNwidth, 160 * scale, XmNheight, 100 * scale, NULL);
        XtRealizeWidget(shell);
        XSync(display, False);
        XtVaGetValues(automatic, XmNshadowThickness, &shadow, NULL);
        CHECK((profile == 1 || profile == 2) ? shadow == 0 : shadow > 0);
        XtVaGetValues(explicit_frame, XmNshadowThickness, &shadow, NULL); CHECK(shadow == 5);
        CHECK(XtIsRealized(work));
        for (unsigned next = 0; next < XtNumber(profiles); ++next) {
            CHECK(XmApplyAppearanceProfile(display, profiles[next]));
            XtVaGetValues(automatic, XmNshadowThickness, &shadow, NULL);
            CHECK((next == 1 || next == 2) ? shadow == 0 : shadow > 0);
            XtVaGetValues(explicit_frame, XmNshadowThickness, &shadow, NULL); CHECK(shadow == 5);
        }
        CHECK(XmApplyAppearanceProfile(display, profiles[profile]));
        Widget clip;
        XtVaGetValues(automatic,XmNclipWindow,&clip,NULL);CHECK(clip);
        const Dimension extents[][2]={{64,64},{32,32},{16,16},{8,8},{1,1},
            {1,100},{100,1},{16,100},{100,16},{100,100}};
        for(unsigned e=0;e<XtNumber(extents);++e) {
            XtResizeWidget(automatic,extents[e][0],extents[e][1],0);
            XSync(display,False);
            if(XtX(clip)<0 || XtY(clip)<0 || XtX(clip)+XtWidth(clip)>XtWidth(automatic) ||
               XtY(clip)+XtHeight(clip)>XtHeight(automatic)) {
                fprintf(stderr,"clip exceeds viewport profile=%u scale=%u outer=%ux%u clip=%d,%d %ux%u\n",
                    profile,scale,extents[e][0],extents[e][1],XtX(clip),XtY(clip),XtWidth(clip),XtHeight(clip));
                exit(1);
            }
            CHECK(XtWidth(clip)>0 && XtHeight(clip)>0);
            XtUnmanageChild(work);
            CHECK(XtX(clip)>=0 && XtY(clip)>=0);
            CHECK(XtWidth(clip)>0 && XtHeight(clip)>0);
            CHECK(XtX(clip)+XtWidth(clip)<=XtWidth(automatic));
            CHECK(XtY(clip)+XtHeight(clip)<=XtHeight(automatic));
            XtManageChild(work);
            Widget bars[2];
            XtVaGetValues(automatic,XmNhorizontalScrollBar,&bars[0],XmNverticalScrollBar,&bars[1],NULL);
            for(unsigned axis=0;axis<2;++axis) {
                /* Off-frame navigators are intentionally hidden. Any visible
                 * navigator must fit fully and leave the viewport unobscured. */
                Widget bar=bars[axis];
                if (e+1==XtNumber(extents))
                    CHECK(XtX(bar)<XtWidth(automatic) && XtY(bar)<XtHeight(automatic));
                if (XtX(bar)<XtWidth(automatic) && XtY(bar)<XtHeight(automatic)) {
                    CHECK(XtX(bar)>=0 && XtY(bar)>=0);
                    CHECK(XtX(bar)+XtWidth(bar)<=XtWidth(automatic));
                    CHECK(XtY(bar)+XtHeight(bar)<=XtHeight(automatic));
                    CHECK(XtX(bar)+XtWidth(bar)<=XtX(clip) ||
                          XtX(clip)+XtWidth(clip)<=XtX(bar) ||
                          XtY(bar)+XtHeight(bar)<=XtY(clip) ||
                          XtY(clip)+XtHeight(clip)<=XtY(bar));
                }
                int minimum,maximum,size;
                XtVaGetValues(bars[axis],XmNminimum,&minimum,XmNmaximum,&maximum,XmNsliderSize,&size,NULL);
                int extent=axis?XtHeight(clip):XtWidth(clip);
                CHECK(size==(extent<maximum-minimum?extent:maximum-minimum));
            }
        }
        XtDestroyWidget(shell); ++cases;
    }
    XtCloseDisplay(display); XtDestroyApplicationContext(app);
    printf("PASS ScrolledWindow appearance: %u cases\n", cases);
    return 0;
}
