---
nav_exclude: true
---
# MultiList search callback safety

Search activation now validates its widget, callback payload, find controls,
and internal I18List before reading callback fields or requesting text.  A
partial MultiList therefore treats a search request as a no-op instead of
passing missing children to the text or list implementation.

The callback still reports found and not-found results exactly as before for a
fully constructed MultiList.
