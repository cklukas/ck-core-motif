---
nav_exclude: true
---
# Paned right-to-left pane origins

Right-to-left horizontal Paned layout now computes each pane origin from
signed container-width and accumulated-position values before converting to
`Position`.  Overfull or collapsed containers therefore keep an off-edge
origin instead of wrapping to a distant positive coordinate.

The source-level regression is `tests/automated/check-paned-rtl-origin.py`.
