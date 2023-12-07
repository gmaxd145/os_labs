.PHONY: all, run, clean

run: all
clean:
	rm receiver
	rm sender
all:
	gcc sender.c -o sender -lpthread
	gcc receiver.c -o receiver -lpthread
