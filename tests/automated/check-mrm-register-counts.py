#!/usr/bin/env python3
"""Check Mrm name registration rejects invalid entry counts."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Mrm"
app = (root / "Mrmappl.c").read_text(encoding="utf-8")
wci = (root / "Mrmwci.c").read_text(encoding="utf-8")
hier = (root / "Mrmhier.c").read_text(encoding="utf-8")

for function, source in (
    ("MrmRegisterNames", app),
    ("MrmRegisterNamesInHierarchy", app),
):
    start = source.index(function + " (")
    end = source.find("\n}\n", start)
    region = source[start:end]
    for needle in ("num_reg < 0", "SIZE_MAX / sizeof(String)",
                   "SIZE_MAX / sizeof(XtPointer)", "num_reg == 0"):
        if needle not in region:
            raise SystemExit(f"{function} count guard missing: {needle}")

for label, source in (("global", wci), ("hierarchy", hier)):
    if "if (num_cb < 0 || (num_cb > 0 &&" not in source:
        raise SystemExit(f"{label} internal registration count guard missing")

print("Mrm name-registration counts are validated")
