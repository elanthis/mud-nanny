mud-nanny: mud-nanny.c
	$(CC) -o $@ $<

dist:
	mkdir mud-nanny/
	cp mud-nanny.c mud-nanny-event Makefile README mud-nanny/
	tar -zcf mud-nanny.tar.gz
	rm -fr mud-nanny/
