# IconG right-to-left render placement

IconG's `Redisplay` routine now computes right-to-left large and small icon
origins in a signed temporary before converting them to `Position`.  This
keeps clipped and unclipped icons at their intended off-edge coordinates when
a gadget is narrower than its icon, instead of allowing unsigned arithmetic to
wrap the draw origin to the opposite side of the window.

The source-level regression is `tests/automated/check-icong-rtl-render.py`.
