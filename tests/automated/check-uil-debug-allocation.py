#!/usr/bin/env python3
"""Check that UIL debug output handles formatter allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstLst.c").read_text()
start = source.index("void\tlst_debug_output")
region = source[start:]
assert "formatted = (char *)_get_memory((size_t)formatted_length + 1);" in region
assert "if (formatted == NULL)" in region
assert "goto done;" in region
assert region.index("if (formatted == NULL)") < region.index("vsnprintf(formatted")
print("PASS UIL debug formatter handles allocation failure")
