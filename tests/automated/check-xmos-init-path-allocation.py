#!/usr/bin/env python3
"""Check that Motif resource-path allocations are checked before copying."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(encoding="utf-8")
start = source.index("_XmOSInitPath(String")
end = source.index("int\nXmeMicroSleep", start)
region = source[start:end]
assert region.count("path = XtMalloc") == 3
assert region.count("if (path == NULL)") == 4
assert "path = XtNewString(ABSOLUTE_PATH);" in region
print("PASS XmOS initialization path allocations are guarded")
