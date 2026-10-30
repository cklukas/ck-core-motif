---
nav_exclude: true
---
# MultiList widget-class guards

All public MultiList selection, row-query, and visibility APIs now validate
that their widget is an `xmMultiListWidgetClass` instance before inspecting
the embedded internal List child. Invalid or null widgets return safely,
while valid MultiList behavior is unchanged.

The source contract is checked by
`tests/automated/check-multilist-widget-class-guards.py`.
