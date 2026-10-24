# SegmentedControl

`XmSegmentedControl` is an opt-in mutually-exclusive picker.  It is a
composite whose segments are ordinary public `XmToggleButton` children; the
control only coordinates their selection and applies the shared segment
treatment.  Existing toggle buttons and radio groups are unchanged.

Applications replace the item model with `XmSegmentedControlSetItems`.  The
control copies each item ID and label, so callers may release their input
after the call.  `XmSegmentedControlGetItemId` returns a borrowed ID that is
valid until the next item replacement or widget destruction.  `Pixmap` is
optional (`XmUNSPECIFIED_PIXMAP` means label-only); icons are rendered with
the normal Motif label/pixmap rules.

Selection is a single item index or `-1` for no selection.  Disabled items are
not selectable.  `XmNSegmentedAllowClear` controls whether activating the
selected item clears it; programmatic selection uses the same validation.
`XmNSegmentedSelectionCallback` reports the new index and borrowed ID after
the child toggle states have been synchronized.  Callback code may destroy
the control; the implementation performs no further work after the callback.

The control is a single horizontal row.  `XmNSegmentedRtl` reverses visual
order and Left/Right keyboard movement.  `XmNSegmentedSpacing` sets the gap
between segments.  When the preferred widths do not fit, the bounded
`XmSEGMENTED_OVERFLOW_CLIP` policy gives each segment a deterministic share
of the available width; labels clip according to the child ToggleButton
renderer.  Wrapping, scrolling, and implicit item menus are intentionally
outside this contract.

All children remain ordinary ToggleButtons for focus, sensitivity, and
accessibility.  The parent reports a list role with the selected index, while
each child retains its native button semantics.  Keyboard operation includes
Left/Right (direction-aware), Up/Down, Home, and End; disabled segments are
skipped.
