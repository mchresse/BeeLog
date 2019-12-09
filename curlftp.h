/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/*******************************************************************************
 * File:   curlftp.h
 * Author: Randolph Esser - Copyright 2017-2019
 * Created on 12. Februar 2017, 20:15
 *
 * This file is part of the "beelog" program/project.
 *******************************************************************************
 */

#ifndef CURLFTP_H
#define CURLFTP_H

#ifdef __cplusplus
extern "C" {
#endif

int ftpput(CURL *curlhandle, char* url, char* srcpath, char* srcname, char* dstpath, char* dstname,
		char * username, char * passwd, char* ftpport, char* proxy, char* proxyport);
int ftpget(char* url, char* srcpath, char* srcname, char* dstpath, char* dstname,
		char * username, char * passwd, char* ftpport, char* proxy, char* proxyport);

#ifdef __cplusplus
}
#endif

#endif /* CURLFTP_H */

