#!/usr/bin/env python3
"""Compile and run both workspace peers' bounded wire validators."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
root=Path(__file__).resolve().parents[2]
with tempfile.TemporaryDirectory(prefix='wsm-wire-') as temporary:
    for index,library in enumerate(('clients/mwm/WmWsmLib','demos/lib/Wsm')):
        binary=Path(temporary)/str(index)
        command=[os.environ.get('CC','cc'),'-std=c99','-Wall','-Wextra','-Werror',
                 '-I'+str(root/library),str(root/library/'wire.c'),
                 str(root/'tests/automated/wsm-wire-validation.c'),'-o',str(binary)]
        if sys.platform=='darwin':command.insert(1,'-I/opt/X11/include')
        subprocess.run(command,check=True)
        subprocess.run([str(binary)],check=True)
