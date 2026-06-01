# ===== Program =====
PRG_N = Chess

# ===== Compiler & compiler settings =====
CC = gcc
CFLAGS = -Wall -Wextra -g -O0 -Iinclude -MMD -MP

# ===== Libraries =====
LIBS = -lcsfml-graphics -lcsfml-system -lcsfml-window -lm

# ===== Program directories =====
INC_DIR = include
SRC_DIR = src
OBJ_DIR = build

# ===== Sub-directories =====
FRONTEND = frontend
DATA = $(FRONTEND)/data
SCREENS = $(FRONTEND)/screens
UI = $(FRONTEND)/ui
INPUT = input
RESOURCES = resources

# ===== Building program from =====
OBJ = $(OBJ_DIR)/main.o \
		$(OBJ_DIR)/$(RESOURCES)/resources.o \
		$(OBJ_DIR)/$(INPUT)/mouse.o \
		$(OBJ_DIR)/$(FRONTEND)/screen_manager.o \
		$(OBJ_DIR)/$(DATA)/screen_data.o \
		$(OBJ_DIR)/$(DATA)/screen_manager_request.o \
		$(OBJ_DIR)/$(SCREENS)/credits_menu.o \
		$(OBJ_DIR)/$(SCREENS)/game_setup_menu.o \
		$(OBJ_DIR)/$(SCREENS)/main_menu.o \
		$(OBJ_DIR)/$(SCREENS)/settings_menu.o \
		$(OBJ_DIR)/$(UI)/button.o \
		$(OBJ_DIR)/$(UI)/display_field.o \
		$(OBJ_DIR)/$(UI)/text_field.o \
		$(OBJ_DIR)/$(UI)/ui_utils.o \

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

