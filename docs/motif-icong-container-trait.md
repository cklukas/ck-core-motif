# IconGadget container-trait validation

IconGadget now validates the resolved container and its `getValues` callback
before trait dispatch. Missing parents or partial traits retain safe defaults
instead of dereferencing null callback pointers.

The regression check is
`tests/automated/check-icong-container-trait.py`.
