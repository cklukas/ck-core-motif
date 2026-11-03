#!/usr/bin/env python3
"""Check WML handle lists never publish a failed string copy."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmlutils.c").read_text(
    encoding="utf-8"
)

helper_start = source.index("static char *\nwmlRequireStringCopy")
helper_end = source.index("void wmlUpperCaseString", helper_start)
helper = source[helper_start:helper_end]
assert "copy = wmlAllocateString (stg);" in helper
assert "if (copy == NULL)" in helper
assert "exit (EXIT_FAILURE);" in helper

list_start = source.index("void wmlInsertInHList")
member_start = source.index("WmlClassResDefPtr wmlResolveResIsMember", list_start)
lists = source[list_start:member_start]
assert lists.count("wmlRequireStringCopy (name)") == 5
assert "objname = wmlAllocateString (name)" not in lists

upper_start = source.index("void wmlUpperCaseString")
upper_end = source.index("void wmlInitHList", upper_start)
upper = source[upper_start:upper_end]
assert "size_t\t\tlength;" in upper
assert "length = strlen(stg);" in upper
assert "ndx<length" in upper

print("PASS WML handle lists reject failed name copies")
