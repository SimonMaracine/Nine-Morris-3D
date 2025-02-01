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

    commands = (
        comm.CdCommand(".."),
        comm.SubprocessCommand(
            ["cargo", "build", "--release"] if build_type == "release" else ["cargo", "build"],
            "nine_morris_3d/extern/muhle_intelligence/"
        ),
        comm.CpCommand(
            f"nine_morris_3d/extern/muhle_intelligence//target/{build_type}/muhle_intelligence",
            "nine_morris_3d_engine_muhle_intelligence"
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
