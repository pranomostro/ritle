PREFIX=/usr/local

CC=cc

CFLAGS=-Wall -Wextra -std=c89 -pedantic -O2
LDFLAGS=-s -ltermkey

TARGET=ritle
OBJS=ritle.o
