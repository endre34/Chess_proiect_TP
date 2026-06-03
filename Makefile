# ===== Program =====
PRG_N = Chess

# ===== Compiler & compiler settings =====
CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude -MMD -MP

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
CHESS = chess
GAME_SESSION = game_session
GAME = $(FRONTEND)/game

# ===== Building program from =====
OBJ = $(OBJ_DIR)/main.o \
		$(OBJ_DIR)/$(RESOURCES)/resources.o \
		$(OBJ_DIR)/$(INPUT)/mouse.o \
		$(OBJ_DIR)/$(FRONTEND)/screen_manager.o \
		$(OBJ_DIR)/$(GAME)/chess_board_view.o \
		$(OBJ_DIR)/$(DATA)/screen_data.o \
		$(OBJ_DIR)/$(DATA)/screen_manager_request.o \
		$(OBJ_DIR)/$(SCREENS)/credits_menu.o \
		$(OBJ_DIR)/$(SCREENS)/game_setup_menu.o \
		$(OBJ_DIR)/$(SCREENS)/main_menu.o \
		$(OBJ_DIR)/$(SCREENS)/settings_menu.o \
		$(OBJ_DIR)/$(SCREENS)/controls_menu.o \
		$(OBJ_DIR)/$(SCREENS)/local_pvp_setup_menu.o \
		$(OBJ_DIR)/$(SCREENS)/engine_pve_setup_menu.o \
		$(OBJ_DIR)/$(SCREENS)/game_engine_pve_screen.o \
		$(OBJ_DIR)/$(SCREENS)/game_local_pvp_screen.o \
		$(OBJ_DIR)/$(UI)/button.o \
		$(OBJ_DIR)/$(UI)/display_field.o \
		$(OBJ_DIR)/$(UI)/text_field.o \
		$(OBJ_DIR)/$(UI)/ui_utils.o \
		$(OBJ_DIR)/$(CHESS)/game.o \
		$(OBJ_DIR)/$(CHESS)/move.o \
		$(OBJ_DIR)/$(CHESS)/piece.o \
		$(OBJ_DIR)/$(CHESS)/board.o \
		$(OBJ_DIR)/$(GAME_SESSION)/game_session.o \
		$(OBJ_DIR)/$(GAME_SESSION)/game_clock.o 

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

