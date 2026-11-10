#!/usr/bin/env python3
"""Check that MWM argument-vector copies bound pointer-array sizes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text(encoding="utf-8")
start = source.index("void CopyArgv")
region = source[start:source.index("} /* END OF FUNCTION CopyArgv */", start)]
assert "argc < 0 || (argc > 0 && argv == NULL)" in region
assert region.count("(size_t)argc + 1 > UINT_MAX / sizeof(char *)") == 2
assert region.count("XtMalloc ((Cardinal)(((size_t)argc + 1) * sizeof(char *)))") == 1
assert "XtMalloc((Cardinal)(((size_t)argc + 1) * sizeof(char *)))" in region
assert "XtMalloc ((argc + 1) * sizeof (char *))" not in region
assert "XtMalloc((argc + 1) * sizeof(char *))" not in region
print("PASS MWM argument-vector allocation is width checked")
