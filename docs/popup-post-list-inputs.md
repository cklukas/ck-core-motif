---
nav_exclude: true
---
# Popup post-list input guards

`XmAddToPostFromList` and `XmRemoveFromPostFromList` now return immediately
for a null menu widget, before casting or application-context lookup. Valid
popup and pulldown menus retain their existing post-from-list behavior.

The focused regression is
`tests/automated/check-popup-post-list-inputs.py`.
