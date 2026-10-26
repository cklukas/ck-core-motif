---
nav_exclude: true
---
# Locale-text property formats

STRING, COMPOUND_TEXT, and UTF8_STRING selection values are byte-oriented X
text properties.  `_XmTextToLocaleText` now rejects 16- and 32-bit format
values before passing them to `XmbTextPropertyToTextList`, preventing malformed
selection data from being interpreted with the wrong element width.

The focused regression is `tests/automated/check-text-locale-format.py`.
