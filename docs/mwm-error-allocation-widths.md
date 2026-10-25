---
nav_exclude: true
---

# MWM diagnostic allocation widths

MWM’s X I/O and workspace warning handlers format diagnostic strings with
`snprintf`. The returned length is signed, while Xt allocation sizes use an
unsigned cardinal width. Both handlers now reject negative or unrepresentable
lengths before allocating and use the Xt-sized conversion only after that
check. Formatting failures continue to use the existing fallback diagnostic.

The change affects only allocation failure behavior; diagnostic text and the
normal warning paths remain unchanged.
