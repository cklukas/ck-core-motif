#!/usr/bin/env python3
"""Ensure transfer argument-list counters match Xt's Cardinal contract."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()

drag_start = source.index("XmeDragSource(")
drop_start = source.index("XmeDropSink(")
destination_start = source.index("/*************************************************************/", drop_start)
drag = source[drag_start:drop_start]
drop = source[drop_start:destination_start]

assert drag.count("Cardinal arg_count;") == 1
assert drop.count("Cardinal arg_count;") == 1
assert "(Cardinal)(sizeof(*args) * (size_t)arg_count)" in drag
assert "(Cardinal)(sizeof(*args) * (size_t)arg_count)" in drop
assert "(size_t)-1 / sizeof(*args)" not in drag + drop

print("PASS transfer argument counters use Cardinal-safe allocation")
