#!/usr/bin/env python3
"""Check FontSelector encoding-list allocation and empty-list handling."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()

initialize_start = source.index("Initialize(Widget request")
initialize = source[initialize_start:source.index("/*\tFunction Name: Destroy", initialize_start)]
assert "XtCalloc(count + 1, (Cardinal)sizeof(*newList))" in initialize
assert "encodings != NULL && encodings[count] != NULL" in initialize
assert "max_entries > (Cardinal)INT_MAX - 1" in initialize
assert "encoding != NULL ? encoding : ANY_ENCODING" in initialize

menu_start = source.index("CreateEncodingMenu(XmFontSelectorWidget fsw")
menu = source[menu_start:source.index("/*\tFunction Name: CheckEncoding", menu_start)]
assert "encodings != NULL && *encodings != NULL" in menu

change_start = source.index("ChangeEncoding(Widget w")
change = source[change_start:source.index("/*\tFunction Name: ToggleScaling", change_start)]
assert change.index("if (encoding_index == 0)") < change.index(
    "if (ENCODING_LIST(fsw) == NULL)")

setvalues_start = source.index("SetValues(Widget old")
setvalues = source[setvalues_start:source.index("/*\tFunction Name: GetValuesHook", setvalues_start)]
assert "encodings != NULL && *encodings != NULL" in setvalues
assert "Boolean encoding_changed;" in setvalues
assert "if (new_encoding_list || encoding_changed)" in setvalues
assert "? ENCODING_STRING(set_fsw) : ANY_ENCODING" in setvalues
assert "ENCODING_STRING(set_fsw) != NULL &&" in setvalues

print("FontSelector encoding lists are allocated with a terminator and accept empty input")
