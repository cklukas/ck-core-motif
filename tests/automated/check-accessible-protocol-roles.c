/* Role integers are protocol values, not toolkit-private enum ordinals. */
#include <Xm/XmAll.h>
#include <Xm/Accessible.h>
#include <Xm/ProgressBar.h>
#include <Xm/TreeView.h>
#include <Xm/Calendar.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
int main(int argc,char **argv) {
    XtAppContext app;Widget shell,board;XmAccessibility *bridge;
    const unsigned expected[]={42,65,5};
    WidgetClass classes[]={xmProgressBarWidgetClass,xmTreeViewWidgetClass,xmCalendarWidgetClass};
    CHECK(XmACCESSIBLE_PROGRESS_BAR==42 && XmACCESSIBLE_TREE==65 && XmACCESSIBLE_CALENDAR==5);
    shell=XtVaAppInitialize(&app,"ProtocolRoles",NULL,0,&argc,argv,NULL,NULL);
    board=XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,NULL);
    for(unsigned i=0;i<XtNumber(classes);++i){
        Widget widget=XtVaCreateManagedWidget("control",classes[i],board,NULL);
        XmAccessibleInfo info;bridge=XmAccessibilityCreate(shell,"Protocol roles");CHECK(bridge);
        CHECK(XmAccessibilityQuery(bridge,XmAccessibilityWidget(bridge,widget),&info));
        CHECK(info.role==expected[i]);XmAccessibleInfoClear(&info);
        XmAccessibilityDestroy(bridge);XtDestroyWidget(widget);
    }
    XtDestroyWidget(shell);XtDestroyApplicationContext(app);
    puts("PASS accessible protocol roles: 3 controls");return 0;
}
