#!/usr/bin/env python3
"""Check Container insertion tolerates siblings without constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("InsertNode(\n\tCwidNode")
end = source.index("SeverNode(\n", start)
region = source[start:end]
lookup = "sc = GetContainerConstraint(next_node->widget_ptr);"
guard = "if (sc == NULL)"
assert region.count(lookup) >= 2
assert region.count(guard) >= 2
assert region.index(guard) > region.index(lookup)
assert region.rindex(guard) > region.rindex(lookup)
assert "next_node = next_node->next_ptr;" in region

print("PASS Container insertion skips siblings without constraints")
