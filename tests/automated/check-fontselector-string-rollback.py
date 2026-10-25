#!/usr/bin/env python3
"""Check FontSelector label replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
start = source.index("#define DoCheck(old_one,new_one)")
end = source.index("#undef DoCheck", start)
region = source[start:end]

assert "XmString replacement;" in region
assert "replacement = (new_one != NULL) ? XmStringCopy(new_one) : NULL;" in region
assert "if (new_one == NULL || replacement != NULL)" in region
assert "XmStringFree(old_one); new_one = replacement; reset = True;" in region
assert "else { new_one = old_one; }" in region
assert region.index("XmStringCopy(new_one)") < region.index("XmStringFree(old_one)")
print("FontSelector labels preserve old values on copy failure")
