#!/usr/bin/env python3
"""Check UIL comment association handles allocation failure safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarObj.c").read_text()
start = source.index("void\t\tsar_assoc_comment")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "if (comment == NULL)" in body
assert "object->obj_header.az_comment = NULL;" in body
assert "comment_text[0] = '\\0';" in body
assert "strcpy(comment, comment_text);" in body
print("PASS UIL comment association handles allocation failure")
