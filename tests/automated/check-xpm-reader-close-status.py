#!/usr/bin/env python3
"""Check XPM file readers propagate close failures after successful parsing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmRdFToI.c").read_text(
    encoding="utf-8"
)
assert "LFUNC(xpmDataClose, int" in source
assert source.count("xpmDataClose(&mdata) != 0 && ErrorStatus == XpmSuccess") == 2
start = source.index("static int\nxpmDataClose")
body = source[start:]
assert "return fclose(mdata->stream.file);" in body
assert "return 0;" in body
print("PASS XPM readers propagate close failures")
