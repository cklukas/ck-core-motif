#!/usr/bin/env python3
"""Check MWM builtin binding buffers validate root-menu sizing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
key = source[source.index("if (pSD->keyBindings == builtinKeyBindingsName)") : source.index("    else\n    {", source.index("if (pSD->keyBindings == builtinKeyBindingsName)"))]
button = source[source.index("if (pSD->buttonBindings == builtinButtonBindingsName)") : source.index("#ifdef NO_MESSAGE_CATALOG", source.index("if (pSD->buttonBindings == builtinButtonBindingsName"))]
for body in (key, button):
    assert "rootLength > SIZE_MAX - formatLength - 1" in body
    assert "formatLength + rootLength + 1 > UINT_MAX" in body
    assert "bufferLength = formatLength + rootLength + 1;" in body
    assert "XtMalloc((Cardinal)bufferLength)" in body

print("MWM builtin binding buffer sizing is checked")
