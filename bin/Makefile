# Please keep this in alphabetical order to prevent confusion
# between how it appears in directory listings like ls and the
# Kate text editor and how it appears here.

BINS := cat clear echo init kill memtesting pwd sh sync yes

all:
	for dir in $(BINS); do \
		$(MAKE) -C $$dir all; \
		cp $$dir/_$$dir ../userspace/bin/; \
	done

clean:
	for dir in $(BINS); do \
		$(MAKE) -C $$dir clean; \
	done
