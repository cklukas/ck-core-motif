---
nav_exclude: true
---

# Validate in-memory XPM image arguments

The data- and buffer-backed XPM constructors now reject null display, input,
or calls with no output target before opening the in-memory source or invoking
the parser.  Raw `XpmImage` constructors also validate their image and info
records.  Optional shape-mask-only output remains valid for compatibility.
