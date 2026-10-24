#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <Xm/InfoBar.h>
#include <stdio.h>
#include <stdlib.h>
static unsigned checks, dismissals, actions;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return EXIT_FAILURE; } ++checks; } while (0)
static void dismissed(Widget w, XtPointer c, XtPointer d) { (void)w; (void)c; (void)d; ++dismissals; }
static void acted(Widget w, XtPointer c, XtPointer d) { (void)w; (void)c; (void)d; ++actions; }
int main(int argc, char **argv) {
    XtAppContext app; Widget shell, board, bar; XmAccessibility *a; XmAccessibleInfo info; XmAccessibleId id; Dimension width, height;
    shell=XtVaAppInitialize(&app,"InfoBarTest",NULL,0,&argc,argv,NULL,XmNwidth,420,XmNheight,100,NULL); board=XmCreateBulletinBoard(shell,(char*)"board",NULL,0); XtManageChild(board);
    bar=XmCreateInfoBar(board,(char*)"bar",NULL,0); XtVaSetValues(bar,XmNinfoSeverity,XmINFO_SEVERITY_WARNING,XmNinfoMessage,(XtArgVal)"Unsaved changes",XmNinfoActionLabel,(XtArgVal)"Save",XmNwidth,360,XmNheight,40,NULL); XtAddCallback(bar,XmNinfoDismissCallback,dismissed,NULL); XtAddCallback(bar,XmNinfoActionCallback,acted,NULL); XtManageChild(bar); XtRealizeWidget(shell); XSync(XtDisplay(shell),False);
    XtVaGetValues(bar,XmNwidth,&width,XmNheight,&height,NULL); CHECK(width==360&&height==40); XmInfoBarInvokeAction(bar); CHECK(actions==1); XmInfoBarDismiss(bar); CHECK(dismissals==1);
    a=XmAccessibilityCreate(shell,"InfoBarTest"); CHECK(a!=NULL); id=XmAccessibilityWidget(a,bar); CHECK(id!=XmACCESSIBLE_NONE); CHECK(XmAccessibilityQuery(a,id,&info)); CHECK(info.has_text&&info.text_length==15&&info.value_text!=NULL); XmAccessibleInfoClear(&info); XmAccessibilityDestroy(a); XtDestroyWidget(bar); printf("PASS info bar: %u checks\n",checks); return EXIT_SUCCESS;
}
