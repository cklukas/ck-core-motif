/* Compound strings must honor clips even when the entire line is outside. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static void pump(XtAppContext a,Display*d){XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);XSync(d,False);}
static void clipped(Widget w,XmRenderTable table,XRectangle clip,Boolean intersects){
 Display*d=XtDisplay(w);Window win=XtWindow(w);Pixel white=WhitePixelOfScreen(XtScreen(w)),black=BlackPixelOfScreen(XtScreen(w));
 GC gc=XCreateGC(d,win,0,NULL);XSetForeground(d,gc,white);XFillRectangle(d,win,gc,0,0,400,160);XSetForeground(d,gc,black);
 XmString text=XmStringCreateLocalized("Clip this text");XmStringDraw(d,win,table,text,gc,20,20,250,XmALIGNMENT_BEGINNING,XmLEFT_TO_RIGHT,&clip);XSync(d,False);
 XImage*im=XGetImage(d,win,0,0,400,160,AllPlanes,ZPixmap);CHECK(im);unsigned ink=0;
 for(int y=0;y<160;y++)for(int x=0;x<400;x++){Pixel p=XGetPixel(im,x,y);if(p!=white){CHECK(x>=clip.x&&x<clip.x+clip.width&&y>=clip.y&&y<clip.y+clip.height);++ink;}}
 CHECK(intersects?ink>0:ink==0);XDestroyImage(im);XmStringFree(text);XFreeGC(d,gc);
}
int main(int argc,char **argv){
 XtAppContext a;Widget shell=XtVaAppInitialize(&a,"StringClip",NULL,0,&argc,argv,NULL,NULL);Display*d=XtDisplay(shell);CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_DARK));
 Widget area=XtVaCreateManagedWidget("area",xmDrawingAreaWidgetClass,shell,XmNwidth,400,XmNheight,160,NULL);
 Widget label=XtVaCreateWidget("font",xmLabelWidgetClass,area,NULL);XmRenderTable modern=NULL;XtVaGetValues(label,XmNrenderTable,&modern,NULL);CHECK(modern);
 XFontStruct*font=XLoadQueryFont(d,"fixed");CHECK(font);XmFontList core=XmFontListCreate(font,XmFONTLIST_DEFAULT_TAG);CHECK(core);
 XtRealizeWidget(shell);pump(a,d);
 for(unsigned i=0;i<2;i++){XmRenderTable table=i?modern:core;
  clipped(area,table,(XRectangle){300,0,90,100},False);
  clipped(area,table,(XRectangle){0,100,400,50},False);
  clipped(area,table,(XRectangle){24,20,25,50},True);
  clipped(area,table,(XRectangle){0,0,400,160},True);
 }
 XmFontListFree(core);XFreeFont(d,font);XtDestroyWidget(shell);puts("PASS compound-string clipping: core and Xft, disjoint, partial and enclosing rectangles");return 0;
}
