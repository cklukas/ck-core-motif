#!/usr/bin/env python3
"""Check TabStack label resources are replaced transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c").read_text()
set_start = source.index("SetValues(Widget current, Widget request, Widget set,")
set_end = source.index("XtVaSetValues(sfield(_tear_off_button)", set_start)
set_region = source[set_start:set_end]
constraint_start = source.index("ConstraintSetValues(Widget current, Widget request, Widget set,")
constraint_end = source.index("\n    for( i = 0", constraint_start)
constraint_region = source[constraint_start:constraint_end]

assert "XmString          replacement;" in set_region
assert "replacement = sfield(tear_off_label) != NULL" in set_region
assert "sfield(tear_off_label) = cfield(tear_off_label);" in set_region
assert set_region.index("XmStringCopy(sfield(tear_off_label))") < set_region.index("XmStringFree(cfield(tear_off_label))")
assert "XmString          replacement;" in constraint_region
assert "replacement = sfield(tab_label_string) != NULL" in constraint_region
assert "sfield(tab_label_string) = cfield(tab_label_string);" in constraint_region
assert constraint_region.index("XmStringCopy(sfield(tab_label_string))") < constraint_region.index("XmStringFree(cfield(tab_label_string))")
print("TabStack labels preserve old values on copy failure")
