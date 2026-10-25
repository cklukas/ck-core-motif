---
nav_exclude: true
---
# XmString component tag-cache failures

`XmStringComponentCreate` now verifies that each charset, locale, and
rendition cache index resolves to the requested tag before constructing the
component. Cache allocation failure no longer aliases the default tag or
indexes a missing locale entry.

The regression check is
`tests/automated/check-xmstring-component-tag-cache.py`.
