# ===== Program =====
PRG_N = Chess

# ===== Compiler & compiler settings =====
CC = gcc
CFLAGS = -Wall -Wextra -g -O0 -I include

# ===== Libraries =====
LIBS = -l csfml-graphics -l csfml-system -l csfml-window -l m

# ===== Program directories =====
SRC_DIR = src
INC_DIR = include
OBJ_DIR = build

# ===== Building program from =====
OBJ = $(OBJ_DIR)/main.o $(OBJ_DIR)/button.o $(OBJ_DIR)/main_menu.o $(OBJ_DIR)/display_field.o \
	$(OBJ_DIR)/ui_utils.o $(OBJ_DIR)/text_field.o $(OBJ_DIR)/mouse.o $(OBJ_DIR)/resources.o

all: $(PRG_N)

$(PRG_N): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(PRG_N) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(INC_DIR)/main_menu.h $(INC_DIR)/mouse.h $(INC_DIR)/resources.h

$(OBJ_DIR)/resources.o: $(SRC_DIR)/resources.c $(INC_DIR)/resources.h

$(OBJ_DIR)/main_menu.o: $(SRC_DIR)/main_menu.c $(INC_DIR)/main_menu.h $(INC_DIR)/interfaces.h $(INC_DIR)/ui_elements.h \
						$(INC_DIR)/button.h $(INC_DIR)/display_field.h $(INC_DIR)/mouse.h $(INC_DIR)/resources.h

$(OBJ_DIR)/mouse.o: $(SRC_DIR)/mouse.c $(INC_DIR)/mouse.h

$(OBJ_DIR)/ui_utils.o: $(SRC_DIR)/ui_utils.c $(INC_DIR)/ui_utils.h
$(OBJ_DIR)/button.o: $(SRC_DIR)/button.c $(INC_DIR)/button.h\
					$(INC_DIR)/ui_elements.h $(INC_DIR)/ui_utils.h $(INC_DIR)/mouse.h
$(OBJ_DIR)/display_field.o: $(SRC_DIR)/display_field.c $(INC_DIR)/display_field.h $(INC_DIR)/ui_elements.h
$(OBJ_DIR)/text_field.o: $(SRC_DIR)/text_field.c $(INC_DIR)/text_field.h $(INC_DIR)/ui_elements.h $(INC_DIR)/mouse.h


.PHONY: clean rebuild run

clean:
	-rm -rf $(PRG_N) $(OBJ) build

rebuild: clean all

run: $(PRG_N)	# build and run
	./$(PRG_N)