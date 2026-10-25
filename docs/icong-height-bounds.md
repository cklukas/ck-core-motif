# IconG vertical render bounds

IconG now computes pixmap copy heights and detail clip heights in signed
arithmetic and clamps negative available space to zero.  Collapsed gadgets
therefore avoid wrapped `Dimension` heights that could request oversized
copies or expose detail text outside the gadget.

The source-level regression is `tests/automated/check-icong-height-bounds.py`.
