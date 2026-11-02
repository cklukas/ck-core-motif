---
nav_exclude: true
---

# MWM accelerator display-text bounds

MWM derives a visible accelerator label from parsed key syntax. Key-name
substitutions can be longer than the text they replace, while malformed
multibyte input can cause `mblen` to report an error. The formatter now
receives the exact destination capacity, checks every append, and stops at the
already-parsed accelerator boundary. It rejects malformed multibyte event
names and missing delimiters instead of stepping backward or reading past the
specification.

Valid accelerator syntax and configured substitutions retain their existing
output. When a substitution is longer than its source spelling, the Xt-backed
display buffer grows after checked arithmetic; an allocation failure leaves no
partial accelerator label published.

The focused regression check is
`tests/automated/check-mwm-accelerator-text-bounds.py`.
`tests/automated/run-mwm-accelerator-text-bounds.py` also compiles the real
formatter into a small harness that covers a growing substitution and malformed
input.
