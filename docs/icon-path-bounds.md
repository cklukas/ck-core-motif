# Icon path bounds

Icon lookup no longer copies the arbitrary-length home directory into its fixed
`MAX_DIR_PATH_LEN` scratch buffer. The path helper fills that buffer itself when
an absolute path is needed, so the old copy was both unnecessary and an overflow
for unusually long `HOME` values. The source contract is covered by
`tests/automated/check-icon-path-bounds.py`.
