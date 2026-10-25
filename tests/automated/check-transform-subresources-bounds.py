#!/usr/bin/env python3
"""Check that Motif subresource transformation validates allocation size."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BaseClass.c").read_text(encoding="utf-8")
start = source.index("_XmTransformSubResources(")
end = source.index("static void", start)
region = source[start:end]

assert "SIZE_MAX / sizeof(XtResource)" in region
assert "*resources = NULL;" in region
assert "*num_resources = 0;" in region
print("PASS subresource transformation allocation size is guarded")
