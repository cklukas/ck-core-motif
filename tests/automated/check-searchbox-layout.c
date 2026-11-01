/* Search decorations participate in native field geometry and editing. */
#include <Xm/XmAll.h>
#include <Xm/SearchBoxP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x); exit(1); } } while(0)
static void drain(XtAppContext app, Display *d) {
    XSync(d,False); while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll); XSync(d,False);
}
static void button(Widget w, int type, int x, int y, unsigned which) {
    XEvent e; memset(&e,0,sizeof(e)); e.xbutton.type=type;
    e.xbutton.display=XtDisplay(w);e.xbutton.window=XtWindow(w);
    e.xbutton.root=RootWindowOfScreen(XtScreen(w));e.xbutton.same_screen=True;
    e.xbutton.button=which;e.xbutton.x=x;e.xbutton.y=y;XtDispatchEvent(&e);
}
static void value_is(Widget w,const char *expected) {
    char *value=XmSearchBoxGetString(w);CHECK(value && strcmp(value,expected)==0);XtFree(value);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;
    const double scales[]={1,1.5,2};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"searchLayout","SearchLayout",NULL,0,&argc,argv);CHECK(d);
    for(unsigned theme=0;theme<3;++theme)for(unsigned sc=0;sc<3;++sc)
    for(unsigned rtl=0;rtl<2;++rtl)for(unsigned clear=0;clear<2;++clear){
        Widget shell,row,field,plain;XmSearchBoxWidget box;XRectangle rect,plain_rect;
        Dimension width,height,plain_width,left,right;Position cx,cy;XmString hint;
        CHECK(XmSetDisplayScaleOverride(d,scales[sc]));
        CHECK(XmApplyAppearanceProfile(d,theme==0?XmAPPEARANCE_CLASSIC:
                theme==1?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
        shell=XtAppCreateShell("searchLayout","SearchLayout",applicationShellWidgetClass,d,NULL,0);
        row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,shell,XmNadjustLast,False,NULL);
        hint=XmStringCreateLocalized("Search documents");
        field=XtVaCreateManagedWidget("search",xmSearchBoxWidgetClass,row,
            XmNsearchPlaceholder,hint,XmNsearchClearButton,clear,XmNcolumns,20,
            XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        XmStringFree(hint);
        plain=XtVaCreateManagedWidget("plain",xmTextFieldWidgetClass,row,XmNcolumns,20,NULL);
        box=(XmSearchBoxWidget)field;
        XtRealizeWidget(shell);drain(app,d);
        XtVaGetValues(field,XmNwidth,&width,XmNheight,&height,NULL);
        XtVaGetValues(plain,XmNwidth,&plain_width,NULL);
        CHECK(XmWidgetGetDisplayRect(field,&rect));CHECK(XmWidgetGetDisplayRect(plain,&plain_rect));
        xmSearchBoxClassRec.text_class.decoration_insets(field,&left,&right);
        fprintf(stderr,"STATE theme=%u scale=%g rtl=%u clear=%u rect=%d,%u plain=%d,%u lanes=%u,%u margin=%u\n",theme,scales[sc],rtl,clear,rect.x,rect.width,plain_rect.x,plain_rect.width,left,right,box->text.margin_width);
        CHECK(rect.x==plain_rect.x+left);
        CHECK(rect.width==width-2*plain_rect.x-left-right);
        CHECK(rect.height==plain_rect.height);
        {
            XtWidgetGeometry pref={0},base={0},intended={0};
            XtQueryGeometry(field,&intended,&pref);XtQueryGeometry(plain,&intended,&base);
            CHECK(pref.width==base.width+left+right);
        }
        /* Empty placeholder must actually draw inside the editor viewport. */
        {
            XImage *img=XGetImage(d,XtWindow(field),rect.x,rect.y,rect.width,rect.height,AllPlanes,ZPixmap);
            unsigned ink=0;CHECK(img);
            for(unsigned y=0;y<rect.height;++y)for(unsigned x=0;x<rect.width;++x)
                if(XGetPixel(img,x,y)!=box->core.background_pixel)++ink;
            CHECK(ink>5);XDestroyImage(img);
        }
        XmSearchBoxSetString(field,"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
        XmTextFieldSetInsertionPosition(field,62);drain(app,d);
        CHECK(XmTextFieldPosToXY(field,62,&cx,&cy));
        CHECK(cx>=rect.x && cx<=rect.x+rect.width);
        XmTextFieldSetInsertionPosition(field,0);drain(app,d);
        CHECK(XmTextFieldPosToXY(field,0,&cx,&cy));CHECK(cx>=rect.x && cx<=rect.x+rect.width);
        XmSearchBoxSetString(field,"before");drain(app,d);
        {
            unsigned size=box->text.font_ascent+box->text.font_descent;
            int border=box->primitive.shadow_thickness+box->primitive.highlight_thickness;
            int x=rtl?border+box->text.margin_width+size/2:
                width-border-box->text.margin_width-size/2;
            /* Read-only is an editing contract, including the clear decoration.
             * Changing editability during a press must cancel that gesture. */
            XtVaSetValues(field,XmNeditable,False,NULL);
            button(field,ButtonPress,x,height/2,Button1);
            button(field,ButtonRelease,x,height/2,Button1);value_is(field,"before");
            {
                XEvent e;memset(&e,0,sizeof(e));e.xkey.type=KeyPress;
                e.xkey.display=d;e.xkey.window=XtWindow(field);
                e.xkey.root=RootWindowOfScreen(XtScreen(field));e.xkey.same_screen=True;
                e.xkey.keycode=XKeysymToKeycode(d,XK_Escape);
                XtDispatchEvent(&e);value_is(field,"before");
            }
            XtVaSetValues(field,XmNeditable,True,NULL);
            button(field,ButtonPress,x,height/2,Button1);
            XtVaSetValues(field,XmNeditable,False,NULL);
            XtVaSetValues(field,XmNeditable,True,NULL);
            button(field,ButtonRelease,x,height/2,Button1);value_is(field,"before");
            button(field,ButtonPress,x,height/2,Button3);
            button(field,ButtonRelease,x,height/2,Button3);value_is(field,"before");
            button(field,ButtonPress,x,height/2,Button1);value_is(field,"before");
            button(field,ButtonRelease,x,height/2,Button1);value_is(field,clear?"":"before");
            XmSearchBoxSetString(field,"before");
            button(field,ButtonPress,x,height/2,Button1);
            button(field,ButtonRelease,0,0,Button1);value_is(field,"before");
        }
        /* Live clear-mode changes recompute viewport and preserve ordinary margins. */
        {
            Dimension margin=box->text.margin_width;XtVaSetValues(field,XmNsearchClearButton,!clear,NULL);
            CHECK(box->text.margin_width==margin);CHECK(XmWidgetGetDisplayRect(field,&rect));
            xmSearchBoxClassRec.text_class.decoration_insets(field,&left,&right);
            CHECK(rect.width==box->core.width-2*plain_rect.x-left-right);
        }
        XtResizeWidget(field,5,5,0);drain(app,d);
        CHECK(XmWidgetGetDisplayRect(field,&rect));CHECK(rect.width==0 && rect.height==0);
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS searchbox layout: %u configurations\n",cases);return 0;
}
