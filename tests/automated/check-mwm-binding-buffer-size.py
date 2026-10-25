#!/usr/bin/env python3
"""Check MWM builtin binding buffers validate root-menu sizing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
key = source[source.index("if (pSD->keyBindings == builtinKeyBindingsName)") : source.index("    else\n    {", source.index("if (pSD->keyBindings == builtinKeyBindingsName)"))]
button = source[source.index("if (pSD->buttonBindings == builtinButtonBindingsName)") : source.index("#ifdef NO_MESSAGE_CATALOG", source.index("if (pSD->buttonBindings == builtinButtonBindingsName"))]
for body, template in ((key, "builtinKeyBindings"), (button, "builtinButtonBindings")):
    assert f"snprintf(NULL, 0, {template}, root_menu)" in body
    assert "messageLength < 0 || (size_t)messageLength >= UINT_MAX" in body
    assert "bufferLength = (size_t)messageLength + 1;" in body
    assert f"snprintf(buffer, bufferLength, {template}, root_menu)" in body
    assert "XtMalloc((Cardinal)bufferLength)" in body

print("MWM builtin binding buffer sizing is checked")
