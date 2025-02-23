NAME= ircserve

CXX= c++
CXXFLAGS= -Wall -Wextra -Werror -std=c++98

SRC= main.cpp \
	 src/Server.cpp \
	 src/Parser.cpp \
	 src/Channel.cpp \
	 src/Client.cpp \
	 src/Commands.cpp\
	 src/Auth.cpp\
	 src/helpCommands/authMsg.cpp\
	 src/helpCommands/help.cpp\
	 

OBJ= $(SRC:.cpp=.o)

HEADERS= ./include/Server.hpp ./include/Parser.hpp ./include/Channel.hpp ./include/Client.hpp ./include/Commands.hpp ./include/Auth.hpp

ifeq ($(shell uname), Linux)
	KQUEUE= -lkqueue
endif

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(KQUEUE)

run: all
	./$(NAME) 4444 123

nc:
	nc 127.0.0.1 4444 

all: $(NAME)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(KQUEUE)

$(NAME): $(OBJ) $(HEADERS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re run nc


	 