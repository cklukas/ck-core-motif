---
nav_exclude: true
---

# Validate in-memory XPM image arguments

The data- and buffer-backed XPM constructors now reject null display, input,
or required output arguments before opening the in-memory source or invoking
the parser.  Raw `XpmImage` constructors also validate their image and info
records.  Optional shape-mask output remains optional for compatibility.
