#!/usr/bin/env python3
"""Check that drop-site diagnostic formatting cannot overflow its stack buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropSMgrI.c").read_text()
start = source.index("_XmDSIGetChildPosition(")
end = source.index("\nvoid \n_XmDSIReplaceChild", start)
region = source[start:end]

assert region.count("char buf[256];") == 2
assert region.count("snprintf(buf, sizeof(buf), MESSAGE3,") == 2
assert "sprintf(buf, MESSAGE3" not in region
print("drop-site diagnostics use bounded formatting")
