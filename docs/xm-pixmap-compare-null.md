# Xm pixmap comparator null safety

`ComparePixmapDatas` now rejects null records or missing color metadata before
comparing cache keys. Valid pixmap entries retain the existing matching rules,
while malformed partial state cannot trigger a null dereference.

The source-level regression is `tests/automated/check-xm-pixmap-compare-null.py`.
