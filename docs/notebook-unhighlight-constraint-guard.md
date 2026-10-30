---
nav_exclude: true
---
# Notebook unhighlight constraint guard

The Notebook unhighlight callback now requires both a non-null child and
constraint storage before selecting the appropriate background GC. Malformed
child state safely returns no GC instead of dereferencing an absent Notebook
constraint; normal focus and tab painting are unchanged.

The source contract is checked by
`tests/automated/check-notebook-unhighlight-constraint-guard.py`.
