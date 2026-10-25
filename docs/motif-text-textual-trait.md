# Text textual trait validation

The public Text string helpers now check the access-textual trait callback
before invoking it for TextField widgets. A partial trait safely leaves a get
result empty or skips a set operation; normal Text and complete TextField
traits retain their existing behavior and ABI.

The source-level regression check is
`tests/automated/check-text-textual-trait.py`.
