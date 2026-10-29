# DataField string input guards

DataField byte-string, wide-string, and substring getters now reject a NULL
widget before Xt application-context lookup. They return the established
failure values (`NULL` for allocated strings and `XmCOPY_FAILED` for bounded
copies) without touching caller storage; valid conversion and truncation
behavior is unchanged.

The focused source contract is checked by
`tests/automated/check-datafield-string-inputs.py`.
