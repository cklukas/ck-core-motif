#!/usr/bin/env python3
"""Check SimpleSpinBox text-child allocation failures are safe."""

from pathlib import Path


source = Path("lib/Xm/SSpinB.c").read_text()

initialize = source[source.index("ssb_p->text_field = XtCreateManagedWidget"):source.index("/*ARGSUSED*/", source.index("ssb_p->text_field = XtCreateManagedWidget"))]
assert "if (ssb_p->text_field == NULL)" in initialize
assert initialize.index("ssb_p->text_field == NULL") < initialize.index("XtSetValues(ssb_p->text_field")

for name in ("XmSimpleSpinBoxAddItem", "XmSimpleSpinBoxDeletePos", "XmSimpleSpinBoxSetItem"):
    start = source.index(name)
    body = source[start:source.index("\n}\n", start) + 3]
    assert "if (ssb_p->text_field == NULL)" in body, name
    assert body.index("ssb_p->text_field == NULL") < body.index("XtVaGetValues(ssb_p->text_field")

print("SimpleSpinBox text-child allocation failures are null-safe")
