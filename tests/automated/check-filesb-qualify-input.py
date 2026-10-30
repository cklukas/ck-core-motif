#!/usr/bin/env python3
"""Ensure FileSelectionBox search qualification validates callback records."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FileSB.c").read_text()
start = source.index("QualifySearchDataProc(\n        Widget w,")
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "if (searchData == NULL || qualifiedSearchData == NULL)" in body
assert "if (qualifiedSearchData->value != NULL)" in body
assert "if (qualifiedSearchData->mask != NULL)" in body
assert "if (qualifiedSearchData->dir != NULL)" in body
assert "if (qualifiedSearchData->pattern != NULL)" in body
assert body.index("if (searchData == NULL") < body.index("qualifiedSearchData->value = NULL")
print("FileSelectionBox search qualification validates callback records")
