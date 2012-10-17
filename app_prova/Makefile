KDIR := ~/progetti/linux
PATH_INITRAMFS = $(KDIR)/arch/openrisc/support
PATH_APP_DIR = $(PATH_INITRAMFS)/initramfs
APP_FILES = app_prova1


app_prova1: app.c
	@ or32-linux-gcc -o app_prova1 app.c
	@ echo "GEN app_prova1"


clean:
	@ rm -f $(APP_FILES)
	@ echo "CLEAN $(APP_FILES)"



install: $(APP_FILES)
	@ cp $(APP_FILES) $(PATH_APP_DIR)
	@ echo "INSTALL app bins"
	

uninstall: $(APP_FILES)
	@ $(foreach file, $(APP_FILES), rm $(PATH_APP_DIR)/$(file);)
	@ echo "UNINSTALL app bins"
