# Legacy menu-accelerator copy bounds

The compatibility row-column getter returns an owned copy of the menu
accelerator.  It now checks the string length before allocating terminator
space and narrowing to Xt's allocation type; an unrepresentable value returns
null without wrapping the allocation.

The focused source contract is checked by
`tests/automated/check-legacy-menu-accelerator-bounds.py`.
