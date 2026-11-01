/* Screen black/white pixel IDs have no meaning in an application colormap. */
#include <Xm/ToggleSwitchP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
int main(int argc,char **argv){
 XtAppContext app;Widget initial=XtVaAppInitialize(&app,"SwitchColormap",NULL,0,&argc,argv,NULL,NULL);
 Display *d=XtDisplay(initial);Screen *screen=XtScreen(initial);
 CHECK(DefaultVisualOfScreen(screen)->class==PseudoColor);
 CHECK(XmSetDisplayScaleOverride(d,1));CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
 Colormap cmap=XCreateColormap(d,RootWindowOfScreen(screen),DefaultVisualOfScreen(screen),AllocAll);
 XColor colors[256];for(unsigned i=0;i<256;i++)colors[i]=(XColor){.pixel=i,.red=0,.green=0,.blue=0,.flags=DoRed|DoGreen|DoBlue};
 colors[BlackPixelOfScreen(screen)].red=65535;
 colors[WhitePixelOfScreen(screen)].green=65535;
 CHECK(BlackPixelOfScreen(screen)!=3 && WhitePixelOfScreen(screen)!=3);
 colors[3].red=colors[3].green=colors[3].blue=65535;
 colors[4].blue=65535;
 XStoreColors(d,cmap,colors,256);
 Widget shell=XtVaAppCreateShell("custom","SwitchColormap",applicationShellWidgetClass,d,XmNcolormap,cmap,XmNbackground,2,NULL);
 Widget w=XtVaCreateManagedWidget("switch",xmToggleSwitchWidgetClass,shell,
  XmNcolormap,cmap,XmNbackground,2,XmNforeground,3,XmNhighlightColor,4,
  XmNtopShadowColor,3,XmNbottomShadowColor,3,XmNset,True,NULL);
 XtRealizeWidget(shell);XmToggleSwitchWidget p=(XmToggleSwitchWidget)w;
 p->primitive.highlighted=False;XtClass(w)->core_class.expose(w,NULL,NULL);XSync(d,False);
 XImage *im=XGetImage(d,XtWindow(w),0,0,p->core.width,p->core.height,AllPlanes,ZPixmap);CHECK(im);
 Pixel thumb=XGetPixel(im,p->core.width-4-12,p->core.height/2);
 fprintf(stderr,"thumb=%lu expected=3 screenWhite=%lu screenBlack=%lu\n",thumb,WhitePixelOfScreen(screen),BlackPixelOfScreen(screen));
 CHECK(thumb==3);
 XDestroyImage(im);XtDestroyWidget(shell);XFreeColormap(d,cmap);XtDestroyWidget(initial);XtCloseDisplay(d);XtDestroyApplicationContext(app);
 puts("PASS switch custom indexed colormap with no free color cells");return 0;
}
