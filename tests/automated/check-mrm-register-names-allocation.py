#!/usr/bin/env python3
"""Check Mrm name registration validates inputs and temporary vectors."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmappl.c").read_text(encoding="utf-8")
for name in ("MrmRegisterNames", "MrmRegisterNamesInHierarchy"):
    start = source.index("\n" + name + " (")
    end = source.index("\n\n\f", start)
    region = source[start:end]
    assert "num_reg > 0 && reglist == NULL" in region
    names = region.index("names = (String *) XtMalloc")
    assert "if (names == NULL)" in region[names:names + 180]
    values = region.index("values = (XtPointer *) XtMalloc")
    assert "if (values == NULL)" in region[values:values + 220]
    assert "XtFree ((char *)names)" in region[values:]
print("Mrm name registration validates lists and temporary allocations")
