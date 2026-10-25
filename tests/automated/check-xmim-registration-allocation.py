#!/usr/bin/env python3
"""Check XmIm registration allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.index("remember_widget(")
end = source.index("\nstatic void\nfree_saved_arg", start)
widget_region = source[start:end]
assert "if (r == NULL) return NULL;" in widget_region

start = source.index("remember_values(")
end = source.index("\nstatic Boolean\nopen_input_method", start)
region = source[start:end]
assert "if (r == NULL) return;" in region
assert "if (copy == NULL)" in region
assert "ArgList new_args;" in region
assert "if (new_args == NULL)" in region
assert "r->args = new_args;" in region

print("PASS XmIm registration allocation checks")
