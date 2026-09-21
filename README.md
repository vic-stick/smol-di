Smol DI is an experimental dependency injection library for C++ using C++26 reflection.

## Build and test

This project uses Bazel with Bzlmod and GoogleTest. Its reflection prototype
currently requires Homebrew GCC 16 on macOS.

Run the test suite:

```sh
bazel test --config=reflection-gcc //...
```

The test suite separates binding lookup, constructor dependency reflection,
binding-aware graph traversal, runtime injection, and singleton lifetime.

Compile-failure fixtures for missing and circular dependencies live under
`tests/compile_fail/`; they are kept separate because Bazel's native shell
test rule was removed in Bazel 9 and will be wired through a dedicated test
rule once that dependency is introduced.
