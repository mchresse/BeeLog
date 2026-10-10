/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
#include <stdio.h>
#include <string.h>

#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "get1wire.h"
#include "getini.h"
#include "beehive.h"
#include "beelog.h"


/* <DESC>
 * Performs an FTP upload and renames the file just after a successful
 * transfer.
 * </DESC>
 */

/* NOTE: if you want this example to work on Windows with libcurl as a
   DLL, you MUST also provide a read callback with CURLOPT_READFUNCTION.
   Failing to do so will give you a crash since a DLL may not use the
   variable's memory when passed in to it from an app like this. */
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
  curl_off_t nread;
  /* in real-world cases, this would probably get this data differently
     as this fread() stuff is exactly what the library already would do
     by default internally */
  size_t retcode = fread(ptr, size, nmemb, stream);

  nread = (curl_off_t)retcode;

  // BHLOG(LOGXFER) fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T " bytes from file\n", nread);
  return retcode;
}

int ftpput(CURL *curl, char* url, char* srcpath, char* srcname, char* dstpath, char* dstname,
		char * username, char * passwd, char* ftpport, char* proxy, char* proxyport){
  
  CURLcode		res = CURLE_GOT_NOTHING;
  FILE *		hd_src;
  struct stat	file_info;
  curl_off_t	fsize;

  char sbuf[1024];
  char dbuf[1024];
  char pbuf[1024];
  
	sprintf(sbuf,    "%s/%s", srcpath, srcname);				// full source path + name
	sprintf(dbuf,    "ftp://%s:%s/%s/%s", url, ftpport, dstpath, dstname);	// full dest.-path+name
	if(*proxy != '\0'){
		sprintf(pbuf,    "socks5://%s:%s", proxy, proxyport);					// full SOCKS5 proxy server
	}
	
	// get the file size of the local file
	if(stat(sbuf, &file_info)) {
		printf("    Export(ftpput): Could not open %s : %s\n", sbuf, strerror(errno));
		return 1;
	}
	fsize = (curl_off_t)file_info.st_size;
	BHLOG(LOGXFER) printf("    Export: From %s to %s (size: %" CURL_FORMAT_CURL_OFF_T "By)\n", sbuf, dbuf, fsize);

	// open local source file
	hd_src = fopen(sbuf, "rb");
	if(!hd_src){
		BHLOG(LOGXFER) printf("    Export(ftpput): file open fails on file: %s\n", sbuf);
		return(2);
	}

  // Init the socket stuff and retreive curl handle
  // curl_global_init(CURL_GLOBAL_ALL);		// => to be done in main function
  // curl = curl_easy_init();				// => to be done in main function
	
  if(curl) {
	curl_easy_reset(curl);	// reset all Easy_setopt params and remaining open connections

    // enable uploading
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    // specify target
    curl_easy_setopt(curl, CURLOPT_URL, dbuf);
    
	// create unknown directories
	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLFTP_CREATE_DIR);

    // we want to use our own read function
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

    // now specify which file to upload
    curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);
	
	// set proxy if needed
	if(*proxy != '\0')	{
		curl_easy_setopt(curl, CURLOPT_PROXY, pbuf);
		BHLOG(LOGXFER) printf("    Export(ftpput): proxy: %s used\n", pbuf);
	}
	// preset user account data of ftp site
	if(username == NULL){
		// or the more secure $(HOME)/.netrc file access
		// in this case username & passwd can be empty
		curl_easy_setopt(curl, CURLOPT_NETRC, 1L); 
		BHLOG(LOGXFER) printf("    Export(ftpput): .netrc authentication used\n");
	}else{
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, passwd); 
	}

    /* Set the size of the file to upload (optional).  If you give a *_LARGE
       option you MUST make sure that the type of the passed-in argument is a
       curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
       make sure that to pass in a type 'long' argument. */
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);

	// Switch on full protocol/debug output
    // BHLOG(LOGXFER) curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // Now run off and do what you've been told!
    res = curl_easy_perform(curl);

    // Check for errors
	curlcount++;			// just count # of curllib calls for debugging reasons
  }	
	
  fclose(hd_src);		// close the source file

  // always cleanup
  // curl_easy_cleanup(curl);   => to be done in main function only
  // curl_global_cleanup();
  
  if(res != CURLE_OK){
	printf ("res = %d\n",res);
    printf("    Export: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	return 3;
  }

  BHLOG(LOGBH) printf("    Export: to %s (size %" CURL_FORMAT_CURL_OFF_T " bytes) done (#%i).\n", dbuf, fsize, curlcount);

return 0;
}

/*
 * Get a single file from an FTP server.
 * 
 */

struct FtpFile {
  char *filename;
  FILE *stream;
};

static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream) {
  struct FtpFile *out=(struct FtpFile *)stream;
 
  if(out && !out->stream) {
    /* open file for writing */
    out->stream=fopen(out->filename, "wb");
    if(!out->stream){
	  printf("    Export(my_fwrite): failure, can't open file %s to write", out->filename);
      return -1; /* failure, can't open file to write */
	}
  }
  return fwrite(buffer, size, nmemb, out->stream);
}


int ftpget(char* url, char* srcpath, char* srcname, char* dstpath, char* dstname,
		char * username, char * passwd, char* ftpport, char* proxy, char* proxyport) {
  CURL *curl;
  CURLcode res;

  struct FtpFile ftpfile={ "", NULL};

  char sbuf[1024];
  char dbuf[1024];
  char pbuf[1024];
  
	sprintf(sbuf,    "ftp://%s:%s/%s/%s", url, ftpport, srcpath, srcname);	// full FTP source path + name
	sprintf(dbuf,    "%s/%s", dstpath, dstname);				// local full dest.-path + name
	ftpfile.filename= dbuf;
	ftpfile.stream	= NULL;

	BHLOG(LOGXFER) printf("    Import: FTP-Get from %s to %s\n", sbuf, dbuf);

	if(*proxy != '\0'){
		sprintf(pbuf,    "socks5://%s:%s", proxy, proxyport);					// full SOCKS5 proxy server
		BHLOG(LOGXFER) printf("    Import: Used proxy:%s\n",pbuf);
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();			// get curl handler
	if(curl) {
	// preset user account data of ftp site
	if(username == NULL){
		// or the more secure $(HOME)/.netrc file access
		// in this case username & paswd can be empty
		curl_easy_setopt(curl, CURLOPT_NETRC, 1L); 
		BHLOG(LOGXFER) printf("    Import: .netrc authentication used\n");
	}else{
		curl_easy_setopt(curl, CURLOPT_USERNAME, username);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, passwd); 
	}


	// set proxy if needed
	if(*proxy != '\0')
		curl_easy_setopt(curl, CURLOPT_PROXY, pbuf);
	
    curl_easy_setopt(curl, CURLOPT_URL, sbuf);		// set ftp source path

    // Define our callback to get called when there's data to be written
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);

    // Set a pointer to our struct to pass to the callback
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

    // We activate SSL and we require it for both control and data
	//    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);

    // Switch on full protocol/debug output
	BHLOG(LOGXFER) 
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    res = curl_easy_perform(curl);

    // always cleanup
    curl_easy_cleanup(curl);

    if(res != CURLE_OK) {
		// we failed
		fprintf(stderr, "    Import: ftp-Get failed: %d\n", res);
    }
  }

  if(ftpfile.stream)
    fclose(ftpfile.stream); // close the local file

  curl_global_cleanup();
  if(res != CURLE_OK)
	  return(-1);
  BHLOG(LOGBH) 
	  printf("    Import: from %s to %s done.\n", sbuf, dbuf);

  return 0;
}