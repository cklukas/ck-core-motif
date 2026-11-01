#!/usr/bin/env python3
"""Run production tabbed-stack copy/free with allocation failure injection."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import resource
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--out', type=Path, required=True)
args = parser.parse_args()
resource.setrlimit(resource.RLIMIT_CORE, (0, 0))
args.out.mkdir(parents=True, exist_ok=False)
(args.out / 'tmp').mkdir()
env = dict(os.environ, TMPDIR=str(args.out / 'tmp'))
path = Path(__file__).resolve().parents[2] / 'lib/Xm/TabList.c'
source = path.read_text()

def function(name, signature):
    start = source.index(name + '(XmTabbedStackList tab_list)')
    begin = source.index('{', start)
    depth = 1
    end = begin + 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return signature + source[begin:end] + '\n'

prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef unsigned int Cardinal;
typedef void *XtPointer;
typedef char *XmString;
typedef struct {
    XmString label_string;
    int string_direction, label_pixmap, label_alignment, pixmap_placement;
    int foreground, background, background_pixmap, sensitive, value_mode;
} XmTabAttributeRec, *XmTabAttributes;
typedef struct _XmTabbedStackListRec {
    int allocated, used;
    XmTabAttributes tabs;
} XmTabbedStackListRec, *XmTabbedStackList;
#define XmTAB_VALUE_COPY 0
#define XmTAB_VALUE_SHARE 1
static int calls, fail_at, live;
static void *XtMalloc(Cardinal size) {
    void *p;
    if (++calls == fail_at) return NULL;
    p = malloc(size); if(p) ++live; return p;
}
static void XtFree(void *p) { if(p) { --live; free(p); } }
static XmString XiXmStringCopy(XmString s) {
    char *p; if(!s)return NULL;
    p = XtMalloc((Cardinal)strlen(s)+1);
    if(p)strcpy(p,s); return p;
}
#define XiXmStringFree(s) XtFree(s)
'''
checks = r'''
int main(void) {
    XmTabAttributeRec tabs[3] = {{0}};
    XmTabbedStackListRec input = {3,3,tabs};
    tabs[0].label_string="alpha";tabs[1].label_string="beta";tabs[2].label_string="gamma";
    tabs[1].value_mode=XmTAB_VALUE_SHARE;tabs[1].foreground=42;
    for(int failure=1;failure<=5;++failure) {
        calls=0;fail_at=failure;
        assert(XmTabbedStackListCopy(&input)==NULL);
        assert(live==0);
        assert(strcmp(tabs[0].label_string,"alpha")==0);
        assert(strcmp(tabs[1].label_string,"beta")==0);
        assert(strcmp(tabs[2].label_string,"gamma")==0);
        assert(tabs[1].value_mode==XmTAB_VALUE_SHARE && tabs[1].foreground==42);
    }
    calls=0;fail_at=0;
    XmTabbedStackList copy=XmTabbedStackListCopy(&input);
    assert(copy && copy->used==3 && copy->allocated==3);
    for(int i=0;i<3;++i) {
        assert(copy->tabs[i].label_string!=tabs[i].label_string);
        assert(strcmp(copy->tabs[i].label_string,tabs[i].label_string)==0);
        assert(copy->tabs[i].value_mode==XmTAB_VALUE_COPY);
    }
    assert(copy->tabs[1].foreground==42);
    XmTabbedStackListFree(copy); assert(live==0);
    tabs[1].label_string=NULL;
    copy=XmTabbedStackListCopy(&input);assert(copy && copy->tabs[1].label_string==NULL);
    XmTabbedStackListFree(copy); assert(live==0);
    input.used=0;copy=XmTabbedStackListCopy(&input);
    assert(copy && copy->used==0 && copy->tabs==NULL);
    XmTabbedStackListFree(copy); assert(live==0);
    assert(XmTabbedStackListCopy(NULL)==NULL);
    puts("PASS tabbed-stack deep copy, rollback at all five allocations, null/empty labels");
}
'''
program=args.out/'tab-copy.c'
program.write_text(prefix + function('XmTabbedStackListCopy',
    'XmTabbedStackList XmTabbedStackListCopy(XmTabbedStackList tab_list)\n') +
    function('XmTabbedStackListFree', 'void XmTabbedStackListFree(XmTabbedStackList tab_list)\n') + checks)
binary=args.out/'tab-copy'
began=time.monotonic()
receipt={'source':str(path),'source_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),
         'started_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
         'duration_baseline':'not established','steps':[],'success':False}
try:
    for command in [['clang','-std=c99','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(program),'-o',str(binary)],[str(binary)]]:
        result=subprocess.run(command,env=env,capture_output=True,text=True,timeout=30)
        receipt['steps'].append({'command':command,'exit':result.returncode,'output':result.stdout+result.stderr})
        if result.returncode: raise RuntimeError(result.stdout+result.stderr)
    receipt['success']=True
finally:
    receipt['seconds']=time.monotonic()-began
    receipt['ended_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat()
    (args.out/'receipt.json').write_text(json.dumps(receipt,indent=2)+'\n')
print('PASS tabbed-stack copy fault injection under ASan/UBSan')
