#!/usr/bin/env python3
"""Check Mrm widget-reference builders handle allocations and appends."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwrefs.c").read_text(encoding="utf-8")
for name in ("Urm__CW_AppendSVWidgetRef", "Urm__CW_AppendCBSVWidgetRef"):
    start = source.index(name + " (")
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "UrmPlistInit (10, svlist) != MrmSUCCESS" in region
    assert "svdesc == NULL" in region
    assert "UrmPlistAppendPointer" in region
    assert "!= MrmSUCCESS" in region[region.index("UrmPlistAppendPointer"):]
callback = source[source.index("Urm__CW_AppendCBSVWidgetRef ("):]
assert "cbdesc == NULL" in callback
assert "XtFree ((char *)cbdesc)" in callback
print("Mrm widget-reference builders validate allocations and appends")
