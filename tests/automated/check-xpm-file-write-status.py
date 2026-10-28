#!/usr/bin/env python3
"""Check XPM file writers propagate stream and close failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmWrFFrI.c").read_text()
assert "LFUNC(xpmDataClose, int" in source
assert "if (ferror(file))" in source
assert "xpmDataClose(&mdata) != 0" in source
assert "return fclose(mdata->stream.file);" in source
assert "return fflush(stdout);" in source
print("XPM file writers propagate stream and close failures")
