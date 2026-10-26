#!/usr/bin/env python3
"""Check that DataField substring ranges are validated before pointer use."""

from pathlib import Path


source = Path("lib/Xm/DataF.c").read_text(encoding="utf-8")

assert "if (buffer == NULL || start < 0 || num_chars < 0 ||" in source
assert "start > XmTextF_string_length(tf))" in source
assert "num_chars > XmTextF_string_length(tf) - start" in source
assert "else\n\tbuffer[0] = '\\0';" in source

print("DataField substring range checks passed")
