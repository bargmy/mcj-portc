CC = gcc
CFLAGS = -Wall -Wextra -O3 -I.
LIBS = -lglfw -lGLU -lGL -lz -lm

SRC = vec3.c \
      aabb.c \
      tesselator.c \
      textures.c \
      tile.c \
      level.c \
      frustum.c \
      chunk.c \
      level_renderer.c \
      entity.c \
      player.c \
      character.c \
      timer.c \
      input.c \
      font.c \
      mobile_ui.c \
      rubydung.c

OBJ = $(SRC:.c=.o)
TARGET = rubydung

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
