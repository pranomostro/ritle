PREFIX=/usr/local

CC=cc

CFLAGS=-Wall -Wextra -std=c89 -pedantic -s -O2
LDFLAGS=-ltermkey

TARGET=ritle
OBJS=ritle.o
