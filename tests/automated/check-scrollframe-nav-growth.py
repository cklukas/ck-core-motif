#!/usr/bin/env python3
"""Check ScrollFrame navigator-list growth is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c").read_text()
start = source.index("Cardinal new_slots = scroll_frame_data->num_nav_slots + 2;")
allocation = source.index("WidgetList new_list", start)
guard = source.index("if (new_list == NULL)", allocation)
commit = source.index("scroll_frame_data->num_nav_slots = new_slots;", guard)
assert start < allocation < guard < commit
assert "return;" in source[guard:commit]
print("PASS ScrollFrame navigator-list growth is transactional")
