---
nav_exclude: true
---
# I18List row conversion bounds

`XmI18List` converts pixel coordinates into row indices during hit testing.
The computed division result and the current first-row offset can exceed the
legacy signed `short` row representation when geometry or scrolling state is
extreme.

Both conversion paths now use the existing bounded-short helper, preserving
normal row selection while preventing wrapped row indices.

The focused source regression is
`tests/automated/check-i18-list-row-conversion-bounds.py`.
