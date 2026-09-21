Smol DI is an experimental dependency injection library for C++ using C++26 reflection.

## Build and test

This project uses Bazel with Bzlmod and GoogleTest. Its reflection prototype
currently requires Homebrew GCC 16 on macOS.

Build and run the example:

```sh
bazel run --config=reflection-gcc //:smol_di_demo
```

Run the tests:

```sh
bazel test --config=reflection-gcc //...
```
