

#ifndef __NTP_CLIENT_H__
#define __NTP_CLIENT_H__


typedef int (*rpc_ntp_config_change_cb)(const int enable);

int rpc_ntp_init(const int unit);
void rpc_ntp_config_change_register(rpc_ntp_config_change_cb cb);
int rpc_ntp_check_enable(void);

#endif

