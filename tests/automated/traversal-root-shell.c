#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleSwitch.h>
#include <stdio.h>
extern void _XmSetRect(XRectangle *, Widget);
int main(int argc,char **argv)
{
    XtAppContext app;
    Widget shell=XtVaAppInitialize(&app,"TraversalRoot",NULL,0,&argc,argv,NULL,NULL);
    Widget row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,shell,NULL);
    Widget button=XtVaCreateManagedWidget("button",xmPushButtonWidgetClass,row,NULL);
    Widget gadget=XtVaCreateManagedWidget("gadget",xmPushButtonGadgetClass,row,NULL);
    Widget direct=XtVaCreateManagedWidget("direct",xmToggleSwitchWidgetClass,shell,NULL);
    XtRealizeWidget(shell);
    Widget widgets[]={shell,row,button,gadget,direct};
    for(unsigned i=0;i<XtNumber(widgets);++i){
        XRectangle rect; Position x,y; Dimension width,height,border;
        _XmSetRect(&rect,widgets[i]);
        XtVaGetValues(widgets[i],XmNwidth,&width,XmNheight,&height,XmNborderWidth,&border,NULL);
        if(i==0){int root_x,root_y;Window child;
            if(!XTranslateCoordinates(XtDisplay(shell),XtWindow(shell),RootWindowOfScreen(XtScreen(shell)),0,0,&root_x,&root_y,&child)){fprintf(stderr,"FAIL root translation\n");return 1;}
            x=(Position)root_x;y=(Position)root_y;
        }else if(i==3){Position local_x,local_y;XtVaGetValues(gadget,XmNx,&local_x,XmNy,&local_y,NULL);
            XtTranslateCoords(row,local_x+border,local_y+border,&x,&y);
        }else XtTranslateCoords(widgets[i],0,0,&x,&y);
        if(rect.x!=x || rect.y!=y || rect.width!=width || rect.height!=height){
            fprintf(stderr,"FAIL traversal rectangle for %s\n",XtName(widgets[i]));return 1;
        }
    }
    /* A shell need not have a next tab group.  The regression is that this
     * traversal entry point must examine the realized root shell safely,
     * irrespective of whether a destination is available. */
    (void)XmProcessTraversal(shell,XmTRAVERSE_NEXT_TAB_GROUP);
    XtDestroyWidget(shell);
    puts("PASS traversal root-shell, child-widget and gadget rectangles and focus traversal");return 0;
}
