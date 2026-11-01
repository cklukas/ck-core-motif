#!/usr/bin/env python3
"""Keep every installed standalone program's executable and asset path accurate."""
from pathlib import Path
import re
root=Path(__file__).resolve().parents[2]
source=root/'demos/programs'
header=(source/'ckmotif-gallery/gallery_demos.h').read_text()
entries=re.findall(r'\{"([^"]+)", "([^"]+)", "([^"]+)", "([^"]+)", "([^"]+)"\}',header)
assert entries and all(len(description)>12 for _,_,_,_,description in entries)
actual=set()
for build_dir,install_dir,binary,title,description in entries:
    make=(source/build_dir/'Makefile.am').read_text()
    targets=re.search(r'^bin_PROGRAMS\s*=\s*(.*)$',make,re.M)
    location=re.search(r'^bindir\s*=\s*\$\{prefix\}/share/Xm/(.*)$',make,re.M)
    assert targets and binary in targets[1].split(),(build_dir,binary)
    assert location and location[1]==install_dir,(build_dir,install_dir)
    assert (build_dir,binary) not in actual,(build_dir,binary)
    actual.add((build_dir,binary))
expected=set()
for makefile in source.rglob('Makefile.am'):
    if makefile.parent.name=='ckmotif-gallery':continue
    match=re.search(r'^bin_PROGRAMS\s*=\s*(.*)$',makefile.read_text(),re.M)
    if match:
        expected.update((str(makefile.parent.relative_to(source)),name) for name in match[1].split())
assert actual==expected,{'missing':sorted(expected-actual),'extra':sorted(actual-expected)}
print(f'PASS all {len(actual)} installed demo targets have descriptions and exact build/install paths')
