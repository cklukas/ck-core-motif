#!/usr/bin/env python3
"""Check WML grammar token IDs are bounded before vector indexing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmloutkey.c").read_text(
    encoding="utf-8"
)

check_start = source.index("if ( grtok->val < 0 || grtok->val >= GrTokenMax )")
index_start = source.index("grtok_vec[grtok->val] = grtok;", check_start)
check = source[check_start:index_start]

assert "free (grtok->token);" in check
assert "free (grtok);" in check
assert "continue;" in check
assert "grtok_vec[grtok->val]" not in check

print("PASS WML grammar token IDs are bounded before vector access")
