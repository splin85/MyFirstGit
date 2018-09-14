/**
 * Copyright (C) 2017 Bei Jing Fu Hua Yu Qi Info Tech, Inc
 *
 * @file dot1x.h
 * @author Ou Fan
 * @date 2018-04-23
 */


#ifndef __RPC_DOT1X_H__
#define __RPC_DOT1X_H__


int rpc_dot1x_port_auth_mode_set(int port, int mode);
int rpc_dot1x_port_auth_egress_set(int port, int enable);

#endif

