#!/usr/bin/env python3
"""Ensure FileSelectionPB validates its qualifier callback before use."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FileSB.c").read_text()
start = source.index("FileSelectionPB(\n        Widget wid,")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "if (FS_QualifySearchDataProc(fs) == NULL)" in body
assert "XmStringFree(searchData.mask);" in body
assert "XmStringFree(searchData.dir);" in body
assert body.index("if (FS_QualifySearchDataProc(fs) == NULL)") < body.index("(*FS_QualifySearchDataProc")
print("FileSelectionPB validates its qualifier callback")
