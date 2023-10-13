#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef DEBUG

#define MEM_CONTENT_LENGTH 100

#define malloc(size) malloc_hook(size, __FILE__, __LINE__)
#define free(p) free_hook(p, __FILE__, __LINE__)

// 原始的 malloc 函数
void *(*original_malloc)(size_t size) = NULL;
void *(*original_free)(void *p) = NULL;
// malloc hook 函数
void *malloc_hook(size_t size, const char *file, int line);
void free_hook(void *p, const char *file, int line);

void init_hook() {
  // 如果原始 malloc 函数尚未初始化，初始化它
  if (!original_malloc) {
    original_malloc = dlsym(RTLD_NEXT, "malloc");
  }
  // 如果原始 free 函数尚未初始化，初始化它
  if (!original_free) {
    original_free = dlsym(RTLD_NEXT, "free");
  }
}

#endif

int main() {
#ifdef DEBUG
  init_hook();
#endif
  // 使用 malloc 分配内存
  int *arr = (int *)malloc(5 * sizeof(int));

  // 为了演示效果，简单地初始化数组元素并释放内存
  for (int i = 0; i < 5; i++) {
    arr[i] = i * 2;
  }

  // 打印数组元素
  printf("Array elements: ");
  for (int i = 0; i < 5; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");

  // 释放内存
  // free(arr);

  return 0;
}

#ifdef DEBUG
// 重定向的 malloc 函数
void *malloc_hook(size_t size, const char *file, int line) {
  void *ptr = original_malloc(size);
  char buff[MEM_CONTENT_LENGTH] = {0};
  sprintf(buff, "./mem/%p.mem", ptr);

  FILE *fp = fopen(buff, "w");

  fprintf(fp, "[%s:%d]malloc --> addr: %p\n", file, line, ptr);
  // printf("[%s:%d]malloc --> addr: %p\n", file, line, ptr);
  fflush(fp);
  return ptr;
}

void free_hook(void *p, const char *file, int line) {
  char buff[MEM_CONTENT_LENGTH] = {0};
  sprintf(buff, "./mem/%p.mem", p);
  if (unlink(buff) < 0) {
    printf("double free: %p\n", p);
  }
  original_free(p);
}

#endif