#!/bin/bash

filetypes=(".c" ".h" ".cc" ".cpp")
directories=("src/boards")  # "src/libraries/rm" "src/libraries/libeasy")

for d in ${directories[@]}
do
    for ft in ${filetypes[@]}
    do
        for f in $(find ./$d -name "*$ft" -type f)
        do
            # clang-format-12 --Werror -i $f
            if ! clang-format-12 --Werror --dry-run --ferror-limit=1 $f
            then
                echo "At least 1 file unformatted. Run clang-format locally to discover all unforamtted files."
                exit 1
            fi
        done
    done
done
