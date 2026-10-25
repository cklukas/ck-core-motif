#!/usr/bin/env python3
"""Check that lock-marker properties use the expected X11 format."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static int \nClipboardWindowExists(")
end = source.index("/* external routines", start)
region = source[start:end]

assert "format != 8" in region
assert "ret_value != Success" in region
print("PASS clipboard lock-marker format is validated")
