#!/usr/bin/env python3
"""Check the MWM debug error handler formats request codes safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmError.c").read_text(
    encoding="utf-8"
)
start = source.index("int\nWmXErrorHandler")
region = source[start:source.index("#endif /* DEBUG */", start)]
assert 'snprintf(message, sizeof(message), "%d"' in region
assert "errorEvent->request_code" in region
assert "renderedLength < 0" in region
assert 'message[0] = \'\\0\';' in region
assert 'sprintf(message, "%d", errorEvent->request_code)' not in region
print("PASS MWM error request codes use bounded formatting")
