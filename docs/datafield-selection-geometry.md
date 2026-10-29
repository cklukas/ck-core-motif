# DataField selection and geometry guards

DataField selection, coordinate conversion, scrolling, and highlight entry
points now reject NULL widgets before Xt application-context lookup.
`XmDataFieldPosToXY` also rejects NULL output coordinates before writing them.
Valid selection ranges, coordinate conversion, scrolling, and rendering remain
unchanged.

The focused source contract is checked by
`tests/automated/check-datafield-selection-geometry.py`.
