/* Native Container disclosure pixmap ownership and expanded-state refresh. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/ContainerP.h>
#include <Xm/Appearance.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
extern Boolean _XmContainerDrawDisclosure(Widget);
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static void drain(XtAppContext app, Display *d) {
 unsigned n=0; XSync(d,False);
 while(XtAppPending(app)) { CHECK(++n<10000); XtAppProcessEvent(app,XtIMAll); XSync(d,False); }
}
static void check_icon(Widget row, Pixmap expected) {
 Widget button=GetContainerConstraint(row)->related_cwid; Pixmap actual=None;
 CHECK(button); XtVaGetValues(button,XmNlabelPixmap,&actual,NULL);
 if(actual!=expected)fprintf(stderr,"row=%s state=%u actual=%lu expected=%lu\n",XtName(row),GetContainerConstraint(row)->outline_state,actual,expected);
 CHECK(actual==expected);
}
static unsigned activations;
static int expected_event=ButtonRelease;
static void outline_changed(Widget w, XtPointer closure, XtPointer data) {
 (void)w; (void)closure;
 XmContainerOutlineCallbackStruct *cbs=data;
 CHECK(cbs->event && cbs->event->type==expected_event);
 activations++;
}
static void click_disclosure(XtAppContext app, Display *d, Widget row) {
 Widget button=GetContainerConstraint(row)->related_cwid;
 Widget container=XtParent(row); Window child; int x,y;
 CHECK(XTranslateCoordinates(d,XtWindow(container),DefaultRootWindow(d),
   XtX(button)+XtWidth(button)/2,XtY(button)+XtHeight(button)/2,&x,&y,&child));
 CHECK(XTestFakeMotionEvent(d,DefaultScreen(d),x,y,CurrentTime));drain(app,d);
 CHECK(XTestFakeButtonEvent(d,1,True,CurrentTime));drain(app,d);
 CHECK(XTestFakeButtonEvent(d,1,False,CurrentTime));drain(app,d);
}
static void disclosure_key(XtAppContext app, Display *d, KeySym key) {
 KeyCode control=XKeysymToKeycode(d,XK_Control_L),code=XKeysymToKeycode(d,key);
 CHECK(control && code);
 CHECK(XTestFakeKeyEvent(d,control,True,CurrentTime));
 CHECK(XTestFakeKeyEvent(d,code,True,CurrentTime));drain(app,d);
 CHECK(XTestFakeKeyEvent(d,code,False,CurrentTime));
 CHECK(XTestFakeKeyEvent(d,control,False,CurrentTime));drain(app,d);
}
static void check_interaction(XtAppContext app, Display *d, Widget container, Widget row) {
 activations=0;XtAddCallback(container,XmNoutlineChangedCallback,outline_changed,NULL);
 CHECK(GetContainerConstraint(row)->outline_state==XmCOLLAPSED);
 click_disclosure(app,d,row);
 CHECK(activations==1);CHECK(GetContainerConstraint(row)->outline_state==XmEXPANDED);
 click_disclosure(app,d,row);
 CHECK(activations==2);CHECK(GetContainerConstraint(row)->outline_state==XmCOLLAPSED);
 XtSetSensitive(row,False);drain(app,d);
 click_disclosure(app,d,row);
 CHECK(activations==2);CHECK(GetContainerConstraint(row)->outline_state==XmCOLLAPSED);
 XtSetSensitive(row,True);drain(app,d);
 click_disclosure(app,d,row);
 CHECK(activations==3);CHECK(GetContainerConstraint(row)->outline_state==XmEXPANDED);
 click_disclosure(app,d,row);
 CHECK(activations==4);CHECK(GetContainerConstraint(row)->outline_state==XmCOLLAPSED);
 XtRemoveCallback(container,XmNoutlineChangedCallback,outline_changed,NULL);
}
int main(int argc,char **argv) {
 XtToolkitInitialize(); XtAppContext app=XtCreateApplicationContext();
 Display *d=XtOpenDisplay(app,NULL,"disclosure","Disclosure",NULL,0,&argc,argv); CHECK(d);
 XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_HIGH_CONTRAST};
 double scales[]={1,1.5,2}; unsigned cases=0;
 for(unsigned p=0;p<4;p++)for(unsigned s=0;s<3;s++)for(unsigned rtl=0;rtl<2;rtl++) {
  fprintf(stderr,"CASE p=%u scale=%g rtl=%u\n",p,scales[s],rtl);
  CHECK(XmApplyAppearanceProfile(d,profiles[p])); CHECK(XmSetDisplayScaleOverride(d,scales[s]));
  Widget shell=XtAppCreateShell("disclosure","Disclosure",applicationShellWidgetClass,d,NULL,0);
  Widget container=XtVaCreateManagedWidget("container",xmContainerWidgetClass,shell,
   XmNlayoutType,XmOUTLINE,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
  Widget rows[2];
  for(unsigned i=0;i<2;i++) {
   rows[i]=XtVaCreateManagedWidget(i?"collapsed":"expanded",xmIconGadgetClass,container,
    XmNoutlineState,i?XmCOLLAPSED:XmEXPANDED,NULL);
   XtVaCreateManagedWidget("child",xmIconGadgetClass,container,XmNentryParent,rows[i],NULL);
  }
  XtRealizeWidget(shell);drain(app,d);
  Widget expanded_button=GetContainerConstraint(rows[0])->related_cwid;
  Widget collapsed_button=GetContainerConstraint(rows[1])->related_cwid;
  check_interaction(app,d,container,rows[1]);
  XSetInputFocus(d,XtWindow(shell),RevertToParent,CurrentTime);
  CHECK(XmProcessTraversal(rows[1],XmTRAVERSE_CURRENT));drain(app,d);
  CHECK(XmGetFocusWidget(container)==rows[1]);
  activations=0;expected_event=KeyPress;
  XtAddCallback(container,XmNoutlineChangedCallback,outline_changed,NULL);
  disclosure_key(app,d,rtl?XK_Left:XK_Right);
  CHECK(activations==1);CHECK(GetContainerConstraint(rows[1])->outline_state==XmEXPANDED);
  disclosure_key(app,d,rtl?XK_Right:XK_Left);
  CHECK(activations==2);CHECK(GetContainerConstraint(rows[1])->outline_state==XmCOLLAPSED);
  XtRemoveCallback(container,XmNoutlineChangedCallback,outline_changed,NULL);
  expected_event=ButtonRelease;
  Boolean modern=p==1 || p==2;
  CHECK(_XmContainerDrawDisclosure(expanded_button)==modern);
  CHECK(_XmContainerDrawDisclosure(collapsed_button)==modern);
  XImage *default_image=NULL;
  if(modern) {
   CHECK(XtWidth(expanded_button)>=20*scales[s]);
   CHECK(XtHeight(expanded_button)>=20*scales[s]);
   Pixel bg;XtVaGetValues(container,XmNbackground,&bg,NULL);
   XSync(d,False);
   XImage *image=XGetImage(d,XtWindow(container),XtX(expanded_button),XtY(expanded_button),
      XtWidth(expanded_button),XtHeight(expanded_button),AllPlanes,ZPixmap);CHECK(image);
   CHECK(XGetPixel(image,0,0)==bg);
   CHECK(XGetPixel(image,image->width-1,image->height-1)==bg);
   default_image=image;
  }
  /* An explicit equal-valued pixmap still transfers ownership to the app. */
  Pixmap original;XtVaGetValues(container,XmNexpandedStatePixmap,&original,NULL);
  XtVaSetValues(container,XmNexpandedStatePixmap,original,NULL);drain(app,d);
  CHECK(!_XmContainerDrawDisclosure(expanded_button));
  if(default_image) {
   XImage *explicit_image=XGetImage(d,XtWindow(container),XtX(expanded_button),XtY(expanded_button),
     default_image->width,default_image->height,AllPlanes,ZPixmap);CHECK(explicit_image);
   unsigned differences=0;
   for(int y=0;y<default_image->height;y++)for(int x=0;x<default_image->width;x++)
     differences+=XGetPixel(default_image,x,y)!=XGetPixel(explicit_image,x,y);
   CHECK(differences>0);XDestroyImage(default_image);XDestroyImage(explicit_image);
  }
  CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);
  CHECK(!_XmContainerDrawDisclosure(collapsed_button));
  CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));drain(app,d);
  CHECK(!_XmContainerDrawDisclosure(expanded_button));
  CHECK(_XmContainerDrawDisclosure(collapsed_button));
  Pixmap icons[4];
  for(unsigned i=0;i<4;i++) {
   icons[i]=XCreatePixmap(d,DefaultRootWindow(d),7+i,7+i,DefaultDepth(d,DefaultScreen(d)));
   CHECK(icons[i]); GC gc=XCreateGC(d,icons[i],0,NULL);
   XSetForeground(d,gc,i%2?WhitePixel(d,DefaultScreen(d)):BlackPixel(d,DefaultScreen(d)));
   XFillRectangle(d,icons[i],gc,0,0,7+i,7+i);XFreeGC(d,gc);
  }
  for(unsigned update=0;update<2;update++) {
   XtVaSetValues(container,XmNexpandedStatePixmap,icons[2*update],XmNcollapsedStatePixmap,icons[2*update+1],NULL);
   fprintf(stderr,"UPDATE %u self=%u expanded=%lu collapsed=%lu expected=%lu\n",update,((XmContainerWidget)container)->container.self,((XmContainerWidget)container)->container.expanded_state_pixmap,((XmContainerWidget)container)->container.collapsed_state_pixmap,icons[2*update]);
   drain(app,d);check_icon(rows[0],icons[2*update]);check_icon(rows[1],icons[2*update+1]);
   CHECK(!_XmContainerDrawDisclosure(expanded_button));
   CHECK(!_XmContainerDrawDisclosure(collapsed_button));
  }
  XtVaSetValues(rows[0],XmNoutlineState,XmCOLLAPSED,NULL);drain(app,d);check_icon(rows[0],icons[3]);
  XtVaSetValues(rows[0],XmNoutlineState,XmEXPANDED,NULL);drain(app,d);check_icon(rows[0],icons[2]);
  XtDestroyWidget(shell);drain(app,d);for(unsigned i=0;i<4;i++)XFreePixmap(d,icons[i]);cases++;
 }
 XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS Container disclosure: %u scenarios\n",cases);return 0;
}
