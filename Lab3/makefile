CFLAGS = -Wall -Werror -Wfatal-errors -Wno-unused-variable
OUT = udp_forwarder
SRC = udp_forwarder.c

all: $(OUT)

$(OUT): $(SRC)
	gcc $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)