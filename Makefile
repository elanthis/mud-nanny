VERSION=0.1

mud-nanny: mud-nanny.c
	$(CC) -o $@ $<

dist:
	( mkdir mud-nanny-$(VERSION)/ && cp mud-nanny.c mud-nanny-event Makefile README mud-nanny-$(VERSION)/ && tar -zcf mud-nanny-$(VERSION).tar.gz mud-nanny-$(VERSION)/ && rm -fr mud-nanny-$(VERSION)/ ) || rm -fr mud-nanny-$(VERSION)/
