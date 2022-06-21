VERSION = 1.2-dev

EXENAME ?= shkd
BUILDROOT ?= build
INSTALLPREFIX ?= /usr/local

CXX ?= c++

CXXFLAGS ?= -Os -s -Wall -static -nostartfiles -e main -fno-PIC
LDFLAGS  ?=
