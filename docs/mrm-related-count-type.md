# Mrm related-count type validation

Related argument codes are count resources for vector-valued arguments only.
The Mrm argument builder now skips a related code for other representation
types instead of fabricating a count argument from unrelated data. Valid
character, compound-string, and integer vector counts retain their existing
behavior.

The source-level regression check is
`tests/automated/check-mrm-related-count-type.py`.
