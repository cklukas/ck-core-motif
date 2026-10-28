#!/usr/bin/env python3
"""Check charset conversion helpers validate tag and output state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/ResEncod.c").read_text()
start = source.rindex("processCharsetAndTextUtf8(")
end = source.index("processCharsetAndText(", start)
utf8 = source[start:end]
narrow_start = source.index("processCharsetAndText(", end)
narrow_end = len(source)
narrow = source[narrow_start:narrow_end]

guard = "if (tag == NULL || outc == NULL || outlen == NULL || prev == NULL)"
assert guard in utf8
assert guard in narrow
assert "return(False);" in utf8 and "return(False);" in narrow

print("PASS charset conversion state is validated")
