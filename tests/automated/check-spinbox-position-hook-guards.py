#!/usr/bin/env python3
"""Check SpinBox position conversion validates state and avoids zero division."""

from pathlib import Path


source = Path("lib/Xm/SpinB.c").read_text()
for signature in ("GetPositionValue(Widget w", "SetPositionValue(Widget w"):
    start = source.rindex(signature)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "w == NULL || value == NULL" in body, signature
    assert "w->core.constraints == NULL" in body, signature
    assert "wc->increment_value != 0" in body, signature

print("SpinBox position hooks validate state and reject zero increments")
