---
nav_exclude: true
---
# Mrm set-values tag-name input validation

Set-values widget and callback references copy an application-provided tag name
when its compressed code is unknown. Both paths now reject a null tag name,
check terminator-inclusive sizing, and handle allocation failure before
copying. Valid tag names retain their existing descriptor contents and list
behavior.

The source-level regression is
`tests/automated/check-mrm-wref-tagname-input.py`.
