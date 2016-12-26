PREFIX=/usr/local

CC=cc

CFLAGS=-Wall -Wextra -std=c89 -pedantic -g
LDFLAGS=-g -ltermkey

TARGET=ritle
OBJS=ritle.o
