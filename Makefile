CC = gcc
CFLAGS = -O2 -fPIC -Wall -D_GNU_SOURCE
TARGET = dh64.so
SHARED = -shared
LUADIR = /usr/local/openresty/luajit/include/luajit-2.1/
LUALIB = /usr/local/openresty/luajit/lib/

.PHONY : all clean

all : $(TARGET)

$(TARGET) : dh64.c
	$(CC) -I$(LUADIR) $(CFLAGS) -L$(LUALIB) $(SHARED) -o $@ $^ 

clean :
	rm -f $(TARGET)
