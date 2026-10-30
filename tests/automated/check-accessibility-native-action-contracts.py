#!/usr/bin/env python3
"""Keep native accessibility action counts aligned with dispatch support."""
from pathlib import Path

source = Path("lib/Xm/Accessible.c").read_text(encoding="utf-8")
query = source[source.index("static Boolean query_impl") : source.index("static char *text_impl")]
dispatch = source[source.index("static Boolean action_impl") : source.index("static char *action_name_impl")]
names = source[source.index("static char *action_name_impl") : source.index("static Boolean set_value_impl")]

toolbar = query[query.index("if (XmIsToolbar(w))") : query.index("if (XmIsSidebar(w))")]
assert "i->action_count = 0;" in toolbar
assert "XmToolbarGetOverflowCount(w) ? 1 : 0" not in toolbar
assert "XmIsSidebar(n->widget) && action == 0" in dispatch
assert "XmSidebarToggle(n->widget, NULL);" in dispatch
assert 'XmIsSidebar(n->widget))\n    return XtNewString("toggle")' in names
assert "XmIsAutoSuggestBox(n->widget) && action == 0" in dispatch
assert "XmAutoSuggestBoxCommit(n->widget, 0, NULL);" in dispatch
assert 'XmIsAutoSuggestBox(n->widget))\n    return XtNewString("commit-first-suggestion")' in names

print("PASS accessibility native action contracts")
