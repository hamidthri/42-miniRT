NAME = minirt

SRC_DIR = ./srcs

SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = $(SRCS:.c=.o)

CC = cc
CFLAGS = -Wall -Wextra -Werror -I./include
MLX_FLAGS = -L ./MLX42/build -lmlx42 -lglfw -framework Cocoa -framework OpenGL -framework IOKit -ldl -lpthread -lm



CC = cc
CFLAGS = -Wall -Wextra -Werror
MLX_FLAGS = -L ./MLX42/build -lmlx42 -lglfw -framework Cocoa -framework OpenGL -framework IOKit -ldl -lpthread -lm

# Libraries
LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a
LIBFT_FLAGS = -L$(LIBFT_DIR) -lft

# Include directories
INCLUDES = -I. -I$(LIBFT_DIR) -I./MLX42/include

# Rules
all: $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

mlx:
	cd MLX42; \
	cmake -B build; \
	make -C build -j4


$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT_FLAGS) $(MLX_FLAGS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	$(RM) $(OBJS)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	$(RM) $(NAME)

re: fclean all

debug: CFLAGS += -g -fsanitize=address
debug: all

norm:
	norminette $(SRCS) $(SRC_DIR)/minirt.h $(LIBFT_DIR)

.PHONY: all clean fclean re mlx debug norm