#!/usr/bin/env python3
"""Execute Container's production tab-pool builder with allocation failures."""
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
source = Path(__file__).resolve().parents[2] / 'lib/Xm/Container.c'
content = source.read_text()
start = content.rindex('GetDumbTabList(')
begin = content.index('{', start)
end, depth = begin + 1, 1
while depth:
    depth += (content[end] == '{') - (content[end] == '}')
    end += 1
body = 'static XmTabList ' + content[start:end]
prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
typedef unsigned int Cardinal;
typedef struct { float value; } *XmTab;
typedef struct { Cardinal count; float *values; } *XmTabList;
#define XmPIXELS 0
#define XmABSOLUTE 0
#define XmALIGNMENT_BEGINNING 0
#define XmS ""
static unsigned calls, fail_at, live;
static void *XtMalloc(Cardinal size) {
    void *p;
    if (++calls == fail_at) return NULL;
    p = malloc(size); if (p) ++live; return p;
}
static void XtFree(void *p) { if (p) { assert(live); --live; free(p); } }
static XmTab XmTabCreate(float value, int units, int model, int alignment, const char *decimal) {
    assert(units==XmPIXELS && model==XmABSOLUTE);
    assert(alignment==XmALIGNMENT_BEGINNING && decimal[0]=='\0');
    XmTab tab = XtMalloc(sizeof(*tab)); if (tab) tab->value=value; return tab;
}
static void XmTabFree(XmTab tab) { XtFree(tab); }
static void XmTabSetValue(XmTab tab, float value) { assert(tab); tab->value=value; }
static XmTabList XmTabListInsertTabs(XmTabList old, XmTab *tabs, Cardinal count, int position) {
    assert(!old && position==0);
    XmTabList list=XtMalloc(sizeof(*list)); if (!list) return NULL;
    list->values=XtMalloc(count*sizeof(*list->values));
    if (!list->values) { XtFree(list); return NULL; }
    list->count=count;
    for (Cardinal i=0;i<count;++i) list->values[i]=tabs[i]->value;
    return list;
}
static void release(XmTabList list) { XtFree(list->values); XtFree(list); }
'''
checks = r'''
int main(void) {
    for (unsigned failure=1;failure<=10;++failure) {
        calls=0; fail_at=failure;
        assert(GetDumbTabList(31,7)==NULL);
        assert(calls==failure && live==0);
    }
    calls=0; fail_at=0;
    assert(GetDumbTabList(31,0)==NULL && calls==0);
    assert(GetDumbTabList(31,UINT_MAX)==NULL && calls==0);
    const Cardinal counts[]={1,257,65537};
    XmTabList first=GetDumbTabList(19,3);
    assert(first && live==2);
    for (unsigned n=0;n<sizeof(counts)/sizeof(counts[0]);++n) {
        Cardinal count=counts[n];
        XmTabList list=GetDumbTabList(INT_MAX,count);
        assert(list && list->count==count && live==4);
        for (Cardinal i=0;i<count;++i)
            assert(list->values[i]==(float)((double)INT_MAX*(double)(i+1)));
        for (Cardinal i=0;i<3;++i) assert(first->values[i]==19.0f*(float)(i+1));
        release(list); assert(live==2);
    }
    release(first); assert(live==0);
    puts("PASS Container tab-pool allocation rollback, large counts and independent results");
}
'''
program = args.out / 'container-tab-pool.c'
program.write_text(prefix + body + checks)
binary = args.out / 'container-tab-pool'
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
print('PASS Container tab-pool executable regression under ASan/UBSan')
