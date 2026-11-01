#!/usr/bin/env python3
"""Execute production compound-string byte-stream length decoding under sanitizers."""
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
source = Path(__file__).resolve().parents[2] / 'lib/Xm/XmString.c'
content = source.read_text()
def extract(name, signature):
    start = content.index('\n' + name + '(')
    begin = content.index('{', start)
    end, depth = begin + 1, 1
    while depth:
        depth += (content[end] == '{') - (content[end] == '}')
        end += 1
    return signature + content[begin:end] + '\n'

body = ''.join(extract(name, signature) for name, signature in [
    ('_is_short_length', 'static Boolean _is_short_length(unsigned char *p)\n'),
    ('_read_header_length', 'static unsigned short _read_header_length(unsigned char *p)\n'),
    ('_read_asn1_length', 'static unsigned short _read_asn1_length(unsigned char *p)\n'),
    ('_read_length', 'static unsigned short _read_length(unsigned char *p)\n'),
    ('_read_string_length', 'static unsigned short _read_string_length(unsigned char *p)\n'),
    ('_is_asn1', 'static Boolean _is_asn1(unsigned char *string)\n'),
    ('XmStringByteStreamLength', 'unsigned int XmStringByteStreamLength(unsigned char *string)\n'),
])
prefix = r'''
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
typedef int Boolean;
#define TRUE 1
#define FALSE 0
#define ASNHEADERLEN 3
#define CSSHORTLEN 1
#define CSLONGLEN 3
#define CSLONGBIT 0x80
#define ASNTAG 1
#define MAXSHORTVALUE 127
#define _calc_header_size(len) (((unsigned short)(len) > MAXSHORTVALUE) ? 6 : 4)
#define _is_asn1_long(p) (*((unsigned char *)(p) + ASNTAG) & CSLONGBIT)
static const unsigned char ASNHeader[3]={0xdf,0x80,0x06};
static int locks;
#define _XmProcessLock() (++locks)
#define _XmProcessUnlock() (--locks)
'''
checks = r'''
int main(void) {
    for(unsigned int length=0;length<=127;++length) {
        unsigned char short_form[]={0xdf,0x80,0x06,(unsigned char)length};
        unsigned char long_form[]={0xdf,0x80,0x06,0x82,0,(unsigned char)length};
        assert(_read_header_length(short_form)==4 && _read_header_length(long_form)==6);
        assert(XmStringByteStreamLength(short_form)==length+4);
        assert(XmStringByteStreamLength(long_form)==length+6);
        assert(locks==0);
    }
    const unsigned int lengths[]={128,255,256,65535};
    for(unsigned int i=0;i<sizeof(lengths)/sizeof(lengths[0]);++i) {
        unsigned int length=lengths[i];
        unsigned char encoded[]={0xdf,0x80,0x06,0x82,(unsigned char)(length>>8),(unsigned char)length};
        assert(XmStringByteStreamLength(encoded)==length+6);
        assert(locks==0);
    }
    const unsigned char components[][4] = {
        {0x01, 0x00}, {0x01, 0x7f},
        {0x01, 0x82, 0x00, 0x00}, {0x01, 0x82, 0x00, 0x7f},
    };
    const unsigned short expected_lengths[] = {0, 127, 0, 127};
    for(unsigned int i=0;i<sizeof(components)/sizeof(components[0]);++i) {
        assert(_read_asn1_length((unsigned char *)components[i]) == expected_lengths[i]);
        assert(_read_length((unsigned char *)components[i]) == (i < 2 ? 2 : 4));
    }
    unsigned char bad_magic[]={0,0,0,0};
    assert(XmStringByteStreamLength(bad_magic)==0);
    assert(XmStringByteStreamLength(NULL)==0 && locks==0);
    puts("PASS 266 ASN.1 header cases: encoded stream and component headers, boundaries, null/magic");
}
'''
program = args.out / 'xmstring-stream-length.c'
program.write_text(prefix + body + checks)
binary = args.out / 'xmstring-stream-length'
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
print('PASS XmString encoded-length regression under ASan/UBSan')
