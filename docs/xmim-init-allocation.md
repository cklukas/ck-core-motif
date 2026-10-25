---
nav_exclude: true
---
# Input-method display initialization allocations

Display input-method state and locale-modifier scratch storage are now checked
before use. Allocation failure leaves the display state uninitialized or skips
the optional modifier update without dereferencing a null buffer.
