#!/usr/bin/env python3
"""Check push/recall does not read an absent panel timeout."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmFunction.c").read_text(
    encoding="utf-8"
)
start = source.index("F_Push_Recall")
end = source.index("#endif /* PANELIST */", start)
body = source[start:end]

assert "long clientTimeout = 0;" in body
assert "if (clientTimeout > 0)" in body
assert body.index("if (clientTimeout > 0)") < body.index(
    "pPRCD->tvTimeout.tv_sec += clientTimeout / 1000;"
)
assert "pPRCD->tvTimeout.tv_sec = 0;" in body
assert "pPRCD->tvTimeout.tv_usec = 0;" in body

print("PASS MWM push/recall timeout is defined without a busy indicator")
