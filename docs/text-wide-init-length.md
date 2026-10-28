# Text wide-value initialization bounds

The Text widget’s wide-value initialization path counts characters with a
legacy signed counter before converting the value to the locale encoding.  The
counter now stops at `INT_MAX`, allowing the existing allocation guard to reject
an unrepresentable value before indexing or sizing can wrap.

Normal wide-value initialization is unchanged.
