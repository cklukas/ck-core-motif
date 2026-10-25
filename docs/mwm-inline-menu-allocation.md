---
nav_exclude: true
---

# MWM inline-menu allocation widths

When MWM expands an inline menu command, it first asks `snprintf` for the
formatted length and then allocates the command and function-argument strings.
Those lengths are returned as signed `int` values. The allocation now accepts
only non-negative values that fit below `UINT_MAX`, and converts them to the
Xt allocation width explicitly. A failed or unrepresentable formatting result
follows the existing “skip this match” path instead of wrapping into a small
allocation.

This preserves the existing menu syntax and matching behavior while making the
allocation boundary deterministic on both 32-bit and 64-bit builds.
