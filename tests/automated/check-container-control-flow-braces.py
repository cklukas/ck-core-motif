#!/usr/bin/env python3
"""Check that Container's warning-prone branches have explicit blocks."""

from pathlib import Path
import re

source = Path(__file__).parents[2] / "lib" / "Xm" / "Container.c"
text = source.read_text()

if len(re.findall(r"if \(pc->outline_state == XmEXPANDED\)\s*\{", text)) != 2:
    raise SystemExit("Container outline-state branches do not both use explicit blocks")

for needle in (
    "if (pc->outline_state == XmEXPANDED)\n\t    {",
    "if ((old_focus == NULL)",
    "\t{\n\t    return new_focus;\n\t}\n\n\tif (((cw =",
):
    if needle not in text:
        raise SystemExit(f"Container control flow is missing explicit block: {needle!r}")

print("Container outline and traversal branches use explicit blocks")
