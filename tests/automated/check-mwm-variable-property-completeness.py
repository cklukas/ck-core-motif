#!/usr/bin/env python3
"""Check that bounded variable-size MWM property reads reject prefixes."""

from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmProperty.c").read_text(
    encoding="utf-8"
)


def function_body(signature: str) -> str:
    start = source.index(signature)
    return source[start:source.index("\n} /* END OF FUNCTION", start)]


protocols = function_body("void ProcessWmProtocols")
messages = function_body("void ProcessMwmMessages")
colormaps = function_body("void ProcessWmColormapWindows")

assert "(leftover != 0)" in protocols
assert "(nitems == 0)" in protocols
assert "(property == NULL)" in protocols
assert "pCD->clientProtocolCount = (int)nitems;" in protocols
assert "i < (int)nitems" in protocols

assert "(leftover != 0)" in messages
assert "pCD->mwmMessagesCount = (int)nitems;" in messages
assert "i < (int)nitems" in messages
assert "(unsigned long)(pCD->mwmMessages[i] = property[i])" in messages

assert "(leftover == 0)" in colormaps
assert "propertyCount = (int)nitems;" in colormaps
assert "i < propertyCount" in colormaps
assert "i == propertyCount" in colormaps

print("MWM variable-size property readers reject truncated payloads")
