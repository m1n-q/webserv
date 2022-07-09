RED         = "\033[1;91m"
GREEN       = "\033[1;92m"
YELLOW      = "\033[1;93m"
EOC         = "\033[0;0m"
LINE_CLEAR  = "\x1b[1A\x1b[M"

CC = c++
CFLAGS = -Wall -Werror -Wextra -std=c++98 -DDEBUG -DPRINTHEADER -g -fsanitize=address#-DDEBUG
INC_DIR = incs
SRC_DIR = srcs
OBJS =  ${SRCS:.cpp=.o}
SRCS =  $(SRC_DIR)/checkFile.cpp \
		$(SRC_DIR)/CGI.cpp \
		$(SRC_DIR)/Config.cpp \
		$(SRC_DIR)/ConfigUtils.cpp \
		$(SRC_DIR)/ConfigChecker.cpp \
		$(SRC_DIR)/ConfigLoader.cpp \
		$(SRC_DIR)/ConnSocket.cpp \
		$(SRC_DIR)/Exceptions.cpp \
		$(SRC_DIR)/FileStream.cpp \
		$(SRC_DIR)/httpError.cpp \
		$(SRC_DIR)/HttpConfig.cpp \
		$(SRC_DIR)/IHeader.cpp \
		$(SRC_DIR)/ISocket.cpp \
		$(SRC_DIR)/Stream.cpp \
		$(SRC_DIR)/IText.cpp \
		$(SRC_DIR)/LocationConfig.cpp \
		$(SRC_DIR)/Pipe.cpp \
		$(SRC_DIR)/Poll.cpp \
		$(SRC_DIR)/ReqBody.cpp \
		$(SRC_DIR)/ReqHeader.cpp \
		$(SRC_DIR)/ResBody.cpp \
		$(SRC_DIR)/ResHeader.cpp \
		$(SRC_DIR)/ServerConfig.cpp \
		$(SRC_DIR)/ServerSocket.cpp \
		$(SRC_DIR)/Timer.cpp \
		$(SRC_DIR)/core.cpp \
		$(SRC_DIR)/main.cpp \
		$(SRC_DIR)/utils.cpp


NAME = webserv
ifndef ECHO
T := $(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
		-nrRf $(firstword $(MAKEFILE_LIST)) \
		ECHO="COUNTTHIS" | grep -c "COUNTTHIS")
N := x
C = $(words $N)$(eval N := x $N)
ECHO = ${GREEN} "[`expr $C '*' 100 / $T`%]"
endif

all : $(NAME)
$(NAME) : $(OBJS)
	@echo $(GREEN) "Source files are compiled!\n\n" $(EOC)
	@echo $(YELLOW) "Building executable $(NAME)" $(EOC)
	@$(CC) $(CFLAGS) $^ -o $@
	@echo $(GREEN) "$(NAME) is created!\n" $(EOC)

$(SRC_DIR)/%.o : $(SRC_DIR)/%.cpp
	@echo $(YELLOW) "Compiling..." ${ECHO}$(YELLOW) $< $(EOC) $(LINE_CLEAR)
	@$(CC) $(CFLAGS) -c $< -o $@ -I $(INC_DIR)

clean :
	@echo $(YELLOW) "Removing object files..." $(EOC)
	@rm -rf $(OBJS)
	@echo $(RED) "Object files are removed!\n" $(EOC)

fclean : clean
	@echo $(YELLOW) "Removing $(NAME)..." $(EOC)
	@rm -rf $(NAME)
	@echo $(RED) "$(NAME) is removed!\n\n" $(EOC)

re : fclean all
