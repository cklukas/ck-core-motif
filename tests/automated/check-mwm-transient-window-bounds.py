#!/usr/bin/env python3
"""Check MWM transient-window cache growth is bounded and transactional."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWinList.c").read_text(
    encoding="utf-8"
)
assert "#include <limits.h>" in source
assert source.count("size_t allocation_count;") >= 2
assert source.count("count > INT_MAX - 5") >= 2
assert source.count("allocation_count > (size_t)UINT_MAX / sizeof(Window)") >= 2
assert source.count("Window *resized;") >= 2
assert source.count("windows = resized;") >= 2
assert "windows =\n\t\t(Window *)WmMalloc" not in source
print("PASS MWM transient-window cache growth is bounded and transactional")
