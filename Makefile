NAME = game


SRCS = main.cpp system/window.cpp app.cpp system/pipeline.cpp system/device.cpp system/swap_chain.cpp gamestructure/interface_model.cpp system/renderer.cpp \
controler/keyboard_movement_controller.cpp system/buffer.cpp system/descriptors.cpp renderer/texture_render_system.cpp gamestructure/texture.cpp \
controler/mouse_movement_controller.cpp parsing/parsing.cpp gamestructure/game_loop.cpp  renderer/window_display.cpp\
song/song.cpp renderer/star_nest_render_system.cpp renderer/exit_render_system.cpp  renderer/choose_level_render_system.cpp \

INCLUDES = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -I/opt/homebrew/include -framework OpenGL -framework GLUT -framework OpenAL  -I./libs


CXXFLAGS = -O3

CC = g++

FLAGS =  -std=c++17 -pthread -ferror-limit=0 -g 

MKDIR = mkdir -p
OUT_DIR := out
DEBUG_DIR := debug
OBJS = $(addprefix $(OUT_DIR)/,$(SRCS:%.cpp=%.o))
OBJS_DEBUG = $(addprefix $(DEBUG_DIR)/,$(SRCS:%.cpp=%.o))

$(OUT_DIR)/%.o: %.cpp
	$(MKDIR) $(@D)
	$(COMPILE.cc) $(FLAGS) $<  -o $@

$(DEBUG_DIR)/%.o: %.cpp
	$(MKDIR) $(@D)
	$(COMPILE.cc) $< -MMD -MP -o $@


$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(FLAGS) $(OBJS) $(INCLUDES) -o $(NAME)

all : $(NAME) gl

debug: CXXFLAGS = -gdwarf-4 -fsanitize=address -std=c++17 

debug: $(OBJS_DEBUG)
	$(CC) $(CXXFLAGS) $(OBJS_DEBUG) $(LDFLAGS) $(INCLUDES) -o $(NAME)_debug

debug_clean:
	rm -rf $(DEBUG_DIR)

debug_fclean: debug_clean
	rm -rf $(NAME)_debug

debug_re: debug_fclean debug

gl:
	glslc shaders/exit.frag -o shaders/exit.frag.spv
	glslc shaders/exit.vert -o shaders/exit.vert.spv 
	glslc shaders/starNest.frag -o shaders/starNest.frag.spv
	glslc shaders/starNest.vert -o shaders/starNest.vert.spv
	glslc shaders/texture.frag -o shaders/texture.frag.spv
	glslc shaders/texture.vert -o shaders/texture.vert.spv
	glslc shaders/Menu.frag -o shaders/Menu.frag.spv
	glslc shaders/Menu.vert -o shaders/Menu.vert.spv
	

clean :
	rm -rf $(OBJS)
	rm -rf shaders/*.spv
	
fclean : clean
	rm -rf $(NAME)

re : fclean all 

format: 
	clang-format -i shaders/*.frag
	clang-format -i shaders/*.vert
	clang-format -i shaders/*.comp

cc:
	bear -- make re

