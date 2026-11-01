#!/usr/bin/env python3
"""Run production tabbed-stack copy/insert/free with fault injection."""
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
    start = source.index(name + '(XmTabbedStackList tab_list')
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
typedef unsigned long XtValueMask;
typedef int Boolean;
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
#define XmTAB_LAST_POSITION -1
#define XmTAB_NOT_FOUND -1
#define XmLIST_GROWTH_FACTOR 10
#define XmTAB_LABEL_STRING (1L<<0)
#define XmTAB_LABEL_PIXMAP (1L<<1)
#define XmTAB_PIXMAP_PLACEMENT (1L<<2)
#define XmTAB_BACKGROUND (1L<<3)
#define XmTAB_FOREGROUND (1L<<4)
#define XmTAB_VALUE_MODE (1L<<5)
#define XmTAB_LABEL_ALIGNMENT (1L<<6)
#define XmTAB_STRING_DIRECTION (1L<<7)
#define XmTAB_BACKGROUND_PIXMAP (1L<<8)
#define XmTAB_SENSITIVE (1L<<9)
#define XmSTRING_DIRECTION_L_TO_R 0
#define XmUNSPECIFIED_PIXMAP 0
#define XmALIGNMENT_CENTER 0
#define XmPIXMAP_RIGHT 0
#define XmCOLOR_DYNAMIC -1
#define XmPIXMAP_DYNAMIC 3
#define True 1
#define False 0
static int calls, fail_at, live;
static void *XtMalloc(Cardinal size) {
    void *p;
    if (++calls == fail_at) return NULL;
    p = malloc(size); if(p) ++live; return p;
}
static void XtFree(void *p) { if(p) { --live; free(p); } }
static void *XtRealloc(void *old, Cardinal size) {
    void *p; if(++calls==fail_at)return NULL;
    p=realloc(old,size);if(p && !old)++live;return p;
}
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
    for(int capacity=1;capacity<=2;++capacity) {
        for(int failure=1;failure<=(capacity==1?2:1);++failure) {
            fail_at=0;
            XmTabAttributes entries=XtMalloc((Cardinal)(capacity*sizeof(*entries)));
            memset(entries,0,(size_t)capacity*sizeof(*entries));
            entries[0].label_string="original";entries[0].value_mode=XmTAB_VALUE_SHARE;
            XmTabbedStackListRec target={capacity,1,entries};
            XmTabAttributeRec incoming={0};incoming.label_string="insert";
            calls=0;fail_at=failure;
            assert(XmTabbedStackListInsert(&target,0,XmTAB_LABEL_STRING,&incoming)==XmTAB_NOT_FOUND);
            assert(target.used==1 && target.allocated==capacity && target.tabs==entries);
            assert(strcmp(entries[0].label_string,"original")==0 && live==1);
            XtFree(entries);assert(live==0);
        }
    }
    fail_at=0;
    XmTabbedStackList target=XtMalloc(sizeof(*target));
    target->allocated=target->used=1;target->tabs=XtMalloc(sizeof(*target->tabs));
    memset(target->tabs,0,sizeof(*target->tabs));
    target->tabs[0].label_string=XiXmStringCopy("aliased");
    assert(XmTabbedStackListInsert(target,0,XmTAB_LABEL_STRING,&target->tabs[0])==0);
    assert(target->used==2 && strcmp(target->tabs[0].label_string,"aliased")==0);
    assert(strcmp(target->tabs[1].label_string,"aliased")==0);
    assert(target->tabs[0].label_string!=target->tabs[1].label_string);
    XmTabbedStackListFree(target);assert(live==0);
    target=XtMalloc(sizeof(*target));
    target->allocated=2;target->used=1;
    target->tabs=XtMalloc(2*sizeof(*target->tabs));
    memset(target->tabs,0,2*sizeof(*target->tabs));
    target->tabs[0].label_string="shared";
    target->tabs[0].value_mode=XmTAB_VALUE_SHARE;
    target->tabs[0].foreground=42;
    target->tabs[0].sensitive=False;
    assert(XmTabbedStackListInsert(target,0,XmTAB_LABEL_STRING|XmTAB_VALUE_MODE|
        XmTAB_FOREGROUND|XmTAB_SENSITIVE,&target->tabs[0])==0);
    assert(target->used==2 && target->tabs[0].label_string==target->tabs[1].label_string);
    assert(target->tabs[0].foreground==42 && target->tabs[0].sensitive==False);
    XmTabbedStackListFree(target);assert(live==0);
    XmTabbedStackListRec empty={0,0,NULL};
    assert(XmTabbedStackListInsert(&empty,XmTAB_LAST_POSITION,0,NULL)==0);
    assert(empty.used==1 && empty.tabs[0].label_string==NULL);
    assert(empty.tabs[0].value_mode==XmTAB_VALUE_COPY && empty.tabs[0].sensitive==True);
    XtFree(empty.tabs);assert(live==0);
    puts("PASS tabbed-stack copy/insert rollback, aliasing, sharing and defaults");
}
'''
program=args.out/'tab-copy.c'
program.write_text(prefix + function('XmTabbedStackListCopy',
    'XmTabbedStackList XmTabbedStackListCopy(XmTabbedStackList tab_list)\n') +
    function('XmTabbedStackListFree', 'void XmTabbedStackListFree(XmTabbedStackList tab_list)\n') +
    function('XmTabbedStackListInsert', 'int XmTabbedStackListInsert(XmTabbedStackList tab_list, int position, XtValueMask mask, XmTabAttributes attributes)\n') + checks)
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
print('PASS tabbed-stack copy/insert fault injection under ASan/UBSan')
