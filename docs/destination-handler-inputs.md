---
nav_exclude: true
---

# Destination handler inputs

`_XmDestinationHandler` is the common entry point for regular selection and
drop destination transfers.  It now rejects a null destination widget or a
`None` selection atom before touching the widget display or class.  This
keeps malformed or teardown-time external-owner callbacks from entering the
transfer state machine with no valid destination.

The regression check is
`tests/automated/check-destination-handler-inputs.py`.
