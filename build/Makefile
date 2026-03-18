all: app

app: comm
	cd ../demo &&make clean	\
	&&cd ../demo && make
	cp ../demo/obj_rv1126/rv1126_ipc ./sample_demo
comm:
	cd ../ai &&make

clean:
	cd ../demo && make clean
	cd ../ai && make clean
	rm rv1126_ipc
	rm sample_demo
