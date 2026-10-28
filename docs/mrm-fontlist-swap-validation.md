---
nav_exclude: true
---

# Mrm font-list swap validation

Current-format font-list fixup now validates the `URMFontListValid` marker and
the signed count after cross-endian byte swapping, before walking font entries.
Malformed swapped records are rejected without iteration; valid font lists keep
their existing relocation behavior.
