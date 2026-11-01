---
nav_exclude: true
---
# DropDown selected-text allocation bounds

When a multi-select `XmDropDown` copies selected list entries into its text
field, it allocates both a temporary entry array and a comma-separated result.
Those allocations are passed through Xt's `Cardinal`-sized interface. The
selection conversion now rejects counts and joined byte sizes that cannot be
represented by that interface before allocating or changing the text field.

Normal selection conversion, separator formatting, and all-or-nothing text
replacement are unchanged. The focused regression is
`tests/automated/check-dropdown-selection-inputs.py`.
