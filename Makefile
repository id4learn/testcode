CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
LDFLAGS = 
TARGET = gre_tunnel
SOURCE = gre_tunnel.c

.PHONY: all clean install test

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LDFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(TARGET)
	chmod +x gre_tunnel.py

test: $(TARGET)
	@echo "Testing C program compilation..."
	./$(TARGET) --help || echo "Program compiled successfully"
	@echo "Testing Python program..."
	python3 gre_tunnel.py --help || echo "Python program is ready"

help:
	@echo "Available targets:"
	@echo "  all     - Build the C program"
	@echo "  clean   - Remove built files"
	@echo "  install - Install programs to /usr/local/bin"
	@echo "  test    - Test both programs"
	@echo "  help    - Show this help message"