---
nav_exclude: true
---
# I18List null-widget guards

The public I18List search and selection helpers now return safely when passed
a null widget: `XmI18ListFindRow`, `Xm18IListUnselectAllItems`,
`Xm18IListUnselectItem`, and `XmI18ListToggleRow`. The guard precedes the
internal list-record access, preventing malformed optional calls from
crashing the process while valid list behavior is unchanged.

The focused regression is
`tests/automated/check-i18list-widget-inputs.py`.
