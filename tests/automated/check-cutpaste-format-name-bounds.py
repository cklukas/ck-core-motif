#!/usr/bin/env python3
"""Check clipboard format atoms use bounded formatting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("ClipboardGetAtomFromFormat(\n")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "snprintf(item, format_name_length + prefix_length + 1" in body
assert "sprintf( item, atomname_format" not in body
print("PASS clipboard format atom names use bounded formatting")
