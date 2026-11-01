#!/usr/bin/env python3
"""Check FontSelector width-safe and XtArgVal-safe declarations."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()

assert "static const short resolutions[] = { 75, 100 };" in source
assert "static short FindResolution(Widget);" in source

resolution_start = source.index("FindResolution(Widget w)")
resolution = source[resolution_start:source.index("/*\tFunction Name: IsXlfdFont", resolution_start)]
assert "Cardinal i;" in resolution
assert "short pref;" in resolution

store_start = source.index("StoreString(register String str")
store = source[store_start:source.index("/*\tFunction Name: CheckFlag", store_start)]
assert "register char c;" in store

mapping_start = source.index("SizeMapping(short size)")
mapping = source[mapping_start:source.index("/*\tFunction Name: DisplayCurrentFont", mapping_start)]
assert "Cardinal count;" in mapping

sizes_start = source.index("UpdateSizes(XmFontSelectorWidget fsw)")
sizes = source[sizes_start:source.index("/*\tFunction Name: SetComboValue", sizes_start)]
assert "Cardinal i;" in sizes

setvalues_start = source.index("SetValues(Widget old")
setvalues = source[setvalues_start:source.index("/*\tFunction Name: GetValuesHook", setvalues_start)]
assert "int current, encoding_index;" in setvalues
assert "encoding_index++, encodings++" in setvalues

values_start = source.index("GetValuesHook(Widget w")
values = source[values_start:]
assert "Cardinal i;" in values
assert "args[i].value == (XtArgVal)0" in values

print("FontSelector preserves its bounded widths and XtArgVal contract")
