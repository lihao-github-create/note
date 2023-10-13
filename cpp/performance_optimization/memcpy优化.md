# memcpy优化

在逛知乎的时候，看到有分析用SIMD指令集来加速，并且给出了分析报告。然后我clone了他们的代码，经测试发现，经SIMD优化的版本加速效果并不是那么明显，有时甚至更慢。甚至在随机size的benchmark中，memcpy明显占优。所以动了去看memcpy源码的想法。

## glibc memcpy实现

```c
#define OP_T_THRES 16
#define OPSIZ 8
void *
MEMCPY (void*dstpp, const void *srcpp, size_t len)
{
  unsigned long int dstp = (long int) dstpp;
  unsigned long int srcp = (long int) srcpp;

  /*Copy from the beginning to the end.*/

  /*If there not too few bytes to copy, use word copy.*/
  if (len >= OP_T_THRES)
    {
      /*Copy just a few bytes to make DSTP aligned.*/
      len -= (-dstp) % OPSIZ;
      BYTE_COPY_FWD (dstp, srcp, (-dstp) % OPSIZ);

      /* Copy whole pages from SRCP to DSTP by virtual address manipulation,
  as much as possible.  */

      PAGE_COPY_FWD_MAYBE (dstp, srcp, len, len);

      /* Copy from SRCP to DSTP taking advantage of the known alignment of
  DSTP.  Number of bytes remaining is put in the third argument,
  i.e. in LEN.  This number may vary from machine to machine.  */

      WORD_COPY_FWD (dstp, srcp, len, len);

      /* Fall out and copy the tail.  */
    }

  /*There are just a few bytes to copy.  Use byte memory operations.*/
  BYTE_COPY_FWD (dstp, srcp, len);

  return dstpp;
}
```

```c
#define BYTE_COPY_FWD(dst_bp, src_bp, nbytes)          \
  do               \
    {               \
      size_t __nbytes = (nbytes);           \
      while (__nbytes > 0)            \
 {              \
   byte __x = ((byte *) src_bp)[0];          \
   src_bp += 1;             \
   __nbytes -= 1;            \
   ((byte *) dst_bp)[0] = __x;           \
   dst_bp += 1;             \
 }              \
    } while (0)
```

```c
#define WORD_COPY_FWD(dst_bp, src_bp, nbytes_left, nbytes)        \
  do               \
    {               \
      if (src_bp % OPSIZ == 0)            \
 _wordcopy_fwd_aligned (dst_bp, src_bp, (nbytes) / OPSIZ);       \
      else              \
 _wordcopy_fwd_dest_aligned (dst_bp, src_bp, (nbytes) / OPSIZ);       \
      src_bp += (nbytes) & -OPSIZ;           \
      dst_bp += (nbytes) & -OPSIZ;           \
      (nbytes_left) = (nbytes) % OPSIZ;           \
    } while (0)
```

```c
#define PAGE_COPY_FWD(dstp, srcp, nbytes_left, nbytes)         \
  ((nbytes_left) = ((nbytes)            \
      - (__vm_copy (__mach_task_self (),         \
      (vm_address_t) srcp, trunc_page (nbytes),   \
      (vm_address_t) dstp) == KERN_SUCCESS       \
         ? trunc_page (nbytes)          \
         : 0)))
```

## 参考

1. [比memcpy还要快的内存拷贝，了解一下？](https://www.cnblogs.com/t-bar/p/17262147.html)
