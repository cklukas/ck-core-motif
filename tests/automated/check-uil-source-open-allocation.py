#!/usr/bin/env python3
"""Check UIL source opening handles allocation failure before dereference."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSrcSrc.c").read_text()
start = source.index("src_open_file (XmConst char")
end = source.index("\n}\n\f\n/*", start)
body = source[start:end]

assert "if (az_fcb == NULL)" in body
assert "unable to allocate UIL file control block" in body
assert "if (az_source_buffer == NULL)" in body
assert "XtFree ((char *)az_fcb);" in body
assert "unable to allocate UIL source buffer" in body
assert body.index("if (az_fcb == NULL)") < body.index("open_source_file (")
assert body.index("if (az_source_buffer == NULL)") < body.index("open_source_file (")
print("UIL source opening checks allocations before use")
