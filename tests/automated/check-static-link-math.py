#!/usr/bin/env python3
"""Keep libXm's static link interface complete for its text metrics."""
from pathlib import Path


makefile = Path("lib/Xm/Makefile.am").read_text(encoding="utf-8")
assert "libXm_la_LIBADD" in makefile
libadd = next(line for line in makefile.splitlines() if line.startswith("libXm_la_LIBADD"))
assert " -lm " in f" {libadd} "

for name in ("TextF.c", "TextOut.c"):
    source = Path("lib/Xm", name).read_text(encoding="utf-8")
    assert "lround" in source

print("PASS libXm exports -lm for lround-based text metrics")
