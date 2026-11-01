#!/usr/bin/env python3
"""Execute the production constructor with deterministic Xt failure injection."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--out', required=True, type=Path)
args = parser.parse_args()
args.out.mkdir(parents=True, exist_ok=False)
(args.out / 'tmp').mkdir()
env = dict(os.environ, TMPDIR=str(args.out / 'tmp'))
path = Path(__file__).resolve().parents[2] / 'lib/Xm/List.c'
source = path.read_text()
start = source.index('\nWidget\nXmCreateScrolledList(')
body = source[start:source.index('\n/* Selection API', start)]
prefix = r'''
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef void *Widget;
typedef unsigned int Cardinal;
typedef struct { const char *name; uintptr_t value; } Arg, *ArgList;
static int parent_id, shell_id, list_id;
static int fail_shell, fail_list, destroyed, callbacks, shells, children;
#define XmNscrollingPolicy "scrollingPolicy"
#define XmAPPLICATION_DEFINED 1
#define XmNvisualPolicy "visualPolicy"
#define XmVARIABLE 2
#define XmNscrollBarDisplayPolicy "scrollBarDisplayPolicy"
#define XmSTATIC 3
#define XmNshadowThickness "shadowThickness"
#define XmNdestroyCallback "destroyCallback"
#define xmScrolledWindowWidgetClass ((void *)1)
#define xmListWidgetClass ((void *)2)
#define ALLOCATE_LOCAL(n) malloc(n)
#define DEALLOCATE_LOCAL(p) free(p)
#define XmStrlen(s) ((s) ? strlen(s) : 0)
#define XtNumber(a) (sizeof(a)/sizeof((a)[0]))
#define XtSetArg(a,n,v) ((a).name=(n),(a).value=(uintptr_t)(v))
static ArgList XtMergeArgLists(ArgList a, Cardinal n, ArgList b, Cardinal m)
{ (void)a;(void)n;(void)b;(void)m; return calloc(1,sizeof(Arg)); }
static void XtFree(char *p) { free(p); }
static Widget XtCreateManagedWidget(char *n, void *c, Widget p, ArgList a, Cardinal z)
{ (void)n;(void)c;(void)a;(void)z; assert(p==&parent_id); ++shells; return fail_shell?NULL:&shell_id; }
static Widget XtCreateWidget(char *n, void *c, Widget p, ArgList a, Cardinal z)
{ (void)n;(void)c;(void)a;(void)z; assert(p==&shell_id); ++children; return fail_list?NULL:&list_id; }
static void XtDestroyWidget(Widget w) { assert(w==&shell_id); ++destroyed; }
static void _XmDestroyParentCallback(void) {}
static void XtAddCallback(Widget w, const char *n, void (*f)(void), void *p)
{ (void)n;(void)f;(void)p; assert(w==&list_id); ++callbacks; }
'''
suffix = r'''
int main(void) {
    fail_shell=1;
    assert(XmCreateScrolledList(&parent_id,"failedShell",NULL,0)==NULL);
    assert(shells==1 && children==0 && callbacks==0 && destroyed==0);
    fail_shell=0;fail_list=1;
    assert(XmCreateScrolledList(&parent_id,"failedList",NULL,0)==NULL);
    assert(shells==2 && children==1 && callbacks==0 && destroyed==1);
    fail_list=0;
    assert(XmCreateScrolledList(&parent_id,"valid",NULL,0)==&list_id);
    assert(callbacks==1 && destroyed==1);
    assert(XmCreateScrolledList(&parent_id,NULL,NULL,0)==&list_id);
    assert(callbacks==2 && destroyed==1);
    assert(XmCreateScrolledList(&parent_id,"overflow",NULL,UINT_MAX)==NULL);
    assert(shells==4 && children==3 && callbacks==2 && destroyed==1);
    puts("PASS scrolled-list failure rollback and successful ownership");
}
'''
program = args.out / 'constructor.c'
program.write_text(prefix + body + suffix)
binary = args.out / 'constructor'
began = time.monotonic()
receipt = {'source': str(path), 'source_sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
           'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
           'duration_baseline': 'not established', 'steps': [], 'success': False}
try:
    for command in [
        ['clang','-std=c99','-Wall','-Wextra','-Werror',
         '-fsanitize=address,undefined',str(program),'-o',str(binary)], [str(binary)]]:
        result = subprocess.run(command, env=env, capture_output=True, text=True, timeout=30)
        receipt['steps'].append({'command':command,'exit':result.returncode,
                                 'output':result.stdout+result.stderr})
        if result.returncode:
            raise RuntimeError(result.stdout+result.stderr)
    receipt['success'] = True
finally:
    receipt['seconds'] = time.monotonic()-began
    receipt['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (args.out/'receipt.json').write_text(json.dumps(receipt,indent=2)+'\n')
print('PASS constructor failure injection under ASan/UBSan')
