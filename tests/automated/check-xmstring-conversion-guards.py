#!/usr/bin/env python3
"""Check XmString conversion handles context initialization failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "if (!XmStringInitContext (&xmStrContext, xmString))" in text
assert "return (NULL);" in text
print("XmString conversion handles context initialization failure")
