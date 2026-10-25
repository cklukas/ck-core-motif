# Mrm auto-child class-string bounds

The auto-child (`UrmSetWidgetInstance`) path now applies the same bounded
class-string validation as ordinary widget creation.  A nonzero class offset
must point inside the widget record and terminate before the record ends,
preventing later resource conversion from scanning malformed UID data.

The source-level regression is
`tests/automated/check-mrm-set-widget-class-bounds.py`.
