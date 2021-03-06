# uber eXtensible Micro-Hypervisor Framework (uberXMHF)

## Changelog

* Version 5.0
	* various documentation fixes
	* rpi3-cortex_a53-armv8_32: refactored secure-boot, interrupt protection, 
	  DMA protection, and FIQ reflection as modular build-time options
	* rpi3-cortex_a53-armv8_32: fixed stability issues within core micro-hypervisor framework
	* pc-intel-x86_32: migrated debug and uobject info library to core uberspark framework
	* pc-intel-x86_32: migrated data types to be stdint compatible
	* pc-intel-x86_32: removed micro-hypervisor specific dependencies on uobject info table
	* pc-intel-x86_32: added new uobject uhmpgtbl to deal with hypervisor page tables for
	  unverified hypervisor uobjects
	* pc-intel-x86_32: added new uobject iotbl to deal with hypervisor legacy I/O tables 
	  for unverified hypervisor uobjects
	* pc-intel-x86_32: revised exhub uobject to handle IDT initialization and operation
	* pc-intel-x86_32: refactored build process to eliminate redundant passes

* Version 4.1
	* added support for Ubuntu 16.04 LTS with Linux kernel 4.4.x 32-bits (CONFIG_X86_PAE=n)
	* migrated uberobject manifests to JSON format
	* various documentation updates

* Version 4.0 
	* first stand-alone uberXMHF release
	* added Raspberry PI 3 hardware platform support
	* consolidated past XMHF x86-32 AMD PC and x86-32 Intel PC (legacy) releases
	* various documentation updates

* Version 3.1 
	* fixed uxmhf build errors

* Version 3.0 
	* added support for Frama-C Phosphorus-20170501
	* added support for Compcert 3.0.1
	* fixed error due to improper inclusion of xh_ssteptrace in the verification process
	* minor build harness fixes and documentation updates

* Version 2.0 
	* separated uberspark, uberspark libraries and uxmhf verification/build processes
	* refined and streamlined uberspark and uxmhf verification/build harness
	* fixed minor errors in documentation and updates to reflect release changes

* Version 1.0 
	* initial release of uberXMHF x86-32 Intel PC
  
* Version 0.2.2
    * various general documentation fixes and cleanup
    * tee-sdk: added patches for newlib and openssl libraries and removed deprecated/non-working examples 
    * re-organized framework components and revised configuration/build harness and related documentation	
    * fixed build errors with gcc 4.6.3
    * xmhf-core: re-factored verification harness and added support for 64-bit CBMC
  
* Version 0.2.1
	* tools: add scripts to deal with release tasks
	* xmhf-core: refactor runtime build harness
	* xmhf-core: add build debug information within generated binaries
	* xmhf-core: segregate Dynamic Root-of-Trust and DMA protection logic and build configuration options
	* xmhf-core: add support for upto 8 CPU cores 
	* xmhf-core: add XSETBV intercept handling on Intel platforms for CPUs with XSAVE capabilities 
	* xmhf-core: fix MTRR logic on Intel platforms to obtain required variable range MTRRs 
	* xmhf-core: fix issue related to physical/virtual address overlap for runtime 
  
* Version 0.2
	* xmhf-core: clarify documentation and add description for build configuration options and verification
	* xmhf-core: add build configuration options --with-target-platform and --with-target-arch to choose target platform and CPU arch.
	* xmhf-core: restructure core components and general cleanup
	* xmhf-core: add XMHF/hypapp verification harness for verifying core memory integrity
	* xmhf-core: fix build error with --enable-debug-vga configure option
  
* Version 0.1.2
    * xmhf-core: stability improvements - we can now handle guest NMIs gracefully
    * xmhf-core: stability improvements - we now support stock MTRR-enabled (linux) guest kernels on Intel platforms
    * test-bed fixes, refactoring and improvements - now supporting 3.2.0-27-generic (and below) with ubuntu
    * added documentation generator which takes in-tree markdown files and generates html output
    * fixed build target install-bin to include correct destination path
  
* Version 0.1.1
    * Added TPM performance profiling.
    * Stability improvements.
    * Intercept handling now serialized in the core.
    * XMHF now builds and runs on Ubuntu 12.04 (precise).
    * Replaced LGPL tlsf implementation with public domain implementation.
    * Added design-documents.
  
* Version 0.1 
   * Initial Release
  