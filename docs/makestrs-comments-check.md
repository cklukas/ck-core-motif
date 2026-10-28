---
nav_exclude: true
---

# `makestrs` comment check invocation

`tests/automated/check-makestrs-comments.py` needs the built `makestrs`
executable and a new absolute results directory, because it runs the binary
against several input cases and records a receipt. When invoked without those
arguments as part of the repository-wide source-contract sweep, it now prints
a `SKIP` result and exits successfully; an explicit invocation still requires
both paths and performs the full check.
