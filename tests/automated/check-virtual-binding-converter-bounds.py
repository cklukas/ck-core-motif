#!/usr/bin/env python3
"""Ensure virtual-binding conversion sizes fit Xt allocations."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text(
    encoding="utf-8"
)
start = source.index("CvtStringToVirtualBinding(Display")
end = source.index("static void\nFillBindingsFromDB", start)
body = source[start:end]

count = body.index("if (count > 0)")
native_guard = body.index("(size_t)count > SIZE_MAX / sizeof(XmKeyBindingRec)")
cardinal_guard = body.index(
    "(size_t)count > (size_t)UINT_MAX / sizeof(XmKeyBindingRec)"
)
bytes_value = body.index("binding_bytes = (size_t)count * sizeof(XmKeyBindingRec);")
loop = body.index("for (tmp = 0; tmp < count; tmp++)")
output_size = body.index("toVal->size = (Cardinal)binding_bytes;")

assert count < native_guard < cardinal_guard < bytes_value < loop < output_size
assert "sizeof(XmKeyBindingRec) * count" not in body

print("PASS virtual-binding conversion bounds output allocation bytes")
