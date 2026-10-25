#!/usr/bin/env python3
"""Check Mrm widget readers guard null records before byte swapping."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwread.c"
text = source.read_text(encoding="utf-8")
for name in ("UrmHGetWidget", "UrmGetIndexedWidget", "UrmGetRIDWidget"):
    start = text.index(name + " (")
    end = text.index("\n}", start)
    body = text[start:end]
    assignment = body.index("widgetrec = (RGMWidgetRecordPtr) UrmRCBuffer")
    guard = body.index("if ( widgetrec == NULL )", assignment)
    deref = body.index("widgetrec->validation", assignment)
    if guard > deref:
        raise SystemExit(f"{name} dereferences a null widget record")
print("PASS Mrm widget readers guard null records")
