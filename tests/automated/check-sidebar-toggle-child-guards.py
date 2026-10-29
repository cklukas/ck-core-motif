#!/usr/bin/env python3
"""Check Sidebar expansion tolerates a missing toggle child."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Sidebar.c").read_text()
start = source.index("static void set_expanded")
end = source.index("Boolean XmSidebarGetExpanded", start)
body = source[start:end]
assert "if (sidebar->sidebar.toggle_button != NULL)" in body
assert "XtVaSetValues(sidebar->sidebar.toggle_button" in body
assert "XmProcessTraversal(sidebar->sidebar.toggle_button" in body
print("PASS Sidebar expansion guards the toggle child")
