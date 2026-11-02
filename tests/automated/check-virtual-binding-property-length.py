#!/usr/bin/env python3
"""Keep virtual-binding X11 property lengths inside Xlib's int range."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text(
    encoding="utf-8"
)
start = source.index("SetBindingsProperty(Display *display")
end = source.index("static void \nLoadVendorBindings", start)
body = source[start:end]

length = body.index("binding_length = strlen(binding);")
guard = body.index("if (binding_length > (size_t)INT_MAX)")
property = body.index("XChangeProperty(display", guard)
count = body.index("(int)binding_length", property)
assert length < guard < property < count

fallback = source.index("_XmVirtKeysLoadFallbackBindings(Display")
fallback_body = source[fallback:]
assert "SetBindingsProperty(display, atoms[XmA_MOTIF_BINDINGS]" in fallback_body
assert "SetBindingsProperty(display, atoms[XmA_MOTIF_DEFAULT_BINDINGS]" in fallback_body

print("PASS virtual-binding publication validates Xlib property length")
