TARGET = make103
BUILD_DIR = build

C_SOURCES2 =  \
Src/stm32f10x_it.c \
Drivers/Src/stm32f10x_gpio.c \
Drivers/Src/stm32f10x_flash.c \
Drivers/Src/stm32f10x_tim.c \
Drivers/Src/stm32f10x_tim.c \
Src/system_stm32f10x.c \
Drivers/Src/stm32f10x_dma.c \
Drivers/Src/stm32f10x_fsmc.c \
Src/main.c \
Drivers/Src/stm32f10x_rcc.c \
Drivers/Src/stm32f10x_pwr.c \
Drivers/Src/stm32f10x_usart.c \
Drivers/Src/stm32f10x_exti.c \
Drivers/Src/misc.c \
Src/uart.c \
Src/sys.c \
hardware/src/led_ze.c \
hardware/src/key_ze.c \
hardware/src/delay.c \
hardware/src/lcd.c \
hardware/src/sccb.c \
hardware/src/ov7670.c \
hardware/src/timer.c \
hardware/src/exti.c \
hardware/src/grey.c \
hardware/src/move.c \
hardware/src/a4950.c \
hardware/src/inout.c \

# C includes
C_INCLUDES =  \
-IInc \
-IDrivers/Inc \
-IDrivers/CMSIS/Device/ST/STM32F1xx/Include \
-IDrivers/CMSIS/Include	\
-Ihardware/inc \

C_SOURCES= $(sort $(C_SOURCES2))

# LDSCRIPT = STM32F103C8Tx_FLASH.ld
LDSCRIPT = STM32F103ZETx_FLASH.ld
# ASM sources
ASM_SOURCES =  \
startup_stm32f103xe.s
# startup_stm32f103xb.s 

# CFLAGS= -mcpu=cortex-m3 -mthumb -DSTM32F103xB -DUSE_STDPERIPH_DRIVER -DSTM32F10X_HD -DAPP_DEBUG=0 $(C_INCLUDES)  -Wall -fdata-sections -ffunction-sections -lc -lm -lnosys  -g -gdwarf-2 -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"
CFLAGS= -mcpu=cortex-m3 -mthumb -DSTM32F103xE -DUSE_STDPERIPH_DRIVER -DSTM32F10X_HD -DAPP_DEBUG=0 $(C_INCLUDES)  -Wall -fdata-sections -ffunction-sections -lc -lm -lnosys  -g -gdwarf-2 -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"

# ASFLAGS= -mcpu=cortex-m3 -mthumb -DSTM32F103xB -DUSE_STDPERIPH_DRIVER -DSTM32F10X_HD -DAPP_DEBUG=0 $(C_INCLUDES)  -Wall -fdata-sections -ffunction-sections -lc -lm -lnosys  -g -gdwarf-2   -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" 
ASFLAGS= -mcpu=cortex-m3 -mthumb -DSTM32F103xE -DUSE_STDPERIPH_DRIVER -DSTM32F10X_HD -DAPP_DEBUG=0 $(C_INCLUDES)  -Wall -fdata-sections -ffunction-sections -lc -lm -lnosys  -g -gdwarf-2   -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" 

LDFLAGS = -mcpu=cortex-m3  -mthumb -specs=nano.specs -T$(LDSCRIPT)  -lc -lm -lnosys -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
  
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

PREFIX = arm-none-eabi-
CC = $(BINPATH) $(PREFIX)gcc
AS = $(BINPATH) $(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH) $(PREFIX)objcopy
AR = $(BINPATH) $(PREFIX)ar
SZ = $(BINPATH) $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

all:   $(BUILD_DIR) $(BUILD_DIR)/$(TARGET).elf  $(BUILD_DIR)/$(TARGET).bin  $(BUILD_DIR)/$(TARGET).hex  
	 
$(BUILD_DIR)/%.o: %.c    
	$(CC) -c $(CFLAGS) $^ -o $@
	@echo Building $@
$(BUILD_DIR)/%.o:%.s   
	$(AS) -c $(CFLAGS) $^ -o $@
	
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@
	$(SZ) $@
	
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf 
	$(HEX) $^ $@ 
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf 
	$(BIN) $^ $@	
	
$(BUILD_DIR):
	mkdir $@
clean:
	-rm -fR .dep $(BUILD_DIR)
