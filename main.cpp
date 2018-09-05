#include<iostream>
#include<cstdio>
#include "zlib.h"

typedef unsigned int gz_size;


void gzinflate(std::string in, gz_size d_size, std::string &out) {
  const char* s = in.c_str();
  gz_size len = in.length();

  z_stream z;
  z.zalloc = Z_NULL;
  z.zfree = Z_NULL;
  z.opaque = Z_NULL;
  if (inflateInit(&z) != Z_OK) {
    printf("error: inflateInit: %s\n", (z.msg) ? z.msg : "???");
  }

  z.next_in = (Bytef *)(s);
  z.avail_in = len;
  gz_size *sl = (gz_size *)s;

  char *d_ptr = new char[ d_size ];
  if (!d_ptr) {
    printf("error:  memory allocation failure: %d\n", d_size);
    return;
  }

  z.next_out = (Bytef *)d_ptr;
  z.avail_out = d_size;
  int r;
  if ((r=inflate(&z, Z_NO_FLUSH)) < 0) {
    printf("error: inflate: %d:%s\n", r, (z.msg) ? z.msg : "???");
    return;
  }
  if (inflateEnd(&z) != Z_OK) {
    printf("error: inflateEnd: %s\n", (z.msg) ? z.msg : "???");
    return;
  }

  out = std::string(d_ptr, d_size);
  delete [] d_ptr;
  return;
}

gz_size gzdeflate(std::string in, std::string &out) {
  const char* s = in.c_str();
  gz_size len = in.length();

  z_stream z;
  z.zalloc = Z_NULL;
  z.zfree = Z_NULL;
  z.opaque = Z_NULL;
  if (deflateInit( &z, Z_DEFAULT_COMPRESSION ) != Z_OK) {
    printf("error: deflateInit: %s\n", (z.msg) ? z.msg : "???");
    return -1;
  }
  z.next_in = (Bytef *)(s);
  z.avail_in = len;
  gz_size d_size = (gz_size)(len*1.1) + 0x100;

  char *d_ptr = new char[ d_size ];
  if (!d_ptr) {
    printf("error:  memory allocation failure: %d\n", d_size);
    return -1;
  }

  z.next_out = (Bytef *)d_ptr;
  z.avail_out = d_size;

  int status;
  while (true) {
    status = deflate( &z, Z_FINISH );
    if (status == Z_STREAM_END) break;
    if (status != Z_OK) break;
  }
  d_size = d_size - z.avail_out;
  if (deflateEnd(&z) != Z_OK) {
    delete [] d_ptr;
    printf("error: deflateEnd: %s\n", (z.msg) ? z.msg : "???");
    return -1;
  }

  out = std::string(d_ptr, d_size);
  delete [] d_ptr;
  return len;
}

int main() {
  std::string s = "hello world";

  std::string compressed;
  std::string decompressed;

  gz_size d_size = gzdeflate(s, compressed);

  for (int i=0 ; i<compressed.length() ; i++) {
    printf("%02x", compressed.c_str()[i]);
  }
  printf("\n");

  gzinflate(compressed, d_size, decompressed);

  std::cout << decompressed << std::endl;
}


