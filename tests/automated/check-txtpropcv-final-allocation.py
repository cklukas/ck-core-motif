#!/usr/bin/env python3
"""Check final locale/text conversion buffer allocation is guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.rfind("for (i = 0, total_size = 0", 0,
                     source.index("final_string =\n\t(unsigned char *)XtMalloc"))
body = source[start:source.index("final_string[0]", start)]
assert "if (final_string == NULL)" in body
assert "for (i = 0; i < count; ++i) XtFree(useable_text[i]);" in body
assert "XtFree((char *)useable_text);" in body
assert "_XmAppUnlock(app);" in body
assert "return BadAlloc;" in body
assert "(size_t)total_size + 1 > UINT_MAX" in body
assert "(Cardinal)(sizeof(char) * ((size_t)total_size + 1))" in body
print("PASS final conversion buffer allocation is guarded")
