#!/usr/bin/env python3
"""Check that VendorS initializes timeout IDs with their declared type."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VendorS.c").read_text()
assert "ttp->timer = (XtIntervalId)0;" in source
assert "ttp->duration_timer = (XtIntervalId)0;" in source
assert "ttp->timer = (int) NULL;" not in source
assert "ttp->duration_timer = (int) NULL;" not in source
print("VendorS tooltip timer initialization is type-correct")
