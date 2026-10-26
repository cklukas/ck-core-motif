#!/usr/bin/env python3
"""Keep the gallery's inactive Example/Source tabs visibly outlined."""

from pathlib import Path

source = (Path(__file__).parents[2] / "demos/programs/ckmotif-gallery/gallery.c").read_text()

required = (
    '"*exampleTab.topShadowColor: #a8afb8"',
    '"*exampleTab.bottomShadowColor: #a8afb8"',
    '"*sourceTab.topShadowColor: #a8afb8"',
    '"*sourceTab.bottomShadowColor: #a8afb8"',
    'XmNshadowThickness, 1',
)

missing = [item for item in required if item not in source]
if missing:
    raise SystemExit("gallery tab outline contract missing: " + ", ".join(missing))

print("PASS gallery inactive tabs retain a one-pixel gray outline")
