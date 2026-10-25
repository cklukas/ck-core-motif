---
nav_exclude: true
---
# Mrm widget-class lookup validation

Mrm widget-class lookup now rejects null class names and null output pointers
before entering the hash table. The class-record lookup also treats a null
name as absent, and its not-found diagnostic uses bounded formatting. Valid
class registration and lookup behavior is unchanged.

The source-level regression is
`tests/automated/check-mrm-wci-lookup-input.py`.
