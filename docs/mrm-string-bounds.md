---
nav_exclude: true
---
# Mrm literal-index string bounds

Mrm value updates now retain literal-index lengths in `size_t` and check the
resource-descriptor addition before allocation. Long or unrepresentable literal
names return `MrmFAILURE` without narrowing the length; ordinary UID value
updates are unchanged.

The source-level regression is `tests/automated/check-mrm-string-bounds.py`.
