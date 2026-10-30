---
nav_exclude: true
---

# TextField string and context callback guards

The TextField synthetic-resource callbacks now validate their inputs before
calling public string getters. Narrow and wide exports return a deterministic
null value when the callback receives a missing widget or output slot instead
of dereferencing invalid state.

The display-destroy cleanup callback also validates its context record and
screen before asking X11 to inspect or delete a context. A screenless record is
freed directly, which keeps teardown safe for partially initialized display
objects while preserving the normal X11 cleanup path.

The focused source-contract check is
`python3 tests/automated/check-textfield-string-getter-guards.py`.
