#!/usr/bin/env python3
"""Check XPM returned-comment allocation status is propagated."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmparse.c").read_text()
assert source.count("ErrorStatus = xpmGetCmt(data") == 3
assert source.count("if (ErrorStatus != XpmSuccess)\n\t    RETURN(ErrorStatus);") >= 3
print("PASS XPM comment allocation status")
