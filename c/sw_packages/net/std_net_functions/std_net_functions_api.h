
#ifndef _std_net_functions_api_h_
#define _std_net_functions_api_h_

/*****************  defines  **************/


/**********  define API  types ************/

#ifdef  __cplusplus
extern "C" {
#endif

uint16_t htons(uint16_t n);
uint16_t ntohs(uint16_t n);
uint32_t htonl(uint32_t n);
uint32_t ntohl(uint32_t n);
uint8_t std_net_functions_api_register_net_device(struct dev_desc_t * ap_dev);
struct hostent*  gethostbyname(const char *name);
struct servent *getservbyname(const char *name, const char *proto);



#ifdef  __cplusplus
}
#endif

#endif
