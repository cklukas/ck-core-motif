# Text source teardown guard

Text-source destruction now handles a null source and a partially initialized
source whose private data was never allocated. Partial teardown releases the
source object itself; normal teardown still frees the Unicode map, buffers,
widget list, and private data in the existing order.

The focused source contract is checked by
`tests/automated/check-text-source-destroy-guard.py`.
