---
nav_exclude: true
---

# DataField class binding allocation

`df_ClassPartInitialize` now checks the temporary translation-table buffer
before assembling the class bindings. If allocation fails, class initialization
returns without dereferencing a null buffer; the inherited translation table
remains intact.

The focused source contract is checked by
`tests/automated/check-datafield-class-bindings.py`.
