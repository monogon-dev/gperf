load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "gperf",
    srcs = glob([
        "src/*.cc",
        "src/*.h",
        "lib/*.cc",
        "lib/*.h",
    ]) + ["config.h"],
    defines = [
        "HAVE_DYNAMIC_ARRAY",
    ],
    visibility = ["//visibility:public"],
)
