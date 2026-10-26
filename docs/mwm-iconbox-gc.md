# MWM icon-box GC source validation

The icon-box highlight and fade graphics contexts are cloned from the
screen's inactive icon appearance GC. If appearance-resource initialization
fails, that source GC can be unavailable even though the icon-box screen
record exists. `MakeShrinkWrapIconsGC` and `MakeFadeIconGC` now return before
calling `XCopyGC` in that state, avoiding a `BadGC` request while leaving the
icon-box usable without the optional decoration.
