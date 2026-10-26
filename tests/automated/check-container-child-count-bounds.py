#!/usr/bin/env python3
"""Ensure Container child-list enumeration cannot wrap its public count."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
start = source.index("int\nXmContainerGetItemChildren")
end = source.index("\n\n/************************************************************************\n *  XmContainerRelayout", start)
region = source[start:end]

assert "while (node->next_ptr)" in region
assert "if (clist_count == INT_MAX)" in region
assert region.index("if (clist_count == INT_MAX)") < region.index("clist_count++")

print("Container child-list counting rejects int overflow")
