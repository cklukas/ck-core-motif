#!/usr/bin/env python3
"""Check Mrm widget callback paths propagate context-list allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
checks = []
for name in ("UrmCreateWidgetInstance", "UrmSetWidgetInstance"):
    start = text.index(name + " (")
    end = text.find("\n\n\f", start)
    region = text[start:] if end < 0 else text[start:end]
    marker = "result = UrmPlistInit (10, &ptrlist)"
    if marker not in region:
        raise SystemExit(f"{name} does not initialize callback context list")
    init = region.index(marker)
    guard = region.find("if ( result != MrmSUCCESS )", init)
    if guard < 0 or guard - init > 220:
        raise SystemExit(f"{name} ignores callback context-list initialization failure")
    checks.append(name)

print("Mrm callback context-list initialization failures propagate")
