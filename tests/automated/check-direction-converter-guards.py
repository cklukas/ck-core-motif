#!/usr/bin/env python3
"""Check layout-direction converters validate widget and value inputs."""

from pathlib import Path


source = Path("lib/Xm/Direction.c").read_text()
for function in ("_XmFromLayoutDirection", "_XmToLayoutDirection"):
    start = source.rindex(function)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "wid == NULL" in body, function
    assert "value == NULL" in body, function

assert "return XmSYNTHETIC_LOAD" in source
print("layout-direction converters validate widget and value inputs")
