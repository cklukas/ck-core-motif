---
nav_exclude: true
---
# Input-method preedit allocation failures

Preedit replacement and feedback buffers are now checked as a pair before
copying or replacing the active buffer. The optional callback conversion buffer
is also checked; if it cannot be allocated, the callback keeps the original
counted `XIMText` view instead of copying through `NULL`.
