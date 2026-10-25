#!/usr/bin/env python3
"""Check that TreeView node-model replacement is transactional."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TreeView.c").read_text()
start = source.index("void XmTreeViewSetNodes(")
end = source.index("\nCardinal XmTreeViewGetNodeCount", start)
region = source[start:end]

assert "items = count ? (XmTreeViewItem *)XtCalloc" in region
assert "if (count != 0 && items == NULL)" in region
assert region.index("if (count != 0 && items == NULL)") < region.index("items[i].id")
assert "if (items[i].label == NULL)" in region
assert "XtFree((char *)items);" in region
assert region.index("if (items[i].label == NULL)") < region.index("view->tree_view.items = items")

print("PASS TreeView model allocation failure preserves the current model")
