run : build
	./testCIO.out
	./testCIOths.out
	./testKernelIO.out
	./testKernelIOths.out
	./testWrite.out
	@cat result.txt
	@echo ""
	./testWriteths.out
	@cat resultths.txt
	@echo "" 

build : testCIO.c testCIOths.c testKernelIO.c testKernelIOths.c
	gcc testCIO.c -o testCIO.out
	gcc testCIOths.c -o testCIOths.out
	gcc testKernelIO.c -o testKernelIO.out
	gcc testKernelIOths.c -o testKernelIOths.out
	gcc testWrite.c -o testWrite.out
	gcc testWriteths.c -o testWriteths.out
	gcc testWriteKths.c -o testWriteKths.out
	gcc testWriteK.c -o testWriteK.out

clean :
	rm *.out
