
genfont: genfont.c
	gcc genfont.c `pkg-config --libs --cflags freetype2` \
	 		`sdl2-config --libs --cflags` -o genfont

genfont_named: genfont_named.c
	gcc genfont_named.c `pkg-config --libs --cflags freetype2` \
	 		`sdl2-config --libs --cflags` -o genfont_named

hack14.h: genfont_named
	 ./genfont_named /usr/share/fonts/truetype/hack/Hack-Regular.ttf 14 hack14 > hack14.h
