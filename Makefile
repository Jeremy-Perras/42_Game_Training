NAME = library

SRCS = main.cpp window.cpp app.cpp device.cpp

INCLUDES = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi 

CXXFLAGS = -O3

CC = g++

FLAGS = -Wall -Werror -Wextra -std=c++17 -ferror-limit=0 

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

all : $(NAME)

debug: CXXFLAGS = -gdwarf-4 -fsanitize=address 

debug: $(OBJS_DEBUG)
	$(CC) $(CXXFLAGS) $(OBJS_DEBUG) $(LDFLAGS) $(INCLUDES) -o $(NAME)_debug

debug_clean:
	rm -rf $(DEBUG_DIR)

debug_fclean: debug_clean
	rm -rf $(NAME)_debug

debug_re: debug_fclean debug

gl: 
	glslc shaders/simple_shader.frag -o shaders/simple_shader.frag.spv
	glslc shaders/simple_shader.vert -o shaders/simple_shader.vert.spv
	# glslc shaders/point_light.vert -o shaders/point_light.vert.spv
	# glslc shaders/point_light.frag -o shaders/point_light.frag.spv

clean :
	rm -rf $(OBJS)
	# rm -rf shaders/frag.spv
	# rm -rf shaders/vert.spv

fclean : clean
	rm -rf $(NAME)

re : fclean all 

cc:
	bear -- make re

