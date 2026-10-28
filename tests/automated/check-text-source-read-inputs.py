#!/usr/bin/env python3
"""Check Text source reads reject invalid source and payload state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("ReadSource(XmTextSource source,")
end = source.index("\nvoid\n_XmTextValidate", start)
body = source[start:end]

assert "source == NULL || block == NULL || source->data == NULL" in body
assert "data->widgets == NULL || data->numwidgets <= 0" in body
assert "block->length > 0 && block->ptr != NULL" in body
assert body.index("return 0") < body.index("_XmStringSourceReadString")

print("PASS Text source reads guard invalid input and null payloads")
