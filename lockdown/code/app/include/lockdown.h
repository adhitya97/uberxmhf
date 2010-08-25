// lockdown specific declarations/definitions
// author: amit vasudevan (amitvasudevan@acm.org)

#ifndef __LOCKDOWN_H__
#define __LOCKDOWN_H__

#define LDN_ENV_TRUSTED_SIGNATURE  0x45555254   //"TRUE"
#define LDN_ENV_UNTRUSTED_SIGNATURE 0x45544E55  //"UNTE"

//specific defines for VT BOX
//#define LDN_ENV_TRUSTED_STARTSECTOR  (63)
//#define LDN_ENV_TRUSTED_ENDSECTOR  (12578894)
//#define LDN_NULL_SECTOR  (300000000)
//#define LDN_IDE_BUS   0xC000
//#define LDN_ALLOWED_SECTORS 0xFFFFFFFFUL

//specific defines for HP 8540p demo laptop
#define LDN_ENV_TRUSTED_STARTSECTOR  (159396993)
#define LDN_ENV_TRUSTED_ENDSECTOR  (222291404)
#define LDN_NULL_SECTOR  (500000000)
#define LDN_IDE_BUS   0x1F0
#define LDN_ALLOWED_SECTORS 159396930


#ifndef __ASSEMBLY__

typedef struct {
  u32 signature;  //trusted or untrusted env. being switched to

} __attribute__((packed)) LDNPB;


#endif //__ASSEMBLY__

#endif //__LOCKDOWN_H__