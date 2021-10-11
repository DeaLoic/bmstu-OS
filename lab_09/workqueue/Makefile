CURRENT = $(shell uname -r)
KDIR = /lib/modules/$(CURRENT)/build
PWD = $(shell pwd)
obj-m := md.o

default: 
	$(MAKE) -C $(KDIR) M=$(PWD)

clean: 
	@rm -f *.o .*.cmd .*.flags *.mod.c *.order 
	@rm -f .*.*.cmd *~ *.*~ TODO.* 
	@rm -fR .tmp* 
	@rm -fR *.ko 
	@rm -fR *.symvers
	@rm -fR *.mod
	@rm -rf .tmp_versions 

disclean: clean 
	@rm *.ko *.symvers
