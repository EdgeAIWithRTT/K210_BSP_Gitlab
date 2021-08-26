#!/bin/bash
if [ -d "$CI_PROJECT_DIR/课后拓展" ]; then 
    rm -rf K210_BSP/applications/* 
    cp $CI_PROJECT_DIR/课后拓展/代码/* K210_BSP/applications -r
    scons -C K210_BSP -j$CPUS
    echo "Done."
fi

