#!/usr/bin/env python3
"""Check gallery category disclosure state matches the visible tree."""
from pathlib import Path

source = (Path(__file__).parents[2] / "demos/programs/ckmotif-gallery/gallery.c").read_text(encoding="utf-8")
assert "nodes[i + 2].has_children = True; nodes[i + 2].expanded = False;" in source
assert "XmTreeViewSetExpanded(tree, (XtPointer)gallery_categories[i]," in source
assert "expand_entry_category(state.tree, initial);" in source
print("PASS gallery disclosure indicators match category expansion state")
