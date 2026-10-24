#!/usr/bin/env python3
"""Check Mrm widget-instance setup validates scratch allocations."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(encoding="utf-8")
for name in ("UrmCreateWidgetInstance", "UrmSetWidgetInstance"):
    start = source.index("\n" + name + " (")
    end = source.find("\nUrmSetWidgetInstance (", start + 1) if name == "UrmCreateWidgetInstance" else len(source)
    region = source[start:end]
    assert "UrmPlistInit (10, &ftllist) != MrmSUCCESS" in region
    assert "if (args == NULL)" in region
    assert "UrmPlistInit (10, &ptrlist) != MrmSUCCESS" in region
    assert "UrmPlistInit (10, &cblist) != MrmSUCCESS" in region
    assert "XtFree ((char *)args)" in region
print("Mrm widget-instance setup validates argument and pointer-list allocations")
