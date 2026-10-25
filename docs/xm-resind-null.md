# Resource unit conversion null-safety

Unit conversion APIs now reject null widgets, screens, and conversion values
before Xt/display access. Numeric conversion functions return zero for invalid
inputs; Xme import hooks return `XmSYNTHETIC_LOAD` and export hooks leave the
value untouched. Valid unit conversions are unchanged.
