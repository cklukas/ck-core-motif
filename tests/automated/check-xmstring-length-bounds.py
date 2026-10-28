#!/usr/bin/env python3
"""Check XmStringLength preserves its signed public ABI safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("int \nXmStringLength")
end = source.index("Boolean \nXmeStringIsValid", start)
region = source[start:end]

assert "len > (unsigned int)INT_MAX ? INT_MAX : (int)len" in region
assert "return((int)len);" not in region
print("XmStringLength clamps byte-stream lengths at INT_MAX")
