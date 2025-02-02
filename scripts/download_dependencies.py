#! /usr/bin/env python3

import sys

import _common as comm


def main(args: list[str]) -> int:
    submodules = (
        "tools/cmake",
        "libs/algorithm",
        "libs/align",
        "libs/array",
        "libs/asio",
        "libs/assert",
        "libs/atomic",
        "libs/bind",
        "libs/concept_check",
        "libs/config",
        "libs/container",
        "libs/container_hash",
        "libs/context",
        "libs/conversion",
        "libs/core",
        "libs/date_time",
        "libs/describe",
        "libs/detail",
        "libs/endian",
        "libs/exception",
        "libs/filesystem",  # TODO avoid
        "libs/function",
        "libs/function_types",
        "libs/functional",
        "libs/fusion",
        "libs/integer",
        "libs/intrusive",
        "libs/io",
        "libs/iterator",
        "libs/lexical_cast",
        "libs/move",
        "libs/mp11",
        "libs/mpl",
        "libs/numeric",
        "libs/optional",
        "libs/pool",
        "libs/predef",
        "libs/preprocessor",
        "libs/process",
        "libs/range",
        "libs/regex",
        "libs/scope",
        "libs/smart_ptr",
        "libs/static_assert",
        "libs/system",
        "libs/throw_exception",
        "libs/tokenizer",
        "libs/tuple",
        "libs/type_index",
        "libs/type_traits",
        "libs/typeof",
        "libs/unordered",
        "libs/utility",
        "libs/variant2",
        "libs/winapi"
    )

    return comm.execute((
        comm.CdCommand(".."),
        comm.SubprocessCommand(["git", "submodule", "update", "--init"]),
        comm.SubprocessCommand(["git", "submodule", "update", "--init", *submodules], "extern/boost")
    ))


if __name__ == "__main__":
    sys.exit(main(sys.argv))
