# Format Testing
Testing to verify `clang-format` configuration does what is expected.

## Formated vs Unformatted
There are two `c++` files in this directory: `unformatted.cc` and `formatted.cc`. These files contain formatting that is highly desired. When making changes to the `.clang-format` configuration, run the `format-test.sh` to ensure these format styles are still preset. Additionally, if adding any desirable formatting, add a test for it in the `c++` files.
