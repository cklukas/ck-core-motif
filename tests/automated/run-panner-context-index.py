#!/usr/bin/env python3
"""Exercise Panner's production pointer-sized X context lookup on the host ABI."""
import os
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parents[2]
text=(root/'demos/programs/panner/panner.c').read_text()
start=text.index('static int ContextDisplayIndex(')
end=text.index('\n}',start)+2
program='''#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
typedef void Display;
typedef unsigned long Window;
typedef char *XPointer;
#define MAX_DISPLAY_COUNT 10
static int context;
static intptr_t stored_index;
static int missing;
static int XFindContext(Display *d,Window w,int c,XPointer *out){
 (void)d;(void)w;(void)c;if(missing)return 1;*out=(XPointer)stored_index;return 0;
}
'''+text[start:end]+'''
int main(void){
 for(stored_index=0;stored_index<MAX_DISPLAY_COUNT;++stored_index)
  assert(ContextDisplayIndex(NULL,0)==stored_index);
 stored_index=-1;assert(ContextDisplayIndex(NULL,0)==-1);
 stored_index=MAX_DISPLAY_COUNT;assert(ContextDisplayIndex(NULL,0)==-1);
 if(sizeof(intptr_t)>sizeof(int)){
  stored_index=(intptr_t)UINT32_MAX+2;assert(ContextDisplayIndex(NULL,0)==-1);
 }
 missing=1;assert(ContextDisplayIndex(NULL,0)==-1);
 puts("PASS Panner pointer-sized context storage, capacity, wide and missing indices");
 return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='panner-context-',dir=os.environ.get('TMPDIR')) as temporary:
 source=Path(temporary)/'test.c';binary=Path(temporary)/'test';source.write_text(program)
 subprocess.run([os.environ.get('CC','cc'),'-std=c99','-Wall','-Wextra','-Werror',
                 '-fsanitize=address,undefined',str(source),'-o',str(binary)],check=True)
 subprocess.run([str(binary)],check=True)
