---
nav_exclude: true
---

# Atom-list converter input validation

The atom-list converter now rejects missing displays, source values, or
destinations before tokenization and `XInternAtoms`. Valid atom-list parsing
and output-size negotiation remain unchanged.

The focused regression check is
`tests/automated/check-atom-list-converter-inputs.py`.
