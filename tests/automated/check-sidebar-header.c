/* Sidebar owns its bounded header, toggle semantics and focus lifecycle. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/SidebarP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <Xm/Accessible.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static unsigned callbacks;
static void toggled(Widget w,XtPointer client,XtPointer call) { (void)w;(void)client;CHECK(call);++callbacks; }
static void drain(XtAppContext app,Display *display) { XSync(display,False); while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll); XSync(display,False); }
static void key(Widget target, unsigned state) {
    XEvent event={0};
    event.xkey.type=KeyPress; event.xkey.display=XtDisplay(target);
    event.xkey.window=XtWindow(target); event.xkey.root=DefaultRootWindow(XtDisplay(target));
    event.xkey.same_screen=True; event.xkey.state=state;
    event.xkey.keycode=XKeysymToKeycode(XtDisplay(target),XK_m);
    XtDispatchEvent(&event);
}
static void bounds(Widget child,Widget parent) {
    CHECK(child->core.x>=0 && child->core.y>=0);
    CHECK(child->core.x+child->core.width<=parent->core.width);
    CHECK(child->core.y+child->core.height<=parent->core.height);
}
int main(int argc,char **argv) {
    XtAppContext app; Display *display; unsigned cases=0;
    const double scales[]={1,1.5,2};
    XtToolkitInitialize(); app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"sidebarHeader","SidebarHeader",NULL,0,&argc,argv); CHECK(display);
    for(unsigned profile=0;profile<4;++profile) for(unsigned compact=0;compact<2;++compact)
    for(unsigned scale=0;scale<3;++scale) for(unsigned rtl=0;rtl<2;++rtl) {
        Widget shell,form,side,content,entry,outside; XmSidebarWidget sidebar;
        XmAccessibility *accessibility; XmAccessibleId id; XmAccessibleInfo info; unsigned char direction;
        XmString title=XmStringCreateLocalized("Components");
        Dimension allocated_height;
        CHECK(XmSetDisplayScaleOverride(display,scales[scale]));
        CHECK(XmApplyAppearanceDensity(display,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(display,(XmAppearanceProfile)profile));
        shell=XtAppCreateShell("sidebarHeader","SidebarHeader",applicationShellWidgetClass,display,NULL,0);
        form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,
            XmNwidth,(int)(550*scales[scale]),XmNheight,(int)(260*scales[scale]),NULL);
        outside=XtVaCreateManagedWidget("outside",xmTextFieldWidgetClass,form,
            XmNx,(int)(340*scales[scale]),XmNy,10,XmNcolumns,6,NULL);
        side=XtVaCreateManagedWidget("sidebar",xmSidebarWidgetClass,form,XmNlabelString,title,
            XmNwidth,(int)(300*scales[scale]),XmNheight,(int)(240*scales[scale]),
            XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        XmStringFree(title); sidebar=(XmSidebarWidget)side;
        content=XmSidebarGetContent(side);
        entry=XtVaCreateManagedWidget("entry",xmTextFieldWidgetClass,content,XmNcolumns,8,NULL);
        XtAddCallback(side,XmNsidebarToggleCallback,toggled,NULL);
        XtRealizeWidget(shell); drain(app,display);
        bounds(sidebar->sidebar.toggle_button,side); bounds(sidebar->sidebar.title_label,side); bounds(content,side);
        CHECK(sidebar->sidebar.toggle_button->core.width==sidebar->sidebar.toggle_button->core.height);
        CHECK(content->core.y>=sidebar->sidebar.toggle_button->core.y+sidebar->sidebar.toggle_button->core.height);
        XtVaGetValues(sidebar->sidebar.toggle_button,XmNarrowDirection,&direction,NULL);
        CHECK(direction==(rtl?XmARROW_RIGHT:XmARROW_LEFT));
        accessibility=XmAccessibilityCreate(shell,"SidebarHeader"); CHECK(accessibility);
        id=XmAccessibilityWidget(accessibility,sidebar->sidebar.toggle_button);
        CHECK(XmAccessibilityQuery(accessibility,id,&info)); CHECK(!strcmp(info.name,"Collapse sidebar")); XmAccessibleInfoClear(&info);
        XSetInputFocus(display,XtWindow(shell),RevertToParent,CurrentTime); drain(app,display);
        CHECK(XmProcessTraversal(entry,XmTRAVERSE_CURRENT)); drain(app,display);
        XtVaGetValues(side,XmNheight,&allocated_height,NULL);
        callbacks=0; CHECK(XmAccessibilityAction(accessibility,id,0)); drain(app,display);
        CHECK(callbacks==1 && !XmSidebarGetExpanded(side));
        CHECK(side->core.height==allocated_height);
        CHECK(!XtIsManaged(content) && !XtIsManaged(sidebar->sidebar.title_label));
        CHECK(XmGetFocusWidget(shell)==sidebar->sidebar.toggle_button);
        CHECK(XmAccessibilityQuery(accessibility,id,&info)); CHECK(!strcmp(info.name,"Expand sidebar")); XmAccessibleInfoClear(&info);
        XmSidebarSetExpanded(side,True); drain(app,display); CHECK(callbacks==2);
        CHECK(XmGetFocusWidget(shell)==entry);
        CHECK(side->core.height==allocated_height);
        key(entry,ControlMask); drain(app,display);
        CHECK(callbacks==3 && !XmSidebarGetExpanded(side));
        key(sidebar->sidebar.toggle_button,ControlMask|LockMask); drain(app,display);
        CHECK(callbacks==4 && XmSidebarGetExpanded(side));
        CHECK(XmGetFocusWidget(shell)==entry);
        {
            Widget nested=XtVaCreateManagedWidget("nested",xmSidebarWidgetClass,content,
                XmNwidth,200,XmNheight,200,NULL);
            Widget nested_entry=XtVaCreateManagedWidget("nestedEntry",xmTextFieldWidgetClass,
                XmSidebarGetContent(nested),XmNcolumns,4,NULL);
            drain(app,display);
            CHECK(XmProcessTraversal(nested_entry,XmTRAVERSE_CURRENT)); drain(app,display);
            key(nested_entry,ControlMask); drain(app,display);
            CHECK(!XmSidebarGetExpanded(nested) && XmSidebarGetExpanded(side) && callbacks==4);
            XtDestroyWidget(nested); drain(app,display);
            CHECK(XmProcessTraversal(entry,XmTRAVERSE_CURRENT)); drain(app,display);
        }
        {
            char *value;
            key(entry,0); drain(app,display);
            value=XmTextFieldGetString(entry);
            CHECK(value && !strcmp(value,"m") && callbacks==4);
            XtFree(value);
        }
        /* Programmatic layout changes must not steal unrelated focus. */
        CHECK(XmProcessTraversal(outside,XmTRAVERSE_CURRENT)); drain(app,display);
        XmSidebarSetExpanded(side,False); drain(app,display);
        CHECK(XmGetFocusWidget(shell)==outside);
        XmSidebarSetExpanded(side,True); drain(app,display);
        CHECK(XmGetFocusWidget(shell)==outside);
        /* Moving away while collapsed cancels focus restoration, even when
         * the old content widget is still alive. */
        CHECK(XmProcessTraversal(entry,XmTRAVERSE_CURRENT)); drain(app,display);
        XmSidebarSetExpanded(side,False); drain(app,display);
        CHECK(XmProcessTraversal(outside,XmTRAVERSE_CURRENT)); drain(app,display);
        XmSidebarSetExpanded(side,True); drain(app,display);
        CHECK(XmGetFocusWidget(shell)==outside && sidebar->sidebar.saved_focus==NULL);
        CHECK(XmProcessTraversal(entry,XmTRAVERSE_CURRENT)); drain(app,display);
        callbacks=2;
        XtVaSetValues(side,XmNsidebarExpanded,False,NULL); drain(app,display); CHECK(callbacks==2);
        XtDestroyWidget(entry); drain(app,display); CHECK(sidebar->sidebar.saved_focus==NULL);
        XtVaSetValues(side,XmNsidebarExpanded,True,NULL); drain(app,display); CHECK(callbacks==2);
        XtResizeWidget(side,12,12,0); drain(app,display); bounds(sidebar->sidebar.toggle_button,side);
        CHECK(!XtIsManaged(content) && !XtIsManaged(sidebar->sidebar.title_label));
        XmAccessibilityDestroy(accessibility); XtDestroyWidget(shell); drain(app,display); ++cases;
    }
    XtCloseDisplay(display); XtDestroyApplicationContext(app);
    printf("PASS sidebar header: %u cases\n",cases); return 0;
}
