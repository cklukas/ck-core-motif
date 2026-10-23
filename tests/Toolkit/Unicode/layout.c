/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/TextLayout.h>
#include <Xm/TextUnicode.h>
#include <X11/Xutil.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void require(int condition, const char *detail) { if (!condition) { fprintf(stderr, "%s\n", detail); exit(1); } }
static void check_positions(XmTextLayout *layout, const char *source) {
    XmTextUnicodeMap *map = XmTextUnicodeMapCreate(source, strlen(source));
    size_t g;
    require(map != NULL, "fixture UTF8 map");
    for (g = 0; g <= XmTextUnicodeGraphemeCount(map); ++g) {
        size_t scalar, hit;
        int affinity;
        double x, inverse;
        XmTextUnicodeScalarAtGrapheme(map, g, &scalar);
        require(XmTextLayoutCaret(layout, scalar, 0, &x), "missing grapheme caret");
        require(XmTextLayoutHit(layout, x, &hit, &affinity), "missing visual hit");
        require(XmTextUnicodeIsGraphemeBoundary(map, hit), "hit split extended grapheme");
        require(XmTextLayoutCaret(layout, hit, affinity, &inverse) && fabs(x - inverse) <= 0.01, "paint/caret/hit coordinate mismatch");
    }
    XmTextUnicodeMapFree(map);
}
static void check_font_context(Display *display,XftFont *font) {
    const XmTextFontContextLimits limits={256,256u*1024u,1};
    XmTextFontContext *context=XmTextFontContextCreate(display,DefaultScreen(display),font,&limits);
    XmTextFontContextUsage before,after;
    XmTextLayout *retained,*repeated;
    XmTextParagraph *small;
    FcConfig *original=FcConfigReference(FcConfigGetCurrent()),*empty;
    double width,x;size_t index;
    const char fallback[]="latin \xe4\xb8\xad\xe6\x96\x87";
    require(context&&XmTextFontContextIsCurrent(context),"font context creation");
    small=XmTextParagraphCreateWithContext(context,fallback,strlen(fallback),64);
    require(small!=NULL,"context fallback paragraph");
    retained=XmTextParagraphLine(small,0,XmTextParagraphScalarCount(small));
    require(retained!=NULL,"context fallback capture");width=XmTextLayoutWidth(retained);
    require(XmTextFontContextGetUsage(context,&before)&&before.queries>0,"fallback query observation");
    repeated=XmTextLayoutCreateWithContext(context,fallback,strlen(fallback),64);
    require(repeated&&XmTextLayoutWidth(repeated)==width,"cached fallback changed geometry");
    require(XmTextFontContextGetUsage(context,&after)&&after.queries==before.queries&&after.cache_hits>before.cache_hits&&after.font_leases==1,"repeated paragraph bypassed resolver cache");
    XmTextLayoutFree(repeated);
    {
        const XmTextFontContextLimits disabled={0,0,0},invalid={256,256u*1024u,65};
        XmTextFontContext *uncached=XmTextFontContextCreate(display,DefaultScreen(display),font,&disabled);
        XmTextFontContextUsage usage;
        require(!XmTextFontContextCreate(display,DefaultScreen(display),font,&invalid),"font lease ceiling accepted oversized cache");
        require(uncached!=NULL,"disabled cache context");
        repeated=XmTextLayoutCreateWithContext(uncached,fallback,strlen(fallback),64);
        require(repeated&&XmTextLayoutWidth(repeated)==width,"disabled caches changed exact font projection");
        require(XmTextFontContextGetUsage(uncached,&usage)&&!usage.entries&&!usage.key_bytes&&!usage.font_leases&&usage.queries>0,
                "disabled context retained resolver/font cache entries");
        XmTextFontContextFree(uncached);
        require(XmTextLayoutWidth(repeated)==width,"disabled-cache capture lost independent font leases");
        XmTextLayoutFree(repeated);
    }
    require(XmTextFontConfigurationChanged()&&!XmTextFontContextIsCurrent(context),"global configuration epoch did not invalidate context");
    require(!XmTextLayoutCreateWithContext(context,"x",1,64),"stale context silently used old configuration");
    require(XmTextFontContextInvalidate(context)&&XmTextFontContextIsCurrent(context),"explicit context refresh");
    empty=FcConfigCreate();require(empty&&FcConfigSetCurrent(empty),"empty font configuration fixture");
    require(!XmTextFontContextIsCurrent(context)&&XmTextFontContextInvalidate(context),"replacement config identity was ignored");
    require(!XmTextLayoutCreateWithContext(context,"\xf4\x8f\xbf\xbf",4,64),"missing complete-grapheme font substituted partial coverage");
    require(XmTextFontContextGetUsage(context,&before),"negative query observation");
    require(!XmTextLayoutCreateWithContext(context,"\xf4\x8f\xbf\xbf",4,64),"negative cache changed refusal");
    require(XmTextFontContextGetUsage(context,&after)&&after.queries==before.queries&&after.cache_hits>before.cache_hits,"missing font outcome was not cached");
    for(index=0;index<512;++index) {
        const unsigned long scalar=0x100000ul+index;
        char utf8[4]={(char)(0xf0|(scalar>>18)),(char)(0x80|((scalar>>12)&63)),
                      (char)(0x80|((scalar>>6)&63)),(char)(0x80|(scalar&63))};
        require(!XmTextLayoutCreateWithContext(context,utf8,sizeof utf8,64),"missing private-plane glyph unexpectedly substituted");
    }
    require(XmTextFontContextGetUsage(context,&after)&&after.entries==256&&after.key_bytes<=256u*1024u,"resolver eviction exceeded entry/byte bounds");
    {
        char cluster[2052];size_t mark;
        for(mark=4;mark<sizeof cluster;mark+=2) {cluster[mark]=(char)0xcc;cluster[mark+1]=(char)0x81;}
        for(index=0;index<200;++index) {
            const unsigned long scalar=0x101000ul+index;
            cluster[0]=(char)(0xf0|(scalar>>18));cluster[1]=(char)(0x80|((scalar>>12)&63));
            cluster[2]=(char)(0x80|((scalar>>6)&63));cluster[3]=(char)(0x80|(scalar&63));
            require(!XmTextLayoutCreateWithContext(context,cluster,sizeof cluster,64),"missing long grapheme substituted partial coverage");
        }
        require(XmTextFontContextGetUsage(context,&after)&&after.entries<256&&after.key_bytes<=256u*1024u&&after.key_bytes>=260000,
                "resolver key-byte ceiling did not evict below its separate entry ceiling");
    }
    require(XmTextLayoutWidth(retained)==width&&XmTextLayoutCaret(retained,0,0,&x),"eviction/config refresh invalidated retained capture");
    require(FcConfigSetCurrent(original)&&XmTextFontContextInvalidate(context),"restore font configuration");
    FcConfigDestroy(empty);FcConfigDestroy(original);
    {
        XftFont *large=XftFontOpenName(display,DefaultScreen(display),"DejaVu Sans:size=32");
        XmTextFontContext *other;XmTextLayout *large_line,*recaptured;
        require(large!=NULL,"different-size font fixture");
        other=XmTextFontContextCreate(display,DefaultScreen(display),large,NULL);require(other!=NULL,"different-size resolver fixture");
        large_line=XmTextLayoutCreateWithContext(other,fallback,strlen(fallback),64);
        require(large_line&&XmTextLayoutWidth(large_line)>width,"different-size font did not exercise independent metrics");
        recaptured=XmTextParagraphLine(small,0,XmTextParagraphScalarCount(small));
        require(recaptured&&XmTextLayoutWidth(recaptured)==width,"cached shaping reused another font size/FTface activation");
        XmTextLayoutFree(recaptured);XmTextLayoutFree(large_line);XmTextFontContextFree(other);XftFontClose(display,large);
    }
    XmTextFontContextFree(context);
    require(XmTextLayoutWidth(retained)==width&&XmTextLayoutCaret(retained,0,0,&x),"resolver destruction invalidated retained font leases");
    {
        Pixmap pixmap=XCreatePixmap(display,DefaultRootWindow(display),240,80,DefaultDepth(display,DefaultScreen(display)));
        XftDraw *draw=XftDrawCreate(display,pixmap,DefaultVisual(display,DefaultScreen(display)),DefaultColormap(display,DefaultScreen(display)));
        XftColor color;XRectangle viewport={0,0,240,80};XImage *image;int px,py,ink=0;
        require(draw&&XftColorAllocName(display,DefaultVisual(display,DefaultScreen(display)),DefaultColormap(display,DefaultScreen(display)),"white",&color),"retained font draw setup");
        XftDrawRect(draw,&color,0,0,240,80);XftColorFree(display,DefaultVisual(display,DefaultScreen(display)),DefaultColormap(display,DefaultScreen(display)),&color);
        require(XftColorAllocName(display,DefaultVisual(display,DefaultScreen(display)),DefaultColormap(display,DefaultScreen(display)),"black",&color),"retained font foreground");
        require(XmTextLayoutDraw(retained,draw,&color,10,50,0,XmTextLayoutScalarCount(retained),&viewport),"evicted font lease failed real draw");
        XSync(display,False);image=XGetImage(display,pixmap,0,0,240,80,AllPlanes,ZPixmap);require(image!=NULL,"retained font readback");
        for(py=0;py<80;++py) for(px=0;px<240;++px) if(XGetPixel(image,px,py)!=XGetPixel(image,239,79)) ++ink;
        require(ink>20,"retained fallback font painted no ink after context destruction");
        XDestroyImage(image);XftColorFree(display,DefaultVisual(display,DefaultScreen(display)),DefaultColormap(display,DefaultScreen(display)),&color);XftDrawDestroy(draw);XFreePixmap(display,pixmap);
    }
    XmTextLayoutFree(retained);XmTextParagraphFree(small);
}
int main(void) {
    Display *display = XOpenDisplay(NULL);
    XftFont *font;
    XmTextLayout *arabic, *mixed, *combining, *ligature, *fallback, *tabs;
    double beginning, end;
    const char *arabic_text = "\xd8\xa7\xd9\x84\xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a\xd8\xa9";
    const char *mixed_text = "abc \xd7\x90\xd7\x91\xd7\x92 \xd8\xa8\xd8\xad\xd8\xb1 xyz";
    XftDraw *draw;
    XftColor color;
    Pixmap pixmap;
    XRectangle viewport = {0, 0, 800, 120};
    XImage *image;
    unsigned long first;
    int changed = 0, x, y;
    require(display != NULL && XmTextLayoutAvailable(), "Unicode layout display/features unavailable");
    font = XftFontOpenName(display, DefaultScreen(display), "DejaVu Sans:size=18");
    require(font != NULL, "fixture font unavailable");
    check_font_context(display,font);
    arabic = XmTextLayoutCreate(display, font, arabic_text, strlen(arabic_text));
    mixed = XmTextLayoutCreate(display, font, mixed_text, strlen(mixed_text));
    combining = XmTextLayoutCreate(display, font, "e\xcc\x81", 3);
    ligature = XmTextLayoutCreate(display, font, "office", 6);
    fallback = XmTextLayoutCreate(display, font, "latin \xe4\xb8\xad\xe6\x96\x87", 12);
    tabs = XmTextLayoutCreateWithTabWidth(display, font, "a\tb", 3, 64);
    require(arabic != NULL, "Arabic paragraph creation failed");
    require(mixed != NULL, "mixed paragraph creation failed");
    require(combining != NULL, "combining paragraph creation failed");
    require(ligature != NULL, "ligature paragraph creation failed");
    require(fallback != NULL, "font fallback paragraph creation failed");
    require(tabs != NULL, "tab paragraph creation failed");
    check_positions(arabic, arabic_text); check_positions(mixed, mixed_text); check_positions(combining, "e\xcc\x81"); check_positions(ligature, "office");
    require(XmTextLayoutCaret(arabic, 0, 0, &beginning) && XmTextLayoutCaret(arabic, 7, 0, &end) && beginning > end, "RTL caret order is logical instead of visual");
    check_positions(fallback, "latin \xe4\xb8\xad\xe6\x96\x87"); check_positions(tabs, "a\tb");
    require(XmTextLayoutCaret(tabs, 2, 0, &beginning) && fabs(beginning - 64) < 0.01, "tab did not use the capture tab stop");
    require(!XmTextLayoutCaret(combining, 1, 0, &beginning), "combining interior admitted as caret");
    pixmap = XCreatePixmap(display, RootWindow(display, DefaultScreen(display)), 800, 120, DefaultDepth(display, DefaultScreen(display)));
    draw = XftDrawCreate(display, pixmap, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)));
    require(draw != NULL && XftColorAllocName(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), "white", &color), "fixture draw/color creation");
    XftDrawRect(draw, &color, 0, 0, 800, 120); XftColorFree(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &color);
    require(XftColorAllocName(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), "black", &color), "fixture foreground");
    require(XmTextLayoutDraw(mixed, draw, &color, 10, 50, 0, XmTextLayoutScalarCount(mixed), &viewport), "captured glyph draw failed");
    XSync(display, False); image = XGetImage(display, pixmap, 0, 0, 800, 120, AllPlanes, ZPixmap);
    require(image != NULL, "fixture readback failed");
    first = XGetPixel(image, 799, 119);
    for (y = 0; y < 120; ++y) for (x = 0; x < 800; ++x) if (XGetPixel(image, x, y) != first) ++changed;
    require(changed > 100, "shaped glyph capture painted no real text");
    XDestroyImage(image); XftColorFree(display, DefaultVisual(display, DefaultScreen(display)), DefaultColormap(display, DefaultScreen(display)), &color);
    XftDrawDestroy(draw); XFreePixmap(display, pixmap);
    XmTextLayoutFree(arabic); XmTextLayoutFree(mixed); XmTextLayoutFree(combining); XmTextLayoutFree(ligature); XmTextLayoutFree(fallback); XmTextLayoutFree(tabs);
    XftFontClose(display, font); XCloseDisplay(display);
    puts("Shaped Arabic/Hebrew/Latin paragraph glyphs, grapheme carets and visual hits passed");
    return 0;
}
