#!/usr/bin/env python3
"""Check XPM returned-comment allocation status is propagated."""
from pathlib import Path

root = Path(__file__).parents[2]
for name in ("Xpmparse.c", "Xpmcreate.c"):
    source = (root / "lib" / "Xm" / name).read_text()
    assert source.count("ErrorStatus = xpmGetCmt(data") == 3
    assert source.count("if (ErrorStatus != XpmSuccess)\n\t    RETURN(ErrorStatus);") >= 3
print("PASS XPM comment allocation status")
