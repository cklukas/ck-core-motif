#!/usr/bin/env python3
"""Check FileSelectionBox up/down action arguments are parsed safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c").read_text()
start = source.index("_XmFileSelectionBoxUpOrDown(")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "if (!argv || !*argv)" in region
assert "parsed_key = strtol(*argv, &end, 10)" in region
assert "errno == ERANGE" in region
assert "parsed_key < 0 || parsed_key > 3" in region
assert "atoi(" not in region
print("FileSelectionBox navigation action validates its key argument")
