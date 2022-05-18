#!/bin/bash

# echo ${PATH}

export PATH="~/.platformio/penv/bin":"~/.platformio/penv":${PATH}

# echo ${PATH}

# pio --help

# pio  run --environment esp32dev

# platformio run --target clean --environment esp32dev

# 编译代码
if [ $# == 0 ];
then
    
    platformio  run --environment nodemcuv2
    
fi

# 执行清理
if [ $# == 1 ];
then

    case "$1" in
    clean)
        platformio run --target clean --environment nodemcuv2
    ;;
    fs)
        platformio run --target buildfs --environment nodemcuv2
    ;;
    info)
        platformio run -v --environment nodemcuv2
    ;;
    esac

fi

