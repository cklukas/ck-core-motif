#!/usr/bin/env python3
"""Ensure FileSelectionBoxUpdate validates search procedure callbacks."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FileSB.c").read_text()
start = source.index("FileSelectionBoxUpdate(\n        XmFileSelectionBoxWidget fs,")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "FS_QualifySearchDataProc(fs) == NULL" in body
assert "FS_DirSearchProc(fs) == NULL" in body
assert "if (FS_FileSearchProc(fs) != NULL)" in body
assert body.index("FS_QualifySearchDataProc(fs) == NULL") < body.index("XtSetMappedWhenManaged")
print("FileSelectionBoxUpdate validates search procedure callbacks")
