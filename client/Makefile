PHONY = clean
IMGDIR = ../imgs/
PARAM = 127.0.0.1 9090 $(IMGDIR)img.jpg 1 1

run:
	cd src && make
	cd src && ./client $(PARAM)

clean:
	cd src && make clean