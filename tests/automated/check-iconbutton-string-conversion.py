#!/usr/bin/env python3
"""Keep IconButton's string-to-XmString bridge inside Xt's input range."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconButton.c").read_text(
    encoding="utf-8"
)
start = source.index("CreateXmString(Widget w, String str)")
end = source.index("/*\n * XmRCallProc routine", start)
body = source[start:end]

null_guard = body.index("if (str == NULL)")
length = body.index("str_length = strlen(str);")
bound = body.index("str_length >= (size_t)UINT_MAX")
assignment = body.index("fromVal.size = (Cardinal)(str_length + 1);")
assert null_guard < length < bound < assignment

print("PASS IconButton string conversion validates Xt source length")
