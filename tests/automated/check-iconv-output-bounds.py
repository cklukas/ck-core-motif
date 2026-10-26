#!/usr/bin/env python3
"""Check that iconv output growth stays within Xt's Cardinal width."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c"
text = source.read_text(encoding="utf-8")
start = text.index("ConvertWithIconv(const char")
start = text.index("{", start)
end = text.index("\n}\n", start)
region = text[start:end]

required = (
    "outbuf_size > SIZE_MAX / 2",
    "outbuf_size > (size_t)UINT_MAX / 2",
    "outbuf_size *= 2",
)
missing = [item for item in required if item not in region]
if missing:
    raise SystemExit("iconv output growth bounds missing: " + ", ".join(missing))

print("iconv output growth stays within Xt Cardinal width")
