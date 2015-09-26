#!/bin/bash

OBJ_DIR=objs

if [ ! -d "$OBJ_DIR" ];
then
    mkdir "$OBJ_DIR"
else
    rm "$OBJ_DIR"/*
fi

#Args:
# 1) name of the source file.
# 2) object file directory.
#Returns:
# name of the object file associated with the specified source file.
function getObjFileName {
    NAME=$(basename "$1")
    BASE="${NAME%.*}"
    echo "$2/$BASE.o"
}

FILES=$(ls *.c 2>/dev/null)
for FILE in $FILES
do
    OBJ_FILE=$(getObjFileName "$FILE" "$OBJ_DIR")
    echo "cc -c -O2 -o$OBJ_FILE $FILE"
    cc -c -O2 -o$OBJ_FILE $FILE
done

OBJS="objs/buildConfig.o objs/commandLineArgs.o objs/errors.o objs/file.o objs/fileInfo.o objs/filename.o objs/libraries.o objs/objectFiles.o objs/stringBuilder.o objs/stringList.o"

echo "cc -s -otest.elf $OBJS objs/test.o"
cc -s -otest.elf $OBJS objs/test.o
echo "cc -s -obuild $OBJS objs/build.o objs/buildfile.o objs/buildOptions.o objs/commandGenerator.o objs/devutils.o"
cc -s -obuild $OBJS objs/build.o objs/buildfile.o objs/buildOptions.o objs/commandGenerator.o objs/devutils.o
