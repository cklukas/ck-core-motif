#!/usr/bin/env python3
"""Keep every gallery view tab visibly outlined when inactive."""

from pathlib import Path

source = (Path(__file__).parents[2] / "demos/programs/ckmotif-gallery/gallery.c").read_text()

required = (
    '"*exampleTab.topShadowColor: #a8afb8"',
    '"*exampleTab.bottomShadowColor: #a8afb8"',
    '"*sourceTab.topShadowColor: #a8afb8"',
    '"*sourceTab.bottomShadowColor: #a8afb8"',
    '"*iconsTab.topShadowColor: #a8afb8"',
    '"*iconsTab.bottomShadowColor: #a8afb8"',
    'state.icons_tab = XtVaCreateManagedWidget("iconsTab"',
    'XmNlabelString, text("Built-in Icons")',
    'XmNshadowThickness, 1',
)

missing = [item for item in required if item not in source]
if missing:
    raise SystemExit("gallery tab outline contract missing: " + ", ".join(missing))

print("PASS gallery view tabs retain one-pixel gray outlines")
