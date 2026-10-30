#!/usr/bin/env python3
"""Check MultiList deferred updates tolerate missing optional children."""

from pathlib import Path


source = Path("lib/Xm/MultiList.c").read_text()
start = source.index("SetValues(Widget current")
end = source.index("/*\tFunction Name: GeometryManager", start)
body = source[start:end]

assert "for (find_index = 0; find_index < NUM_FIND_WIDGETS; find_index++)" in body
assert "if (find_widgets[find_index] == NULL)" in body
assert "XtManageChild(find_widgets[find_index])" in body
assert "XtUnmanageChild(find_widgets[find_index])" in body

find_update = body.index("XtSetArg(args[0], XmNlabelString")
assert "if (XmMultiList_find(e_set) != NULL)" in body[find_update:]
title_update = body.index("XtSetArg(zargs[znum_args], XmNlabelString")
assert "if (XmMultiList_title_wid(e_set) != NULL)" in body[title_update:]

print("MultiList optional child updates are null-safe")
