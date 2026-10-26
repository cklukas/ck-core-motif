#!/usr/bin/env python3
"""Check MWM TARGETS conversion reports X11 item count, not host bytes."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProtocol.c").read_text(
    encoding="utf-8"
)
start = source.index("if (target == wmGD.xa_TARGETS)")
end = source.index("      found = True;", start)
region = source[start:end]
assert "*outputType   = XA_ATOM;" in region
assert "*outputLen = (unsigned long)targetCount;" in region
assert "*outputLen = (targetCount * sizeof(Atom)) >> 2;" not in region
print("PASS MWM TARGETS conversion reports the X11 item count")
