# Mrm indexed-resource keys

Mrm indexed-resource construction now rejects a null index with the existing
`MrmNULL_INDEX` result before calling `strlen`. Valid non-empty index strings
retain their existing descriptor behavior.

The source-level regression is `tests/automated/check-mrm-resource-index.py`.
