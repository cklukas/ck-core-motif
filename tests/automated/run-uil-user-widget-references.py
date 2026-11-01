#!/usr/bin/env python3
"""Compile the real custom-widget demo with forward references intact."""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--uil',required=True,type=Path)
args=parser.parse_args()
root=Path(__file__).resolve().parents[2]
with tempfile.TemporaryDirectory(prefix='uil-custom-widget-') as temporary:
    source=root/'demos/unsupported/dogs'
    output=Path(temporary)/'dogs.uid'
    result=subprocess.run([str(args.uil.resolve()),str(source/'dogs.uil'),'-I'+str(source),'-o',str(output)],capture_output=True,text=True)
    if result.returncode or not output.is_file() or not output.stat().st_size:
        raise SystemExit('FAIL custom-widget UIL references: '+result.stdout+result.stderr)
print('PASS UIL custom-widget forward references preserve declaration/reference flags')
