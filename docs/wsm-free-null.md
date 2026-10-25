---
nav_exclude: true
---

# WSM cleanup null-safety

`FreeRequest` and `FreeReply` now accept null handles and tolerate partially
initialized count/pointer pairs during failure cleanup. Nested WSM arrays are
checked before traversal, so allocation failures and malformed caller-owned
structures do not turn cleanup into a second crash.
