#! /usr/bin/python3

import os
import sys
import shutil
import dataclasses


class AssetsDirectoryProcessError(RuntimeError):
    pass


@dataclasses.dataclass(frozen=True)
class ManifestFile:
    assets_paths: list[str]


def process_assets_directory(assets_directory_path: str, output_path: str):
    manifest_file = os.path.join(assets_directory_path, "manifest")

    if not os.path.isfile(manifest_file):
        raise AssetsDirectoryProcessError(f"`{manifest_file}` is not a file")

    manifest = read_manifest_file(manifest_file)

    create_new_assets_directory(assets_directory_path, output_path, manifest)


def read_manifest_file(file_path: str) -> ManifestFile:
    with open(file_path, "r") as file:
        lines = file.readlines()

    lines = map(lambda file_path: file_path.strip(), lines)
    lines = filter(lambda file_path: file_path, lines)

    return ManifestFile(list(lines))


def strip_trailing_slash(file_path: str) -> str:
    if file_path.endswith("/"):
        return file_path[:-1]


def create_new_assets_directory(assets_directory_path: str, output_path: str, manifest: ManifestFile):
    new_directory = os.path.join(output_path, os.path.basename(strip_trailing_slash(assets_directory_path)))
    try:
        os.mkdir(new_directory)
        print(f"Created new assets directory: {new_directory}")
    except FileExistsError:
        pass

    for file_path in manifest.assets_paths:
        os.makedirs(os.path.join(new_directory, os.path.dirname(file_path)), exist_ok=True)
        shutil.copyfile(
            os.path.join(assets_directory_path, file_path),
            os.path.join(new_directory, file_path)
        )
        print(f"Copied: {file_path}")


def print_help():
    print("Usage: export_assets.py <output_directory> <assets_directories...>", file=sys.stderr)


def main(args: list[str]) -> int:
    if len(args) < 3:
        print("Error: no assets directory", file=sys.stderr)
        print_help()
        return 1

    output = args[1]
    print(f"Output: {output}")

    directories = args[2:]
    print(f"Processing directories: {directories}")

    if not os.path.isdir(output):
        print(f"Error: `{output}` is not a directory", file=sys.stderr)
        print_help()
        return 1

    exit_code = 0

    for directory in directories:
        if not os.path.isdir(directory):
            print(f"Error: `{directory}` is not a directory", file=sys.stderr)
            exit_code = 1
            continue

        try:
            process_assets_directory(directory, output)
        except AssetsDirectoryProcessError as err:
            print(f"Error: {err}", file=sys.stderr)
            exit_code = 1

    return exit_code


if __name__ == "__main__":
    sys.exit(main(sys.argv))
