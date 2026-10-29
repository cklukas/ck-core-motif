# I18List row-helper input guards

Shared I18List row-selection helpers now validate their widget before reading
row storage or looking up an Xt application context. This covers selected-row
array retrieval and all public select/deselect wrappers while preserving valid
selection and notification behavior.

The focused source contract is checked by
`tests/automated/check-i18list-row-helper-inputs.py`.
