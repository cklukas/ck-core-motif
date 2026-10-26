#!/usr/bin/env python3
"""Check traversal sorting buffers are bounded and allocation-checked."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/TraversalI.c").read_text(
    encoding="utf-8"
)

assert source.count("num_nodes > UINT_MAX / sizeof(XmTraversalNode)") >= 3
assert source.count("if (node_list == NULL)") >= 3
print("Traversal node-list stack buffers are bounded and checked")
