---
nav_exclude: true
---
# TabBox mode resource conversion

`XmTabBox` converts the `XmNtabMode` string resource through an Xt type
converter. The converter keeps its result in static conversion storage, so each
recognized spelling must assign a result. The `BASIC` and `XmTABS_BASIC`
spellings now explicitly reset that result to `XmTABS_BASIC`; a previous
conversion of `STACKED`, `STACKED_STATIC`, or `SCROLLED` can no longer leak into
a later basic-mode resource conversion.

The focused regression check is
`tests/automated/check-tabbox-mode-converter.py`.
