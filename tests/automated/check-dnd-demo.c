/* Native XTEST regression: run only on the runner's isolated display. */
#include <assert.h>
#include <stdlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#define main demo_main
#include "../../demos/programs/drag_and_drop/DNDDemo.c"
#undef main
#include "../../lib/Xm/TransferI.h"
static int stage;
static Pixel color;
static Display *display;
static void pointer(Widget w, int x, int y) {
    Position rx, ry;
    XtTranslateCoords(w, x, y, &rx, &ry);
    XTestFakeMotionEvent(display, -1, rx, ry, 0);
}
static void button(int n, Bool down) { XTestFakeButtonEvent(display,n,down,0); }
static void key(KeySym sym, Bool down) {
    XTestFakeKeyEvent(display, XKeysymToKeycode(display,sym), down,0);
}
static unsigned long pixel(int x, int y) {
    XImage *im = XGetImage(display,XtWindow(drawingArea),x,y,1,1,AllPlanes,ZPixmap);
    unsigned long value = XGetPixel(im,0,0);
    XDestroyImage(im); return value;
}
static void tick(XtPointer unused, XtIntervalId *id) {
    Widget swatch = XtNameToWidget(topLevel,"*PushButton5");
    RectPtr rect;
    fprintf(stderr,"stage %d\n",stage);
    switch(stage) {
    case 0: {
        Atom selection = XmInternAtom(display, XmS_MOTIF_DROP, False);
        Atom target = XmInternAtom(display, XmS_MOTIF_EXPORT_TARGETS, False);
        Atom type; XtPointer value; unsigned long count; int format;
        assert(swatch);
        assert(!_XmConvertHandler(swatch,&selection,&target,&type,&value,&count,&format));
        _XmConvertHandlerSetLocal();
        assert(_XmConvertHandler(swatch,&selection,&target,&type,&value,&count,&format));
        assert(type == XA_ATOM && count == 1 && format == 32);
        XtFree(value);
        assert(!_XmConvertHandler(swatch,&selection,&target,&type,&value,&count,&format));
        assert(value == NULL && count == 0 && type == None);
        pointer(drawingArea,20,20); button(1,True); break;
    }
    case 1: pointer(drawingArea,100,100); break;
    case 2:
        assert(appInfo->creatingRect && appInfo->numRects == 0);
        assert(pixel(60,20) != pixel(60,60));
        XClearArea(display,XtWindow(drawingArea),0,0,0,0,True); break;
    case 3: assert(pixel(60,20) != pixel(60,60)); button(1,False); break;
    case 4:
        assert(appInfo->numRects == 1); rect=RectFind(50,50); assert(rect);
        assert(rect->width==80 && rect->height==80);
        XtVaGetValues(swatch,XmNbackground,&color,NULL);
        pointer(swatch,30,12); button(1,True); break;
    case 5: pointer(drawingArea,50,50); break;
    case 6: button(1,False); break;
    case 7:
        assert(RectFind(50,50)->color == color); assert(pixel(30,30)==color);
        pointer(drawingArea,50,50); button(1,True); break;
    case 8: pointer(drawingArea,180,70); break;
    case 9: button(1,False); break;
    case 10:
        assert(appInfo->numRects==1); assert(!RectFind(50,50));
        rect=RectFind(180,70); assert(rect && rect->color==color);
        assert(rect->width==80 && rect->height==80);
        key(XK_Control_L,True); pointer(drawingArea,180,70); button(2,True); break;
    case 11: pointer(drawingArea,70,70); break;
    case 12: button(2,False); break;
    case 13:
        key(XK_Control_L,False);
        assert(appInfo->numRects==2); assert(RectFind(180,70));
        assert(RectFind(70,70)->color==color);
        pointer(drawingArea,280,160); button(1,True); break;
    case 14: pointer(drawingArea,250,130); break;
    case 15: button(1,False); break;
    case 16:
        rect=RectFind(260,140); assert(rect && rect->color==color);
        assert(rect->width==30 && rect->height==30);
        pointer(drawingArea,10,160); button(1,True); break;
    case 17: pointer(drawingArea,30,140); break;
    case 18: key(XK_Escape,True); key(XK_Escape,False); break;
    case 19: button(1,False); break;
    case 20:
        assert(!appInfo->creatingRect && appInfo->numRects==3);
        pointer(drawingArea,180,70); button(1,True); break;
    case 21: pointer(drawingArea,200,150); key(XK_Escape,True); key(XK_Escape,False); break;
    case 22: button(1,False); break;
    case 23:
        assert(appInfo->numRects==3 && RectFind(180,70));
        puts("PASS: live/exposed preview, color drop, move, middle-button Ctrl-copy, selected drawing color, reverse draw and Escape cancellation");
        exit(0);
    }
    stage++; XFlush(display); XtAppAddTimeOut(appContext,700,tick,NULL);
}
int main(int argc,char **argv) {
    InitializeApplication(&argc,argv); CreateLayout(); RegisterDropSite(drawingArea);
    SetInvalidIcon(drawingArea); XtRealizeWidget(topLevel); CreateRectGC();
    display=XtDisplay(topLevel);
    XSetInputFocus(display,XtWindow(topLevel),RevertToParent,CurrentTime);
    XtAppAddTimeOut(appContext,300,tick,NULL); XtAppMainLoop(appContext); return 1;
}
