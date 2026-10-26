#!/usr/bin/env python3
"""Check compound-text concatenation propagates allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
assert "static Boolean ctextConcat(" in source
assert "OctetPtr *str1" in source
assert "unsigned int *str1len" in source
assert "if (new_str == NULL)" in source
assert "return False;" in source[source.index("static Boolean ctextConcat("):]
assert "CTEXT_APPEND_OR_RETURN" in source
assert "CTEXT_APPEND_OR_FAIL_TOP" in source
assert "_XmStringContextFree(&stack_context)" in source
assert "outlen += CTEXT_SET_ISO8859_9_LEN" not in source
print("PASS compound-text concatenation failure propagation")
