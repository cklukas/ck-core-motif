/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/Form.h>
#include <Xm/TextUnicode.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define REPEATS 8192u
#define BLOCK_BYTES 131u
#define BLOCK_SCALARS 93u
#define BLOCK_GRAPHEMES 85u
/* Explicit counts include combining marks, supplementary scalars, ZWJ and RI.
 * The final separator is LF in Text and a space in the single-line TextField. */
static const char block[] = "abc e\xcc\x81 \xe4\xb8\xad\xe6\x96\x87 "
    "\xd7\x90\xd7\x91\xd7\x92 \xd8\xa8\xd8\xad\xd8\xb1 "
    "\xf0\x9f\x91\xa9\xe2\x80\x8d\xf0\x9f\x91\xa9\xe2\x80\x8d"
    "\xf0\x9f\x91\xa7\xe2\x80\x8d\xf0\x9f\x91\xa6 "
    "\xf0\x9f\x87\xa9\xf0\x9f\x87\xaa \xf0\x9f\x98\x80 0123456789 "
    "wrap wrap wrap wrap wrap wrap wrap wrap wrap wrap \n";
static const char family[] = "\xf0\x9f\x91\xa9\xe2\x80\x8d\xf0\x9f\x91\xa9\xe2\x80\x8d"
    "\xf0\x9f\x91\xa7\xe2\x80\x8d\xf0\x9f\x91\xa6";
static void require(int condition,const char *detail) {
    if(!condition) {fprintf(stderr,"large text: %s\n",detail);exit(1);}
}
static double now(void) {
    struct timespec value;
    require(clock_gettime(CLOCK_MONOTONIC,&value)==0,"monotonic clock");
    return value.tv_sec+value.tv_nsec/1e9;
}
static void timing(const char *stage,double start) {
    printf("%s seconds=%.6f\n",stage,now()-start);fflush(stdout);
}
static char *source(int field) {
    size_t i;char *value=malloc(REPEATS*BLOCK_BYTES+1);
    require(value!=NULL&&sizeof(block)-1==BLOCK_BYTES,"fixture byte count/allocation");
    for(i=0;i<REPEATS;++i) {
        memcpy(value+i*BLOCK_BYTES,block,BLOCK_BYTES);
        if(field) value[(i+1)*BLOCK_BYTES-1]=' ';
    }
    value[REPEATS*BLOCK_BYTES]=0;return value;
}
static void check_map(const XmTextUnicodeMap *map) {
    const size_t samples[]={0,REPEATS/2,REPEATS-1};size_t sample;
    require(XmTextUnicodeByteCount(map)==REPEATS*BLOCK_BYTES,"full byte count");
    require(XmTextUnicodeScalarCount(map)==REPEATS*BLOCK_SCALARS,"full scalar count");
    require(XmTextUnicodeGraphemeCount(map)==REPEATS*BLOCK_GRAPHEMES,"full grapheme count");
    for(sample=0;sample<sizeof(samples)/sizeof(samples[0]);++sample) {
        const size_t scalar=samples[sample]*BLOCK_SCALARS,grapheme=samples[sample]*BLOCK_GRAPHEMES;
        size_t byte,inverse,next;
        require(XmTextUnicodeByteAtScalar(map,scalar,&byte)&&byte==samples[sample]*BLOCK_BYTES,"distant byte/scalar count");
        require(XmTextUnicodeScalarAtByte(map,byte,&inverse)&&inverse==scalar,"distant byte/scalar inverse");
        require(XmTextUnicodeScalarAtGrapheme(map,grapheme+17,&inverse)&&inverse==scalar+18,"distant family identity");
        require(XmTextUnicodeNextGrapheme(map,scalar+18,&next)&&next==scalar+25,"family grapheme boundary");
        require(!XmTextUnicodeIsGraphemeBoundary(map,scalar+5),"combining mark admitted as caret boundary");
        require(XmTextUnicodeNextGrapheme(map,scalar+26,&next)&&next==scalar+28,"flag grapheme boundary");
    }
    {size_t end;require(XmTextUnicodeByteAtScalar(map,REPEATS*BLOCK_SCALARS,&end)&&end==REPEATS*BLOCK_BYTES,"end mapping");}
}
static char *get(Widget widget,int field) {return field?XmTextFieldGetString(widget):XmTextGetString(widget);}
static void replace(Widget widget,int field,XmTextPosition first,XmTextPosition last,const char *value) {
    if(field) XmTextFieldReplace(widget,first,last,(char *)value);
    else XmTextReplace(widget,first,last,(char *)value);
}
static void check_widget(Widget widget,int field,const char *value,const XmTextUnicodeMap *map) {
    const size_t samples[]={0,REPEATS/2,REPEATS-1};size_t sample;
    double start=now();char *actual;
    if(field) XmTextFieldSetString(widget,(char *)value);else XmTextSetString(widget,(char *)value);
    timing(field?"TextField.set":"Text.set",start);
    actual=get(widget,field);require(actual&&strcmp(actual,value)==0,"widget truncated/changed full source");XtFree(actual);
    require((field?XmTextFieldGetLastPosition(widget):XmTextGetLastPosition(widget))==REPEATS*BLOCK_SCALARS,"widget positions are not full scalar positions");
    start=now();
    for(sample=0;sample<sizeof(samples)/sizeof(samples[0]);++sample) {
        const XmTextPosition position=(XmTextPosition)(samples[sample]*BLOCK_SCALARS+29);
        Position x,y,back_x,back_y;XmTextPosition hit;
        if(field) {XmTextFieldSetInsertionPosition(widget,position);XmTextFieldShowPosition(widget,position);}
        else {XmTextSetInsertionPosition(widget,position);XmTextShowPosition(widget,position);}
        require(field?XmTextFieldPosToXY(widget,position,&x,&y):XmTextPosToXY(widget,position,&x,&y),"distant supplementary caret unavailable");
        hit=field?XmTextFieldXYToPos(widget,x,y):XmTextXYToPos(widget,x,y);
        require(hit>=0&&(size_t)hit<=REPEATS*BLOCK_SCALARS&&XmTextUnicodeIsGraphemeBoundary(map,(size_t)hit),"distant hit escaped source or split grapheme");
        require(field?XmTextFieldPosToXY(widget,hit,&back_x,&back_y):XmTextPosToXY(widget,hit,&back_x,&back_y),"distant hit inverse unavailable");
        require(abs((int)x-back_x)<=1&&y==back_y,"distant caret/hit geometry disagrees");
    }
    timing(field?"TextField.carets":"Text.carets",start);
    {
        const XmTextPosition first=(XmTextPosition)(REPEATS/2*BLOCK_SCALARS+4);
        const char changed[]="Q\xf0\x9f\x98\x80";char substring[16];size_t byte;
        require(XmTextUnicodeByteAtScalar(map,(size_t)first,&byte),"edit byte boundary");start=now();
        replace(widget,field,first,first+2,changed);
        require((field?XmTextFieldGetSubstring(widget,first,2,sizeof(substring),substring):XmTextGetSubstring(widget,first,2,sizeof(substring),substring))==XmCOPY_SUCCEEDED&&strcmp(substring,changed)==0,"supplementary edit substring/scalar mismatch");
        actual=get(widget,field);
        require(strlen(actual)==REPEATS*BLOCK_BYTES+2&&memcmp(actual,value,byte)==0&&memcmp(actual+byte,changed,sizeof(changed)-1)==0&&strcmp(actual+byte+sizeof(changed)-1,value+byte+3)==0,"edit altered unrelated distant source");XtFree(actual);
        replace(widget,field,first,first+2,"e\xcc\x81");actual=get(widget,field);
        require(strcmp(actual,value)==0,"restored mutation not byte exact");XtFree(actual);
        timing(field?"TextField.mutate":"Text.mutate",start);
    }
    {
        const XmTextPosition first=(XmTextPosition)(REPEATS/2*BLOCK_SCALARS+18);size_t byte;XEvent event;
        require(XmTextUnicodeByteAtScalar(map,(size_t)first,&byte),"family edit byte boundary");
        memset(&event,0,sizeof event);event.xkey.type=KeyPress;event.xkey.display=XtDisplay(widget);event.xkey.window=XtWindow(widget);event.xkey.time=CurrentTime;
        if(field) XmTextFieldSetInsertionPosition(widget,first);else XmTextSetInsertionPosition(widget,first);
        XtCallActionProc(widget,"forward-character",&event,NULL,0);
        require((field?XmTextFieldGetInsertionPosition(widget):XmTextGetInsertionPosition(widget))==first+7,"large widget navigation split supplementary ZWJ family");
        XtCallActionProc(widget,"delete-previous-character",&event,NULL,0);actual=get(widget,field);
        require(strlen(actual)==REPEATS*BLOCK_BYTES-(sizeof(family)-1)&&memcmp(actual,value,byte)==0&&strcmp(actual+byte,value+byte+sizeof(family)-1)==0,"large widget deletion split family or unrelated source");XtFree(actual);
        replace(widget,field,first,first,family);actual=get(widget,field);require(strcmp(actual,value)==0,"family restoration changed source");XtFree(actual);
    }
    if(!field) {
        Position x,y,other_x,other_y;
        XmTextShowPosition(widget,0);
        require(XmTextPosToXY(widget,0,&x,&y)&&XmTextPosToXY(widget,91,&other_x,&other_y)&&other_y>y,"large multiline source did not wrap its first paragraph");
    }
}
int main(int argc,char **argv) {
    XtAppContext app;Widget shell,form,text,field;XmFontListEntry entry;XmFontList fonts;
    char *text_source,*field_source;XmTextUnicodeMap *text_map,*field_map;double start;
    require(setlocale(LC_ALL,"C.UTF-8")!=NULL,"UTF8 locale unavailable");XtSetLanguageProc(NULL,NULL,NULL);
    require(XmTextUnicodeAvailable(),"Unicode support unavailable");
    text_source=source(0);field_source=source(1);start=now();
    text_map=XmTextUnicodeMapCreate(text_source,REPEATS*BLOCK_BYTES);field_map=XmTextUnicodeMapCreate(field_source,REPEATS*BLOCK_BYTES);
    require(text_map&&field_map,"large Unicode maps");check_map(text_map);check_map(field_map);timing("Unicode.maps",start);
    shell=XtVaAppInitialize(&app,"LargeUnicodeFixture",NULL,0,&argc,argv,NULL,NULL);
    form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,XmNwidth,480,XmNheight,400,XmNresizePolicy,XmRESIZE_NONE,NULL);
    entry=XmFontListEntryLoad(XtDisplay(shell),"DejaVu Sans",XmFONT_IS_XFT,XmFONTLIST_DEFAULT_TAG);require(entry!=NULL,"fixture Xft font unavailable");
    fonts=XmFontListAppendEntry(NULL,entry);XmFontListEntryFree(&entry);
    field=XtVaCreateManagedWidget("field",xmTextFieldWidgetClass,form,XmNfontList,fonts,XmNwidth,440,XmNheight,45,XmNx,10,XmNy,10,NULL);
    text=XtVaCreateManagedWidget("text",xmTextWidgetClass,form,XmNfontList,fonts,XmNwidth,440,XmNheight,320,XmNx,10,XmNy,70,XmNeditMode,XmMULTI_LINE_EDIT,XmNwordWrap,True,XmNscrollHorizontal,False,XmNresizeWidth,False,XmNresizeHeight,False,NULL);
    XmFontListFree(fonts);XtRealizeWidget(shell);start=now();
    check_widget(text,0,text_source,text_map);check_widget(field,1,field_source,field_map);timing("Widgets.total",start);
    printf("large text passed bytes=%u scalars=%u graphemes=%u per widget\n",REPEATS*BLOCK_BYTES,REPEATS*BLOCK_SCALARS,REPEATS*BLOCK_GRAPHEMES);
    XmTextUnicodeMapFree(text_map);XmTextUnicodeMapFree(field_map);free(text_source);free(field_source);
    XtDestroyWidget(shell);XtDestroyApplicationContext(app);return 0;
}
