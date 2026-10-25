#!/usr/bin/env python3
"""Check ScrollFrame navigator-list growth is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c").read_text()
start = source.index("if (scroll_frame_data->num_nav_list ==")
new_slots = source.index("Cardinal new_slots =", start)
realloc = source.index("WidgetList new_list =", new_slots)
guard = source.index("if (new_list == NULL)", realloc)
publish = source.index("scroll_frame_data->num_nav_slots = new_slots;", guard)
assert start < new_slots < realloc < guard < publish
assert "scroll_frame_data->nav_list = new_list;" in source[publish:source.index("}", publish) + 1]
print("PASS ScrollFrame navigator growth is transactional")
