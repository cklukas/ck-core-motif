#!/usr/bin/env python3
"""Check that lock-window marker reads are bounded and status-checked."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static int \nClipboardWindowExists(")
end = source.index("/* external routines", start)
region = source[start:end]

assert "int ret_value;" in region
assert "ret_value = XGetWindowProperty" in region
assert "1, /*length: lock-access marker is one property unit*/" in region
assert "ret_value != Success" in region
assert "10000000" not in region
print("PASS clipboard lock-marker reads are bounded")
