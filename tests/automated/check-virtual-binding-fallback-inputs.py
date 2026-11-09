#!/usr/bin/env python3
"""Keep fallback virtual-binding loading from dereferencing null inputs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmVirtKeysLoadFallbackBindings(Display")
body = source[start:]

guard = body.index("if (display == NULL || binding == NULL)")
clear_output = body.index("*binding = NULL;", guard)
intern = body.index("XInternAtoms(display", clear_output)

assert guard < clear_output < intern

print("PASS virtual-binding fallback loader validates its direct inputs")
