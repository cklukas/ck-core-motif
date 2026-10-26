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
    "XmNumberBoxSetValue(example, 42, False)",
    "XmRatingControlSetValue(example, 4, False)",
    "XmProgressBarSetValue(example, 68, False)",
    "XmProgressRingSetValue(example, 68, False)",
    "XmStepperSetValue(example, 7, False)",
    "XmColorPickerSetValue(example, 0x3478d4ffUL, False)",
    "XmBreadcrumbBarSetSegments(example, segments, 3)",
    "XmAutoSuggestBoxSetSuggestions(example, suggestions, 3, 1)",
    "XmToastShow(example)",
    'XmNindicatorOn, True',
    '"Kitchen lights"',
    'XmNset, False',
):
    assert fragment in examples, fragment
assert "gallery_create_segmented_control_example" in gallery
assert 'entry->class_symbol, "xmSegmentedControlWidgetClass"' in gallery
assert 'blocks["segmented-control-construction"]' in generator
assert 'XmNorientation, XmHORIZONTAL' in gallery
assert 'XmNsidebarExpandedWidth, 280' in gallery
assert 'center_shell_frame(shell)' in gallery
assert 'XQueryTree(display, XtWindow(shell)' in gallery
assert 'XMoveWindow(display, parent' in gallery
assert 'select_page(state, 2)' in gallery
assert 'component_name(entry' in gallery
assert 'create_welcome(&state)' in gallery
assert 'search_changed' in gallery
assert 'XmPACK_NONE' in gallery
assert 'Activate example' not in gallery
print("PASS gallery uses interactive data-backed examples and native content layout")
