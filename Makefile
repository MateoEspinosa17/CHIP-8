# ==========================================
# VARIABLES DE CONFIGURACIÓN
# ==========================================
CC       := gcc
CFLAGS   := -Wall -Wextra -pedantic -std=c11 -Iinclude -g -DSDL_MAIN_HANDLED
LDFLAGS  := -lSDL2main -lSDL2
TARGET   := main.exe

# DIRECTORIOS
SRC_DIR  := src
BUILD_DIR:= build

# BÚSQUEDA AUTOMÁTICA DE ARCHIVOS
SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# ==========================================
# REGLAS Y RECETAS
# ==========================================

# Regla por defecto (se ejecuta al escribir solo 'make' o 'mingw32-make')
all: $(BUILD_DIR)/$(TARGET)

# Vincula los objetos (.o) para crear el ejecutable (.exe)
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo [make] Vinculando el ejecutable final
		$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo [make] Compilacion completada con exito

# Compila cada archivo .c en un archivo objeto .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo [make] Compilando $<
		$(CC) $(CFLAGS) -c $< -o $@

# Crea el directorio de compilación si no existe
$(BUILD_DIR):
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

# Compila y ejecuta el programa inmediatamente
run: all
	@echo [make] Running 
	.\$(BUILD_DIR)\$(TARGET)

# Limpia los archivos compilados para empezar desde cero
clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@echo [make] Limpieza completada

# Indica a Make que estas reglas no corresponden a archivos reales
.PHONY: all run clean