#!/usr/bin/env python3
"""Check narrow XIM preedit append reallocations are bounded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("if (tf->text.overstrike && rest_len)")
end = source.index("\n    } else {\n        if (call_data->text)", start)
body = source[start:end]

assert body.count("extra_length > SIZE_MAX - current_length - 1") == 2
assert body.count("current_length + extra_length + 1 > UINT_MAX") == 2
assert body.count("XtRealloc(mb, (Cardinal)required_size)") == 2
assert "XtRealloc(mb, strlen(mb)+strlen(over_mb)+1)" not in body
assert "XtRealloc(mb, strlen(mb)+(recover_len+1))" not in body

print("PASS TextField preedit append allocation bounds")
