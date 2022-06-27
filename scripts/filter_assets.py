import os
from os import path
from distutils import dir_util

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


def visit_directory(directory: str):
    all_items = os.listdir(directory)
    os.chdir(directory)
    current_directory = os.getcwd()

    for item in all_items:
        item_full_path = path.join(current_directory, item)

        if path.isfile(item_full_path):
            if item not in FILES_TO_KEEP and path.splitext(item)[1] != SUFFIX:
                print(f"Removing {item_full_path}")
                os.remove(item_full_path)
        elif path.isdir(item_full_path):
            visit_directory(item_full_path)


if __name__ == "__main__":
    os.chdir("..")
    print("Copying...")
    dir_util.copy_tree(DATA, DATA_FILTERED)
    visit_directory(DATA_FILTERED)
    print("Done")
