import os
import shutil
from os import path

ENGINE_DATA = "engine_data"
ENGINE_DATA_FILTERED = "engine_data-filtered"
DATA = "data"
DATA_FILTERED = "data-filtered"
SUFFIX = ".dat"
FILES_TO_KEEP = (
    "GOODDOGP.TXT",
    "GOODDP__.TTF",
    "LICENSE.txt",
    "OpenSans-Regular.ttf",
    "OpenSans-Semibold.ttf",
    "ninemorris3d.png"
)


def copy_tree(source: str, destination: str):
    for item in os.listdir(source):
        src = path.join(source, item)
        dest = path.join(destination, item)

        if os.path.isdir(src):
            shutil.copytree(src, dest, dirs_exist_ok=True)
        else:
            shutil.copy2(src, dest)


def visit_directory(directory: str):
    all_items = os.listdir(directory)
    os.chdir(directory)
    current_directory = os.getcwd()

    for item in all_items:
        item_full_path = path.join(current_directory, item)

        if path.isfile(item_full_path):
            if item not in FILES_TO_KEEP and path.splitext(item)[1] != SUFFIX:
                os.remove(item_full_path)
                print(f"Removed {item_full_path}")
        elif path.isdir(item_full_path):
            visit_directory(item_full_path)

    os.chdir("..")


os.chdir("..")

print("Copying...")
copy_tree(DATA, DATA_FILTERED)
copy_tree(ENGINE_DATA, ENGINE_DATA_FILTERED)

print("Filtering...")
visit_directory(DATA_FILTERED)
visit_directory(ENGINE_DATA_FILTERED)

print("Done")
