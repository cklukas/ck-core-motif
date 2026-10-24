# Mrm no-clobber close handling

The no-clobber write-open path now checks the close result for the existing
file descriptor before returning `MrmEXISTS`. A close failure returns
`MrmFAILURE`, avoiding a leaked descriptor and an inaccurate status.
