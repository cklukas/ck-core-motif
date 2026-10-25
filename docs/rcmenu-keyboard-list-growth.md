---
nav_exclude: true
---
# RCMenu keyboard-list growth

Row-column menus keep keyboard accelerator and mnemonic entries in a compact
list whose capacity is stored as a signed `short` for ABI compatibility.
Adding an entry grows that list in ten-entry increments.

Growth now validates the signed capacity and byte count before calling Xt, and
publishes the new pointer and capacity only after the allocation succeeds. A
failed or unrepresentable growth leaves all existing entries intact and skips
the new entry safely.
