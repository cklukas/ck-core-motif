---
nav_exclude: true
---
# ButtonBox layout bounds

ButtonBox still follows its established equal-size, fill, orientation, and
right-to-left layout policies.  Its internal major/minor coordinate arithmetic
now remains wide until the final X11 `Position` assignment, where it is
saturated to the representable signed range.  This prevents a large child or
margin calculation from wrapping to the opposite side of the box.

The geometry-request mask is narrowed deliberately to Xt's width and height
bits, and query-only requests are represented as a Boolean rather than an
arbitrary bit-mask value.
