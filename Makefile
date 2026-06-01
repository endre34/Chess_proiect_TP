# ===== Program =====
PRG_N = Chess

# ===== Compiler & compiler settings =====
CC = gcc
CFLAGS = -Wall -Wextra -g -O0 -I include -MMD -MP

# ===== Libraries =====
LIBS = -lcsfml-graphics -lcsfml-system -lcsfml-window -lm

# ===== Program directories =====
SRC_DIR = src
INC_DIR = include
OBJ_DIR = build

# ===== Building program from =====
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/button.o $(OBJ_DIR)/main_menu.o $(OBJ_DIR)/display_field.o \
	$(OBJ_DIR)/ui_utils.o $(OBJ_DIR)/text_field.o $(OBJ_DIR)/mouse.o $(OBJ_DIR)/resources.o

# Dependencies
DEP = $(OBJ:.o=.d)


.DEFAULT_GOAL: all

all: $(PRG_N)

$(PRG_N): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(PRG_N) $(LIBS)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEP)


.PHONY: all clean rebuild run

clean:
	-rm -rf $(PRG_N) $(OBJ_DIR)

rebuild: clean all

run: $(PRG_N)	# build and run
	./$(PRG_N)

