#! /usr/bin/python3

import sys
import os
import subprocess


def print_help():
    print("Usage: setup.py [rel | dist]", file=sys.stderr)


def main(args: list[str]) -> int:
    build_type="Debug"
    distribution_mode="OFF"
    asan="OFF"
    build_directory="build"

    try:
        match args[1]:
            case "rel":
                build_type="Release"
            case "dist":
                build_type="Release"
                distribution_mode="ON"

                if sys.platform == "linux":
                    build_directory="build_dist"
            case invalid:
                print(f"Invalid argument: {invalid}", file=sys.stderr)
                print_help()
                return 1
    except IndexError:
        pass

    cmake_command = [
        "cmake",
        "..",
        f"-DCMAKE_BUILD_TYPE={build_type}",
        f"-DNM3D_DISTRIBUTION_MODE={distribution_mode}",
        f"-DNM3D_ASAN={asan}"
    ]

    if sys.platform == "win32":
        cmake_command.append("-A")
        cmake_command.append("x64")

    try:
        os.chdir("..")
        os.makedirs(build_directory, exist_ok=True)
        os.chdir(build_directory)
        subprocess.run(cmake_command).check_returncode()
    except subprocess.CalledProcessError as err:
        print(f"An error occurred: {err}", file=sys.stderr)
        return 1
    except Exception as err:
        print(f"An unexpected error occurred: {err}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
