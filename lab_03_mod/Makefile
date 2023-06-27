CONFIG_MODULE_SIG=n
# obj-m += my_module.o

# all:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# clean:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)

obj-m := my_module.o
default: 
				make -C $(KDIR) M=$(PWD) modules 
clean: 
				@rm -f *.o .*.cmd .*.flags *.mod.c *.order *.mod *.ko *.symvers 
				@rm -f .*.*.cmd *~ *.*~ TODO.* .*.d
				@rm -fR .tmp* 
				@rm -rf .tmp_versions 
disclean: clean 
				@rm *.ko *.symvers
