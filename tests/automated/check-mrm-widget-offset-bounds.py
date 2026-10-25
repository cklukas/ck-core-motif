#!/usr/bin/env python3
"""Check widget class and callback offsets stay inside Mrm records."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c"
text = source.read_text(encoding="utf-8")
if text.count("widgetrec->class_offs != 0") < 2:
    raise SystemExit("both widget-instance paths lack class offset checks")
if text.count("UrmWidgetStringValid(widgetrec, widgetrec->class_offs)") < 2:
    raise SystemExit("both widget-instance paths lack class string validation")
if text.count("widgetrec->creation_offs != 0") < 2:
    raise SystemExit("both widget-instance paths lack creation offset checks")
if text.count("UrmCallbackRecordValid(widgetrec") < 2:
    raise SystemExit("both widget-instance paths lack offset bounds")
print("PASS Mrm widget class and callback offsets are bounded")
