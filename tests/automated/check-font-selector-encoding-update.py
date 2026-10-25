#!/usr/bin/env python3
"""Check that FontSelector encoding updates preserve state on allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text(encoding="utf-8")
start = source.index("String new_encoding;")
end = source.index("return(omenu);", start)
region = source[start:end]
assert "String new_encoding;" in region
assert "new_encoding = XtNewString" in region
assert "if (new_encoding != NULL)" in region
assert "ENCODING_STRING(fsw) = new_encoding;" in region
print("PASS FontSelector encoding update preserves state on allocation failure")
