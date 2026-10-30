#!/usr/bin/env python3
"""Check UIL debug output rejects a failed second formatting pass."""

from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstLst.c").read_text(
    encoding="utf-8"
)
start = source.index("void\tlst_debug_output")
region = source[start:]
assert "int written;" in region
assert "written = vsnprintf(formatted" in region
assert "if (written < 0 || written > formatted_length)" in region
failure = region.index("if (written < 0 || written > formatted_length)")
assert region.index("_free_memory(formatted);", failure) < region.index(
    "lst_debug_append(formatted);", failure
)
print("PASS UIL debug output checks the second formatting result")
