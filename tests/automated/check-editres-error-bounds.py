#!/usr/bin/env python3
"""Check Editres diagnostics never use a message as a format string."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)
start = source.index('\nHandleToolkitErrors(')
end = source.index('\n/*\tFunction Name: ResourceTypeOf', start)
region = source[start:end]

assert "ExpandToolkitMessage(msg, params," in region
assert "FormatMessage(" in region
assert "snprintf(NULL, 0, msg" not in source
assert "sprintf(pbuf" not in source
assert "sprintf(buf," not in source
print("EditRes diagnostic formatting is bounded")
