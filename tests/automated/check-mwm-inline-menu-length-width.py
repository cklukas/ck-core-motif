#!/usr/bin/env python3
"""Check inline MWM menu formatting checks before Cardinal narrowing."""
from pathlib import Path
text = (Path(__file__).parents[2] / "clients/mwm/WmMenu.c").read_text()
start = text.index("static void SearchForOtherMatches")
body = text[start:text.index("static void InsertTreeOnClient", start)]
assert body.count("(size_t)command_length < UINT_MAX") == 1
assert body.count("(size_t)funcargs_length < UINT_MAX") == 1
assert "(unsigned int)command_length < UINT_MAX" not in body
assert "(unsigned int)funcargs_length < UINT_MAX" not in body
print("PASS MWM inline menu lengths are checked before Cardinal narrowing")
