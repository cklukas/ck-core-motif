# I18List visible-window bounds

`MakePositionVisible` now computes the visible-window end once in
`long long` arithmetic.  Derived beginning, ending, movement, and remaining
width values are clamped before entering legacy `short` helpers.  This keeps
wide widget dimensions from wrapping while preserving normal-size scrolling
behavior and the existing API.

The source-contract regression is
`tests/automated/check-i18-list-visible-window-bounds.py`.
