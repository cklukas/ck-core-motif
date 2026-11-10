#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <Xm/InfoBar.h>
#include <stdio.h>
#include <stdlib.h>
static unsigned checks, dismissals, actions;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return EXIT_FAILURE; } ++checks; } while (0)
static void dismissed(Widget w, XtPointer c, XtPointer d) { (void)w; (void)c; (void)d; ++dismissals; }
static void acted(Widget w, XtPointer c, XtPointer d) { (void)w; (void)c; (void)d; ++actions; }
static void click(Widget widget, int x, int y) {
    XEvent event = {0}; Position root_x, root_y;
    event.xbutton.type = ButtonPress; event.xbutton.display = XtDisplay(widget);
    event.xbutton.window = XtWindow(widget); event.xbutton.root = RootWindowOfScreen(XtScreen(widget));
    XtTranslateCoords(widget, x, y, &root_x, &root_y);
    event.xbutton.x = x; event.xbutton.y = y; event.xbutton.x_root = root_x; event.xbutton.y_root = root_y;
    event.xbutton.button = Button1; event.xbutton.same_screen = True;
    XtDispatchEvent(&event);
}
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, board, bar, rtl; XmAccessibility *a; XmAccessibleInfo info; XmAccessibleId id; Dimension width, height;
    shell=XtVaAppInitialize(&app,"InfoBarTest",NULL,0,&argc,argv,NULL,XmNwidth,420,XmNheight,100,NULL); board=XmCreateBulletinBoard(shell,(char*)"board",NULL,0); XtManageChild(board);
    bar=XmCreateInfoBar(board,(char*)"bar",NULL,0); XtVaSetValues(bar,XmNinfoSeverity,XmINFO_SEVERITY_WARNING,XmNinfoMessage,(XtArgVal)"Unsaved changes",XmNinfoActionLabel,(XtArgVal)"Save",XmNwidth,360,XmNheight,40,NULL); XtAddCallback(bar,XmNinfoDismissCallback,dismissed,NULL); XtAddCallback(bar,XmNinfoActionCallback,acted,NULL); XtManageChild(bar); XtRealizeWidget(shell); XSync(XtDisplay(shell),False);
    XtVaGetValues(bar,XmNwidth,&width,XmNheight,&height,NULL); CHECK(width==360&&height==40); { XtWidgetGeometry requested={0},preferred={0}; requested.request_mode=CWWidth|CWHeight; requested.width=width; requested.height=height; CHECK(XtQueryGeometry(bar,&requested,&preferred)==XtGeometryYes); CHECK(preferred.width==width&&preferred.height==height); } XmInfoBarInvokeAction(bar); CHECK(actions==1); XmInfoBarDismiss(bar); CHECK(!XtIsManaged(bar)); CHECK(dismissals==1);
    rtl=XtVaCreateManagedWidget("rtl",xmInfoBarWidgetClass,board,XmNlayoutDirection,XmRIGHT_TO_LEFT,XmNinfoMessage,(XtArgVal)"RTL message",XmNinfoActionLabel,(XtArgVal)"Save",XmNwidth,360,XmNheight,40,NULL); XtAddCallback(rtl,XmNinfoDismissCallback,dismissed,NULL); XtAddCallback(rtl,XmNinfoActionCallback,acted,NULL); XSync(XtDisplay(shell),False);
    click(rtl,35,20); CHECK(actions==2); CHECK(XtIsManaged(rtl));
    click(rtl,12,20); CHECK(!XtIsManaged(rtl)); CHECK(dismissals==2);
    a=XmAccessibilityCreate(shell,"InfoBarTest"); CHECK(a!=NULL); id=XmAccessibilityWidget(a,bar); CHECK(id!=XmACCESSIBLE_NONE); CHECK(XmAccessibilityQuery(a,id,&info)); CHECK(info.has_text&&info.text_length==15&&info.value_text!=NULL); XmAccessibleInfoClear(&info); XmAccessibilityDestroy(a); XtDestroyWidget(rtl); XtDestroyWidget(bar); printf("PASS info bar: %u checks\n",checks); return EXIT_SUCCESS;
}
