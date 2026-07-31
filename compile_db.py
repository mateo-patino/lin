import json
import argparse
import os

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Create database of compile commands")

    # Name of .json file to write commands to
    parser.add_argument(
        "--filename",
        required=True,
        type=str,
        help="File name where compile commands will be saved"
    )

    # Current directory (should be $(CURDIR))
    parser.add_argument(
        "--curdir",
        required=True,
        type=str,
        help="Current working directory"
    )

    # Compile command (should be $(COMPILE.c))
    parser.add_argument(
        "--compile-cmd",
        required=True,
        type=str,
        help="Command for compiling .c files"
    )

    # List of source files (app and test files)
    parser.add_argument(
        "--srcs",
        required=True,
        nargs='+',
        help="List of source file paths to save compile commands for"
    )


    args = parser.parse_args()

    # Produce list of dictionaries, one per source file
    data = []
    for src_name in args.srcs:
        data.append({   
            "directory": args.curdir,
            "command": args.compile_cmd + f" -c {src_name} -o _",
            "file": os.path.join(args.curdir, src_name)
        })

    # Dump list of dictionaries in target file
    with open(args.filename, "w") as file:
        json.dump(data, file, indent=4)

