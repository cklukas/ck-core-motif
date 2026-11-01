#!/usr/bin/env python3
"""Check TabStack bounds and validates filtered argument-list merges."""
from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/TabStack.c").read_text(
    encoding="utf-8"
)
initialize_start = source.index("Initialize(Widget request, Widget set")
initialize_end = source.index("#define rfield", initialize_start)
initialize = source[initialize_start:initialize_end]

assert "num_filtered_args > (Cardinal)(~(Cardinal)0) - n" in initialize
assert "merged_args == NULL" in initialize
assert "XmTabStack_tab_box(ts) == NULL" in initialize
assert "_XmAppearanceInheritFont(XmTabStack_tab_box(ts), set);" in initialize
assert initialize.index("XmTabStack_tab_box(ts) == NULL") < initialize.index(
    "_XmAppearanceInheritFont(XmTabStack_tab_box(ts), set);"
)

set_values_start = source.index("SetValues(Widget current, Widget request")
set_values_end = source.index("#ifdef TEAR_OFF_TABS", set_values_start)
set_values = source[set_values_start:set_values_end]
assert "ArgList          filtered_args = NULL, merged_args = NULL;" in set_values
assert "num_filtered_args <= (Cardinal)(~(Cardinal)0) - (Cardinal)n" in set_values
assert "if (merged_args != NULL)" in set_values
assert "XtSetValues(XmTabStack_tab_box(s_tab), merged_args," in set_values
print("PASS TabStack argument merges are bounded and null-safe")
