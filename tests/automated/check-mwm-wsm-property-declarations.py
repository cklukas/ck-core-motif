#!/usr/bin/env python3
"""Check WSM workspace-property code includes its direct declarations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients/mwm/WmProperty.c").read_text(
    encoding="utf-8"
)
assert '#include "WmIconBox.h"' in source
assert "#include <Xm/AtomMgr.h>" in source
assert source.index('#include "WmIconBox.h"') < source.index("WmXmStringToString")
assert source.index("#include <Xm/AtomMgr.h>") < source.index("XmInternAtom")
print("PASS MWM WSM property code declares its Motif and icon-box APIs")
