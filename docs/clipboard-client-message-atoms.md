---
nav_exclude: true
---

# Clipboard client-message Atom payloads

`ClipboardSendMessage` now rejects Atom IDs that cannot fit the signed
`long` payload slots of `XClientMessageEvent` and rejects unknown message
types instead of sending an uninitialized payload.

Regression: `tests/automated/check-clipboard-client-message-atoms.py`
