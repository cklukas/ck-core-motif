#!/usr/bin/env python3
"""Check XmString concatenation rejects a failed first-line copy."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmString \nXmStringConcatAndFree")
end = source.index("\n\n/*", start)
body = source[start:end]

copy = "_XmStrEntry(a_str)[a_lc] = _XmStringEntryCopy(b_line);"
position = body.index(copy)
failure = body[position:position + 300]
assert "if (_XmStrEntry(a_str)[a_lc] == NULL)" in failure
assert "XmStringFree(a);" in failure
assert "XmStringFree(b);" in failure
assert "return NULL;" in failure
assert failure.index("return NULL;") < failure.index("a_lc++;")

print("PASS XmString concatenation rejects a failed first-line copy")
