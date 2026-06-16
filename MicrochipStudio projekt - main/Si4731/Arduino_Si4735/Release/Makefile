################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../Sketch.cpp \
../vfd.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
Sketch.o \
vfd.o

OBJS_AS_ARGS +=  \
Sketch.o \
vfd.o

C_DEPS +=  \
Sketch.d \
vfd.d

C_DEPS_AS_ARGS +=  \
Sketch.d \
vfd.d

OUTPUT_FILE_PATH +=Arduino_Si4735.elf

OUTPUT_FILE_PATH_AS_ARGS +=Arduino_Si4735.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+=  \
D:/Elektro/_Projekty/Radio\ Si4731-C40/MicrochipStudio\ projekt/Si4731/ArduinoCore/Release/libArduinoCore.a

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler
./Sketch.o: .././Sketch.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DNDEBUG -DF_CPU=16000000L -DARDUINO=108019 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR  -I"..\\..\ArduinoCore\include\core" -I"..\\..\ArduinoCore\include\libraries\pu2clr-si4735" -I"..\\..\ArduinoCore\include\libraries\Wire" -I"..\\..\ArduinoCore\include\libraries\Wire\utility" -I"..\\..\ArduinoCore\include\variants\eightanaloginputs" -I"..\\..\ArduinoCore\include\variants\standard" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\2.0.401\include"  -Os -fno-threadsafe-statics -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -w -mmcu=atmega328 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\2.0.401\gcc\dev\atmega328" -c -std=gnu++11 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

./vfd.o: .././vfd.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DNDEBUG -DF_CPU=16000000L -DARDUINO=108019 -DARDUINO_AVR_NANO -DARDUINO_ARCH_AVR  -I"..\\..\ArduinoCore\include\core" -I"..\\..\ArduinoCore\include\libraries\pu2clr-si4735" -I"..\\..\ArduinoCore\include\libraries\Wire" -I"..\\..\ArduinoCore\include\libraries\Wire\utility" -I"..\\..\ArduinoCore\include\variants\eightanaloginputs" -I"..\\..\ArduinoCore\include\variants\standard" -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\2.0.401\include"  -Os -fno-threadsafe-statics -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -w -mmcu=atmega328 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\2.0.401\gcc\dev\atmega328" -c -std=gnu++11 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	




# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Linker : 5.4.0
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="Arduino_Si4735.map" -Wl,--start-group -Wl,-lm -Wl,-lArduinoCore  -Wl,--end-group -Wl,-L"D:\Elektro\_Projekty\Radio Si4731-C40\MicrochipStudio projekt\Si4731\ArduinoCore\Release"  -Wl,--gc-sections -mmcu=atmega328 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\2.0.401\gcc\dev\atmega328" -Os  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "Arduino_Si4735.elf" "Arduino_Si4735.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "Arduino_Si4735.elf" "Arduino_Si4735.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "Arduino_Si4735.elf" > "Arduino_Si4735.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "Arduino_Si4735.elf" "Arduino_Si4735.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "Arduino_Si4735.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "Arduino_Si4735.elf" "Arduino_Si4735.a" "Arduino_Si4735.hex" "Arduino_Si4735.lss" "Arduino_Si4735.eep" "Arduino_Si4735.map" "Arduino_Si4735.srec" "Arduino_Si4735.usersignatures"
	