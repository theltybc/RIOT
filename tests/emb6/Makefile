# overwrite board, do not set native as default
BOARD ?= samr21-xpro
include ../Makefile.tests_common

# MSP-430 doesn't support C11's atomic functionality yet
FEATURES_BLACKLIST += arch_msp430

USEPKG += emb6

USEMODULE += emb6_router
USEMODULE += emb6_sock_udp
USEMODULE += ipv6_addr
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += od

# define the driver to be used for selected boards
ifneq (,$(filter samr21-xpro,$(BOARD)))
  DRIVER := at86rf233
endif
ifneq (,$(filter iotlab-m3 fox,$(BOARD)))
  DRIVER := at86rf231
endif
ifneq (,$(filter mulle,$(BOARD)))
  DRIVER := at86rf212b
endif

# use the at86rf231 as fallback device
DRIVER ?= at86rf231

# include the selected driver
USEMODULE += $(DRIVER)

CFLAGS += -Wno-unused-parameter -Wno-unused-function -Wno-type-limits
CFLAGS += -Wno-sign-compare -Wno-missing-field-initializers

include $(RIOTBASE)/Makefile.include
