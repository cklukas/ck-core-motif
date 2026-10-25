# PanedWindow pane-stack allocation handling

PanedWindow’s private pane stack now checks its node allocation before linking
and initializing the node.  Allocation failure leaves the existing stack
unchanged and returns from the optional stack push safely.

The regression check is `tests/automated/check-paned-stack-allocation.py`.
