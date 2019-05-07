# XPCCI: an XPCOM implementation

XPCCI is a pet project that reimplement almost from scratch the mozilla XPCOM library.

The source code has bee extracted from the venerable https://github.com/dmarteau/daim project and has been proven efficient
on production code.

The project is self contained  has no dependencies on external libraries.

### Building XPCCI

Building XPCCI requires the following:

- Python 3
- Gnu makefile
- The ply python module (using a virtualenv is recommended)

And run the following commands:

```
> ./configure
> make

```

The COM API is the same as the former XPCOM api, see references for details.

## References:

- https://fr.wikipedia.org/wiki/XPCOM
- https://github.com/MoonchildProductions/UXP, for the original XPCOM implementation 


