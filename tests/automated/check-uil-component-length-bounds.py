#!/usr/bin/env python3
"""Check UIL compound-string component lengths fit Xm's unsigned API width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
helper = source[source.index("static unsigned int\nsar_component_length"):source.index("\n\f\n/*", source.index("static unsigned int\nsar_component_length"))]

assert "length > UINT_MAX" in helper
assert "XtError(\"UIL compound-string component is too large\")" in helper
assert "return (unsigned int)length;" in helper
assert source.count("sar_component_length(_MOTIF_DEFAULT_LOCALE)") == 1
assert source.count("sar_component_length(str)") == 1
assert "XmStringComponentCreate(type, strlen(str)" not in source
print("UIL compound-string component lengths are bounded")
