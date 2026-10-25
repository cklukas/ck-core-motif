#!/usr/bin/env python3
"""Check unparse pattern skipping handles context end without asserting."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
assert "if (m_ctype == XmSTRING_COMPONENT_END)" in source
assert "assert(m_ctype != XmSTRING_COMPONENT_END);" not in source

print("XmString unparse context-end handling is assertion-free")
