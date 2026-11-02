/* Disabled tabs cannot leak their drawing color into another tab or repaint. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static void pump(XtAppContext a,Display*d){XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);XSync(d,False);}
int main(int argc,char **argv){
 XtAppContext a;Widget s=XtVaAppInitialize(&a,"TabColor",NULL,0,&argc,argv,NULL,NULL);Display*d=XtDisplay(s);
 CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_DARK));CHECK(XmSetDisplayScaleOverride(d,1));
 Widget root=XtVaCreateManagedWidget("root",xmRowColumnWidgetClass,s,NULL);
 Widget stack=XtVaCreateManagedWidget("stack",xmTabStackWidgetClass,root,XmNwidth,420,XmNheight,180,NULL);
 XmString text=XmStringCreateLocalized("Readable tab label");
 XtVaCreateManagedWidget("page",xmLabelWidgetClass,stack,XmNtabLabelString,text,XmNlabelString,text,NULL);XmStringFree(text);
 XtRealizeWidget(s);pump(a,d);
 Widget tabs=XtNameToWidget(stack,"*tabBox");CHECK(tabs);
 XtSetSensitive(stack,False);pump(a,d);XtSetSensitive(stack,True);pump(a,d);
 Widget canvas=XtNameToWidget(tabs,"*canvas");CHECK(canvas);Pixel fg;XtVaGetValues(tabs,XmNforeground,&fg,NULL);
 XImage*im=XGetImage(d,XtWindow(canvas),0,0,XtWidth(canvas),XtHeight(canvas),AllPlanes,ZPixmap);CHECK(im);
 unsigned ink=0,black=0;for(int y=0;y<im->height;y++)for(int x=0;x<im->width;x++){Pixel p=XGetPixel(im,x,y);ink+=p==fg;black+=p==BlackPixelOfScreen(XtScreen(s));}
 CHECK(ink>10);CHECK(black==0);XDestroyImage(im);XtDestroyWidget(s);puts("PASS tab drawing color is local after disabled repaint");return 0;
}
