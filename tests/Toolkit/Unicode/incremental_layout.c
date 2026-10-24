/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Native Text's retained paragraph/line captures versus a fresh native widget.
 * This fixture uses installed private Text output ABI, not a new editor model. */
#include <Xm/TextP.h>
#include <Xm/TextOutP.h>
#include <Xm/TextStrSoP.h>
#include <Xm/TextLayout.h>
#include <Xm/TextUnicode.h>
#include <Xm/DrawingA.h>
#include <Xm/Display.h>
#include <fontconfig/fontconfig.h>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Native invalidation seam, declared by the source-private TextI.h. */
extern void _XmTextInvalidate(XmTextWidget, XmTextPosition, XmTextPosition, long);
#define MAX_LINES 64
static size_t assertions, compared_lines, native_layout_warnings;
static const char *stage="startup";
static void require(int ok,const char *message) {
    ++assertions;
    if (!ok) { fprintf(stderr,"incremental layout: %s: %s\n",stage,message); exit(1); }
}
static void warning(String message) {
    if(strstr(message,"Unicode paragraph layout failed")) ++native_layout_warnings;
    fprintf(stderr,"incremental layout native warning stage=%s: %s\n",stage,message);
    fflush(stderr);
}
static void drain(XtAppContext app,Display *display) {
    unsigned int i;
    for(i=0;i<3;++i) {
        struct timespec pause={0,2000000};
        XSync(display,False);
        while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll);
        nanosleep(&pause,NULL);
    }
}
static void retired(Widget widget,XtPointer data,XtPointer call) {
    (void)widget; (void)call; *(Widget *)data=NULL;
}
static XmFontList fonts(Widget owner,int points) {
    Arg args[3]; XmRendition rendition; XmFontList result;
    XtSetArg(args[0],XmNfontType,XmFONT_IS_XFT);
    XtSetArg(args[1],XmNfontName,"DejaVu Sans");
    XtSetArg(args[2],XmNfontSize,points);
    rendition=XmRenditionCreate(owner,XmFONTLIST_DEFAULT_TAG,args,3);
    require(rendition!=NULL,"explicit Xft rendition unavailable");
    result=XmRenderTableAddRenditions(NULL,&rendition,1,XmMERGE_REPLACE);
    XmRenditionFree(rendition);
    require(result!=NULL,"explicit Xft font table unavailable");
    return result;
}
static void pixels(Display *display,const XmTextLayout *a,const XmTextLayout *b) {
    int screen=DefaultScreen(display),i,x,y;
    unsigned int width=(unsigned int)ceil(fmax(XmTextLayoutWidth(a),XmTextLayoutWidth(b)))+40;
    const unsigned int height=96;
    Pixmap pixmap[2]; XImage *image[2]; XftDraw *draw;
    XftColor foreground; XRectangle viewport={0,0,0,96}; GC gc;
    size_t ink=0;
    require(width>0 && width<4096,"line readback width outside fixture budget");
    viewport.width=(unsigned short)width;
    require(XftColorAllocName(display,DefaultVisual(display,screen),
        DefaultColormap(display,screen),"black",&foreground),"Xft foreground allocation");
    for(i=0;i<2;++i) {
        const XmTextLayout *line=i?b:a;
        pixmap[i]=XCreatePixmap(display,RootWindow(display,screen),width,height,
                               (unsigned int)DefaultDepth(display,screen));
        require(pixmap[i]!=None,"line Pixmap allocation");
        gc=XCreateGC(display,pixmap[i],0,NULL);
        XSetForeground(display,gc,WhitePixel(display,screen));
        XFillRectangle(display,pixmap[i],gc,0,0,width,height); XFreeGC(display,gc);
        draw=XftDrawCreate(display,pixmap[i],DefaultVisual(display,screen),DefaultColormap(display,screen));
        require(draw!=NULL,"line XftDraw allocation");
        require(XmTextLayoutDraw(line,draw,&foreground,16,64,0,
            XmTextLayoutScalarCount(line),&viewport),"native capture real glyph draw");
        XftDrawDestroy(draw);
        XSync(display,False);
        image[i]=XGetImage(display,pixmap[i],0,0,width,height,AllPlanes,ZPixmap);
        require(image[i]!=NULL,"line pixel readback");
    }
    for(y=0;y<(int)height;++y) for(x=0;x<(int)width;++x) {
        unsigned long first=XGetPixel(image[0],x,y),second=XGetPixel(image[1],x,y);
        require(first==second,"edited native capture differs from fresh glyph pixels");
        if(first!=WhitePixel(display,screen)) ++ink;
    }
    if(XmTextLayoutGlyphCount(a)) require(ink>0,"nonempty glyph capture painted no pixels");
    for(i=0;i<2;++i) { XDestroyImage(image[i]); XFreePixmap(display,pixmap[i]); }
    XftColorFree(display,DefaultVisual(display,screen),DefaultColormap(display,screen),&foreground);
}
static void layout(Display *display,const XmTextLayout *a,const XmTextLayout *b) {
    size_t count=XmTextLayoutScalarCount(a),n,an,bn;
    int affinity,direction,ar,br,aa,ba;
    double ax,bx,x,width=XmTextLayoutWidth(a);
    XRectangle viewport={0,0,1024,96};
    require(count==XmTextLayoutScalarCount(b) &&
            XmTextLayoutGlyphCount(a)==XmTextLayoutGlyphCount(b) &&
            fabs(width-XmTextLayoutWidth(b))<0.001,"line metrics differ from fresh native capture");
    for(n=0;n<=count;++n) {
        for(direction=-1;direction<=1;direction+=2) {
            ar=XmTextLayoutBoundary(a,n,direction,&an);
            br=XmTextLayoutBoundary(b,n,direction,&bn);
            require(ar==br && (!ar || an==bn),"grapheme boundary differs from fresh capture");
        }
        for(affinity=0;affinity<2;++affinity) {
            ar=XmTextLayoutCaret(a,n,affinity,&ax);
            br=XmTextLayoutCaret(b,n,affinity,&bx);
            require(ar==br && (!ar || fabs(ax-bx)<0.001),"both-affinity caret differs from fresh capture");
            if(ar) for(direction=-1;direction<=1;direction+=2) {
                ar=XmTextLayoutVisualMove(a,n,affinity,direction,&an,&aa);
                br=XmTextLayoutVisualMove(b,n,affinity,direction,&bn,&ba);
                require(ar==br && (!ar || (an==bn && aa==ba)),"visual movement differs from fresh capture");
            }
        }
        {
            Region first=XmTextLayoutSelectionRegion(a,16,n,count,&viewport);
            Region second=XmTextLayoutSelectionRegion(b,16,n,count,&viewport);
            require((first!=NULL)==(second!=NULL),"selection availability differs");
            if(first) { require(XEqualRegion(first,second),"selection region differs from fresh capture");
                XDestroyRegion(first); XDestroyRegion(second); }
        }
    }
    for(x=-7;x<=width+7;x+=5.25) {
        ar=XmTextLayoutHit(a,x,&an,&aa); br=XmTextLayoutHit(b,x,&bn,&ba);
        require(ar==br && (!ar || (an==bn && aa==ba)),"visual hit differs from fresh capture");
    }
    pixels(display,a,b); ++compared_lines;
}
static void geometry_diagnostic(const char *name,Widget widget) {
    XmTextWidget tw=(XmTextWidget)widget;
    OutputData data=tw->text.output->data;
    double points=-1;
    FcChar8 *family=NULL;
    XWindowAttributes actual;
    int have_actual=XGetWindowAttributes(XtDisplay(widget),XtWindow(widget),&actual);
    if(data->use_xft && data->font) {
        XftFont *font=(XftFont *)data->font;
        (void)FcPatternGetDouble(font->pattern,FC_SIZE,0,&points);
        (void)FcPatternGetString(font->pattern,FC_FAMILY,0,&family);
    }
    fprintf(stderr,
        "  %s widget=%p requested=%ux%u inner=%ux%u actual=%dx%d margins=%d,%d "
        "font=%p family=%s points=%.6f ascent=%u descent=%u lineheight=%u "
        "output_lines=%d table_lines=%u top=%ld bottom=%ld source_scalars=%ld "
        "needs_refigure=%d wordwrap=%d use_xft=%d\n",
        name,(void *)widget,(unsigned)tw->core.width,(unsigned)tw->core.height,
        (unsigned)tw->text.inner_widget->core.width,(unsigned)tw->text.inner_widget->core.height,
        have_actual?actual.width:-1,have_actual?actual.height:-1,
        data->leftmargin,data->rightmargin,(void *)data->font,
        family?(const char *)family:"(none)",points,
        (unsigned)data->font_ascent,(unsigned)data->font_descent,(unsigned)data->lineheight,
        data->number_lines,(unsigned)tw->text.number_lines,
        (long)tw->text.top_character,(long)tw->text.bottom_position,
        (long)tw->text.source->data->length,(int)tw->text.needs_refigure_lines,
        (int)data->wordwrap,(int)data->use_xft);
}
typedef struct { XmTextLayout *native[MAX_LINES],*fresh[MAX_LINES]; size_t count; } Retained;
static void compare(Widget native,Widget fresh,Retained *keep) {
    XmTextPosition position=0,total=XmTextGetLastPosition(native);
    size_t lines=0;
    char *value=XmTextGetString(native);
    require(value!=NULL,"native source read");
    XmTextSetString(fresh,value); XtFree(value);
    for(;;) {
        XmTextPosition af=0,al=0,bf=0,bl=0;
        XmTextLayout *a=_XmTextGetLineCapture((XmTextWidget)native,position,&af,&al);
        XmTextLayout *b=_XmTextGetLineCapture((XmTextWidget)fresh,position,&bf,&bl);
        if((a!=NULL)!=(b!=NULL) ||
            (a && (af!=bf || al!=bl || af>position || al<position))) {
            fprintf(stderr,"incremental layout diagnostic stage=%s compared_lines=%lu position=%ld total=%ld "
                "native_capture=%p range=[%ld,%ld] fresh_capture=%p range=[%ld,%ld]\n",
                stage,(unsigned long)lines,(long)position,(long)total,
                (void *)a,(long)af,(long)al,(void *)b,(long)bf,(long)bl);
            geometry_diagnostic("native",native); geometry_diagnostic("fresh",fresh);
            fflush(stderr);
        }
        require((a!=NULL)==(b!=NULL),"native/fresh visible line availability differs");
        if(!a) break;
        require(af==bf && al==bl && af<=position && al>=position,
                "native/fresh wrapped source ranges differ");
        require(al>=af && XmTextLayoutScalarCount(a)==(size_t)(al-af),"line capture source range mismatch");
        layout(XtDisplay(native),a,b);
        if(keep) {
            require(keep->count<MAX_LINES,"retained capture budget");
            keep->native[keep->count]=a; keep->fresh[keep->count++]=b;
        } else { XmTextLayoutFree(a); XmTextLayoutFree(b); }
        ++lines;
        if(al>=total) break;
        {
            XmTextPosition previous=position;
            XmTextSource source=XmTextGetSource(native);
            position=al;
            /* Ask the owning scanner only whether this capture ends at a hard
             * terminator, then skip that whole terminator. Independent cases
             * below assert the scanner's exact mandatory-break boundaries. */
            if(position<total && source->Scan(source,position,XmSELECT_LINE,
                XmsdRight,1,False)==position)
                position=source->Scan(source,position,XmSELECT_LINE,XmsdRight,1,True);
            require(position>previous,"visible line capture and hard separator did not advance");
        }
        require(lines<MAX_LINES,"visible line iteration budget");
    }
    require(lines>=3,"fixture did not qualify multiple native visual lines");
}
static XmTextShapingUsage usage(Widget widget) {
    XmTextShapingUsage result;
    require(XmTextFontContextGetShapingUsage(((XmTextWidget)widget)->text.output->data->unicode_font_context,
        &result),"native shaping counters unavailable");
    return result;
}
static void request_fits(Widget widget,Dimension width,Dimension height) {
    XmTextWidget tw=(XmTextWidget)widget;
    Widget parent=XtParent(widget);
    Dimension margin_width=0,margin_height=0,parent_width=0,parent_height=0;
    XtVaGetValues(parent,XmNmarginWidth,&margin_width,XmNmarginHeight,&margin_height,
        XmNwidth,&parent_width,XmNheight,&parent_height,NULL);
    require(tw->core.x>=0 && tw->core.y>=0 &&
        (size_t)tw->core.x+width+2*(size_t)tw->core.border_width+margin_width<=parent_width &&
        (size_t)tw->core.y+height+2*(size_t)tw->core.border_width+margin_height<=parent_height,
        "fixture request exceeds fixed DrawingArea geometry/margins");
}
static void exact_geometry(Widget widget,Dimension width,Dimension height) {
    XmTextWidget tw=(XmTextWidget)widget;
    OutputData data=tw->text.output->data;
    XWindowAttributes actual;
    int have_actual=XGetWindowAttributes(XtDisplay(widget),XtWindow(widget),&actual);
    if(tw->core.width!=width || tw->core.height!=height || !have_actual ||
       actual.width!=(int)width || actual.height!=(int)height) {
        fprintf(stderr,"incremental layout geometry diagnostic stage=%s widget=%p expected=%ux%u "
            "core=%ux%u server=%dx%d rows=%d columns=%d rows_set=%d columns_set=%d "
            "averagecharwidth=%d lineheight=%u ascent=%u descent=%u margins_lrtb=%d,%d,%d,%d\n",
            stage,(void *)widget,(unsigned)width,(unsigned)height,
            (unsigned)tw->core.width,(unsigned)tw->core.height,
            have_actual?actual.width:-1,have_actual?actual.height:-1,
            (int)data->rows,(int)data->columns,(int)data->rows_set,(int)data->columns_set,
            data->averagecharwidth,(unsigned)data->lineheight,
            (unsigned)data->font_ascent,(unsigned)data->font_descent,
            data->leftmargin,data->rightmargin,data->topmargin,data->bottommargin);
        geometry_diagnostic("dimension failure",widget); fflush(stderr);
    }
    require(tw->core.width==width && tw->core.height==height,
            "explicit or row/column-derived native geometry differs");
    require(have_actual && actual.width==(int)width && actual.height==(int)height,
        "actual X window differs from accepted native dimensions");
}
static Dimension rows_height(Widget widget,unsigned int rows) {
    OutputData data=((XmTextWidget)widget)->text.output->data;
    return (Dimension)(rows*data->lineheight+data->topmargin+data->bottommargin);
}
static Dimension columns_width(Widget widget,unsigned int columns) {
    OutputData data=((XmTextWidget)widget)->text.output->data;
    return (Dimension)(columns*data->averagecharwidth+data->leftmargin+data->rightmargin);
}
static void scanner_contract(Widget widget) {
    static const struct {
        XmTextPosition position;
        XmTextScanDirection direction;
        int count;
        Boolean include;
        XmTextPosition expected;
    } cases[]={
        {0,XmsdRight,1,False,1},{0,XmsdRight,1,True,3},
        {3,XmsdRight,1,False,4},{3,XmsdRight,1,True,5},
        {5,XmsdRight,1,False,6},{5,XmsdRight,1,True,7},
        {3,XmsdLeft,1,False,3},{3,XmsdLeft,1,True,1},
        {5,XmsdLeft,1,False,5},{5,XmsdLeft,1,True,4},
        {7,XmsdLeft,1,False,7},{7,XmsdLeft,1,True,6},
        {0,XmsdRight,2,False,4},{0,XmsdRight,2,True,5},
        {7,XmsdLeft,2,False,5},{7,XmsdLeft,2,True,4},
        {2,XmsdRight,1,False,4},{2,XmsdRight,1,True,5},
        {2,XmsdLeft,1,False,0},{2,XmsdLeft,1,True,0}
    };
    unsigned int pass; size_t i;
    XmTextSource source;
    XmTextSetString(widget,"a\r\nb\rc\n");
    source=XmTextGetSource(widget);
    for(pass=0;pass<2;++pass) {
        char *value=XmTextGetString(widget);
        require(value && !strcmp(value,"a\r\nb\rc\n") && XmTextGetLastPosition(widget)==7,
                "hard-line source bytes or scalar count changed"); XtFree(value);
        require(source->CountLines(source,0,7)==3,"whole-source hard-line count");
        require(source->CountLines(source,1,1)==0,"CRLF prefix must not double count pair");
        require(source->CountLines(source,2,1)==1,"CRLF LF must complete one line");
        require(source->CountLines(source,4,1)==1,"standalone CR must complete one line");
        for(i=0;i<sizeof(cases)/sizeof(cases[0]);++i) {
            XmTextPosition actual=source->Scan(source,cases[i].position,XmSELECT_LINE,
                cases[i].direction,cases[i].count,cases[i].include);
            require(actual==cases[i].expected && actual!=2,"native hard-line scalar scanner contract");
        }
        /* Recreate the same source with its gap between CR and LF. */
        if(!pass) { XmTextReplace(widget,2,2,"x"); XmTextReplace(widget,2,3,""); }
    }
}
static const char mandatory[]="a\vb\fc" "\xc2\x85" "d" "\xe2\x80\xa8" "e" "\xe2\x80\xa9" "f";
static void mandatory_contract(Widget widget) {
    XmTextSource source;
    XmTextPosition n;
    char *value;
    XmTextSetString(widget,(char *)mandatory);
    source=XmTextGetSource(widget); value=XmTextGetString(widget);
    require(value && !strcmp(value,mandatory) && XmTextGetLastPosition(widget)==11,
            "mandatory hard-line controls changed bytes/scalar count"); XtFree(value);
    require(source->CountLines(source,0,11)==5,"mandatory hard-line whole-source count");
    for(n=0;n<10;n+=2) {
        require(source->CountLines(source,n+1,1)==1,"mandatory control omitted from line count");
        require(source->Scan(source,n,XmSELECT_LINE,XmsdRight,1,False)==n+1,
                "mandatory control exclusion boundary");
        require(source->Scan(source,n,XmSELECT_LINE,XmsdRight,1,True)==n+2,
                "mandatory control inclusion boundary");
        require(source->Scan(source,n+2,XmSELECT_LINE,XmsdLeft,1,False)==n+2 &&
                source->Scan(source,n+2,XmSELECT_LINE,XmsdLeft,1,True)==n+1,
                "mandatory control reverse boundary");
    }
    require(source->Scan(source,0,XmSELECT_LINE,XmsdRight,2,False)==3 &&
            source->Scan(source,0,XmSELECT_LINE,XmsdRight,2,True)==4,
            "mandatory control repeated scan count");
}
static void crlf_carets(Widget widget,XtAppContext app,Display *display) {
    Position x0,y0,xb,yb,xn,yn,unused_x,unused_y;
    XmTextSetString(widget,"abc\r\nxyz"); drain(app,display);
    require(XmTextPosToXY(widget,0,&x0,&y0),"CRLF text start has no public caret");
    require(XmTextPosToXY(widget,3,&xb,&yb) && yb==y0 && xb>x0,
            "CRLF position before CR has no correct trailing caret");
    require(!XmTextPosToXY(widget,4,&unused_x,&unused_y),"CRLF interior LF admitted public caret");
    require(XmTextPosToXY(widget,5,&xn,&yn) && yn>y0,"CRLF next-line public caret unavailable");
}
static const char initial[]=
    "Unchanged first paragraph has several words for stable wrapping.\n"
    "Latin e\xcc\x81 \xd7\x90\xd7\x91\xd7\x92 (12) \xd8\xa7\xd9\x84\xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a\xd8\xa9\t"
    "\xf0\x9f\x91\xa9\xe2\x80\x8d\xf0\x9f\x92\xbb and more words.\n"
    "123 [\xe2\x81\xa7\xd7\x90\xd7\x91\xd7\x92 \xd8\xa8\xe2\x81\xa9] neutral context across wrapped words and distant base.\n"
    "Final untouched paragraph with stable words and a combining e\xcc\x81.";
static XmTextPosition scalar_at(Widget widget,const char *needle) {
    char *value=XmTextGetString(widget),*found;
    XmTextUnicodeMap *map; size_t scalar=0;
    require(value!=NULL,"source read for fixture edit"); found=strstr(value,needle);
    require(found!=NULL,"fixture edit marker missing");
    map=XmTextUnicodeMapCreate(value,strlen(value));
    require(map && XmTextUnicodeScalarAtByte(map,(size_t)(found-value),&scalar),"fixture edit marker scalar mapping");
    XmTextUnicodeMapFree(map); XtFree(value); return (XmTextPosition)scalar;
}
int main(int argc,char **argv) {
    XtAppContext app; Widget shell,area,native[2],fresh[2],native_display;
    Display *display; XmFontList table[2]; Retained old={0};
    XmTextShapingUsage before[2],after; size_t i; XmTextPosition at,suffix_position;
    XmTextPosition suffix_first[2]={0,0},suffix_last[2]={0,0};
    XmTextLayout *suffix_before[2]={NULL,NULL};
    int font_column_15,font_line_12;
    require(setlocale(LC_ALL,"C.UTF-8")!=NULL && XmTextUnicodeLocaleAvailable(),"UTF-8 locale unavailable");
    XtSetLanguageProc(NULL,NULL,NULL);
    shell=XtVaAppInitialize(&app,"IncrementalLayout",NULL,0,&argc,argv,NULL,XmNwidth,1400,XmNheight,800,NULL);
    XtAppSetWarningHandler(app,warning);
    display=XtDisplay(shell); native_display=XmGetXmDisplay(display);
    XtAddCallback(native_display,XmNdestroyCallback,retired,&native_display);
    table[0]=fonts(shell,12); table[1]=fonts(shell,15);
    area=XtVaCreateManagedWidget("area",xmDrawingAreaWidgetClass,shell,
        XmNwidth,1400,XmNheight,800,XmNresizePolicy,XmRESIZE_NONE,NULL);
    for(i=0;i<2;++i) {
        int width=i?530:350,x=i?390:10;
        native[i]=XtVaCreateManagedWidget("edited",xmTextWidgetClass,area,XmNx,x,XmNy,10,
            XmNwidth,width,XmNheight,305,XmNfontList,table[0],XmNeditMode,XmMULTI_LINE_EDIT,
            XmNwordWrap,True,XmNresizeWidth,False,XmNresizeHeight,False,XmNblinkRate,0,NULL);
        fresh[i]=XtVaCreateManagedWidget("fresh",xmTextWidgetClass,area,XmNx,x,XmNy,335,
            XmNwidth,width,XmNheight,305,XmNfontList,table[0],XmNeditMode,XmMULTI_LINE_EDIT,
            XmNwordWrap,True,XmNresizeWidth,False,XmNresizeHeight,False,XmNblinkRate,0,NULL);
    }
    XmTextSetString(native[0],(char *)initial);
    XmTextSetSource(native[1],XmTextGetSource(native[0]),0,0);
    require(XmTextGetSource(native[0])==XmTextGetSource(native[1]),"native sources not shared");
    XtRealizeWidget(shell); drain(app,display);
    font_line_12=((XmTextWidget)native[0])->text.output->data->lineheight;
    stage="hard-line scanner independent contract";
    scanner_contract(fresh[0]);
    stage="CRLF public caret contract";
    crlf_carets(fresh[0],app,display);
    stage="UAX mandatory hard-line source contract";
    mandatory_contract(fresh[0]);
    XmTextSetString(native[0],(char *)mandatory); drain(app,display);
    for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    XmTextSetString(native[0],(char *)initial); drain(app,display);
    stage="initial shared source captures";
    for(i=0;i<2;++i) compare(native[i],fresh[i],i?NULL:&old);
    suffix_position=scalar_at(native[0],"Final");
    for(i=0;i<2;++i) {
        suffix_before[i]=_XmTextGetLineCapture((XmTextWidget)native[i],suffix_position,
            &suffix_first[i],&suffix_last[i]);
        before[i]=usage(native[i]);
    }
    stage="single hard paragraph edit reuse";
    at=scalar_at(native[0],"Latin"); XmTextReplace(native[0],at,at+1,"lX"); drain(app,display);
    for(i=0;i<2;++i) {
        XmTextPosition first,last;
        XmTextLayout *capture;
        compare(native[i],fresh[i],NULL);
        if(i==0) {
            capture=_XmTextGetLineCapture((XmTextWidget)native[i],0,&first,&last);
            require(capture && capture==old.native[0],"unaffected first native line capture was rebuilt");
            XmTextLayoutFree(capture);
        }
        if(suffix_before[i]) {
            capture=_XmTextGetLineCapture((XmTextWidget)native[i],suffix_position+1,&first,&last);
            require(capture && capture==suffix_before[i] &&
                first==suffix_first[i]+1 && last==suffix_last[i]+1,
                "unaffected suffix capture was not reused with rebased source range");
            XmTextLayoutFree(capture); XmTextLayoutFree(suffix_before[i]);
            suffix_before[i]=NULL;
        }
        after=usage(native[i]);
        require(after.paragraph_builds>=before[i].paragraph_builds &&
            after.paragraph_builds-before[i].paragraph_builds<=2,"edit rebuilt unrelated hard paragraphs");
        require(after.paragraph_scalars>=before[i].paragraph_scalars &&
            after.paragraph_scalars-before[i].paragraph_scalars<180,"edit reanalyzed unrelated paragraph scalars");
        printf("native width%lu edit paragraph_builds=%lu paragraph_scalars=%lu shape_calls=%lu shaped_scalars=%lu line_captures=%lu\n",
            (unsigned long)i,(unsigned long)(after.paragraph_builds-before[i].paragraph_builds),
            (unsigned long)(after.paragraph_scalars-before[i].paragraph_scalars),
            (unsigned long)(after.shape_calls-before[i].shape_calls),
            (unsigned long)(after.shaped_scalars-before[i].shaped_scalars),
            (unsigned long)(after.line_captures-before[i].line_captures));
    }
    stage="non-source invalidation sentinel";
    for(i=0;i<2;++i) _XmTextInvalidate((XmTextWidget)native[i],0,0,NODELTA);
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="distant bidi paragraph base change";
    at=scalar_at(native[0],"123 ["); XmTextReplace(native[0],at,at,"\xd7\x90 ");
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="LF paragraph join";
    at=scalar_at(native[0],"\nlXatin"); XmTextReplace(native[1],at,at+1," ");
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="CRLF hard paragraph split";
    at=scalar_at(native[0],"lXatin"); XmTextReplace(native[0],at,at,"\r\n");
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="standalone CR after paired LF deletion";
    at=scalar_at(native[0],"\r\n"); XmTextReplace(native[1],at+1,at+2,"");
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="restore paired CRLF";
    at=scalar_at(native[0],"\rlXatin"); XmTextReplace(native[0],at+1,at+1,"\n");
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="insert scalar between CRLF";
    at=scalar_at(native[0],"\r\n"); XmTextReplace(native[1],at+1,at+1,"x");
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="empty hard paragraph";
    at=scalar_at(native[0],"Final"); XmTextReplace(native[0],at,at,"\n");
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="hard boundary insertion";
    at=scalar_at(native[0],"Final"); XmTextReplace(native[1],at,at,"prefix ");
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="width invalidation with shared source";
    XtVaSetValues(native[0],XmNwidth,310,NULL);
    XtVaSetValues(fresh[0],XmNwidth,310,NULL);
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="wrap disabled on warmed paragraphs";
    for(i=0;i<2;++i) {
        Dimension width=((XmTextWidget)native[i])->core.width;
        Dimension height=((XmTextWidget)native[i])->core.height;
        XtVaSetValues(native[i],XmNwordWrap,False,XmNwidth,width,XmNheight,height,NULL);
        XtVaSetValues(fresh[i],XmNwordWrap,False,XmNwidth,width,XmNheight,height,NULL);
    }
    drain(app,display);
    for(i=0;i<2;++i) {
        require(!((XmTextWidget)native[i])->text.output->data->wordwrap,"wrap disable was not applied");
        compare(native[i],fresh[i],NULL);
    }
    stage="wrap enabled after unwrapped captures";
    for(i=0;i<2;++i) {
        Dimension width=((XmTextWidget)native[i])->core.width;
        Dimension height=((XmTextWidget)native[i])->core.height;
        XtVaSetValues(native[i],XmNwordWrap,True,XmNwidth,width,XmNheight,height,NULL);
        XtVaSetValues(fresh[i],XmNwordWrap,True,XmNwidth,width,XmNheight,height,NULL);
    }
    drain(app,display);
    for(i=0;i<2;++i) {
        require(((XmTextWidget)native[i])->text.output->data->wordwrap,"wrap enable was not applied");
        compare(native[i],fresh[i],NULL);
    }
    stage="font invalidation";
    for(i=0;i<2;++i) {
        Dimension width=((XmTextWidget)native[i])->core.width;
        Dimension height=((XmTextWidget)native[i])->core.height;
        require(width==((XmTextWidget)fresh[i])->core.width &&
            height==((XmTextWidget)fresh[i])->core.height,
            "font comparison requires matching pre-change geometry");
        /* Font SetValues can resize from rows/columns unless the fixed test
         * geometry is explicitly requested in the same resource update. */
        request_fits(native[i],width,height); request_fits(fresh[i],width,height);
        XtVaSetValues(native[i],XmNfontList,table[1],XmNwidth,width,XmNheight,height,NULL);
        XtVaSetValues(fresh[i],XmNfontList,table[1],XmNwidth,width,XmNheight,height,NULL);
        exact_geometry(native[i],width,height); exact_geometry(fresh[i],width,height);
    }
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    font_column_15=((XmTextWidget)native[0])->text.output->data->averagecharwidth;
    stage="rows and columns with fixed geometry";
    for(i=0;i<2;++i) {
        Dimension width=((XmTextWidget)native[i])->core.width;
        Dimension height=((XmTextWidget)native[i])->core.height;
        XtVaSetValues(native[i],XmNrows,10,XmNcolumns,28,XmNwidth,width,XmNheight,height,NULL);
        XtVaSetValues(fresh[i],XmNrows,10,XmNcolumns,28,XmNwidth,width,XmNheight,height,NULL);
        exact_geometry(native[i],width,height); exact_geometry(fresh[i],width,height);
    }
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    for(i=0;i<2;++i) {
        OutputData data=((XmTextWidget)native[i])->text.output->data;
        require(data->rows_set==10 && data->columns_set==28,
                "explicit row/column sizing counts were not retained independently of visible counts");
    }
    stage="font change with unchanged explicit width only";
    for(i=0;i<2;++i) {
        Dimension width=((XmTextWidget)native[i])->core.width;
        OutputData data=((XmTextWidget)native[i])->text.output->data;
        Dimension expected_height=(Dimension)(10*font_line_12+data->topmargin+data->bottommargin);
        request_fits(native[i],width,expected_height); request_fits(fresh[i],width,expected_height);
        XtVaSetValues(native[i],XmNfontList,table[0],XmNwidth,width,NULL);
        XtVaSetValues(fresh[i],XmNfontList,table[0],XmNwidth,width,NULL);
        exact_geometry(native[i],width,rows_height(native[i],10));
        exact_geometry(fresh[i],width,rows_height(fresh[i],10));
    }
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="font change with unchanged explicit height only";
    for(i=0;i<2;++i) {
        Dimension height=((XmTextWidget)native[i])->core.height;
        OutputData data=((XmTextWidget)native[i])->text.output->data;
        Dimension expected_width=(Dimension)(28*font_column_15+data->leftmargin+data->rightmargin);
        request_fits(native[i],expected_width,height); request_fits(fresh[i],expected_width,height);
        XtVaSetValues(native[i],XmNfontList,table[1],XmNheight,height,NULL);
        XtVaSetValues(fresh[i],XmNfontList,table[1],XmNheight,height,NULL);
        exact_geometry(native[i],columns_width(native[i],28),height);
        exact_geometry(fresh[i],columns_width(fresh[i],28),height);
    }
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="rows and columns autosizing";
    for(i=0;i<2;++i) {
        Dimension expected_width=columns_width(native[i],34),expected_height=rows_height(native[i],12);
        request_fits(native[i],expected_width,expected_height);
        request_fits(fresh[i],expected_width,expected_height);
        XtVaSetValues(native[i],XmNrows,12,XmNcolumns,34,NULL);
        XtVaSetValues(fresh[i],XmNrows,12,XmNcolumns,34,NULL);
        exact_geometry(native[i],columns_width(native[i],34),rows_height(native[i],12));
        exact_geometry(fresh[i],columns_width(fresh[i],34),rows_height(fresh[i],12));
    }
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="configuration invalidation";
    require(XmTextFontConfigurationChanged(),"font configuration notification unavailable");
    drain(app,display); for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    stage="distant first-strong hard-paragraph context";
    {
        char distant[512]="";
        unsigned int repeat;
        for(repeat=0;repeat<40;++repeat) strcat(distant,"123 (45) ");
        strcat(distant,"A distant first strong character.\nSecond hard paragraph.\nThird hard paragraph.");
        XmTextSetString(native[0],distant); drain(app,display);
        for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
        at=scalar_at(native[0],"A distant");
        require(at>=360,"distant context fixture lost its neutral prefix");
        XmTextReplace(native[1],at,at+1,"\xd7\x90"); drain(app,display);
        for(i=0;i<2;++i) compare(native[i],fresh[i],NULL);
    }
    stage="immutable captures after edits";
    for(i=0;i<old.count;++i) layout(display,old.native[i],old.fresh[i]);
    XtDestroyWidget(shell);
    if(native_display) XtDestroyWidget(native_display);
    stage="immutable captures after widget destruction";
    for(i=0;i<old.count;++i) {
        layout(display,old.native[i],old.fresh[i]);
        XmTextLayoutFree(old.native[i]); XmTextLayoutFree(old.fresh[i]);
    }
    stage="native valid-operation warning acceptance";
    if(native_layout_warnings) fprintf(stderr,"incremental layout native layout failure warning count=%lu\n",
        (unsigned long)native_layout_warnings);
    require(native_layout_warnings==0,"valid native operations emitted Unicode paragraph layout failure warnings");
    /* Captures borrow the primary font; keep the fixture's font tables and
     * Display alive until the last capture has been released. */
    XmRenderTableFree(table[0]); XmRenderTableFree(table[1]);
    XtCloseDisplay(display); XtDestroyApplicationContext(app); FcFini();
    printf("incremental-layout: %lu assertions, %lu native/fresh line comparisons; retained paragraph reuse, not incremental shaping inside an edited hard paragraph\n",
           (unsigned long)assertions,(unsigned long)compared_lines);
    fflush(stdout); /* Retain completed-body evidence if exit-time leak checks fail. */
    return 0;
}
