/* SPDX-License-Identifier: LGPL-2.0-or-later
 * Display-independent RGB raster operations used by the drawing demo.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "paint.h"
#ifdef PNG_SUPPORTED
#include <png.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>

int paint_image_create(PaintImage *im, int w, int h)
{
    size_t n, i;
    uint32_t *p;
    if (w < 1 || h < 1 || w > PAINT_MAX_SIDE || h > PAINT_MAX_SIDE) return 0;
    n = (size_t)w * h;
    p = malloc(n * sizeof(*p));
    if (!p) return 0;
    for (i = 0; i < n; ++i) p[i] = PAINT_WHITE;
    paint_image_free(im);
    im->pixels = p; im->width = w; im->height = h;
    return 1;
}
void paint_image_free(PaintImage *im) { free(im->pixels); memset(im, 0, sizeof(*im)); }
int paint_image_copy(PaintImage *to, const PaintImage *from)
{
    if (to == from) return 1;
    if (!paint_image_create(to, from->width, from->height)) return 0;
    memcpy(to->pixels, from->pixels, (size_t)from->width * from->height * sizeof(uint32_t));
    return 1;
}
static void clear_stack(PaintImage *s, int *n) { while (*n) paint_image_free(&s[--*n]); }
static void push(PaintImage *s, int *n, PaintImage im)
{
    if (*n == PAINT_HISTORY) {
        paint_image_free(s);
        memmove(s, s + 1, (PAINT_HISTORY - 1) * sizeof(*s));
        --*n;
    }
    s[(*n)++] = im;
}
int paint_begin(PaintDocument *d)
{
    PaintImage copy = {0};
    if (!paint_image_copy(&copy, &d->image)) return 0;
    push(d->undo, &d->undo_count, copy);
    clear_stack(d->redo, &d->redo_count);
    return 1;
}
int paint_undo(PaintDocument *d)
{
    if (!d->undo_count) return 0;
    push(d->redo, &d->redo_count, d->image);
    d->image = d->undo[--d->undo_count];
    memset(&d->undo[d->undo_count], 0, sizeof(PaintImage));
    return 1;
}
int paint_redo(PaintDocument *d)
{
    if (!d->redo_count) return 0;
    push(d->undo, &d->undo_count, d->image);
    d->image = d->redo[--d->redo_count];
    memset(&d->redo[d->redo_count], 0, sizeof(PaintImage));
    return 1;
}
void paint_document_free(PaintDocument *d)
{
    paint_image_free(&d->image);
    clear_stack(d->undo, &d->undo_count); clear_stack(d->redo, &d->redo_count);
}
uint32_t paint_get(const PaintImage *im, int x, int y)
{
    if (x < 0 || y < 0 || x >= im->width || y >= im->height) return PAINT_WHITE;
    return im->pixels[(size_t)y * im->width + x];
}
static void pixel(PaintImage *im, int x, int y, uint32_t c)
{
    if (x >= 0 && y >= 0 && x < im->width && y < im->height)
        im->pixels[(size_t)y * im->width + x] = c;
}
void paint_dot(PaintImage *im, int x, int y, int size, uint32_t c)
{
    int i, j, lo, hi;
    if (size < 1) size = 1;
    if (size > 128) size = 128;
    lo = -(size / 2); hi = lo + size;
    for (j = lo; j < hi; ++j) for (i = lo; i < hi; ++i)
        if (size < 3 || i*i + j*j <= size*size/4) pixel(im, x+i, y+j, c);
}
void paint_line(PaintImage *im, int x0, int y0, int x1, int y1, int size, uint32_t c)
{
    int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1-y0), sy = y0 < y1 ? 1 : -1, err = dx+dy, e;
    for (;;) {
        paint_dot(im, x0, y0, size, c);
        if (x0 == x1 && y0 == y1) break;
        e = 2*err;
        if (e >= dy) { err += dy; x0 += sx; }
        if (e <= dx) { err += dx; y0 += sy; }
    }
}
static void order(int *a, int *b) { if (*a > *b) { int t = *a; *a = *b; *b = t; } }
void paint_rectangle(PaintImage *im, int x0, int y0, int x1, int y1, int size, int fill, uint32_t c)
{
    int x, y;
    order(&x0, &x1); order(&y0, &y1);
    if (fill) {
        if (x0 < 0) x0 = 0;
        if (y0 < 0) y0 = 0;
        if (x1 >= im->width) x1 = im->width-1;
        if (y1 >= im->height) y1 = im->height-1;
        for (y = y0; y <= y1; ++y) for (x = x0; x <= x1; ++x) pixel(im,x,y,c);
    } else {
        paint_line(im,x0,y0,x1,y0,size,c); paint_line(im,x1,y0,x1,y1,size,c);
        paint_line(im,x1,y1,x0,y1,size,c); paint_line(im,x0,y1,x0,y0,size,c);
    }
}
void paint_ellipse(PaintImage *im, int x0, int y0, int x1, int y1, int size, int fill, uint32_t c)
{
    double rx, ry, cx, cy, t;
    int x, y, steps, px, py, nx, ny, i;
    order(&x0,&x1); order(&y0,&y1);
    if (x0 == x1 || y0 == y1) { paint_line(im,x0,y0,x1,y1,size,c); return; }
    rx = (x1-x0)/2.0; ry = (y1-y0)/2.0; cx = x0+rx; cy = y0+ry;
    if (fill) {
        for (y = y0 < 0 ? 0 : y0; y <= y1 && y < im->height; ++y)
            for (x = x0 < 0 ? 0 : x0; x <= x1 && x < im->width; ++x)
                if ((x-cx)*(x-cx)/(rx*rx)+(y-cy)*(y-cy)/(ry*ry) <= 1.0) pixel(im,x,y,c);
    } else {
        steps = (int)(6.28318530718 * (rx+ry)) + 1;
        px = x1; py = (int)lround(cy);
        for (i = 1; i <= steps; ++i) {
            t = i*6.28318530718/steps;
            nx = (int)lround(cx+rx*cos(t)); ny = (int)lround(cy+ry*sin(t));
            paint_line(im,px,py,nx,ny,size,c); px=nx; py=ny;
        }
    }
}
int paint_fill(PaintImage *im, int x, int y, uint32_t c)
{
    size_t *queue, head=0, tail=0, n=(size_t)im->width*im->height, p, q;
    uint32_t old;
    if (x<0 || y<0 || x>=im->width || y>=im->height) return 1;
    old=paint_get(im,x,y); if (old==c) return 1;
    queue=malloc(n*sizeof(*queue)); if (!queue) return 0;
    p=(size_t)y*im->width+x; queue[tail++]=p; im->pixels[p]=c;
    while (head<tail) {
        p=queue[head++]; x=(int)(p%im->width); y=(int)(p/im->width);
#define VISIT(Q) do { q=(Q); if (im->pixels[q]==old) { im->pixels[q]=c; queue[tail++]=q; } } while(0)
        if (x>0) VISIT(p-1);
        if (x+1<im->width) VISIT(p+1);
        if (y>0) VISIT(p-im->width);
        if (y+1<im->height) VISIT(p+im->width);
#undef VISIT
    }
    free(queue); return 1;
}
int paint_crop(PaintImage *out, const PaintImage *im, int x, int y, int w, int h)
{
    int i,j;
    if (!paint_image_create(out,w,h)) return 0;
    for(j=0;j<h;++j) for(i=0;i<w;++i) out->pixels[(size_t)j*w+i]=paint_get(im,x+i,y+j);
    return 1;
}
void paint_paste(PaintImage *im, const PaintImage *clip, int x, int y)
{
    int i,j;
    for(j=0;j<clip->height;++j) for(i=0;i<clip->width;++i) pixel(im,x+i,y+j,paint_get(clip,i,j));
}
static unsigned long le(const unsigned char *p, int n)
{ unsigned long v=0; int i; for(i=n-1;i>=0;--i) v=(v<<8)|p[i]; return v; }
static void putle(unsigned char *p, unsigned long v, int n)
{ int i; for(i=0;i<n;++i) { p[i]=(unsigned char)v; v>>=8; } }
#ifdef PNG_SUPPORTED
static int load_png(PaintImage *im, const char *path, char *error, size_t cap)
{
    png_image png; png_color background={255,255,255}; PaintImage next={0};
    unsigned char *bytes=NULL; size_t i,n;
    memset(&png,0,sizeof(png)); png.version=PNG_IMAGE_VERSION;
    if(!png_image_begin_read_from_file(&png,path)) goto failed;
    if(png.width>PAINT_MAX_SIDE || png.height>PAINT_MAX_SIDE ||
       !paint_image_create(&next,(int)png.width,(int)png.height)) {
        snprintf(error,cap,"PNG dimensions exceed the canvas limit or memory is exhausted.");
        png_image_free(&png); return 0;
    }
    png.format=PNG_FORMAT_RGB; bytes=malloc(PNG_IMAGE_SIZE(png));
    if(!bytes) { snprintf(error,cap,"Not enough memory to open PNG."); goto cleanup; }
    if(!png_image_finish_read(&png,&background,bytes,0,NULL)) goto failed;
    n=(size_t)next.width*next.height;
    for(i=0;i<n;++i) next.pixels[i]=((uint32_t)bytes[3*i]<<16)|((uint32_t)bytes[3*i+1]<<8)|bytes[3*i+2];
    free(bytes);png_image_free(&png);paint_image_free(im);*im=next;return 1;
failed:
    snprintf(error,cap,"Cannot read PNG: %.160s",png.message);
cleanup:
    free(bytes);png_image_free(&png);paint_image_free(&next);return 0;
}
#endif
int paint_load(PaintImage *im, const char *path, char *error, size_t cap)
{
    FILE *f=fopen(path,"rb"); unsigned char h[54], rgb[3];
    PaintImage next={0}; int w,height,x,y,top; long offset; size_t stride,pad;
    if (!f) { snprintf(error,cap,"Cannot open: %s",strerror(errno)); return 0; }
#ifdef PNG_SUPPORTED
    if(fread(h,1,8,f)==8 && !png_sig_cmp(h,0,8)) { fclose(f); return load_png(im,path,error,cap); }
    rewind(f);
#endif
    if (fread(h,1,54,f)!=54 || h[0]!='B' || h[1]!='M' || le(h+14,4)<40 ||
        le(h+26,2)!=1 || le(h+28,2)!=24 || le(h+30,4)!=0) goto invalid;
    w=(int32_t)le(h+18,4); height=(int32_t)le(h+22,4);
    if (height == INT32_MIN) goto invalid;
    top=height<0; if(top) height=-height;
    offset=(long)le(h+10,4);
    if(offset<54 || !paint_image_create(&next,w,height) || fseek(f,offset,SEEK_SET)) goto invalid;
    stride=((size_t)w*3+3)&~(size_t)3; pad=stride-w*3;
    for(y=0;y<height;++y) {
        for(x=0;x<w;++x) {
            if(fread(rgb,1,3,f)!=3) goto invalid;
            pixel(&next,x,top?y:height-1-y,((uint32_t)rgb[2]<<16)|((uint32_t)rgb[1]<<8)|rgb[0]);
        }
        if(pad && fread(rgb,1,pad,f)!=pad) goto invalid;
    }
    fclose(f); paint_image_free(im); *im=next; return 1;
invalid:
    fclose(f); paint_image_free(&next);
    snprintf(error,cap,"Use an uncompressed 24-bit BMP, at most %d x %d pixels. The file may be incomplete.",PAINT_MAX_SIDE,PAINT_MAX_SIDE);
    return 0;
}
int paint_save(const PaintImage *im, const char *path, char *error, size_t cap)
{
    /* Write beside the destination, then rename: failed writes preserve it. */
    unsigned char h[54]={0}, rgb[3], zero[3]={0};
    size_t stride=((size_t)im->width*3+3)&~(size_t)3, pad=stride-im->width*3;
    const char *extension=strrchr(path,'.');
    char *temporary; FILE *f; int fd,x,y,ok=1; uint32_t c;
    if(!extension || (strcasecmp(extension,".bmp")
#ifdef PNG_SUPPORTED
       && strcasecmp(extension,".png")
#endif
    )) { snprintf(error,cap,"Choose a supported filename extension (.bmp"
#ifdef PNG_SUPPORTED
        " or .png"
#endif
        ")."); return 0; }
    temporary=malloc(strlen(path)+16);
    if(!temporary) { snprintf(error,cap,"Out of memory."); return 0; }
    sprintf(temporary,"%s.XXXXXX",path);
    fd=mkstemp(temporary);
    f=fd<0?NULL:fdopen(fd,"wb");
    if(!f) { if(fd>=0) { close(fd); remove(temporary); } free(temporary); snprintf(error,cap,"Cannot save: %s",strerror(errno)); return 0; }
#ifdef PNG_SUPPORTED
    {
        const char *suffix=strrchr(path,'.');
        if(suffix && !strcasecmp(suffix,".png")) {
            png_image png; unsigned char *bytes; size_t i,n=(size_t)im->width*im->height;
            memset(&png,0,sizeof(png));png.version=PNG_IMAGE_VERSION;
            png.width=im->width;png.height=im->height;png.format=PNG_FORMAT_RGB;
            bytes=malloc(n*3);
            if(bytes) {
                for(i=0;i<n;++i) { c=im->pixels[i];bytes[3*i]=(unsigned char)(c>>16);bytes[3*i+1]=(unsigned char)(c>>8);bytes[3*i+2]=(unsigned char)c; }
                ok=png_image_write_to_stdio(&png,f,0,bytes,0,NULL);free(bytes);
            } else ok=0;
            if(fclose(f)) ok=0;
            if(ok && rename(temporary,path)) ok=0;
            if(!ok) { snprintf(error,cap,"Could not save PNG: %.160s",png.message);remove(temporary); }
            png_image_free(&png);free(temporary);return ok;
        }
    }
#endif
    h[0]='B'; h[1]='M'; putle(h+2,54+stride*im->height,4); putle(h+10,54,4);
    putle(h+14,40,4); putle(h+18,im->width,4); putle(h+22,im->height,4);
    putle(h+26,1,2); putle(h+28,24,2); putle(h+34,stride*im->height,4);
    if(fwrite(h,1,54,f)!=54) ok=0;
    for(y=im->height-1;y>=0 && ok;--y) {
        for(x=0;x<im->width;++x) {
            c=paint_get(im,x,y); rgb[0]=(unsigned char)c; rgb[1]=(unsigned char)(c>>8); rgb[2]=(unsigned char)(c>>16);
            if(fwrite(rgb,1,3,f)!=3) { ok=0; break; }
        }
        if(pad && fwrite(zero,1,pad,f)!=pad) ok=0;
    }
    if(fclose(f)) ok=0;
    if(ok && rename(temporary,path)) ok=0;
    if(!ok) { snprintf(error,cap,"Could not finish saving: %s",strerror(errno)); remove(temporary); }
    free(temporary); return ok;
}
