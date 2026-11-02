#!/usr/bin/env python3
"""Check bounded MWM resource-key construction."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
helper_start = source.index("static void\nAppendResourceNamePart")
helper_end = source.index("\nString\nResCat", helper_start)
helper = source[helper_start:helper_end]
assert "value == NULL || remaining == NULL || *remaining == 0" in helper
assert "appendLength = strlen ((char *)value);" in helper
assert "if (appendLength > *remaining)" in helper
assert "appendLength = *remaining;" in helper
assert "*remaining -= appendLength;" in helper

rescat_start = source.index("String\nResCat")
rescat_end = source.index("} /* END OF FUNCTION ResCat */", rescat_start)
rescat = source[rescat_start:rescat_end]
assert "size_t remaining;" in rescat
assert "int count;" not in rescat
assert "strncat" not in rescat
assert rescat.count("AppendResourceNamePart") == 7
assert "if (s4)" in rescat

print("PASS MWM resource-key construction keeps truncation bounded")
