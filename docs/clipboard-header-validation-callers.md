---
nav_exclude: true
---

# Clipboard header validation in copy/message paths

Clipboard data-request messaging and copy-by-name operations now use the same
header validator as `ClipboardOpen`. They reject malformed data-item list
offsets or counts before touching the header, while valid legacy clipboard
headers continue through the existing paths.

The focused regression check is
`tests/automated/check-clipboard-header-validation-callers.py`.
