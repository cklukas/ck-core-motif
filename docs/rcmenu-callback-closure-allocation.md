---
nav_exclude: true
---
# RCMenu callback-closure snapshot allocation

Entry activation snapshots callback closures before invoking application
callbacks, because later Xt calls can invalidate the callback list. The
snapshot allocation is now checked before copying closures or dispatching
them. Both host-size arithmetic and Xt's `Cardinal` allocation width are
validated; allocation failure leaves the menu state intact and returns safely.
