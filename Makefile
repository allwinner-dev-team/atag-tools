CFLAGS=-Wall -Wextra --std=c99

.PHONY: build clean

build: atagblob dumpatags

atagblob.o: atagblob.c genatags.h
genatags.o: genatags.c genatags.h atags.h omap_tags.h
dumpatags.o: dumpatags.c atags.h omap_tags.h

atagblob: atagblob.o genatags.o
	gcc -o $@ $^ $(CFLAGS)

dumpatags: dumpatags.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	@rm -rf *.o atagblob dumpatags
