---
nav_exclude: true
---

# ProgressRing render bounds

ProgressRing keeps the ring center and circular text-opening calculations in
signed working coordinates.  It only converts text origins through the
existing bounded `Position` helper, and squares an explicitly positive opening
width when checking whether the label fits.
