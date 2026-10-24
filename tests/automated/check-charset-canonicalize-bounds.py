#!/usr/bin/env python3
"""Check that private charset canonicalization handles null input safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c").read_text()
start = source.index("_isISO(")
end = source.index("return (new_s);", start)
region = source[start:end]
assert "if (charset == NULL) return (False);" in region
assert "if (charset == NULL) return (NULL);" in region
assert 'snprintf(new_s, 10, "ISO%s", charset)' in region
assert 'sprintf(new_s, "ISO%s", charset)' not in region
print("charset canonicalization handles null input and bounded ISO output")
