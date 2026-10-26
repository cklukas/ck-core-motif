# Text RTL autoscroll axis

The RTL left-edge branch of `CheckTimerScrolling` now updates the horizontal
selection position (`select_pos_x`). It previously wrote the computed edge
coordinate into `select_pos_y`, leaving x stale and perturbing vertical
selection movement while autoscrolling left.

The focused source contract is checked by
`tests/automated/check-text-rtl-autoscroll-axis.py`.
