#!/usr/bin/env python3
"""Run list Unicode navigation against an exact installed library."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import subprocess
import time

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("prefix", type=Path)
parser.add_argument("results", type=Path)
args = parser.parse_args()
if platform.system() != 'Linux':
    parser.error('installed-library validation requires Linux')
if not args.prefix.is_absolute() or not args.results.is_absolute():
    parser.error('prefix and fresh results directory must be absolute')
prefix = args.prefix.resolve(strict=True)
source = Path(__file__).with_name('check-list-unicode.c').resolve(strict=True)
library = (prefix / 'lib/libXm.so').resolve(strict=True)
output = args.results.resolve()
if output == prefix or prefix in output.parents or output in prefix.parents:
    parser.error('results must be separate from the installed prefix')
output.mkdir(parents=True, exist_ok=False)
(output / 'tmp').mkdir()
env = dict(os.environ, TMPDIR=str(output / 'tmp'), LC_ALL='C.UTF-8',
           LD_LIBRARY_PATH=str(prefix / 'lib'), CKMOTIF_EXPECT_LIBRARY=str(library))
env.pop('DISPLAY', None)
sha = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
headers = {str(p.relative_to(prefix)): sha(p)
           for p in sorted((prefix / 'include/Xm').glob('*.h'))}
if not headers:
    parser.error('prefix must contain matching installed Motif headers')
receipt = {'started_utc': datetime.datetime.now(datetime.timezone.utc).isoformat(),
           'source': str(source), 'source_sha256': sha(source),
           'library': str(library), 'library_sha256': sha(library),
           'header_sha256': headers, 'configuration': 'Xvfb installed List Unicode navigation',
           'duration_baseline': 'not established', 'steps': [], 'success': False}
began = time.monotonic()
binary = output / 'check-list-unicode'
try:
    commands = [
        ['gcc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-g', '-O2',
         '-I' + str(prefix / 'include'), str(source), '-L' + str(prefix / 'lib'),
         '-Wl,-rpath,' + str(prefix / 'lib'), '-lXm', '-lXt', '-lX11', '-ldl',
         '-o', str(binary)], ['xvfb-run', '-a', str(binary)],
    ]
    for index, command in enumerate(commands):
        started = time.monotonic()
        result = subprocess.run(command, env=env, cwd=output, capture_output=True,
                                text=True, timeout=60)
        (output / f'step-{index}.log').write_text(result.stdout + result.stderr)
        receipt['steps'].append({'command': command, 'exit': result.returncode,
                                 'seconds': time.monotonic() - started})
        if result.returncode:
            raise RuntimeError(f'step {index} failed; inspect output log')
    if sha(library) != receipt['library_sha256'] or sha(source) != receipt['source_sha256']:
        raise RuntimeError('source or library changed during validation')
    for relative, digest in headers.items():
        if sha(prefix / relative) != digest:
            raise RuntimeError('installed headers changed during validation')
    receipt['binary_sha256'] = sha(binary)
    receipt['success'] = True
finally:
    receipt['seconds'] = time.monotonic() - began
    receipt['ended_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (output / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
print('PASS installed List Unicode navigation; receipt:', output / 'receipt.json')
