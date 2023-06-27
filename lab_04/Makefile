CONFIG_MODULE_SIG=n
# obj-m += my_module.o

# all:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# clean:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
TARGET1 = md1
TARGET2 = md2
TARGET3 = md3

obj-m := $(TARGET1).o $(TARGET2).o $(TARGET3).o
default: 
				make -C $(KDIR) M=$(PWD) modules 
clean: 
				@rm -f *.o .*.cmd .*.flags *.mod.c *.order *.mod *.ko *.symvers 
				@rm -f .*.*.cmd *~ *.*~ TODO.* .*.d
				@rm -fR .tmp* 
				@rm -rf .tmp_versions 
disclean: clean 
				@rm *.ko *.symvers
