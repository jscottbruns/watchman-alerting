//add by Golden 20111030 for x86 LinPAC(LX800 and Atom)

#if LINUX_VERSION_CODE == KERNEL_VERSION(2,6,18)
#define x86
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2,6,26)
#define x86
#elif LINUX_VERSION_CODE == KERNEL_VERSION(2,6,33)
#define x86
#else
#define PXA270
#endif
