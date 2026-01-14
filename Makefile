CC := gcc
CFLAGS := -Wall -Werror -ggdb -std=c99 -O2
LDFLAGS := -I./raylib-5.5_linux_amd64/include -L./raylib-5.5_linux_amd64/lib -l:libraylib.a -lm

SRC_DIR := src
OUT_DIR := build

$(OUT_DIR)/game: $(SRC_DIR)/main.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: run
run: $(OUT_DIR)/game
	$<

$(OUT_DIR):
	mkdir -p $@
