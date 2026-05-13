CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

NAME = expr

SRCS = main.cpp scanner.cpp ibterpreter.cpp
OBJS=$(SRCS:.cpp=.o)

all:$(NAME)

$(NAME): $(OBJS)
$(CXX)$(CXXFLAGS)-o$(NAME)$(OBJS)

%.o: %.cpp Node.hpp
$(CXX)$(CXXFLAGS)-c$< -o$@

clean:
	rm -f $(OBJS)

fclean:clean
	rm -f$(NAME)
re: fclean all