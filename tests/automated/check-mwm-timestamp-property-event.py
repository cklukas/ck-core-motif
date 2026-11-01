#!/usr/bin/env python3
"""Check MWM's zero-length timestamp property update has no data pointer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmEvent.c").read_text(
    encoding="utf-8"
)
start = source.index("Time GetTimestamp (void)")
end = source.index("END OF FUNCTION GetTimestamp", start)
body = source[start:end]

assert "XChangeProperty" in body
assert "NULL, 0);" in body
assert "long property;" not in body
assert "(unsigned char *)&property" not in body
print("PASS MWM timestamp update uses a null payload for zero property items")
