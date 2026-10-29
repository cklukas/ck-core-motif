#!/usr/bin/env python3
"""Check that optional Hierarchy disclosure buttons fail without null calls."""

from pathlib import Path


source = Path("lib/Xm/Hierarchy.c").read_text()

set_values = source[source.index("SetChildValues("):source.index("RecursiveSetChildValues(")]
assert "if (XmHierarchyC_open_close_button(node) == NULL)" in set_values
assert set_values.index("if (XmHierarchyC_open_close_button(node) == NULL)") < set_values.index("XtSetValues")

reset_start = source.index("static void\nResetOpenCloseButton")
reset = source[reset_start:source.index("static void\nToggleNodeState", reset_start)]
assert "XtCreateManagedWidget(\"openAndClose\"" in reset
create = reset.index("XtCreateManagedWidget")
post_create = reset[create:]
assert "if (XmHierarchyC_open_close_button(node) == NULL)" in post_create
assert post_create.index("if (XmHierarchyC_open_close_button(node) == NULL)") < post_create.index("XtSetMappedWhenManaged")
assert post_create.index("if (XmHierarchyC_open_close_button(node) == NULL)") < post_create.index("XtAddCallback")

print("Hierarchy disclosure-button allocation failures are null-safe")
