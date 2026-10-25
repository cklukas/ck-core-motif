---
nav_exclude: true
---
# Mrm validation macro null safety

The public Mrm validation macros for hierarchies, class descriptors, widget
records, and interface modules now return false for null pointers before
reading validation fields. Existing valid and invalid non-null records retain
their previous results, while callers avoid null-pointer crashes.
