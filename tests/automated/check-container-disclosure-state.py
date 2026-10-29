#!/usr/bin/env python3
"""Check that refreshed disclosure buttons follow their associated rows."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "Container.c"
text = source.read_text()

start = text.index("ChangeOutlineButtons(")
end = text.index("\n\n/************************************************************************\n * ExpandCwid", start)
body = text[start:end]

if "if (c == NULL || c->related_cwid == NULL)" not in body:
    raise SystemExit("disclosure refresh does not reject an unassociated button")
if "c = GetContainerConstraint(c->related_cwid);" not in body:
    raise SystemExit("disclosure refresh does not read the associated row state")
if "if (c == NULL)" not in body:
    raise SystemExit("disclosure refresh does not guard the associated row lookup")

print("Container disclosure refresh follows the associated row state")
