AC_INIT([uxmhf-rpi3], [5.0])


###### autoconf general options
AC_SUBST([prefix])

###### simple configuration of tools. (just looks for matching binary on PATH)
AC_CHECK_PROG([MAKE], [make], [make])
AC_CHECK_PROG([CCERT], [arm-linux-gnueabihf-gcc], [arm-linux-gnueabihf-gcc])
AC_CHECK_PROG([CC], [arm-linux-gnueabihf-gcc], [arm-linux-gnueabihf-gcc])
AC_CHECK_PROG([AS], [arm-linux-gnueabihf-as], [arm-linux-gnueabihf-as])
AC_CHECK_PROG([AR], [arm-linux-gnueabihf-ar], [arm-linux-gnueabihf-ar])
AC_CHECK_PROG([RM], [rm -f], [rm -f])
AC_CHECK_PROG([CP], [cp], [cp])
AC_CHECK_PROG([MKDIR], [mkdir], [mkdir])
AC_CHECK_PROG([LD], [arm-linux-gnueabihf-ld], [arm-linux-gnueabihf-ld])
AC_CHECK_PROG([OBJCOPY], [arm-linux-gnueabihf-objcopy], [arm-linux-gnueabihf-objcopy])
AC_CHECK_PROG([CAT], [cat], [cat])



###### uxmhf-rpi3 specific options

# core start address
AC_SUBST([CORE_STARTADDR])
AC_ARG_WITH([core_startaddr],
        AS_HELP_STRING([--with-core-startaddr=@<:@STARTADDR@:>@],
                [select core_startaddr (0x1000000)]),
                , [with_core_startaddr=0x1000000])
CORE_STARTADDR=$[]with_core_startaddr

# core size
AC_SUBST([CORE_SIZE])
AC_ARG_WITH([core_size],
        AS_HELP_STRING([--with-core-size=@<:@SIZE@:>@],
                [select core_size (0xC00000)]),
                , [with_core_size=0xC00000])
CORE_SIZE=$[]with_core_size


# boot partition start sector
AC_SUBST([BOOT_PARTITION_START])
AC_ARG_WITH([boot_partition_start],
        AS_HELP_STRING([--with-boot-partition-start=@<:@SECTORNUM@:>@],
                [select boot_partition_start (8192)]),
                , [with_boot_partition_start=8192])
BOOT_PARTITION_START=$[]with_boot_partition_start

# boot partition end sector
AC_SUBST([BOOT_PARTITION_END])
AC_ARG_WITH([boot_partition_end],
        AS_HELP_STRING([--with-boot-partition-end=@<:@SECTORNUM@:>@],
                [select boot_partition_end (137215)]),
                , [with_boot_partition_end=137215])
BOOT_PARTITION_END=$[]with_boot_partition_end

# selectively enable/disable secure boot
AC_SUBST([SECBOOT])
AC_ARG_ENABLE([secboot],
        AS_HELP_STRING([--enable-secboot@<:@=yes|no@:>@],
                [enable Secure Boot]),
                , [enable_secboot=no])
AS_IF([test "x${enable_secboot}" != "xno"],
      [SECBOOT=y],
      [SECBOOT=n])

# selectively enable/disable DMA protection
AC_SUBST([DMAPROT])
AC_ARG_ENABLE([dmaprot],
        AS_HELP_STRING([--enable-dmaprot@<:@=yes|no@:>@],
                [enable DMA Protection]),
                , [enable_dmaprot=no])
AS_IF([test "x${enable_dmaprot}" != "xno"],
      [DMAPROT=y],
      [DMAPROT=n])

# selectively enable/disable interrupt protection
AC_SUBST([INTPROT])
AC_ARG_ENABLE([intprot],
        AS_HELP_STRING([--enable-intprot@<:@=yes|no@:>@],
                [enable Interrupt Protection]),
                , [enable_intprot=no])
AS_IF([test "x${enable_intprot}" != "xno"],
      [INTPROT=y],
      [INTPROT=n])

# selectively enable/disable FIQ reflection
AC_SUBST([FIQREFLECTION])
AC_ARG_ENABLE([fiqreflection],
        AS_HELP_STRING([--enable-fiqreflection@<:@=yes|no@:>@],
                [enable FIQ interrupt Reflection]),
                , [enable_fiqreflection=no])
AS_IF([test "x${enable_fiqreflection}" != "xno"],
      [FIQREFLECTION=y],
      [FIQREFLECTION=n])


###### autoconf output files
AC_CONFIG_FILES(
			[./include/config.h Makefile]
	)

AC_OUTPUT
