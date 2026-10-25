#!/usr/bin/env python3
"""Check that clipboard format metadata accepts only supported unit sizes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static int \nClipboardGetLenFromFormat(")
end = source.index("static Boolean\nClipboardHeaderRecordValid(", start)
region = source[start:end]

assert "stored_length != 8 && stored_length != 16 && stored_length != 32" in region
assert "*format_length = 8;" in region
assert "ret_value = ClipboardFail;" in region
print("PASS clipboard format-unit metadata is validated")
