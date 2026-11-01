#!/usr/bin/env python3
"""Capture readiness requires a viewable shell and drained post-map events."""
from pathlib import Path
root=Path(__file__).resolve().parents[2]
source=(root/'demos/programs/ckmotif-gallery/gallery.c').read_text()
body=source.split('static Boolean capture_ready(XtPointer client)',1)[1].split('return True;',1)[0]
assert body.index('attributes.map_state != IsViewable') < body.index('XChangeProperty')
assert body.index('XtAppPending(') < body.index('XChangeProperty')
runner=(root/'tests/automated/run-gallery-screenshots.py').read_text()
assert 'Map State: IsViewable' in runner
print('PASS screenshot readiness follows shell mapping and event draining')
