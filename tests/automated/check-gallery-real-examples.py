#!/usr/bin/env python3
"""Check that the gallery uses a real data-backed segmented example."""
from pathlib import Path

root = Path(__file__).parents[2]
examples = (root / "demos/programs/ckmotif-gallery/gallery_examples.c").read_text()
gallery = (root / "demos/programs/ckmotif-gallery/gallery.c").read_text()
generator = (root / "demos/programs/ckmotif-gallery/generate-gallery-assets.py").read_text()
for fragment in (
    "XmSegmentedControlItem items[3]",
    '"Overview"',
    '"Details"',
    '"Activity"',
    "XmSegmentedControlSetItems(example, items, 3)",
    "XmSegmentedControlSetSelectedIndex(example, 0, False)",
):
    assert fragment in examples, fragment
assert "gallery_create_segmented_control_example" in gallery
assert 'entry->class_symbol, "xmSegmentedControlWidgetClass"' in gallery
assert 'blocks["segmented-control-construction"]' in generator
assert 'XmNorientation, XmHORIZONTAL' in gallery
assert 'XmNsidebarExpandedWidth, 280' in gallery
print("PASS gallery segmented control uses a real data-backed example")
