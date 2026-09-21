load("@rules_cc//cc:cc_toolchain_config_lib.bzl", "tool_path")
load("@rules_cc//cc/common:cc_common.bzl", "cc_common")
load("@rules_cc//cc/toolchains:cc_toolchain_config_info.bzl", "CcToolchainConfigInfo")

_GCC_ROOT = "/opt/homebrew/Cellar/gcc/16.2.0"

def _impl(ctx):
    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = "homebrew-gcc-16",
        host_system_name = "local",
        target_system_name = "local",
        target_cpu = "aarch64",
        target_libc = "macos",
        compiler = "gcc",
        abi_version = "gcc-16",
        abi_libc_version = "gcc-16",
        cxx_builtin_include_directories = [
            _GCC_ROOT + "/include/c++/16",
            _GCC_ROOT + "/include/c++/16/aarch64-apple-darwin24",
            _GCC_ROOT + "/include/c++/16/backward",
            _GCC_ROOT + "/lib/gcc/current/gcc/aarch64-apple-darwin24/16/include",
            _GCC_ROOT + "/lib/gcc/current/gcc/aarch64-apple-darwin24/16/include-fixed",
            "/Library/Developer/CommandLineTools/SDKs/MacOSX15.sdk/usr/include",
            "/Library/Developer/CommandLineTools/SDKs/MacOSX15.sdk/System/Library/Frameworks",
        ],
        tool_paths = [
            tool_path(name = "ar", path = "/opt/homebrew/opt/llvm/bin/llvm-ar"),
            tool_path(name = "cpp", path = "/opt/homebrew/bin/g++-16"),
            tool_path(name = "gcc", path = "/opt/homebrew/bin/g++-16"),
            tool_path(name = "gcov", path = "/opt/homebrew/bin/gcov-16"),
            tool_path(name = "ld", path = "/opt/homebrew/bin/g++-16"),
            tool_path(name = "nm", path = "/opt/homebrew/bin/gcc-nm-16"),
            tool_path(name = "objcopy", path = "/usr/bin/objcopy"),
            tool_path(name = "objdump", path = "/usr/bin/objdump"),
            tool_path(name = "strip", path = "/usr/bin/strip"),
        ],
    )

gcc16_toolchain_config = rule(
    implementation = _impl,
    provides = [CcToolchainConfigInfo],
)
