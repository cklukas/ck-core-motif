/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include "../../demos/programs/draw/paint.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc,char **argv)
{
    PaintDocument d={0}; PaintImage clip={0}, loaded={0}; char error[256],path[1024]; int i; FILE *f;
    assert(argc==2); snprintf(path,sizeof(path),"%s/roundtrip.bmp",argv[1]);
    assert(paint_image_create(&d.image,31,19));
    assert(!paint_image_create(&clip,0,10)); assert(!paint_image_create(&clip,4097,10));
    assert(paint_begin(&d)); paint_line(&d.image,1,1,29,17,1,0);
    assert(paint_get(&d.image,1,1)==0 && paint_get(&d.image,29,17)==0);
    assert(paint_undo(&d) && paint_get(&d.image,1,1)==PAINT_WHITE);
    assert(paint_redo(&d) && paint_get(&d.image,1,1)==0);
    assert(paint_begin(&d)); paint_rectangle(&d.image,2,2,20,15,1,0,0xff0000);
    assert(paint_fill(&d.image,3,2,0x00ff00)); assert(paint_get(&d.image,2,2)==0x00ff00);
    assert(paint_begin(&d)); paint_rectangle(&d.image,3,3,10,10,1,1,0x123456);
    assert(paint_get(&d.image,5,5)==0x123456); assert(paint_crop(&clip,&d.image,3,3,8,8));
    paint_paste(&d.image,&clip,22,12); assert(paint_get(&d.image,24,14)==0x123456);
    paint_ellipse(&d.image,3,3,15,15,1,1,0xabcdef); assert(paint_get(&d.image,9,9)==0xabcdef);
    paint_ellipse(&d.image,3,3,15,15,1,0,0); assert(paint_get(&d.image,15,9)==0);
    assert(paint_save(&d.image,path,error,sizeof(error)));
    assert(paint_load(&loaded,path,error,sizeof(error)));
    assert(loaded.width==31 && loaded.height==19);
    assert(!memcmp(loaded.pixels,d.image.pixels,31*19*sizeof(uint32_t)));
#ifdef PNG_SUPPORTED
    snprintf(path,sizeof(path),"%s/roundtrip.png",argv[1]);
    assert(paint_save(&d.image,path,error,sizeof(error)));
    assert(paint_load(&loaded,path,error,sizeof(error)));
    assert(!memcmp(loaded.pixels,d.image.pixels,31*19*sizeof(uint32_t)));
#endif
    f=fopen(path,"wb"); assert(f); fputs("BMbroken",f); fclose(f);
    assert(!paint_load(&loaded,path,error,sizeof(error)));
    assert(!memcmp(loaded.pixels,d.image.pixels,31*19*sizeof(uint32_t)));
    assert(paint_undo(&d)); assert(paint_begin(&d)); assert(!paint_redo(&d));
    for(i=0;i<PAINT_HISTORY+5;++i) { assert(paint_begin(&d));paint_dot(&d.image,0,0,1,(uint32_t)i); }
    assert(d.undo_count==PAINT_HISTORY);
    for(i=0;i<PAINT_HISTORY;++i) assert(paint_undo(&d));
    assert(!paint_undo(&d));
    paint_document_free(&d); paint_image_free(&clip); paint_image_free(&loaded);
    assert(paint_image_create(&loaded,512,512)); assert(paint_fill(&loaded,0,0,0x314159));
    for(i=0;i<512*512;++i) assert(loaded.pixels[i]==0x314159);
    paint_image_free(&loaded); remove(path);
    puts("PASS: raster tools, flood fill, clipping, 16-step undo/redo, branch edits, PNG/BMP padding/roundtrip and failed-load preservation");
    return 0;
}
