/* SPDX-License-Identifier: LGPL-2.0-or-later */
#define PAINT_NO_MAIN
#include "../../demos/programs/draw/draw.c"
#include <assert.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/shape.h>
#include <time.h>
#include <Xm/LabelP.h>
static void drain(void)
{
    int i; for(i=0;i<20;++i) { XSync(app.display,False); while(XtAppPending(app.context)) XtAppProcessEvent(app.context,XtIMAll); }
}
static void pointer(int type,int x,int y)
{
    Position rx,ry;
    XtTranslateCoords(app.canvas,app.origin_x+(int)((x+0.5)*app.scale),app.origin_y+(int)((y+0.5)*app.scale),&rx,&ry);
    XTestFakeMotionEvent(app.display,-1,rx,ry,CurrentTime);
    if(type==ButtonPress || type==ButtonRelease) XTestFakeButtonEvent(app.display,1,type==ButtonPress,CurrentTime);
    XFlush(app.display);drain();
}
static void click_tool(int tool)
{
    Position x,y;Dimension width,height;
    XtVaGetValues(app.tools[tool],XmNwidth,&width,XmNheight,&height,NULL);
    XtTranslateCoords(app.tools[tool],width/2,height/2,&x,&y);
    XTestFakeMotionEvent(app.display,-1,x,y,CurrentTime);
    XTestFakeButtonEvent(app.display,1,True,CurrentTime);XTestFakeButtonEvent(app.display,1,False,CurrentTime);
    XFlush(app.display);drain();assert(app.tool==tool);
}
static void key(KeySym symbol, int control)
{
    if(control) XTestFakeKeyEvent(app.display,XKeysymToKeycode(app.display,XK_Control_L),True,CurrentTime);
    XTestFakeKeyEvent(app.display,XKeysymToKeycode(app.display,symbol),True,CurrentTime);
    XTestFakeKeyEvent(app.display,XKeysymToKeycode(app.display,symbol),False,CurrentTime);
    if(control) XTestFakeKeyEvent(app.display,XKeysymToKeycode(app.display,XK_Control_L),False,CurrentTime);
    XFlush(app.display);drain();
}
static void check_icon(Widget w)
{
    Dimension width,height;Pixmap pix;XmString tip=NULL;unsigned pw,ph,border,depth;int x,y;Window root;
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,XmNlabelPixmap,&pix,XmNtoolTipString,&tip,NULL);
    assert(width==height && width>=32 && width<=80);assert(tip && !XmStringEmpty(tip));
    assert(pix && pix!=XmUNSPECIFIED_PIXMAP);
    assert(XGetGeometry(app.display,pix,&root,&x,&y,&pw,&ph,&border,&depth));
    assert(pw==ph && pw+10<=width && pw>=24); /* toolTipString is borrowed. */
    {
        XmLabelWidget label=(XmLabelWidget)w;Pixel bg;XImage *image;int left=(int)pw,top=(int)ph,right=-1,bottom=-1,i,j;
        int origin_x=label->label.TextRect.x+label->label.PixmapRect.x;
        int origin_y=label->label.TextRect.y+label->label.PixmapRect.y;
        assert(abs(2*origin_x+(int)pw-(int)width)<=1);
        assert(abs(2*origin_y+(int)ph-(int)height)<=1);
        XtVaGetValues(w,XmNbackground,&bg,NULL);image=XGetImage(app.display,pix,0,0,pw,ph,AllPlanes,ZPixmap);assert(image);
        for(j=0;j<(int)ph;++j)for(i=0;i<(int)pw;++i)if(XGetPixel(image,i,j)!=bg){if(i<left)left=i;if(i>right)right=i;if(j<top)top=j;if(j>bottom)bottom=j;}
        XDestroyImage(image);assert(right>=left && bottom>=top);
        assert(abs(left+right+1-(int)pw)<=1 && abs(top+bottom+1-(int)ph)<=1);
        assert(origin_x+left>=5 && (int)width-origin_x-right-1>=5);
        assert(origin_y+top>=5 && (int)height-origin_y-bottom-1>=5);
    }
}
/* Sample the visible root pixels, including the true one-pixel X border. */
static int sample_tip(Widget tip, XRectangle *clip, const char *capture)
{
    Widget shell=XtParent(tip);Window root=RootWindowOfScreen(XtScreen(tip)),child;
    Bool bs,cs;int bx,by,cx,cy,rx,ry,x,y,ink=0;unsigned bw,bh,cw,ch;
    Pixel fg,bg,outline;XImage *im;FILE *file=NULL;
    assert(XShapeQueryExtents(app.display,XtWindow(shell),&bs,&bx,&by,&bw,&bh,&cs,&cx,&cy,&cw,&ch));
    clip->x=cx;clip->y=cy;clip->width=cw;clip->height=ch;
    XtVaGetValues(tip,XmNforeground,&fg,XmNbackground,&bg,NULL);
    XtVaGetValues(shell,XmNborderColor,&outline,NULL);assert(outline!=bg && fg!=bg);
    XTranslateCoordinates(app.display,XtWindow(tip),root,cx,cy,&rx,&ry,&child);
    im=XGetImage(app.display,root,rx-1,ry-1,cw+2,ch+2,AllPlanes,ZPixmap);assert(im);
    assert(XGetPixel(im,0,ch/2)==outline && XGetPixel(im,cw+1,ch/2)==outline);
    assert(XGetPixel(im,cw/2,0)==outline && XGetPixel(im,cw/2,ch+1)==outline);
    if(capture){file=fopen(capture,"wb");assert(file);fprintf(file,"P6\n%u %u\n255\n",cw+2,ch+2);}
    for(y=0;y<(int)ch+2;++y)for(x=0;x<(int)cw+2;++x){
        unsigned long color=XGetPixel(im,x,y);
        if(x>0 && x<=(int)cw && y>0 && y<=(int)ch && color==fg)++ink;
        if(file){unsigned char rgb[3]={(unsigned char)(color>>16),(unsigned char)(color>>8),(unsigned char)color};fwrite(rgb,1,3,file);}
    }
    if(file)fclose(file);XDestroyImage(im);return ink;
}
static void hover_tip(Widget w,const char *expected,int cancel_early)
{
    Position x,y;Dimension width,height,tw,th,text_w,text_h;Widget tip=NULL;int i,ink,changes=0;
    char *text=NULL,filename[64];XmString label_string;XmRenderTable table;
    struct timespec delay={0,10000000};XWindowAttributes attributes;XRectangle first,prior,current;
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,NULL);XtTranslateCoords(w,width/2,height/2,&x,&y);
    XTestFakeMotionEvent(app.display,-1,x,y,CurrentTime);XFlush(app.display);
    for(i=0;i<150;++i){
        drain();tip=XtNameToWidget(app.shell,"*TipShell*TipLabel");
        if(tip && XtIsRealized(tip)){
            XGetWindowAttributes(app.display,XtWindow(XtParent(tip)),&attributes);
            if(attributes.map_state==IsViewable)break;
        }
        nanosleep(&delay,NULL);
    }
    assert(i<150 && tip && attributes.border_width==1);
    XtVaGetValues(tip,XmNlabelString,&label_string,XmNrenderTable,&table,XmNwidth,&tw,XmNheight,&th,NULL);
    assert(XmStringGetLtoR(label_string,XmFONTLIST_DEFAULT_TAG,&text) && !strcmp(text,expected));
    XmStringExtent(table,label_string,&text_w,&text_h);XtFree(text);XmStringFree(label_string);
    assert(attributes.width==tw && attributes.height==th);
    snprintf(filename,sizeof(filename),"tooltip-%s.ppm",XtName(w));ink=sample_tip(tip,&first,filename);
    assert(ink>20 && first.width>=text_w && first.height>=text_h);
    if(cancel_early){
        if(cancel_early==2)XtDestroyWidget(app.shell);
        else{XtTranslateCoords(app.canvas,10,10,&x,&y);XTestFakeMotionEvent(app.display,-1,x,y,CurrentTime);XFlush(app.display);}
        for(i=0;i<15;++i){nanosleep(&delay,NULL);drain();}
        if(cancel_early!=2){XGetWindowAttributes(app.display,XtWindow(XtParent(tip)),&attributes);assert(attributes.map_state!=IsViewable);}
        return;
    }
    prior=first;
    for(i=0;i<20;++i){
        nanosleep(&delay,NULL);drain();
        XGetWindowAttributes(app.display,XtWindow(XtParent(tip)),&attributes);
        assert(attributes.map_state==IsViewable && attributes.width==tw && attributes.height==th);
        assert(sample_tip(tip,&current,NULL)==ink);
        assert(current.width>=prior.width && current.height>=prior.height && current.width<=first.width+10 && current.height<=first.height+6);
        assert(abs(2*current.x+current.width-tw)<=1 && abs(2*current.y+current.height-th)<=1);
        if(current.width!=prior.width || current.height!=prior.height)++changes;
        if(i>=12)assert(current.width==tw && current.height==th);
        prior=current;
    }
    assert(changes>0 && prior.width>first.width && prior.height>first.height);
    fprintf(stderr,"%s: %u x %u to %u x %u in %d observed reveal steps, %d text pixels retained in every frame\n",expected,first.width,first.height,tw,th,changes,ink);
}
int main(int argc,char **argv)
{
    Dimension cw,ch; PaintImage original={0}; XmSelectionBoxCallbackStruct input={0}; Widget prompt;
    app.tool=BRUSH;app.size=6;assert(paint_image_create(&app.doc.image,960,640));create_ui(&argc,argv);drain();
    XtVaGetValues(app.canvas,XmNwidth,&cw,XmNheight,&ch,NULL);
    {
        Widget palette_widget=XtParent(app.swatch),solid=XtNameToWidget(palette_widget,"Solid shapes");
        Dimension pw,sw; Position sx;
        XtVaGetValues(palette_widget,XmNwidth,&pw,NULL);XtVaGetValues(solid,XmNx,&sx,XmNwidth,&sw,NULL);
        assert(sx>=0 && sx+sw<=pw);
    }
    fprintf(stderr,"canvas %u x %u\n",cw,ch);assert(cw>750 && ch>500);assert(!XtIsManaged(app.hscroll) && !XtIsManaged(app.vscroll));
    {
        WidgetList children;Cardinal n,i;Widget actions=XtNameToWidget(app.form,"actions"),pal=XtParent(app.swatch);
        XtVaGetValues(actions,XmNchildren,&children,XmNnumChildren,&n,NULL);assert(n==12);
        for(i=0;i<n;++i)check_icon(children[i]);
        for(i=0;i<TOOL_COUNT;++i)check_icon(app.tools[i]);
        for(i=0;i<16;++i){char name[24];Dimension w,h;snprintf(name,sizeof(name),"color%02u",i);Widget color=XtNameToWidget(pal,name);XtVaGetValues(color,XmNwidth,&w,XmNheight,&h,NULL);assert(w==h && w<=40);}
        hover_tip(app.tools[PENCIL],"Pencil",0);hover_tip(XtNameToWidget(actions,"Save"),"Save picture (Ctrl+S)",0);
        hover_tip(app.tools[BRUSH],"Brush",1);
    }
    pointer(ButtonPress,40,40);pointer(MotionNotify,300,200);pointer(ButtonRelease,300,200);
    assert(paint_get(&app.doc.image,40,40)==0 && paint_get(&app.doc.image,300,200)==0);
    key(XK_z,1);assert(paint_get(&app.doc.image,40,40)==PAINT_WHITE);
    key(XK_y,1);assert(paint_get(&app.doc.image,40,40)==0);
    click_tool(LINE);pointer(ButtonPress,700,500);pointer(MotionNotify,800,600);key(XK_Escape,0);pointer(ButtonRelease,800,600);
    assert(!app.dragging && !app.preview.pixels && !app.doc.redo_count);
    assert(paint_get(&app.doc.image,750,550)==PAINT_WHITE);
    click_tool(RECTANGLE); app.filled=1; app.color=0xff0000;
    pointer(ButtonPress,100,100);pointer(MotionNotify,200,200);
    assert(paint_get(&app.doc.image,150,150)!=0xff0000);assert(paint_get(&app.preview,150,150)==0xff0000);
    pointer(ButtonRelease,200,200);assert(paint_get(&app.doc.image,150,150)==0xff0000);
    click_tool(SELECT);pointer(ButtonPress,120,120);pointer(ButtonRelease,160,160);request_command(COPY);request_command(PASTE);
    pointer(ButtonPress,500,400);pointer(ButtonRelease,500,400);assert(paint_get(&app.doc.image,510,410)==0xff0000);
    click_tool(ERASER);pointer(ButtonPress,510,410);pointer(ButtonRelease,510,410);assert(paint_get(&app.doc.image,510,410)==PAINT_WHITE);
    assert(paint_image_copy(&original,&app.doc.image));request_command(ZOOM_IN);request_command(ZOOM_IN);drain();
    assert(XtIsManaged(app.hscroll) && XtIsManaged(app.vscroll));
    app.offset_x=200;app.offset_y=100;redraw(); app.tool=PENCIL;app.color=0x1976d2;
    pointer(ButtonPress,400,300);pointer(ButtonRelease,400,300);assert(paint_get(&app.doc.image,400,300)==app.color);
    request_command(UNDO); assert(!memcmp(original.pixels,app.doc.image.pixels,960*640*sizeof(uint32_t)));
    request_command(FIT);drain();assert(!XtIsManaged(app.hscroll) && !XtIsManaged(app.vscroll));
    XtVaSetValues(app.shell,XmNwidth,920,XmNheight,600,NULL);drain();
    assert(app.doc.image.width==960 && app.doc.image.height==640);
    assert(!memcmp(original.pixels,app.doc.image.pixels,960*640*sizeof(uint32_t)));
    prompt=XmCreatePromptDialog(app.shell,"testSize",NULL,0);input.value=XmStringCreateLocalized("1000 x 700");
    resize_ok(prompt,NULL,&input);XmStringFree(input.value);assert(app.doc.image.width==1000 && app.doc.image.height==700);
    assert(paint_get(&app.doc.image,150,150)==0xff0000);request_command(UNDO);assert(app.doc.image.width==960);
    prompt=XmCreatePromptDialog(app.shell,"testText",NULL,0);input.value=XmStringCreateLocalized("Paint");app.text_x=20;app.text_y=400;
    text_ok(prompt,NULL,&input);XmStringFree(input.value);
    {int x,y,n=0;for(y=400;y<420;++y)for(x=20;x<100;++x)if(paint_get(&app.doc.image,x,y)==app.color)++n;assert(n>10);}
    paint_image_free(&original);hover_tip(app.tools[ELLIPSE],"Ellipse",2);paint_document_free(&app.doc);
    puts("PASS: square swatches, scalable icons, real hover tooltips, native layout, canvas input, previews, selection/paste, eraser, zoom/scroll coordinates, fit, resize preservation, text and undo");return 0;
}
