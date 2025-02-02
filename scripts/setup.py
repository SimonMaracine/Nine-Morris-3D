#! /usr/bin/env python3

import sys

import _common as comm


def print_help():
    print("Usage: setup.py [rel | dist]", file=sys.stderr)


def main(args: list[str]) -> int:
    build_type = "Debug"
    distribution_mode = "OFF"
    asan = "OFF"
    build_directory = "build"

    try:
        match args[1]:
            case "rel":
                build_type = "Release"
            case "dist":
                build_type = "Release"
                distribution_mode = "ON"

                if sys.platform == "linux":
                    build_directory = "build_dist"
            case invalid:
                print(f"Invalid argument: {invalid}", file=sys.stderr)
                print_help()
                return 1
    except IndexError:
        pass

    cmake_command = comm.SubprocessCommand(
        [
            "cmake",
            "..",
            f"-DCMAKE_BUILD_TYPE={build_type}",
            f"-DNM3D_DISTRIBUTION_MODE={distribution_mode}",
            f"-DNM3D_ASAN={asan}"
        ],
        build_directory
    )

    if sys.platform == "win32":
        cmake_command.append("-A")
        cmake_command.append("x64")

    return comm.execute((
        comm.CdCommand(".."),
        comm.MakeDirsCommand(build_directory, True),
        cmake_command
    ))


if __name__ == "__main__":
    sys.exit(main(sys.argv))
