#!/bin/bash

git submodule update --recursive;
cp STM32CubeF1/Drivers/STM32F1xx\_HAL\_Driver/Inc/stm32f1xx\_hal\_conf\_template.h \
   STM32CubeF1/Drivers/STM32F1xx\_HAL\_Driver/Inc/stm32f1xx\_hal\_conf.h;
rm STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/*_template.c;

echo -e "#define STM32F103x6\n#define USE_HAL_DRIVER\n$(<STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h)" > STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f1xx.h


