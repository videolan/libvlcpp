ACLOCAL_AMFLAGS = -I m4

MEDIALIB_CPPFLAGS = $(AM_CPPFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/src

libvlcppdir = $(includedir)/vlcpp

libvlcpp_HEADERS =          \
	vlcpp/common.hpp              \
	vlcpp/Equalizer.hpp           \
	vlcpp/EventManager.hpp        \
	vlcpp/Instance.hpp            \
	vlcpp/Internal.hpp            \
	vlcpp/MediaDiscoverer.hpp     \
	vlcpp/Media.hpp               \
	vlcpp/MediaLibrary.hpp        \
	vlcpp/MediaList.hpp           \
	vlcpp/MediaListPlayer.hpp     \
	vlcpp/MediaPlayer.hpp         \
	vlcpp/Dialog.hpp			  \
	vlcpp/structures.hpp          \
	vlcpp/vlc.hpp                 \
	$(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libvlcpp.pc

if HAVE_EXAMPLES
noinst_PROGRAMS = helloworld tests imem

AM_CPPFLAGS = $(vlc_CFLAGS) -Wextra -Wall
AM_LDFLAGS = $(vlc_LIBS)

if HAVE_WERROR
# This is meant to make our builds fail if a deprecated method is present
AM_CPPFLAGS += -Werror
endif

helloworld_SOURCES = examples/helloworld/main.cpp
imem_SOURCES = examples/imem/imem.cpp
tests_SOURCES = test/main.cpp

endif
