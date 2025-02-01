#! /usr/bin/env python3

import sys
import subprocess

import _common as comm


def print_help():
    print("Usage: build_engines.py [dist]", file=sys.stderr)


def main(args: list[str]) -> int:
    build_type = "debug"

    try:
        match args[1]:
            case "dist":
                build_type = "release"
            case invalid:
                print(f"Invalid argument: {invalid}", file=sys.stderr)
                print_help()
                return 1
    except IndexError:
        pass

    build_command = ["cargo", "build"]

    if build_type == "release":
        build_command.append("--release")

    if sys.platform == "win32":
        build_command.append("--features")
        build_command.append("muhle_intelligence_windows")

    exe_suffix = ".exe" if sys.platform == "win32" else ""

    commands = (
        comm.CdCommand(".."),
        comm.SubprocessCommand(
            build_command,
            "nine_morris_3d/extern/muhle_intelligence/"
        ),
        comm.CpCommand(
            f"nine_morris_3d/extern/muhle_intelligence/target/{build_type}/muhle_intelligence{exe_suffix}",
            f"nine_morris_3d_engine_muhle_intelligence{exe_suffix}"
        )
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
