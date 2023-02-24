cc_library(
    name = "weave",
    srcs = [
        "message_write_request.cc",
        "packet.cc",
        "packetizer.cc",
    ],
    hdrs = [
        "message_write_request.h",
        "packet.h",
        "packet_counter_generator.h",
        "packetizer.h",
        "single_packet_write_request.h",
        "write_request.h",
    ],
    deps = [
        "//internal/platform:base",
        "//internal/platform:types",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "packet_test",
    srcs = [
        "packet_test.cc",
    ],
    deps = [
        ":weave",
        "//internal/platform/implementation/g3",  # build_cleaner: keep
        "@com_github_protobuf_matchers//protobuf-matchers",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "packet_counter_generator_test",
    srcs = [
        "packet_counter_generator_test.cc",
    ],
    deps = [
        ":weave",
        "//internal/platform/implementation/g3",  # build_cleaner: keep
        "@com_github_protobuf_matchers//protobuf-matchers",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "packetizer_test",
    srcs = [
        "packetizer_test.cc",
    ],
    deps = [
        ":weave",
        "//internal/platform/implementation/g3",  # build_cleaner: keep
        "@com_github_protobuf_matchers//protobuf-matchers",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "single_packet_write_request_test",
    srcs = [
        "single_packet_write_request_test.cc",
    ],
    deps = [
        ":weave",
        "//internal/platform/implementation/g3",  # build_cleaner: keep
        "@com_github_protobuf_matchers//protobuf-matchers",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "message_write_request_test",
    srcs = [
        "message_write_request_test.cc",
    ],
    deps = [
        ":weave",
        "//internal/platform/implementation/g3",  # build_cleaner: keep
        "@com_github_protobuf_matchers//protobuf-matchers",
        "@com_google_googletest//:gtest_main",
    ],
)
