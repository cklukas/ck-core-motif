---
nav_exclude: true
---

# Xft draw inputs

`_XmXftDrawCreate` now rejects a null display or `None` drawable before cache
lookup and Xlib queries. This makes teardown and error paths safe while valid
display/drawable pairs retain their existing cache and rendering behavior.
