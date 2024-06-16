NAME = soundLibrary

SRCS = openal.cpp

INCLUDES = 

CXXFLAGS = -O3

CC = g++

FLAGS = -Wall  -Wextra -std=c++17 -ferror-limit=0 -g

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
	g++ -std=c++17 -I/opt/homebrew/include -framework OpenGL -framework GLUT -framework OpenAL -framework Foundation -Wall -o openal-demo $(SRCS) 

all : $(NAME) gl

debug: CXXFLAGS = -gdwarf-4 -fsanitize=address -std=c++17 

debug: $(OBJS_DEBUG)
	$(CC) $(CXXFLAGS) $(OBJS_DEBUG) $(LDFLAGS)  -o $(NAME)_debug

debug_clean:
	rm -rf $(DEBUG_DIR)

debug_fclean: debug_clean
	rm -rf $(NAME)_debug

debug_re: debug_fclean debug

gl: 
	glslc shaders/shader.frag -o shaders/shader.frag.spv
	glslc shaders/shader.vert -o shaders/shader.vert.spv


clean :
	rm -rf $(OBJS)
	rm -rf shaders/shader.frag.spv
	rm -rf shaders/shader.vert.spv

fclean : clean
	rm -rf $(NAME)

re : fclean all 

cc:
	bear -- make re

