#!/usr/bin/env python3
"""Guard TextField modify-verify snapshot sizing and allocation."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("insert_length_orig = insert_length;")
end = source.index("\n    if (!ModifyVerify", start)
region = source[start:end]

assert "SIZE_MAX / size" in region
assert "UINT_MAX / size" in region
assert "insert_orig == NULL" in region
assert "bcopy(insert, insert_orig, insert_bytes)" in region
print("PASS TextField modify-verify snapshot allocation guard")
