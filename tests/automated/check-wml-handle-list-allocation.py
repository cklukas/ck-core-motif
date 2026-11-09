#!/usr/bin/env python3
"""Check WML dynamic handle lists reject invalid and failed allocations."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmlutils.c").read_text(
    encoding="utf-8"
)

init_start = source.index("void wmlInitHList")
resize_start = source.index("void wmlResizeHList", init_start)
init = source[init_start:resize_start]
assert "size <= 0 || (size_t)size > (size_t)-1 / sizeof(ObjectHandleDef)" in init
assert "new_vec = (ObjectHandleDefPtr) malloc ((size_t)size * sizeof(ObjectHandleDef));" in init
assert "if ( new_vec == NULL )" in init
assert init.index("listptr->max = size;") > init.index("if ( new_vec == NULL )")

clear_start = source.index("void wmlClearHList", resize_start)
resize = source[resize_start:clear_start]
assert "(size_t)new_size > (size_t)-1 / sizeof(ObjectHandleDef)" in resize
assert "(listptr->hvec, (size_t)new_size * sizeof(ObjectHandleDef));" in resize
assert "if ( new_vec == NULL )" in resize
assert resize.index("listptr->max = new_size;") > resize.index("if ( new_vec == NULL )")

print("PASS WML handle-list allocation failures preserve list state")
