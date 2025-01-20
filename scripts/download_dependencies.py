#! /usr/bin/env python3

import sys
import subprocess

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

    commands = (
        comm.CdCommand(".."),
        comm.SubprocessCommand(["git", "submodule", "update", "--init"]),
        comm.SubprocessCommand(["git", "submodule", "update", "--init", *submodules], "extern/boost")
    )

    try:
        for command in commands:
            print(command)
            command.execute()
    except subprocess.CalledProcessError as err:
        print(f"An error occurred: {err}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        print()
        return 1
    except Exception as err:
        print(f"An unexpected error occurred: {err}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
