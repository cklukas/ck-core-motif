#!/usr/bin/env python3
"""Check FontSelector encoding matching bounds and inputs."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("CheckEncoding(XmFontSelectorWidget fsw")
body = source[start:source.index("/************************************************************", start)]

assert "fsw == NULL || fam == NULL" in body
assert "ENCODING_STRING(fsw) == NULL" in body
assert "fam->encoding_alloc <= 0" in body
assert "if (i >= fam->encoding_alloc)" in body
assert body.index("if (i >= fam->encoding_alloc)") < body.index(
    "fam->encodings[i] == curr")

print("FontSelector encoding matching is bounded and guarded")
