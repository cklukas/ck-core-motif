#!/usr/bin/env python3
"""Check DataField wide callback insertion allocation arithmetic."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("_XmDataFieldCountCharacters(tf, vcb.text->ptr")
end = source.index("*replace_prev = vcb.startPos", start)
body = source[start:end]

assert "(size_t)*insert_length + 1 >" in body
assert "(size_t)UINT_MAX / sizeof(wchar_t)" in body
assert "XtMalloc((unsigned)(*insert_length + 1)" in body
assert body.index("UINT_MAX / sizeof(wchar_t)") < body.index("XtMalloc((unsigned)(*insert_length + 1)")

print("PASS DataField wide insertion allocation guard")
