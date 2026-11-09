#!/usr/bin/env python3
"""Keep virtual-key lookup result counts and allocation bytes representable."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text(
    encoding="utf-8"
)
start = source.index("XmeVirtualToActualKeysyms(Display")
end = source.index("Boolean \n_XmVirtKeysLoadFileBindings", start)
body = source[start:end]

limit = body.index("if (matches == INT_MAX)")
increment = body.index("matches++;", limit)
native_guard = body.index("(size_t)matches > SIZE_MAX / sizeof(XmKeyBindingRec)")
cardinal_guard = body.index(
    "(size_t)matches > (size_t)UINT_MAX / sizeof(XmKeyBindingRec)"
)
bytes_value = body.index("binding_bytes = (size_t)matches * sizeof(XmKeyBindingRec);")
allocation = body.index("XtMalloc((Cardinal)binding_bytes)")

assert limit < increment < native_guard < cardinal_guard < bytes_value < allocation

print("PASS virtual-key result lookup bounds count and Xt allocation bytes")
