#!/usr/bin/env python3
"""Check that DataField multibyte counting terminates on zero-length results."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("_XmDataFieldCountCharacters")
end = source.index("return count;", start)
region = source[start:end]
assert "if (char_size <= 0) break;" in region
assert "if (char_size < 0) break;" not in region
print("DataField multibyte counting terminates when mblen returns zero")
