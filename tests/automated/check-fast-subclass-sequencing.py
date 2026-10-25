#!/usr/bin/env python3
"""Check that fast-subclass helpers do not reuse a shared lookup temporary."""
from pathlib import Path
import sys

if len(sys.argv) != 2:
    raise SystemExit(f"usage: {sys.argv[0]} BASE_CLASS_HEADER")
text = Path(sys.argv[1]).read_text(encoding="utf-8")
assert "_XmFastSubclassInitLocal" in text
assert "_XmIsFastSubclassLocal" in text
assert "XmBaseClassExt *fastPtr = _XmGetBaseClassExtPtr" in text
assert "_Xm_fastPtr = _XmGetBaseClassExtPtr" not in text
print("PASS fast-subclass helpers use local lookup state")
