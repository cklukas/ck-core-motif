#!/usr/bin/env python3
"""Check widget class and callback offsets stay inside Mrm records."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
needle = '''  if ((widgetrec->class_offs != 0 &&
       widgetrec->class_offs >= widgetrec->size) ||
      (widgetrec->creation_offs != 0 &&
       widgetrec->creation_offs >= widgetrec->size))
    return MrmFAILURE ;'''
if text.count(needle) != 2:
    raise SystemExit("both widget-instance paths lack offset bounds")
print("PASS Mrm widget class and callback offsets are bounded")
