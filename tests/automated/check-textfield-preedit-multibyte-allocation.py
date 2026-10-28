#!/usr/bin/env python3
"""Check XIM multibyte preedit buffers use byte lengths, not character counts."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void PreeditDraw(")
end = source.index("\nstatic void\nPreeditCaret", start)
body = source[start:end]

assert body.count("input_bytes = strlen(call_data->text->string.multi_byte);") == 2
assert body.count("input_bytes + 1 > UINT_MAX") == 2
assert body.count("XtMalloc((Cardinal)(input_bytes + 1))") == 2
assert "XtMalloc((insert_length+1)*sizeof(char))" not in body
assert "XtMalloc((insert_length+1)*(tf->text.max_char_size))" not in body

print("PASS TextField multibyte preedit allocation bounds")
