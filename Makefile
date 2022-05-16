CC := clang
cflags= -O3 -std=c11 -mavx -mavx2 $(CFLAGS)
TARGET=bench

$(TARGET): src/inverse_cdf.c src/boxmuller.c src/irwin_hall_distribution.c src/rng.c src/main.c
	$(CC) -o $(TARGET) $(cflags) src/main.c

clean:
	rm -f $(TARGET)

.PHONY: clean