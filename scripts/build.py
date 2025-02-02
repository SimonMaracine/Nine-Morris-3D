#! /usr/bin/env python3

import sys

import _common as comm


def print_help():
    print("Usage: build.py [dev | dist] [target]", file=sys.stderr)


def main(args: list[str]) -> int:
    config = "Debug"
    build_directory = "build"
    target = "all"

    try:
        match args[1]:
            case "dev":
                pass
            case "dist":
                config = "Release"

                if sys.platform == "linux":
                    build_directory = "build_dist"
            case invalid:
                print(f"Invalid argument: {invalid}", file=sys.stderr)
                print_help()
                return 1

        target = args[2]
    except IndexError:
        pass

    command = comm.SubprocessCommand(
        ["cmake", "--build", ".", "--target", target, "-j", "10"],
        f"../{build_directory}"
    )

    if sys.platform == "win32":
        command.append("--config")
        command.append(config)

    return comm.execute([command])


if __name__ == "__main__":
    sys.exit(main(sys.argv))
