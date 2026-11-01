#!/usr/bin/env python3
"""Compile the production keysym parser and checked integer helpers together."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
root=Path(__file__).resolve().parents[2]
text=(root/'clients/mwm/WmResParse.c').read_text()
def function(name):
    start=text.rindex(('void ' if name=='ScanWhitespace' else 'static Boolean ')+name)
    end=text.index('} /* END OF FUNCTION '+name+' */',start)+1
    return text[start:end]
preamble='''#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#define NO_MULTIBYTE 1
#define MAX_KEYSYM_STRLEN 100
#define min(a,b) ((a)<(b)?(a):(b))
static Boolean StrToNum(unsigned char *, unsigned int *);
static Boolean StrToHex(unsigned char *, unsigned int *);
static Boolean StrToOct(unsigned char *, unsigned int *);
'''
main='''int main(void) {
 struct { const char *text; int valid; unsigned value; } cases[] = {
 {"123456789",1,123456789U},{"4294967295",1,UINT_MAX},{"4294967296",0,0},
 {"0x1234",1,0x1234},{"0777",1,0777},{"089",0,0},{"unknown_keysym",0,0},
 {"Return",1,0xff0d},{"123:rest",1,123}};
 for (unsigned i=0;i<sizeof(cases)/sizeof(cases[0]);++i) {
  unsigned char buffer[120]; strcpy((char *)buffer,cases[i].text);
  unsigned char *cursor=buffer; unsigned value=999;
  Boolean valid=ParseKeySym(&cursor,0,&value);
  if (valid!=cases[i].valid || (valid && value!=cases[i].value) || (!valid && value!=NoSymbol)) {
   fprintf(stderr,"FAIL keysym %s: valid=%d value=%u\\n",cases[i].text,valid,value);return 1;
  }
 }
 puts("PASS production numeric keysym caller, named symbols, bounds and invalid input");return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='mwm-keysym-') as temporary:
    source=Path(temporary)/'test.c';binary=Path(temporary)/'test'
    source.write_text(preamble+'\n'.join(function(name) for name in ('StrToNum','StrToHex','StrToOct','ScanWhitespace','ParseKeySym'))+main)
    command=[os.environ.get('CC','cc'),'-std=c99','-Wall','-Wextra','-Werror','-Wno-unused-parameter',str(source),'-lX11','-o',str(binary)]
    if sys.platform=='darwin':command[1:1]=['-I/opt/X11/include','-L/opt/X11/lib']
    subprocess.run(command,check=True);subprocess.run([str(binary)],check=True)
