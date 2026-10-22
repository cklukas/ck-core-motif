#!/usr/bin/env python3
"""Exercise WML generation and UIL loading, including unchanged WMD framing."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('build_root', type=Path)
p.add_argument('results', type=Path)
p.add_argument('--legacy-uil', type=Path)
a = p.parse_args()
assert a.build_root.is_absolute() and a.results.is_absolute()
assert json.loads((a.build_root / 'build-receipt.json').read_text())['success']
a.results.mkdir(parents=True, exist_ok=False)
(a.results / 'tmp').mkdir()
env = dict(os.environ, TMPDIR=str(a.results / 'tmp'), LC_ALL='C.UTF-8')
receipt = {'steps': [], 'scope': 'Generated WMD and UIL reader/CLI regressions'}


def run(label, command, success=True, environment=None):
    result = subprocess.run([str(x) for x in command], cwd=a.results,
                            env=environment or env, capture_output=True, timeout=60)
    output = result.stdout + result.stderr
    (a.results / (label + '.log')).write_bytes(output)
    receipt['steps'].append({'name': label, 'exit': result.returncode,
                             'command': [str(x) for x in command]})
    assert (result.returncode == 0) == success, label
    assert b'AddressSanitizer' not in output and b'runtime error:' not in output, label


try:
    generator = a.build_root / 'build/tools/wml/wmldbcreate'
    uil = a.build_root / 'prefix/bin/uil'
    directory = a.results / ('long-output-directory-' + 'a' * 90)
    directory.mkdir()
    database = directory / 'motif.wmd'
    run('generate-debug-long-path', [generator, '-debug', '-o', database])
    assert database.stat().st_size > 0 and (a.results / 'motif.dbg').stat().st_size > 0
    run('missing-output-argument', [generator, '-o'], success=False)
    run('output-write-error', [generator, '-o', '/dev/full'], success=False)
    source = a.results / 'minimal.uil'
    source.write_text("""module wmd_roundtrip
names = case_sensitive
object button : XmPushButton {
  arguments { XmNwidth = 42; XmNlabelString = 'WMD round trip'; };
};
end module;
""")
    run('builtin', [uil, '-o', a.results / 'builtin.uid', source])
    run('absolute-database', [uil, '-wmd', database, '-o', a.results / 'absolute.uid', source])
    short = a.results / 'x.wmd'
    short.write_bytes(database.read_bytes())
    search_env = dict(env, WMDPATH=str(a.results / '%N%S'))
    run('short-name-environment-path', [uil, '-wmd', 'x', '-o', a.results / 'short.uid', source],
        environment=search_env)
    if a.legacy_uil:
        run('legacy-reader', [a.legacy_uil, '-wmd', database,
                               '-o', a.results / 'legacy.uid', source])
    receipt['database_sha256'] = hashlib.sha256(database.read_bytes()).hexdigest()
    receipt['success'] = True
finally:
    (a.results / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
print('PASS WMD generation, bounds, long/short paths, output errors and UIL round trip')
