---
nav_exclude: true
---
# Prompt resource and GC safety

`XmPrompt` now skips drawing when its private `GC` cannot be created, avoiding
a null-GC Xlib request. Prompt text replacements release the previous owned
`XmString` only after a replacement succeeds, retaining the old value if
copying fails. Successful rendering and resource updates are unchanged.

`tests/automated/check-prompt-resource-safety.py` protects these paths.
