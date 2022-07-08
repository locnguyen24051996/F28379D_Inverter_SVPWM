################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_21.12.0.STS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="D:/NCKH/C2000/DSP_Inverter_SVPWM-master" --include_path="C:/ti/C2000Ware_4_00_00_00/device_support/f2837xd/common/include" --include_path="C:/ti/C2000Ware_4_00_00_00/device_support/f2837xd/headers/include" --include_path="C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_21.12.0.STS/include" --define=CPU1 --define=_FLASH -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_21.12.0.STS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="D:/NCKH/C2000/DSP_Inverter_SVPWM-master" --include_path="C:/ti/C2000Ware_4_00_00_00/device_support/f2837xd/common/include" --include_path="C:/ti/C2000Ware_4_00_00_00/device_support/f2837xd/headers/include" --include_path="C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_21.12.0.STS/include" --define=CPU1 --define=_FLASH -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.C $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_21.12.0.STS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="D:/NCKH/C2000/DSP_Inverter_SVPWM-master" --include_path="C:/ti/C2000Ware_4_00_00_00/device_support/f2837xd/common/include" --include_path="C:/ti/C2000Ware_4_00_00_00/device_support/f2837xd/headers/include" --include_path="C:/ti/ccs1010/ccs/tools/compiler/ti-cgt-c2000_21.12.0.STS/include" --define=CPU1 --define=_FLASH -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


