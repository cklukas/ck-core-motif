#!/usr/bin/env python3
"""Check Sidebar expansion tolerates a missing toggle child."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Sidebar.c").read_text()
start = source.index("static void set_expanded")
end = source.index("Boolean XmSidebarGetExpanded", start)
body = source[start:end]
assert "if (!expanded && focus_inside && sidebar->sidebar.toggle_button != NULL)" in body
assert "XmProcessTraversal(sidebar->sidebar.toggle_button" in body
layout_start = source.index("static void layout(XmSidebarWidget sidebar)")
layout = source[layout_start:source.index("\nstatic ", layout_start + 20)]
assert layout.index("if (sidebar->sidebar.toggle_button == NULL ||") < layout.index("XtSetMappedWhenManaged")
print("PASS Sidebar expansion guards the toggle child")
