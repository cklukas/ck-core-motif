#!/usr/bin/env python3
"""Check that Container icon headers tolerate empty heading resources."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("CreateIconHeader(\n    Widget", 60000)
start = source.index("{", start)
end = source.index("\n}", start)
body = source[start:end]

assert "detail_heading == NULL" in body
assert "detail_heading_count == 0" in body
assert "detail_heading_count == INVALID_COUNT" in body
assert "assert(cw->container.detail_heading_count > 0)" not in body

print("Container empty-heading guard: ok")
