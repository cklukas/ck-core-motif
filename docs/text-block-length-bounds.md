# Text block length bounds

Text source and preedit replacement callbacks expose block lengths as signed
`int` values, while their byte strings are measured by `strlen()` as `size_t`.
The affected initialization and replacement paths now reject lengths above
`INT_MAX` before narrowing them.

Normal text and preedit updates retain their existing cleanup and replacement
behavior.
