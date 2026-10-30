---
nav_exclude: true
---
# I18List widget-class guards

The exported I18List selection convenience APIs now validate the
`xmI18ListWidgetClass` before reading row data or toggling selection. Null and
unrelated widgets return safely; valid MultiList-backed I18List behavior is
unchanged.

The source contract is checked by
`tests/automated/check-i18list-widget-class-guards.py`.
