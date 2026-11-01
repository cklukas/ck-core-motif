#!/usr/bin/env python3
"""Execute production text-property conversion with allocation failures."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--out', type=Path, required=True)
args = parser.parse_args()
if not args.out.is_absolute():
    parser.error('--out must be an absolute fresh directory')
args.out.mkdir(parents=True, exist_ok=False)
(args.out / 'tmp').mkdir()
env = dict(os.environ, TMPDIR=str(args.out / 'tmp'))
source = Path(__file__).resolve().parents[2] / 'lib/Xm/TxtPropCv.c'
content = source.read_text()
start = content.index('XmCvtXmStringTableToTextProperty(Display *display,')
begin = content.index('{', start)
end, depth = begin + 1, 1
while depth:
    depth += (content[end] == '{') - (content[end] == '}')
    end += 1
body = 'int ' + content[start:end]
prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef unsigned int Cardinal;
typedef int Boolean;
typedef int Display;
typedef unsigned long Atom;
typedef unsigned char *XmString;
typedef XmString *XmStringTable;
typedef struct { unsigned char *value; Atom encoding; int format; unsigned long nitems; } XTextProperty;
typedef enum {XmSTYLE_COMPOUND_TEXT, XmSTYLE_COMPOUND_STRING, XmSTYLE_LOCALE,
 XmSTYLE_TEXT, XmSTYLE_STRING, XmSTYLE_STANDARD_ICC_TEXT} XmICCEncodingStyle;
#define True 1
#define False 0
#define Success 0
#define BadValue 2
#define BadAlloc 11
#define XNoMemory -1
#define XLocaleNotSupported -2
#define _LOCALE_TEXT 0
#define _STRING_TEXT 1
#define XA_STRING 31
#define XmSCOMPOUND_TEXT "COMPOUND_TEXT"
#define XmS_MOTIF_COMPOUND_STRING "MOTIF_COMPOUND_STRING"
#define _XmDisplayToAppContext(d) (void)(d)
#define _XmAppLock(a) (++locks)
#define _XmAppUnlock(a) (--locks)
static int calls, fail_at, live, locks, forced_error, forced_item, converted, overflow;
static void *XtMalloc(Cardinal size) {
    void *p; if (++calls==fail_at) return NULL;
    p=malloc(size ? size : 1); if(p)++live;return p;
}
static void XtFree(char *p) { if(p){assert(live>0);--live;free(p);} }
static Atom XInternAtom(Display *d,const char *name,Boolean only) {
    (void)d;(void)name;(void)only;return 42;
}
static Atom GetLocaleEncodingAtom(Display *d) { (void)d;return 43; }
static char *duplicate(const char *s) {
    size_t n=strlen(s)+1;char *p=XtMalloc((Cardinal)n);
    if(p)memcpy(p,s,n);return p;
}
static char *XmCvtXmStringToCT(XmString s) { return duplicate((char *)s); }
static int GetUseableText(Display *d,XmString s,char **out,Boolean strict,unsigned char type) {
    (void)d;(void)strict;(void)type;
    if(++converted==forced_item) return forced_error;
    *out=duplicate((char *)s);return *out ? Success : XNoMemory;
}
static int XmCvtXmStringToByteStream(XmString s,unsigned char **out) {
    int n=(int)strlen((char *)s)+1;
    if(out) { *out=(unsigned char *)duplicate((char *)s); if(!*out)return -1; }
    return n;
}
static size_t checked_length(const char *s) {
    if(overflow && !strcmp(s,"first")) return INT_MAX;
    return strlen(s);
}
#define strlen(s) checked_length(s)
'''
checks = r'''
int main(void) {
    XmString table[]={(XmString)"first",(XmString)"second",(XmString)"third"};
    XTextProperty property={0};
    const XmICCEncodingStyle styles[]={XmSTYLE_LOCALE,XmSTYLE_TEXT,XmSTYLE_STRING,XmSTYLE_STANDARD_ICC_TEXT};
    for(unsigned style=0;style<sizeof(styles)/sizeof(styles[0]);++style) {
        for(int failure=1;failure<=5;++failure) {
            calls=0;fail_at=failure;converted=0;
            assert(XmCvtXmStringTableToTextProperty(NULL,table,3,styles[style],&property)!=Success);
            assert(live==0 && locks==0);
        }
        fail_at=0;
        for(int item=1;item<=3;++item) {
            converted=0;forced_item=item;forced_error=XNoMemory;
            assert(XmCvtXmStringTableToTextProperty(NULL,table,3,styles[style],&property)==XNoMemory);
            assert(live==0 && locks==0);
        }
        forced_item=0;converted=0;overflow=1;
        assert(XmCvtXmStringTableToTextProperty(NULL,table,3,styles[style],&property)==BadAlloc);
        assert(live==0 && locks==0);overflow=0;
        converted=0;
        assert(XmCvtXmStringTableToTextProperty(NULL,table,3,styles[style],&property)==Success);
        assert(live==1 && locks==0 && property.format==8 && property.nitems==19);
        assert(memcmp(property.value,"first\0second\0third\0\0",20)==0);
        XtFree((char *)property.value);property.value=NULL;
    }
    for(int item=1;item<=3;++item) {
        converted=0;forced_item=item;forced_error=XLocaleNotSupported;
        assert(XmCvtXmStringTableToTextProperty(NULL,table,3,XmSTYLE_TEXT,&property)==Success);
        assert(live==1 && locks==0 && property.encoding==42);
        assert(memcmp(property.value,"first\0second\0third\0\0",20)==0);
        XtFree((char *)property.value);property.value=NULL;
    }
    assert(live==0);
    puts("PASS text-property conversion allocation/overflow cleanup, strict fallback and output bytes");
}
'''
program = args.out / 'text-property.c'
program.write_text(prefix + body + checks)
binary = args.out / 'text-property'
receipt = {'source': str(source), 'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
           'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
           'duration_baseline': 'not established', 'steps': [], 'success': False}
began = time.monotonic()
try:
    for command in [
        ['clang', '-std=c99', '-Wall', '-Wextra', '-Werror', '-fsanitize=address,undefined',
         str(program), '-o', str(binary)], [str(binary)]]:
        result = subprocess.run(command, env=env, capture_output=True, text=True, timeout=60)
        receipt['steps'].append({'command': command, 'exit': result.returncode,
                                 'output': result.stdout + result.stderr})
        if result.returncode:
            raise RuntimeError(result.stdout + result.stderr)
    receipt['success'] = True
finally:
    receipt['seconds'] = time.monotonic() - began
    receipt['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (args.out / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
print('PASS text-property ownership regression under ASan/UBSan')
