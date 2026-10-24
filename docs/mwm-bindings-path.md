# Mwm binding-path bounds

The Linux `mwm` startup path loads `.motifbind` from the user's home directory.
The legacy code copied that directory into a fixed `MAXWMPATH+1` array before
appending the filename, so unusually long home paths could truncate or
overwrite the path buffer.

The Linux path now allocates the complete home-directory-plus-filename string,
formats it with a bounded write, and releases it after loading the bindings.
The property and fallback binding behavior are unchanged.

`tests/automated/check-mwm-bindings-path.py` protects this contract.
