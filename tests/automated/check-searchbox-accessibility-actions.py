#!/usr/bin/env python3
"""Check SearchBox accessibility exposes live commit/clear actions."""

from pathlib import Path


source = Path("lib/Xm/Accessible.c").read_text()

assert "search_clear_available(Widget widget)" in source
assert "i->action_count = 1 + (search_clear_available(w) ? 1 : 0);" in source
assert 'action == 0 ? "commit" : "clear"' in source
assert "if (action == 1 && search_clear_available(n->widget))" in source
assert "XmSearchBoxCommit(n->widget, NULL);" in source
assert "XmSearchBoxClear(n->widget);" in source

print("SearchBox accessibility exposes commit and conditional clear actions")
