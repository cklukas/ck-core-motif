/* SPDX-License-Identifier: LGPL-2.0-or-later
 * Paint: a Motif raster drawing application demo.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Xm/XmAll.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "paint.h"
#include "paint-icons.h"
#include <Xm/Display.h>

enum { PENCIL, BRUSH, ERASER, LINE, RECTANGLE, ELLIPSE, FILL, PICKER, TEXT, SELECT, TOOL_COUNT };
enum { NEW, OPEN, SAVE, SAVE_AS, QUIT, UNDO, REDO, COPY, CUT, PASTE, CLEAR, FIT, ZOOM_IN, ZOOM_OUT, HELP, RESIZE };
static const char *tool_names[]={"Pencil", "Brush", "Eraser", "Line", "Rectangle", "Ellipse", "Fill", "Eyedropper", "Text", "Select"};
static const uint32_t palette[]={0x000000,0x666666,0xffffff,0xc0c0c0,0x923c28,0xe53935,0xff8c00,0xffd600,0x43a047,0x00a9a5,0x1976d2,0x7038b5,0xef68a8,0xffc9a9,0xa9d8ff,0xc5e1a5};
typedef struct {
    XtAppContext context;
    Widget shell, form, canvas, status, hscroll, vscroll, swatch, tools[TOOL_COUNT];
    Widget undo_button, redo_button, file_dialog;
    Display *display; GC gc; XImage *view;
    PaintDocument doc;
    PaintImage preview, clipboard;
    int tool, size, filled, dragging, start_x, start_y, last_x, last_y;
    int selection, sx, sy, ex, ey, paste_mode, dirty;
    int offset_x, offset_y, origin_x, origin_y, view_w, view_h;
    int pending, file_save, text_x, text_y;
    Cursor cross_cursor, text_cursor;
    double zoom, scale;
    uint32_t color, stroke_color;
    char *filename;
} Application;
static Application app;
#ifdef PNG_SUPPORTED
#define FILE_FORMATS "PNG or 24-bit BMP"
#define DEFAULT_FILE "Untitled.png"
#else
#define FILE_FORMATS "24-bit BMP"
#define DEFAULT_FILE "Untitled.bmp"
#endif
static void redraw(void);
static void update(void);
static void command(Widget, XtPointer, XtPointer);
static void request_command(int);
static void cancel_dialog(Widget, XtPointer, XtPointer);
static void label(Widget w, const char *text)
{
    XmString s=XmStringCreateLocalized((char *)text);
    XtVaSetValues(w,XmNlabelString,s,NULL); XmStringFree(s);
}
static void message(const char *text)
{
    Widget d=XmCreateErrorDialog(app.shell,"error",NULL,0);
    XmString s=XmStringCreateLocalized((char *)text);
    XtVaSetValues(d,XmNmessageString,s,XmNdialogTitle,s,NULL); XmStringFree(s);
    XtUnmanageChild(XmMessageBoxGetChild(d,XmDIALOG_HELP_BUTTON));
    XtUnmanageChild(XmMessageBoxGetChild(d,XmDIALOG_CANCEL_BUTTON));
    XtManageChild(d);
}
static int begin_edit(void)
{
    if(!paint_begin(&app.doc)) { message("Not enough memory to retain undo. No changes were made."); return 0; }
    app.dirty=1; return 1;
}
static unsigned long component(unsigned value, unsigned long mask)
{
    unsigned shift=0; unsigned long max;
    if(!mask) return 0;
    while(!(mask & (1UL<<shift))) ++shift;
    max=mask>>shift; return ((value*max+127)/255)<<shift;
}
static unsigned long native_color(uint32_t c)
{
    Visual *v=DefaultVisual(app.display,DefaultScreen(app.display));
    return component(c>>16,v->red_mask)|component((c>>8)&255,v->green_mask)|component(c&255,v->blue_mask);
}
static void scroll_range(Widget w, int total, int visible, int *offset)
{
    int max=total>visible?total:visible;
    if(max<1) max=1;
    if(visible<1) visible=1;
    if(visible>max) visible=max;
    if(*offset>max-visible) *offset=max-visible;
    if(*offset<0) *offset=0;
    XtVaSetValues(w,XmNminimum,0,XmNmaximum,max,XmNsliderSize,visible,
                  XmNvalue,*offset,XmNincrement,24,XmNpageIncrement,visible>24?visible-24:1,NULL);
}
static void layout_view(void)
{
    Dimension w,h; double fit; int dw,dh,hs=0,vs=0,i;
    XtVaGetValues(XtParent(app.canvas),XmNwidth,&w,XmNheight,&h,NULL);
    fit=fmin((double)w/app.doc.image.width,(double)h/app.doc.image.height);
    app.scale=app.zoom?app.zoom:fit;
    if(app.scale<0.01) app.scale=0.01;
    dw=(int)ceil(app.doc.image.width*app.scale); dh=(int)ceil(app.doc.image.height*app.scale);
    for(i=0;i<2;++i) {
        hs=app.zoom && dw>(int)w-vs*20;
        vs=app.zoom && dh>(int)h-hs*20;
    }
    XtVaSetValues(app.canvas,XmNrightOffset,vs*20,XmNbottomOffset,hs*20,NULL);
    XtVaSetValues(app.hscroll,XmNrightOffset,vs*20,NULL);
    XtVaSetValues(app.vscroll,XmNbottomOffset,hs*20,NULL);
    if(hs) XtManageChild(app.hscroll); else XtUnmanageChild(app.hscroll);
    if(vs) XtManageChild(app.vscroll); else XtUnmanageChild(app.vscroll);
    XtVaGetValues(app.canvas,XmNwidth,&w,XmNheight,&h,NULL);
    app.view_w=w; app.view_h=h;
    scroll_range(app.hscroll,dw,w,&app.offset_x); scroll_range(app.vscroll,dh,h,&app.offset_y);
    app.origin_x=dw<w?(w-dw)/2:-app.offset_x;
    app.origin_y=dh<h?(h-dh)/2:-app.offset_y;
}
static void redraw(void)
{
    static int drawing=0;
    int x,y,ix,iy; uint32_t c; const PaintImage *im;
    if(drawing) return;
    if(!app.gc || !XtIsRealized(app.canvas)) return;
    drawing=1;
    layout_view();
    if(app.view && (app.view->width!=app.view_w || app.view->height!=app.view_h)) { XDestroyImage(app.view); app.view=NULL; }
    if(!app.view) {
        app.view=XCreateImage(app.display,DefaultVisual(app.display,DefaultScreen(app.display)),
            DefaultDepth(app.display,DefaultScreen(app.display)),ZPixmap,0,NULL,app.view_w,app.view_h,32,0);
        if(!app.view) { drawing=0; return; }
        app.view->data=calloc((size_t)app.view->bytes_per_line,app.view_h);
        if(!app.view->data) { XDestroyImage(app.view); app.view=NULL; drawing=0; return; }
    }
    im=app.dragging && app.preview.pixels?&app.preview:&app.doc.image;
    for(y=0;y<app.view_h;++y) for(x=0;x<app.view_w;++x) {
        ix=(int)floor((x-app.origin_x)/app.scale); iy=(int)floor((y-app.origin_y)/app.scale);
        c=(ix<0 || iy<0 || ix>=im->width || iy>=im->height)?0xb8bec6:paint_get(im,ix,iy);
        XPutPixel(app.view,x,y,native_color(c));
    }
    XPutImage(app.display,XtWindow(app.canvas),app.gc,app.view,0,0,0,0,app.view_w,app.view_h);
    if(app.selection) {
        int x0=app.sx<app.ex?app.sx:app.ex, y0=app.sy<app.ey?app.sy:app.ey;
        int ww=(int)((abs(app.ex-app.sx)+1)*app.scale), hh=(int)((abs(app.ey-app.sy)+1)*app.scale);
        XSetForeground(app.display,app.gc,native_color(0xffffff));
        XSetLineAttributes(app.display,app.gc,1,LineSolid,CapButt,JoinMiter);
        XDrawRectangle(app.display,XtWindow(app.canvas),app.gc,app.origin_x+(int)(x0*app.scale),app.origin_y+(int)(y0*app.scale),ww,hh);
        XSetForeground(app.display,app.gc,native_color(0x222222));
        XSetLineAttributes(app.display,app.gc,1,LineOnOffDash,CapButt,JoinMiter);
        XDrawRectangle(app.display,XtWindow(app.canvas),app.gc,app.origin_x+(int)(x0*app.scale),app.origin_y+(int)(y0*app.scale),ww,hh);
        XSetLineAttributes(app.display,app.gc,1,LineSolid,CapButt,JoinMiter);
    }
    drawing=0;
}
static void update(void)
{
    char text[1024], title[1024];
    snprintf(text,sizeof(text),"%s%s  |  %d x %d px  |  %s %.0f%%  |  %s",tool_names[app.tool],app.paste_mode?" / click to paste":"",
        app.doc.image.width,app.doc.image.height,app.zoom?"Zoom":"Fit",app.scale*100,
        app.dirty?"Unsaved changes":"Ready");
    label(app.status,text);
    snprintf(title,sizeof(title),"%s%s - Motif Paint",app.dirty?"* ":"",app.filename?app.filename:"Untitled");
    XtVaSetValues(app.shell,XmNtitle,title,NULL);
    XtSetSensitive(app.undo_button,app.doc.undo_count>0); XtSetSensitive(app.redo_button,app.doc.redo_count>0);
    XtVaSetValues(app.swatch,XmNbackground,native_color(app.color),XmNforeground,native_color(app.color==0?0xffffff:0),NULL);
}
static void expose(Widget w, XtPointer p, XtPointer call) { (void)w;(void)p;(void)call; redraw(); update(); }
static void scroll(Widget w, XtPointer p, XtPointer call)
{
    XmScrollBarCallbackStruct *c=call; (void)p;
    if(w==app.hscroll) app.offset_x=c->value; else app.offset_y=c->value;
    redraw();
}
static void tool_changed(Widget w, XtPointer p, XtPointer call)
{
    int i; XmToggleButtonCallbackStruct *c=call; (void)w;
    if(!c->set) { XmToggleButtonSetState(w,True,False); return; }
    app.tool=(int)(intptr_t)p; app.paste_mode=0; app.selection=0;
    for(i=0;i<TOOL_COUNT;++i) XmToggleButtonSetState(app.tools[i],i==app.tool,False);
    if(app.gc) XDefineCursor(app.display,XtWindow(app.canvas),app.tool==TEXT?app.text_cursor:app.cross_cursor);
    redraw(); update();
}
static void color_changed(Widget w, XtPointer p, XtPointer c) { (void)w;(void)c; app.color=palette[(int)(intptr_t)p]; update(); }
static void custom_color_ok(Widget w, XtPointer p, XtPointer call)
{
    XmSelectionBoxCallbackStruct *c=call; char *text=NULL,*end; unsigned long color; (void)p;
    if(!XmStringGetLtoR(c->value,XmFONTLIST_DEFAULT_TAG,&text)) return;
    if(text[0]!='#' || strlen(text)!=7) { XtFree(text);message("Enter a color as #RRGGBB, for example #1976D2.");return; }
    color=strtoul(text+1,&end,16);
    if(*end || color>0xffffff) { XtFree(text);message("Use six hexadecimal digits after #.");return; }
    XtFree(text);app.color=(uint32_t)color;XtDestroyWidget(w);update();
}
static void custom_color(Widget w, XtPointer p, XtPointer call)
{
    Widget d=XmCreatePromptDialog(app.shell,"customColor",NULL,0); char hex[16]; XmString s;
    (void)w;(void)p;(void)call;
    s=XmStringCreateLocalized("Ink color (#RRGGBB)");XtVaSetValues(d,XmNselectionLabelString,s,XmNautoUnmanage,False,NULL);XmStringFree(s);
    snprintf(hex,sizeof(hex),"#%06X",app.color);s=XmStringCreateLocalized(hex);XtVaSetValues(d,XmNtextString,s,NULL);XmStringFree(s);
    XtUnmanageChild(XmSelectionBoxGetChild(d,XmDIALOG_HELP_BUTTON));
    XtAddCallback(d,XmNokCallback,custom_color_ok,NULL);
    XtAddCallback(d,XmNcancelCallback,cancel_dialog,NULL);XtManageChild(d);
}
static void size_changed(Widget w, XtPointer p, XtPointer c) { (void)w;(void)p; app.size=((XmScaleCallbackStruct *)c)->value; }
static void filled_changed(Widget w, XtPointer p, XtPointer c) { (void)w;(void)p; app.filled=((XmToggleButtonCallbackStruct *)c)->set; }
static void shape(PaintImage *im,int x,int y)
{
    switch(app.tool) {
    case LINE: paint_line(im,app.start_x,app.start_y,x,y,app.size,app.stroke_color); break;
    case RECTANGLE: paint_rectangle(im,app.start_x,app.start_y,x,y,app.size,app.filled,app.stroke_color); break;
    case ELLIPSE: paint_ellipse(im,app.start_x,app.start_y,x,y,app.size,app.filled,app.stroke_color); break;
    }
}
static void text_ok(Widget w, XtPointer p, XtPointer call)
{
    XmSelectionBoxCallbackStruct *c=call; char *s=NULL; XFontStruct *font;
    Pixmap pix; GC gc; XImage *im; int width,height,x,y; (void)p;
    if(!XmStringGetLtoR(c->value,XmFONTLIST_DEFAULT_TAG,&s) || !s || !*s) { if(s) XtFree(s); return; }
    font=XLoadQueryFont(app.display,"9x15"); if(!font) font=XLoadQueryFont(app.display,"fixed");
    if(!font) { XtFree(s); message("No X11 bitmap font is available."); return; }
    width=XTextWidth(font,s,(int)strlen(s)); height=font->ascent+font->descent;
    if(width<1 || width>PAINT_MAX_SIDE) { XFreeFont(app.display,font); XtFree(s); message("Text is too long."); return; }
    pix=XCreatePixmap(app.display,XtWindow(app.canvas),width,height,1); gc=XCreateGC(app.display,pix,0,NULL);
    XSetForeground(app.display,gc,0); XFillRectangle(app.display,pix,gc,0,0,width,height);
    XSetForeground(app.display,gc,1); XSetFont(app.display,gc,font->fid);
    XDrawString(app.display,pix,gc,0,font->ascent,s,(int)strlen(s));
    im=XGetImage(app.display,pix,0,0,width,height,1,ZPixmap);
    if(im && begin_edit()) {
        for(y=0;y<height;++y) for(x=0;x<width;++x)
            if(XGetPixel(im,x,y)) paint_dot(&app.doc.image,app.text_x+x,app.text_y+y,1,app.color);
    }
    if(im) XDestroyImage(im);
    XFreeGC(app.display,gc); XFreePixmap(app.display,pix); XFreeFont(app.display,font); XtFree(s);
    XtDestroyWidget(w); redraw(); update();
}
static void cancel_dialog(Widget w, XtPointer p, XtPointer c) { (void)p;(void)c; XtDestroyWidget(w); }
static void prompt_text(int x,int y)
{
    Widget d=XmCreatePromptDialog(app.shell,"text",NULL,0); XmString s=XmStringCreateLocalized("Text (X11 bitmap font)");
    app.text_x=x; app.text_y=y;
    XtVaSetValues(d,XmNselectionLabelString,s,XmNdialogTitle,s,NULL); XmStringFree(s);
    XtUnmanageChild(XmSelectionBoxGetChild(d,XmDIALOG_HELP_BUTTON));
    XtAddCallback(d,XmNokCallback,text_ok,NULL); XtAddCallback(d,XmNcancelCallback,cancel_dialog,NULL); XtManageChild(d);
}
static void canvas_event(Widget w, XtPointer p, XEvent *e, Boolean *cont)
{
    int x,y,inside; (void)w;(void)p;(void)cont;
    if(e->type!=ButtonPress && e->type!=ButtonRelease && e->type!=MotionNotify) return;
    x=e->type==MotionNotify?e->xmotion.x:e->xbutton.x;
    y=e->type==MotionNotify?e->xmotion.y:e->xbutton.y;
    x=(int)floor((x-app.origin_x)/app.scale); y=(int)floor((y-app.origin_y)/app.scale);
    inside=x>=0 && y>=0 && x<app.doc.image.width && y<app.doc.image.height;
    if(e->type==ButtonPress && (e->xbutton.button==4 || e->xbutton.button==5)) {
        app.offset_y+=e->xbutton.button==4?-48:48; redraw(); return;
    }
    if(x<0)x=0; if(y<0)y=0;
    if(x>=app.doc.image.width)x=app.doc.image.width-1;
    if(y>=app.doc.image.height)y=app.doc.image.height-1;
    if(e->type==ButtonPress) {
        if(!inside || (e->xbutton.button!=1 && e->xbutton.button!=3)) return;
        XmProcessTraversal(app.canvas,XmTRAVERSE_CURRENT);
        app.stroke_color=e->xbutton.button==3?PAINT_WHITE:app.color;
        if(app.paste_mode) {
            if(begin_edit()) paint_paste(&app.doc.image,&app.clipboard,x,y);
            app.paste_mode=0; app.selection=0;
        } else if(app.tool==PICKER) app.color=paint_get(&app.doc.image,x,y);
        else if(app.tool==TEXT) prompt_text(x,y);
        else if(app.tool==FILL) {
            if(paint_get(&app.doc.image,x,y)!=app.stroke_color && begin_edit() && !paint_fill(&app.doc.image,x,y,app.stroke_color)) {
                paint_undo(&app.doc); message("Not enough memory to fill this region.");
            }
        } else {
            app.start_x=app.last_x=x; app.start_y=app.last_y=y;
            if(app.tool==SELECT) { app.selection=1; app.sx=app.ex=x; app.sy=app.ey=y; app.dragging=1; }
            else if(begin_edit()) {
                app.selection=0; app.dragging=1;
                if(app.tool>=LINE && app.tool<=ELLIPSE) {
                    if(!paint_image_copy(&app.preview,&app.doc.image)) { paint_undo(&app.doc); app.dragging=0; message("Not enough memory for a shape preview."); }
                } else paint_dot(&app.doc.image,x,y,app.tool==PENCIL?1:app.size,app.tool==ERASER?PAINT_WHITE:app.stroke_color);
            }
        }
    } else if(app.dragging) {
        if(app.tool==SELECT) { app.ex=x; app.ey=y; }
        else if(app.tool>=LINE && app.tool<=ELLIPSE) {
            memcpy(app.preview.pixels,app.doc.image.pixels,(size_t)app.doc.image.width*app.doc.image.height*sizeof(uint32_t)); shape(&app.preview,x,y);
        } else paint_line(&app.doc.image,app.last_x,app.last_y,x,y,app.tool==PENCIL?1:app.size,app.tool==ERASER?PAINT_WHITE:app.stroke_color);
        app.last_x=x; app.last_y=y;
        if(e->type==ButtonRelease) {
            if(app.preview.pixels) { paint_image_free(&app.doc.image); app.doc.image=app.preview; memset(&app.preview,0,sizeof(app.preview)); }
            app.dragging=0;
        }
    }
    redraw(); update();
}
static void discard_ok(Widget w, XtPointer p, XtPointer c)
{ int action=(int)(intptr_t)p; (void)c; XtDestroyWidget(w); app.pending=1; request_command(action); app.pending=0; }
static void confirm_discard(int action)
{
    Widget d=XmCreateQuestionDialog(app.shell,"unsaved",NULL,0);
    XmString s=XmStringCreateLocalized("Discard unsaved changes? Cancel to return and save your picture.");
    XmString ok=XmStringCreateLocalized("Discard");
    XtVaSetValues(d,XmNmessageString,s,XmNokLabelString,ok,XmNdefaultButtonType,XmDIALOG_CANCEL_BUTTON,NULL);
    XmStringFree(s); XmStringFree(ok); XtUnmanageChild(XmMessageBoxGetChild(d,XmDIALOG_HELP_BUTTON));
    XtAddCallback(d,XmNokCallback,discard_ok,(XtPointer)(intptr_t)action);
    XtAddCallback(d,XmNcancelCallback,cancel_dialog,NULL); XtManageChild(d);
}
static int save_file(const char *path)
{
    char error[256];
    if(!paint_save(&app.doc.image,path,error,sizeof(error))) { message(error); return 0; }
    free(app.filename); app.filename=strdup(path); app.dirty=0; update(); return 1;
}
static void overwrite_ok(Widget w, XtPointer p, XtPointer c)
{ char *path=p; (void)c; if(save_file(path) && app.file_dialog) XtUnmanageChild(app.file_dialog); free(path); XtDestroyWidget(w); }
static void overwrite_cancel(Widget w, XtPointer p, XtPointer c) { (void)c; free(p); XtDestroyWidget(w); }
static void file_ok(Widget w, XtPointer p, XtPointer call)
{
    XmFileSelectionBoxCallbackStruct *c=call; char *path=NULL,error[256]; PaintImage image={0}; FILE *f; (void)p;
    if(!XmStringGetLtoR(c->value,XmFONTLIST_DEFAULT_TAG,&path) || !path) return;
    if(app.file_save) {
        f=fopen(path,"rb");
        if(f) {
            Widget d=XmCreateQuestionDialog(app.shell,"overwrite",NULL,0); XmString s=XmStringCreateLocalized("Replace the existing file?");
            fclose(f); XtVaSetValues(d,XmNmessageString,s,XmNdefaultButtonType,XmDIALOG_CANCEL_BUTTON,NULL); XmStringFree(s);
            XtUnmanageChild(XmMessageBoxGetChild(d,XmDIALOG_HELP_BUTTON));
            { char *owned=strdup(path);
              XtAddCallback(d,XmNokCallback,overwrite_ok,owned);
              XtAddCallback(d,XmNcancelCallback,overwrite_cancel,owned); }
            XtManageChild(d);
        } else if(save_file(path)) XtUnmanageChild(w);
    } else if(paint_load(&image,path,error,sizeof(error))) {
        paint_document_free(&app.doc); app.doc.image=image;
        free(app.filename); app.filename=strdup(path); app.dirty=0; app.selection=0; app.zoom=0; app.offset_x=app.offset_y=0;
        XtUnmanageChild(w); redraw(); update();
    } else message(error);
    XtFree(path);
}
static void file_cancel(Widget w, XtPointer p, XtPointer c) { (void)p;(void)c; XtUnmanageChild(w); }
static void file_dialog(int saving)
{
    XmString s;
    if(!app.file_dialog) {
        app.file_dialog=XmCreateFileSelectionDialog(app.shell,"file",NULL,0);
        XtUnmanageChild(XmFileSelectionBoxGetChild(app.file_dialog,XmDIALOG_HELP_BUTTON));
        XtAddCallback(app.file_dialog,XmNokCallback,file_ok,NULL); XtAddCallback(app.file_dialog,XmNcancelCallback,file_cancel,NULL);
        XtVaSetValues(app.file_dialog,XmNautoUnmanage,False,NULL);
    }
    app.file_save=saving; s=XmStringCreateLocalized(saving?"Save picture - " FILE_FORMATS:"Open picture - " FILE_FORMATS);
    XtVaSetValues(app.file_dialog,XmNdialogTitle,s,NULL); XmStringFree(s);
    s=XmStringCreateLocalized("*"); XtVaSetValues(app.file_dialog,XmNpattern,s,NULL); XmStringFree(s);
    if(saving) { s=XmStringCreateLocalized(app.filename?app.filename:DEFAULT_FILE); XtVaSetValues(app.file_dialog,XmNdirSpec,s,NULL); XmStringFree(s); }
    XtManageChild(app.file_dialog);
}
static void resize_ok(Widget w, XtPointer p, XtPointer call)
{
    XmSelectionBoxCallbackStruct *c=call; char *s=NULL,tail; int width,height; PaintImage im={0}; (void)p;
    if(!XmStringGetLtoR(c->value,XmFONTLIST_DEFAULT_TAG,&s)) return;
    if(sscanf(s," %d x %d %c",&width,&height,&tail)!=2 || !paint_image_create(&im,width,height)) {
        XtFree(s); message("Enter width x height, from 1 x 1 to 4096 x 4096."); return;
    }
    XtFree(s);
    if(begin_edit()) { paint_paste(&im,&app.doc.image,0,0); paint_image_free(&app.doc.image); app.doc.image=im; app.selection=0; app.zoom=0; }
    else paint_image_free(&im);
    XtDestroyWidget(w); redraw(); update();
}
static void request_command(int action)
{
    int x,y,width,height; Widget d; XmString s;
    if(app.dragging) return;
    if((action==NEW || action==OPEN || action==QUIT) && app.dirty && !app.pending) { confirm_discard(action); return; }
    switch(action) {
    case NEW: {
        PaintImage im={0}; if(!paint_image_create(&im,960,640)) { message("Not enough memory."); break; }
        paint_document_free(&app.doc); app.doc.image=im; free(app.filename); app.filename=NULL;
        app.selection=0; app.zoom=0; app.dirty=0; break;
    }
    case OPEN: file_dialog(0); break;
    case SAVE: if(app.filename) { char *path=strdup(app.filename); save_file(path); free(path); } else file_dialog(1); break;
    case SAVE_AS: file_dialog(1); break;
    case QUIT: XtDestroyWidget(app.shell); paint_document_free(&app.doc); exit(0);
    case UNDO: if(paint_undo(&app.doc)) { app.dirty=1; app.selection=0; } break;
    case REDO: if(paint_redo(&app.doc)) { app.dirty=1; app.selection=0; } break;
    case COPY: case CUT:
        if(!app.selection) { message("Drag a rectangle with Select first."); break; }
        x=app.sx<app.ex?app.sx:app.ex; y=app.sy<app.ey?app.sy:app.ey;
        width=abs(app.ex-app.sx)+1; height=abs(app.ey-app.sy)+1;
        if(!paint_crop(&app.clipboard,&app.doc.image,x,y,width,height)) { message("Not enough memory to copy."); break; }
        if(action==CUT && begin_edit()) paint_rectangle(&app.doc.image,x,y,x+width-1,y+height-1,1,1,PAINT_WHITE);
        break;
    case PASTE:
        if(!app.clipboard.pixels) message("Copy or cut a selection first. The clipboard belongs to this drawing session.");
        else { app.paste_mode=1; app.selection=0; } break;
    case CLEAR:
        if(begin_edit()) {
            if(app.selection) paint_rectangle(&app.doc.image,app.sx,app.sy,app.ex,app.ey,1,1,PAINT_WHITE);
            else paint_rectangle(&app.doc.image,0,0,app.doc.image.width-1,app.doc.image.height-1,1,1,PAINT_WHITE);
        } break;
    case FIT: app.zoom=0; app.offset_x=app.offset_y=0; break;
    case ZOOM_IN: app.zoom=fmin(8,app.scale*1.5); break;
    case ZOOM_OUT: app.zoom=fmax(0.125,app.scale/1.5); break;
    case RESIZE: {
        char text[64]; d=XmCreatePromptDialog(app.shell,"canvasSize",NULL,0);
        s=XmStringCreateLocalized("Canvas width x height (crop or extend, without scaling)");
        XtVaSetValues(d,XmNselectionLabelString,s,XmNautoUnmanage,False,NULL); XmStringFree(s);
        snprintf(text,sizeof(text),"%d x %d",app.doc.image.width,app.doc.image.height); s=XmStringCreateLocalized(text);
        XtVaSetValues(d,XmNtextString,s,NULL); XmStringFree(s);
        XtUnmanageChild(XmSelectionBoxGetChild(d,XmDIALOG_HELP_BUTTON));
        XtAddCallback(d,XmNokCallback,resize_ok,NULL); XtAddCallback(d,XmNcancelCallback,cancel_dialog,NULL); XtManageChild(d); break;
    }
    case HELP:
        d=XmCreateInformationDialog(app.shell,"help",NULL,0);
        s=XmStringCreateLocalized("MOTIF PAINT\n\nChoose a tool at the left, a color above, and drag on the white canvas.\nPencil is 1 pixel; Brush, Eraser and shapes use Size.\nSolid shapes fills rectangles and ellipses. Right-drag paints white.\nFill colors a connected region. Eyedropper picks a pixel color.\nText places a bitmap text label at the clicked point.\n\nSelect: drag a rectangle, then Copy/Cut. Paste: click its new position.\nThe selection clipboard is local to this application.\nOpen and Save use " FILE_FORMATS ".\nCanvas size crops or extends the document, without stretching artwork.\nFit shows the whole picture. Zoom enables scrolling only when needed.\n\nCtrl+N/O/S: New/Open/Save   Ctrl+Shift+S: Save As\nCtrl+Z/Y: Undo/Redo   Ctrl+C/X/V: Copy/Cut/Paste\nDelete: Clear selection (or canvas)   Escape: Cancel gesture/paste\n+/-: Zoom   0: Fit\n\nUndo retains the most recent 16 edits, including canvas size changes.");
        XtVaSetValues(d,XmNmessageString,s,NULL); XmStringFree(s);
        XtUnmanageChild(XmMessageBoxGetChild(d,XmDIALOG_CANCEL_BUTTON)); XtUnmanageChild(XmMessageBoxGetChild(d,XmDIALOG_HELP_BUTTON)); XtManageChild(d); break;
    }
    redraw(); update();
}
static void command(Widget w, XtPointer p, XtPointer c) { (void)w;(void)c; request_command((int)(intptr_t)p); }
static void key_event(Widget w, XtPointer p, XEvent *e, Boolean *cont)
{
    KeySym k; int action=-1; (void)w;(void)p;
    if(e->type!=KeyPress) return;
    k=XLookupKeysym(&e->xkey,0);
    if(e->xkey.state&ControlMask) {
        switch(k) { case XK_n: action=NEW;break; case XK_o:action=OPEN;break; case XK_s:action=(e->xkey.state&ShiftMask)?SAVE_AS:SAVE;break;
        case XK_z:action=(e->xkey.state&ShiftMask)?REDO:UNDO;break;case XK_y:action=REDO;break;case XK_c:action=COPY;break;case XK_x:action=CUT;break;case XK_v:action=PASTE;break; }
    } else if(k==XK_Delete || k==XK_BackSpace) action=CLEAR;
    else if(k==XK_plus || k==XK_equal || k==XK_KP_Add) action=ZOOM_IN;
    else if(k==XK_minus || k==XK_KP_Subtract) action=ZOOM_OUT;
    else if(k==XK_0) action=FIT;
    else if(k==XK_Escape) {
        if(app.dragging && app.tool!=SELECT) {
            paint_undo(&app.doc);
            if(app.doc.redo_count) paint_image_free(&app.doc.redo[--app.doc.redo_count]);
        }
        app.dragging=app.selection=app.paste_mode=0; paint_image_free(&app.preview); redraw(); update(); *cont=False;
    }
    if(action>=0) { request_command(action); *cont=False; }
}
static void tooltip(Widget w,const char *text)
{
    XmString tip=XmStringCreateLocalized((char *)text);
    XtVaSetValues(w,XmNtoolTipString,tip,NULL);XmStringFree(tip);
}
static Widget action_button(Widget parent,const char *name,int action,int icon,const char *tip)
{
    Widget w=XtVaCreateManagedWidget(name,xmPushButtonWidgetClass,parent,NULL);
    paint_button_icon(w,icon,False);tooltip(w,tip);
    XtAddCallback(w,XmNactivateCallback,command,(XtPointer)(intptr_t)action);return w;
}
static Widget button(Widget parent,const char *name,int action)
{
    Widget w=XtVaCreateManagedWidget(name,xmPushButtonWidgetClass,parent,NULL);
    XtAddCallback(w,XmNactivateCallback,command,(XtPointer)(intptr_t)action); return w;
}
static Widget menu(Widget bar,const char *name)
{
    Widget pane=XmCreatePulldownMenu(bar,(char *)name,NULL,0);
    XtVaCreateManagedWidget(name,xmCascadeButtonWidgetClass,bar,XmNsubMenuId,pane,NULL); return pane;
}
static void install_keys(Widget w)
{
    WidgetList children; Cardinal count,i;
    if(!XtIsWidget(w)) return;
    XtAddEventHandler(w,KeyPressMask,False,key_event,NULL);
    if(XtIsComposite(w)) {
        XtVaGetValues(w,XmNchildren,&children,XmNnumChildren,&count,NULL);
        for(i=0;i<count;++i) install_keys(children[i]);
    }
}
static void create_ui(int *argc,char **argv)
{
    Widget bar,pane,top,colors,rail,frame,size,filled,size_label; int i; Atom wm_delete;
    double ui_scale=1; int unit,gap,tile,swatch,grid_x,options_x;
    String resources[]={"*XmSelectionBox.dialogStyle: dialog_full_application_modal", "*XmFileSelectionBox.dialogStyle: dialog_full_application_modal", "*XmMessageBox.dialogStyle: dialog_full_application_modal", "*fontList: -*-helvetica-medium-r-normal--14-*-*-*-*-*-*-*", "*highlightThickness: 1", NULL};
    app.shell=XtVaAppInitialize(&app.context,"MotifPaint",NULL,0,argc,argv,resources,
        XmNtitle,"Motif Paint",XmNwidth,1100,XmNheight,800,XmNminWidth,900,XmNminHeight,540,
        XmNdeleteResponse,XmDO_NOTHING,XmNtoolTipEnable,True,XmNtoolTipPostDelay,500,XmNtoolTipPostDuration,5000,NULL);
    app.display=XtDisplay(app.shell);
    if(DefaultVisual(app.display,DefaultScreen(app.display))->class!=TrueColor) {
        fprintf(stderr,"Motif Paint needs a TrueColor X11 display.\n"); exit(1);
    }
    XmGetWidgetScale(app.shell,&ui_scale);
    unit=(int)lround(4*ui_scale);gap=unit;tile=(int)lround(38*ui_scale);swatch=(int)lround(23*ui_scale);
    grid_x=(int)lround(52*ui_scale);options_x=grid_x+8*(swatch+gap)+3*gap;
    app.form=XtVaCreateManagedWidget("paint",xmFormWidgetClass,app.shell,XmNfractionBase,100,NULL);
    bar=XtVaCreateManagedWidget("menu",xmRowColumnWidgetClass,app.form,XmNrowColumnType,XmMENU_BAR,
        XmNtopAttachment,XmATTACH_FORM,XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,NULL);
    pane=menu(bar,"File"); button(pane,"New",NEW);button(pane,"Open...",OPEN);button(pane,"Save",SAVE);button(pane,"Save As...",SAVE_AS);button(pane,"Quit",QUIT);
    pane=menu(bar,"Edit");button(pane,"Undo",UNDO);button(pane,"Redo",REDO);button(pane,"Copy selection",COPY);button(pane,"Cut selection",CUT);button(pane,"Paste",PASTE);button(pane,"Clear",CLEAR);
    pane=menu(bar,"View");button(pane,"Fit to window",FIT);button(pane,"Zoom in",ZOOM_IN);button(pane,"Zoom out",ZOOM_OUT);button(pane,"Canvas size...",RESIZE);
    pane=menu(bar,"Help");button(pane,"Drawing guide",HELP);
    top=XtVaCreateManagedWidget("actions",xmRowColumnWidgetClass,app.form,XmNorientation,XmHORIZONTAL,XmNpacking,XmPACK_TIGHT,
        XmNspacing,gap,XmNmarginWidth,gap,XmNmarginHeight,gap,XmNadjustLast,False,XmNentryAlignment,XmALIGNMENT_CENTER,
        XmNtopAttachment,XmATTACH_WIDGET,XmNtopWidget,bar,XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,NULL);
    action_button(top,"New",NEW,ICON_NEW,"New picture (Ctrl+N)");
    action_button(top,"Open",OPEN,ICON_OPEN,"Open picture (Ctrl+O)");
    action_button(top,"Save",SAVE,ICON_SAVE,"Save picture (Ctrl+S)");
    app.undo_button=action_button(top,"Undo",UNDO,ICON_UNDO,"Undo (Ctrl+Z)");
    app.redo_button=action_button(top,"Redo",REDO,ICON_REDO,"Redo (Ctrl+Y)");
    action_button(top,"Copy",COPY,ICON_COPY,"Copy selection (Ctrl+C)");
    action_button(top,"Cut",CUT,ICON_CUT,"Cut selection (Ctrl+X)");
    action_button(top,"Paste",PASTE,ICON_PASTE,"Paste at next click (Ctrl+V)");
    action_button(top,"Zoom out",ZOOM_OUT,ICON_ZOOM_OUT,"Zoom out (-)");
    action_button(top,"Fit",FIT,ICON_FIT,"Fit picture to window (0)");
    action_button(top,"Zoom in",ZOOM_IN,ICON_ZOOM_IN,"Zoom in (+)");
    action_button(top,"Canvas size",RESIZE,ICON_RESIZE,"Canvas size: crop or extend");
    colors=XtVaCreateManagedWidget("palette",xmFormWidgetClass,app.form,XmNheight,2*swatch+3*gap,
        XmNtopAttachment,XmATTACH_WIDGET,XmNtopWidget,top,XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,NULL);
    app.swatch=XtVaCreateManagedWidget("Ink",xmPushButtonWidgetClass,colors,XmNx,gap,XmNy,gap,
        XmNwidth,(int)lround(40*ui_scale),XmNheight,2*swatch+gap,XmNrecomputeSize,False,XmNmarginWidth,0,XmNmarginHeight,0,NULL);
    tooltip(app.swatch,"Current ink - choose a custom color (#RRGGBB)");
    XtAddCallback(app.swatch,XmNactivateCallback,custom_color,NULL);
    for(i=0;i<(int)(sizeof(palette)/sizeof(*palette));++i) {
        char name[24],tip[40]; XmString blank=XmStringCreateLocalized(""); Widget w;
        snprintf(name,sizeof(name),"color%02d",i);snprintf(tip,sizeof(tip),"Set ink to #%06X",palette[i]);
        w=XtVaCreateManagedWidget(name,xmPushButtonWidgetClass,colors,XmNlabelString,blank,XmNbackground,native_color(palette[i]),
            XmNx,grid_x+(i%8)*(swatch+gap),XmNy,gap+(i/8)*(swatch+gap),
            XmNwidth,swatch,XmNheight,swatch,XmNmarginWidth,0,XmNmarginHeight,0,XmNrecomputeSize,False,NULL);XmStringFree(blank);
        tooltip(w,tip);XtAddCallback(w,XmNactivateCallback,color_changed,(XtPointer)(intptr_t)i);
    }
    size_label=XtVaCreateManagedWidget("Size (px)",xmLabelWidgetClass,colors,XmNx,options_x,XmNy,gap,
        XmNmarginWidth,0,XmNmarginHeight,0,NULL);
    (void)size_label;
    size=XtVaCreateManagedWidget("Size",xmScaleWidgetClass,colors,XmNorientation,XmHORIZONTAL,XmNminimum,1,XmNmaximum,48,
        XmNx,options_x,XmNy,(int)lround(20*ui_scale),XmNvalue,6,XmNshowValue,True,XmNscaleWidth,(int)lround(120*ui_scale),NULL);
    tooltip(size,"Brush, eraser and shape width in document pixels");
    XtAddCallback(size,XmNvalueChangedCallback,size_changed,NULL);XtAddCallback(size,XmNdragCallback,size_changed,NULL);
    filled=XtVaCreateManagedWidget("Solid shapes",xmToggleButtonWidgetClass,colors,
        XmNx,options_x+(int)lround(150*ui_scale),XmNy,(int)lround(17*ui_scale),NULL);
    tooltip(filled,"Fill rectangles and ellipses instead of drawing outlines");
    XtAddCallback(filled,XmNvalueChangedCallback,filled_changed,NULL);
    app.status=XtVaCreateManagedWidget("status",xmLabelWidgetClass,app.form,XmNalignment,XmALIGNMENT_BEGINNING,
        XmNbottomAttachment,XmATTACH_FORM,XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,NULL);
    rail=XtVaCreateManagedWidget("tools",xmFormWidgetClass,app.form,XmNwidth,2*tile+3*gap,XmNheight,5*tile+6*gap,
        XmNtopAttachment,XmATTACH_WIDGET,XmNtopWidget,colors,XmNleftAttachment,XmATTACH_FORM,NULL);
    for(i=0;i<TOOL_COUNT;++i) {
        app.tools[i]=XtVaCreateManagedWidget(tool_names[i],xmToggleButtonWidgetClass,rail,XmNindicatorOn,False,
            XmNx,gap+(i%2)*(tile+gap),XmNy,gap+(i/2)*(tile+gap),XmNfillOnSelect,True,XmNset,i==BRUSH,NULL);
        paint_button_icon(app.tools[i],i,True);tooltip(app.tools[i],tool_names[i]);
        XtAddCallback(app.tools[i],XmNvalueChangedCallback,tool_changed,(XtPointer)(intptr_t)i);
    }
    frame=XtVaCreateManagedWidget("viewport",xmFormWidgetClass,app.form,
        XmNtopAttachment,XmATTACH_WIDGET,XmNtopWidget,colors,XmNbottomAttachment,XmATTACH_WIDGET,XmNbottomWidget,app.status,
        XmNleftAttachment,XmATTACH_WIDGET,XmNleftWidget,rail,XmNrightAttachment,XmATTACH_FORM,NULL);
    app.hscroll=XtVaCreateWidget("horizontal",xmScrollBarWidgetClass,frame,XmNorientation,XmHORIZONTAL,XmNheight,20,
        XmNbottomAttachment,XmATTACH_FORM,XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,NULL);
    app.vscroll=XtVaCreateWidget("vertical",xmScrollBarWidgetClass,frame,XmNorientation,XmVERTICAL,
        XmNtopAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_FORM,XmNwidth,20,NULL);
    app.canvas=XtVaCreateManagedWidget("canvas",xmDrawingAreaWidgetClass,frame,XmNresizePolicy,XmRESIZE_NONE,
        XmNmarginWidth,0,XmNmarginHeight,0,XmNtraversalOn,True,
        XmNtopAttachment,XmATTACH_FORM,XmNleftAttachment,XmATTACH_FORM,
        XmNrightAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_FORM,NULL);
    XtAddCallback(app.canvas,XmNexposeCallback,expose,NULL);XtAddCallback(app.canvas,XmNresizeCallback,expose,NULL);
    XtAddEventHandler(app.canvas,ButtonPressMask|ButtonReleaseMask|ButtonMotionMask,False,canvas_event,NULL);
    install_keys(app.form);
    XtAddCallback(app.hscroll,XmNvalueChangedCallback,scroll,NULL);XtAddCallback(app.hscroll,XmNdragCallback,scroll,NULL);
    XtAddCallback(app.vscroll,XmNvalueChangedCallback,scroll,NULL);XtAddCallback(app.vscroll,XmNdragCallback,scroll,NULL);
    wm_delete=XmInternAtom(app.display,"WM_DELETE_WINDOW",False); XmAddWMProtocolCallback(app.shell,wm_delete,command,(XtPointer)(intptr_t)QUIT);
    XtRealizeWidget(app.shell); app.gc=XCreateGC(app.display,XtWindow(app.canvas),0,NULL);
    app.cross_cursor=XCreateFontCursor(app.display,XC_crosshair);
    app.text_cursor=XCreateFontCursor(app.display,XC_xterm);
    XDefineCursor(app.display,XtWindow(app.canvas),app.cross_cursor);
    redraw();update();
}
#ifndef PAINT_NO_MAIN
int main(int argc,char **argv)
{
    app.tool=BRUSH;app.size=6;
    if(!paint_image_create(&app.doc.image,960,640)) return 1;
    create_ui(&argc,argv);
    XtAppMainLoop(app.context); return 0;
}
#endif
