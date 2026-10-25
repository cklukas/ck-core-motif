#!/usr/bin/env python3
"""Check that format registration rejects a missing format atom."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static int \nRegisterFormat(")
end = source.index("static void\nCleanupHeader(", start)
region = source[start:end]

assert "formatatom == None" in region
assert "return ClipboardFail;" in region
assert region.index("formatatom == None") < region.index("XChangeProperty( display")
print("PASS clipboard format registration checks its atom")
