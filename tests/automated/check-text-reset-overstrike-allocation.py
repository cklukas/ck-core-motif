#!/usr/bin/env python3
"""Ensure input-method reset checks its overstrike cursor buffer."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("if (data->overstrike) {", text.index("_XmTextResetIC"))
end = text.index("\n      size = _XmTextBytesToCharacters", start)
block = text[start:end]

assert "n < 0" in block
assert "UINT_MAX / (size_t)tw->text.char_size" in block
assert "tmp_mb = XtMalloc((Cardinal)" in block
assert "if (tmp_mb == NULL)" in block
assert "ResetUnder(tw);" in block

print("text reset overstrike allocation guard: ok")
