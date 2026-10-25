#!/usr/bin/env python3
"""Check allocation failure handling for two-byte Text conversion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("case 2: {", source.index("_XmTextCharactersToBytes"))
end = source.index("\n  } /* end case 2 */", start)
region = source[start:end]
assert "temp_char = (unsigned char*) XtMalloc (max_char_size);" in region
assert "if (temp_char == NULL)" in region
assert "*bytes = '\\0';" in region
assert "return 0;" in region
assert region.index("if (temp_char == NULL)") < region.index("temp_char[j]")
print("PASS two-byte Text conversion handles temporary allocation failure")
