---
nav_exclude: true
---

# WSM window-list rollback

When decoding a `WSM_WM_WINDOWS` request, window properties are allocated
before match-attribute arrays.  If either the outer match-attribute array or a
nested attribute pair cannot be allocated, the decoder now releases the
already-built window-property list and clears its bookkeeping.  This prevents
memory leaks during allocation pressure while preserving the existing failure
return behavior.
