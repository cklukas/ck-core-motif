/* Internal extension/cache objects do not have the visual-object lifecycle. */
#include <Xm/XmAll.h>
#include <Xm/ExtObjectP.h>
#include <Xm/ContainerP.h>
#include <Xm/XmI.h>
#include <stdio.h>
static unsigned calls, helper_destroyed;
static void gone(Widget w,XtPointer client,XtPointer data)
{ (void)w;(void)client;(void)data;++helper_destroyed; }
static void scaled(Widget widget,double before,double after,XtPointer closure)
{ (void)widget;(void)before;(void)after;(void)closure;++calls; }
int main(int argc,char **argv)
{
    XtAppContext app;Widget shell=XtVaAppInitialize(&app,"AppearanceLifetime",NULL,0,&argc,argv,NULL,NULL);
    if(!XmSetAppearanceProfile(XtDisplay(shell),XmAPPEARANCE_MODERN_LIGHT))return 2;
    Widget board=XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,NULL);
    Widget extension=XtVaCreateWidget("privateExtension",xmExtObjectClass,board,XmNlogicalParent,board,NULL);
    int failures=0;
    if(_XmRegisterAppearanceChange(extension,NULL,0)){fputs("FAIL nonvisual appearance registration\n",stderr);++failures;}
    if(_XmRegisterScaleChange(extension,scaled,NULL)){fputs("FAIL nonvisual scale registration\n",stderr);++failures;}
    Widget label=XtVaCreateManagedWidget("label",xmLabelWidgetClass,board,NULL);
    Widget gadget=XtVaCreateManagedWidget("gadget",xmLabelGadgetClass,board,NULL);
    if(!_XmRegisterAppearanceChange(label,NULL,0) || !_XmRegisterAppearanceChange(gadget,NULL,0) ||
       !_XmRegisterScaleChange(label,scaled,NULL) || !_XmRegisterScaleChange(gadget,scaled,NULL))return 3;
    XtRemoveAllCallbacks(gadget,XmNdestroyCallback);
    XtRealizeWidget(shell);
    if(!XmSetDisplayScaleOverride(XtDisplay(shell),1.5) || calls!=2)return 4;
    for(int i=0;i<30;i++) {
        XmString text=XmStringCreateLocalized(i%2?"cache update":"gadget label");
        XtVaSetValues(gadget,XmNlabelString,text,XmNmarginWidth,i%5,NULL);XmStringFree(text);
    }
    XtDestroyWidget(gadget);
    if(!XmApplyAppearanceProfile(XtDisplay(shell),XmAPPEARANCE_MODERN_DARK) ||
       !XmSetDisplayScaleOverride(XtDisplay(shell),2.0))return 5;
    Widget replacement=XtVaCreateManagedWidget("replacement",xmLabelGadgetClass,board,NULL);
    (void)replacement;
    Widget container=XtVaCreateManagedWidget("container",xmContainerWidgetClass,board,XmNlayoutType,XmOUTLINE,NULL);
    Widget helper=((XmContainerWidget)container)->container.size_ob;
    if(!helper)return 5;
    XtAddCallback(helper,XmNdestroyCallback,gone,NULL);
    XtDestroyWidget(container);
    if(helper_destroyed!=1){fputs("FAIL orphaned Container sizing gadget\n",stderr);return 6;}
    XtDestroyWidget(shell);XtDestroyApplicationContext(app);
    if(!failures)puts("PASS visual-only lifetime hooks, mutable destroy callbacks, gadget updates and hierarchy teardown");
    return failures?1:0;
}
