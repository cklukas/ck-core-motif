# Mrm clobber-open close handling

When write mode replaces an existing file, Mrm now checks the close result for
the original descriptor before calling `creat`. A close failure returns
immediately instead of proceeding with an uncertain descriptor state.
