#!/usr/bin/env python3
"""Check UIL object comments are bounded before Xt allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarObj.c").read_text()
start = source.index("void\t\tsar_assoc_comment")
end = source.index("\n}\n", start)
body = source[start:end]

assert "comment_length >= (size_t)UINT_MAX" in body
assert "XtError(\"UIL object comment is too large\")" in body
assert "_get_memory((Cardinal)(comment_length + 1))" in body
assert "_get_memory(strlen(comment_text)+1)" not in body
print("UIL object comment allocation is bounded")
