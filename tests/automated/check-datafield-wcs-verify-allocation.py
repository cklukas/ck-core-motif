#!/usr/bin/env python3
"""Check wide modify-verify conversion allocations before use."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)

needle = "wcs_newblock.wcsptr = (wchar_t *) XtMalloc"
if source.count(needle) != 2:
    raise SystemExit("expected both wide modify-verify allocations")
if source.count("if (wcs_newblock.wcsptr == NULL)") != 2:
    raise SystemExit("wide modify-verify allocations are not guarded")
if "XtFree(newblock.ptr);" not in source:
    raise SystemExit("narrow temporary is not released on wide allocation failure")

print("DataField wide modify-verify allocation checks passed")
