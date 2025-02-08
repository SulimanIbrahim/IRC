NAME= ircserve

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98

SRC= main.cpp \
	 src/Server.cpp \
	 src/Parser.cpp \
	 src/Channel.cpp \
	 src/Client.cpp 

OBJ= $(SRC:.cpp=.o)

HEADERS= ./include/Server.hpp ./include/Parser.hpp ./include/Channel.hpp ./include/Client.hpp

all: $(NAME)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(NAME): $(OBJ) $(HEADERS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re


	 