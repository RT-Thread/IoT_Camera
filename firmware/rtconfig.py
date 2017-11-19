import os
import sys
import re

# toolchains options
ARCH         = 'arm'
CPU          = 'armv6'
OUTPUT_NAME  = 'rtthread'
CROSS_TOOL   = 'gcc' # we use gcc compiler always
PLATFORM     = 'gcc'
LD_NAME      = 'link'

EXEC_PATH    = r'arm-2014.05/bin'
if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

if os.getenv('RTT_ROOT'):
    RTT_ROOT = os.getenv('RTT_ROOT')

BUILD = 'release'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX = 'arm-none-eabi-'
    CC  = PREFIX + 'gcc'
    CXX = PREFIX + 'g++'
    AS  = PREFIX + 'gcc'
    AR  = PREFIX + 'ar'
    LINK = PREFIX + 'g++'
    TARGET_EXT = '.elf'
    SIZE = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY = PREFIX + 'objcopy'
    STRIP = PREFIX + 'strip'

    DEVICE = ' -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=soft'
    CFLAGS = DEVICE + ' -mno-unaligned-access -Wall'
    CFLAGS += ' -Wno-error=sequence-point '
    CFLAGS += ' -fno-strict-aliasing '
    CFLAGS += ' -Wno-address '
    AFLAGS = ' -c' + DEVICE + ' -x assembler-with-cpp -D__ASSEMBLY__'
    LFLAGS = DEVICE + ' -Wl,--gc-sections,-Map='+ OUTPUT_NAME +'.map,-cref,-u,_start -T' + LD_NAME +'.ld'
    CPATH = ''
    LPATH = ''
    if BUILD == 'debug':
        CFLAGS += ' -O0 -gdwarf-2 '
        AFLAGS += ' -gdwarf-2'
    else:
        CFLAGS += ' -O2'

    CXXFLAGS = CFLAGS
    POST_ACTION = OBJCPY + ' -O binary $TARGET '+ OUTPUT_NAME +'.bin\n' + SIZE + ' $TARGET \n'

    M_CFLAGS = CFLAGS.replace('-O0 -gdwarf-2', '-O2') + ' -mlong-calls -fPIC '
    M_CXXFLAGS = CXXFLAGS.replace('-O0 -gdwarf-2', '-O2') + ' -mlong-calls -fPIC'
    M_LFLAGS = DEVICE + CXXFLAGS + ' -Wl,--gc-sections,-z,max-page-size=0x4' +\
                                    ' -shared -fPIC -nostartfiles -static-libgcc'
    M_POST_ACTION = STRIP + ' -R .hash $TARGET\n' + SIZE + ' $TARGET \n'
