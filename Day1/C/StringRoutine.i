# 0 ".\\StringRoutine.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 ".\\StringRoutine.c"
# 1 "C:/msys64/ucrt64/include/stdio.h" 1 3
# 9 "C:/msys64/ucrt64/include/stdio.h" 3
# 1 "C:/msys64/ucrt64/include/corecrt_stdio_config.h" 1 3
# 10 "C:/msys64/ucrt64/include/corecrt_stdio_config.h" 3
# 1 "C:/msys64/ucrt64/include/corecrt.h" 1 3
# 10 "C:/msys64/ucrt64/include/corecrt.h" 3
# 1 "C:/msys64/ucrt64/include/_mingw.h" 1 3
# 10 "C:/msys64/ucrt64/include/_mingw.h" 3
# 1 "C:/msys64/ucrt64/include/_mingw_mac.h" 1 3
# 108 "C:/msys64/ucrt64/include/_mingw_mac.h" 3
             
# 117 "C:/msys64/ucrt64/include/_mingw_mac.h" 3
             
# 326 "C:/msys64/ucrt64/include/_mingw_mac.h" 3
       
# 405 "C:/msys64/ucrt64/include/_mingw_mac.h" 3
       
# 11 "C:/msys64/ucrt64/include/_mingw.h" 2 3
# 1 "C:/msys64/ucrt64/include/_mingw_secapi.h" 1 3
# 12 "C:/msys64/ucrt64/include/_mingw.h" 2 3
# 306 "C:/msys64/ucrt64/include/_mingw.h" 3
# 1 "C:/msys64/ucrt64/include/vadefs.h" 1 3
# 9 "C:/msys64/ucrt64/include/vadefs.h" 3
# 1 "C:/msys64/ucrt64/include/_mingw.h" 1 3
# 10 "C:/msys64/ucrt64/include/vadefs.h" 2 3




#pragma pack(push,_CRT_PACKING)
# 24 "C:/msys64/ucrt64/include/vadefs.h" 3
  
# 24 "C:/msys64/ucrt64/include/vadefs.h" 3
 typedef __builtin_va_list __gnuc_va_list;






  typedef __gnuc_va_list va_list;
# 103 "C:/msys64/ucrt64/include/vadefs.h" 3
#pragma pack(pop)
# 307 "C:/msys64/ucrt64/include/_mingw.h" 2 3
# 604 "C:/msys64/ucrt64/include/_mingw.h" 3
void __attribute__((__cdecl__)) __debugbreak(void);
extern __inline__ __attribute__((__always_inline__,__gnu_inline__)) void __attribute__((__cdecl__)) __debugbreak(void)
{



  __asm__ __volatile__("int {$}3":);





}
# 625 "C:/msys64/ucrt64/include/_mingw.h" 3
void __attribute__((__cdecl__)) __attribute__ ((__noreturn__)) __fastfail(unsigned int _Code);
extern __inline__ __attribute__((__always_inline__,__gnu_inline__)) void __attribute__((__cdecl__)) __attribute__ ((__noreturn__)) __fastfail(unsigned int _Code)
{




  __asm__ __volatile__("int {$}0x29"::"c"(_Code));






  __builtin_unreachable();
}
# 665 "C:/msys64/ucrt64/include/_mingw.h" 3
const char *__mingw_get_crt_info (void);
# 11 "C:/msys64/ucrt64/include/corecrt.h" 2 3




#pragma pack(push,_CRT_PACKING)
# 35 "C:/msys64/ucrt64/include/corecrt.h" 3
__extension__ typedef unsigned long long size_t;
# 45 "C:/msys64/ucrt64/include/corecrt.h" 3
__extension__ typedef long long ssize_t;






typedef size_t rsize_t;
# 62 "C:/msys64/ucrt64/include/corecrt.h" 3
__extension__ typedef long long intptr_t;
# 75 "C:/msys64/ucrt64/include/corecrt.h" 3
__extension__ typedef unsigned long long uintptr_t;
# 88 "C:/msys64/ucrt64/include/corecrt.h" 3
__extension__ typedef long long ptrdiff_t;
# 98 "C:/msys64/ucrt64/include/corecrt.h" 3
typedef unsigned short wchar_t;







typedef unsigned short wint_t;
typedef unsigned short wctype_t;





typedef int errno_t;




typedef long __time32_t;




__extension__ typedef long long __time64_t;
# 138 "C:/msys64/ucrt64/include/corecrt.h" 3
typedef __time64_t time_t;
# 157 "C:/msys64/ucrt64/include/corecrt.h" 3
__attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _invalid_parameter_noinfo(void);
__attribute__ ((__dllimport__)) __attribute__ ((__noreturn__)) void __attribute__((__cdecl__)) _invalid_parameter_noinfo_noreturn(void);
__attribute__ ((__dllimport__)) __attribute__ ((__noreturn__)) void __attribute__((__cdecl__)) _invoke_watson(const wchar_t *expression, const wchar_t *function_name, const wchar_t *file_name, unsigned int line_number, long long unsigned int reserved);
# 437 "C:/msys64/ucrt64/include/corecrt.h" 3
struct threadlocaleinfostruct;
struct threadmbcinfostruct;
typedef struct threadlocaleinfostruct *pthreadlocinfo;
typedef struct threadmbcinfostruct *pthreadmbcinfo;
struct __lc_time_data;

typedef struct localeinfo_struct {
  pthreadlocinfo locinfo;
  pthreadmbcinfo mbcinfo;
} _locale_tstruct,*_locale_t;



typedef struct tagLC_ID {
  unsigned short wLanguage;
  unsigned short wCountry;
  unsigned short wCodePage;
} LC_ID,*LPLC_ID;
# 473 "C:/msys64/ucrt64/include/corecrt.h" 3
#pragma pack(pop)
# 11 "C:/msys64/ucrt64/include/corecrt_stdio_config.h" 2 3





unsigned long long* __attribute__((__cdecl__)) __local_stdio_printf_options(void);
unsigned long long* __attribute__((__cdecl__)) __local_stdio_scanf_options(void);
# 10 "C:/msys64/ucrt64/include/stdio.h" 2 3

#pragma pack(push,_CRT_PACKING)

       

       

       

       
# 33 "C:/msys64/ucrt64/include/stdio.h" 3
  struct _iobuf {

    void *_Placeholder;
# 46 "C:/msys64/ucrt64/include/stdio.h" 3
  };
  typedef struct _iobuf FILE;
# 99 "C:/msys64/ucrt64/include/stdio.h" 3
# 1 "C:/msys64/ucrt64/include/_mingw_off_t.h" 1 3




  typedef long _off_t;

  typedef long off32_t;





  __extension__ typedef long long _off64_t;

  __extension__ typedef long long off64_t;
# 26 "C:/msys64/ucrt64/include/_mingw_off_t.h" 3
typedef off32_t off_t;
# 100 "C:/msys64/ucrt64/include/stdio.h" 2 3

__attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) __acrt_iob_func(unsigned index);

  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) __iob_func(void);
# 112 "C:/msys64/ucrt64/include/stdio.h" 3
  __extension__ typedef long long fpos_t;
# 156 "C:/msys64/ucrt64/include/stdio.h" 3
extern
  __attribute__((__format__(__gnu_scanf__, 2,3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_sscanf(const char * __restrict__ _Src,const char * __restrict__ _Format,...);
extern
  __attribute__((__format__(__gnu_scanf__, 2,0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vsscanf (const char * __restrict__ _Str,const char * __restrict__ Format,va_list argp);
extern
  __attribute__((__format__(__gnu_scanf__, 1,2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_scanf(const char * __restrict__ _Format,...);
extern
  __attribute__((__format__(__gnu_scanf__, 1,0))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_vscanf(const char * __restrict__ Format, va_list argp);
extern
  __attribute__((__format__(__gnu_scanf__, 2,3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_fscanf(FILE * __restrict__ _File,const char * __restrict__ _Format,...);
extern
  __attribute__((__format__(__gnu_scanf__, 2,0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vfscanf (FILE * __restrict__ fp, const char * __restrict__ Format,va_list argp);

extern
  __attribute__((__format__(__gnu_printf__,3,0))) __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __mingw_vsnprintf(char * __restrict__ _DstBuf,size_t _MaxCount,const char * __restrict__ _Format,
                               va_list _ArgList);
extern
  __attribute__((__format__(__gnu_printf__,3,4))) __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __mingw_snprintf(char * __restrict__ s, size_t n, const char * __restrict__ format, ...);
extern
  __attribute__((__format__(__gnu_printf__,1,2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_printf(const char * __restrict__ , ... ) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__gnu_printf__,1,0))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_vprintf (const char * __restrict__ , va_list) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__gnu_printf__,2,3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_fprintf (FILE * __restrict__ , const char * __restrict__ , ...) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__gnu_printf__,2,0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vfprintf (FILE * __restrict__ , const char * __restrict__ , va_list) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__gnu_printf__,2,3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_sprintf (char * __restrict__ , const char * __restrict__ , ...) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__gnu_printf__,2,0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vsprintf (char * __restrict__ , const char * __restrict__ , va_list) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__gnu_printf__,2,3))) __attribute__((nonnull (1,2)))
  int __attribute__((__cdecl__)) __mingw_asprintf(char ** __restrict__ , const char * __restrict__ , ...) __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__gnu_printf__,2,0))) __attribute__((nonnull (1,2)))
  int __attribute__((__cdecl__)) __mingw_vasprintf(char ** __restrict__ , const char * __restrict__ , va_list) __attribute__ ((__nothrow__));

extern
  __attribute__((__format__(__ms_scanf__, 2,3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_sscanf(const char * __restrict__ _Src,const char * __restrict__ _Format,...)
  __asm__("sscanf");
extern
  __attribute__((__format__(__ms_scanf__, 2,0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vsscanf(const char * __restrict__ _Str,const char * __restrict__ _Format,va_list argp)
  __asm__("vsscanf");
extern
  __attribute__((__format__(__ms_scanf__, 1,2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_scanf(const char * __restrict__ _Format,...)
  __asm__("scanf");
extern
  __attribute__((__format__(__ms_scanf__, 1,0))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_vscanf(const char * __restrict__ _Format,va_list argp)
  __asm__("vscanf");
extern
  __attribute__((__format__(__ms_scanf__, 2,3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_fscanf(FILE * __restrict__ _File,const char * __restrict__ _Format,...)
  __asm__("fscanf");
extern
  __attribute__((__format__(__ms_scanf__, 2,0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vfscanf(FILE * __restrict__ _File,const char * __restrict__ _Format,va_list argp)
  __asm__("vfscanf");

extern
  __attribute__((__format__(__ms_printf__, 1,2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_printf(const char * __restrict__ , ... )
  __asm__("printf") __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__ms_printf__, 1,0))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_vprintf (const char * __restrict__ , va_list)
  __asm__("vprintf") __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__ms_printf__, 2,3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_fprintf (FILE * __restrict__ , const char * __restrict__ , ...)
  __asm__("fprintf") __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__ms_printf__, 2,0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vfprintf (FILE * __restrict__ , const char * __restrict__ , va_list)
  __asm__("vfprintf") __attribute__ ((__nothrow__))
;
extern
  __attribute__((__format__(__ms_printf__, 2,3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_sprintf (char * __restrict__ , const char * __restrict__ , ...)
  __asm__("sprintf") __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__ms_printf__, 2,0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vsprintf (char * __restrict__ , const char * __restrict__ , va_list)
  __asm__("vsprintf") __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__ms_printf__, 3,4))) __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __ms_snprintf (char * __restrict__ , size_t , const char * __restrict__ , ...)
  __asm__("snprintf") __attribute__ ((__nothrow__));
extern
  __attribute__((__format__(__ms_printf__, 3,0))) __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __ms_vsnprintf (char * __restrict__ , size_t , const char * __restrict__ , va_list)
  __asm__("vsnprintf") __attribute__ ((__nothrow__));


  int __attribute__((__cdecl__)) __stdio_common_vsprintf(unsigned long long options, char *str, size_t len, const char *format, _locale_t locale, va_list valist);
  int __attribute__((__cdecl__)) __stdio_common_vfprintf(unsigned long long options, FILE *file, const char *format, _locale_t locale, va_list valist);
  int __attribute__((__cdecl__)) __stdio_common_vsscanf(unsigned long long options, const char *input, size_t length, const char *format, _locale_t locale, va_list valist);
  int __attribute__((__cdecl__)) __stdio_common_vfscanf(unsigned long long options, FILE *file, const char *format, _locale_t locale, va_list valist);
# 444 "C:/msys64/ucrt64/include/stdio.h" 3
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

  __attribute__((__format__ (__gnu_printf__, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) fprintf(FILE * __restrict__ _File,const char * __restrict__ _Format,...);
  __attribute__((__format__ (__gnu_printf__, 1, 2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) printf(const char * __restrict__ _Format,...);
  __attribute__((__format__ (__gnu_printf__, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) sprintf(char * __restrict__ _Dest,const char * __restrict__ _Format,...) ;

  __attribute__((__format__ (__gnu_printf__, 2, 0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) vfprintf(FILE * __restrict__ _File,const char * __restrict__ _Format,va_list _ArgList);
  __attribute__((__format__ (__gnu_printf__, 1, 0))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) vprintf(const char * __restrict__ _Format,va_list _ArgList);
  __attribute__((__format__ (__gnu_printf__, 2, 0))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) vsprintf(char * __restrict__ _Dest,const char * __restrict__ _Format,va_list _Args) ;

 
  __attribute__((__format__ (__gnu_scanf__, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) fscanf(FILE * __restrict__ _File,const char * __restrict__ _Format,...);
 
  __attribute__((__format__ (__gnu_scanf__, 1, 2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) scanf(const char * __restrict__ _Format,...);
 
  __attribute__((__format__ (__gnu_scanf__, 2, 3))) __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) sscanf(const char * __restrict__ _Src,const char * __restrict__ _Format,...);







  __attribute__((__format__ (__gnu_scanf__, 2, 0))) __attribute__ ((__nonnull__ (2)))
  int vfscanf (FILE *__stream, const char *__format, __builtin_va_list __local_argv);

  __attribute__((__format__ (__gnu_scanf__, 2, 0))) __attribute__ ((__nonnull__ (2)))
  int vsscanf (const char * __restrict__ __source, const char * __restrict__ __format, __builtin_va_list __local_argv);
  __attribute__((__format__ (__gnu_scanf__, 1, 0))) __attribute__ ((__nonnull__ (1)))
  int vscanf(const char *__format, __builtin_va_list __local_argv);


#pragma GCC diagnostic pop
# 535 "C:/msys64/ucrt64/include/stdio.h" 3
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _filbuf(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _flsbuf(int _Ch,FILE *_File);



  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _fsopen(const char *_Filename,const char *_Mode,int _ShFlag);

  void __attribute__((__cdecl__)) clearerr(FILE *_File);
  int __attribute__((__cdecl__)) fclose(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fcloseall(void);



  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _fdopen(int _FileHandle,const char *_Mode);

  int __attribute__((__cdecl__)) feof(FILE *_File);
  int __attribute__((__cdecl__)) ferror(FILE *_File);
  int __attribute__((__cdecl__)) fflush(FILE *_File);
  int __attribute__((__cdecl__)) fgetc(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fgetchar(void);
  int __attribute__((__cdecl__)) fgetpos(FILE * __restrict__ _File ,fpos_t * __restrict__ _Pos);
  int __attribute__((__cdecl__)) fgetpos64(FILE * __restrict__ _File ,fpos_t * __restrict__ _Pos);
  char *__attribute__((__cdecl__)) fgets(char * __restrict__ _Buf,int _MaxCount,FILE * __restrict__ _File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fileno(FILE *_File);







  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _tempnam(const char *_DirName,const char *_FilePrefix);



  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _flushall(void);
  FILE *__attribute__((__cdecl__)) fopen(const char * __restrict__ _Filename,const char * __restrict__ _Mode) ;
  FILE *__attribute__((__cdecl__)) fopen64(const char * __restrict__ filename,const char * __restrict__ mode);
  int __attribute__((__cdecl__)) fputc(int _Ch,FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fputchar(int _Ch);
  int __attribute__((__cdecl__)) fputs(const char * __restrict__ _Str,FILE * __restrict__ _File);
  size_t __attribute__((__cdecl__)) fread(void * __restrict__ _DstBuf,size_t _ElementSize,size_t _Count,FILE * __restrict__ _File);
  FILE *__attribute__((__cdecl__)) freopen(const char * __restrict__ _Filename,const char * __restrict__ _Mode,FILE * __restrict__ _File) ;
  FILE *__attribute__((__cdecl__)) freopen64(const char * __restrict__ _Filename,const char * __restrict__ _Mode,FILE * __restrict__ _File);
  int __attribute__((__cdecl__)) fsetpos(FILE *_File,const fpos_t *_Pos);
  int __attribute__((__cdecl__)) fsetpos64(FILE *_File,const fpos_t *_Pos);
  int __attribute__((__cdecl__)) fseek(FILE *_File,long _Offset,int _Origin);
  long __attribute__((__cdecl__)) ftell(FILE *_File);

  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fseeki64(FILE *_File,long long _Offset,int _Origin);
  __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _ftelli64(FILE *_File);



  int __attribute__((__cdecl__)) fseeko(FILE *_File, _off_t _Offset, int _Origin);

  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) fseeko64(FILE *_File, _off64_t _Offset, int _Origin);



  _off_t __attribute__((__cdecl__)) ftello(FILE *_File);

  __attribute__ ((__dllimport__)) _off64_t __attribute__((__cdecl__)) ftello64(FILE *_File);

  size_t __attribute__((__cdecl__)) fwrite(const void * __restrict__ _Str,size_t _Size,size_t _Count,FILE * __restrict__ _File);
  int __attribute__((__cdecl__)) getc(FILE *_File);
  int __attribute__((__cdecl__)) getchar(void);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _getmaxstdio(void);
  char *__attribute__((__cdecl__)) gets(char *_Buffer)

    __attribute__((__warning__("Using gets() is always unsafe - use fgets() instead")))

;
  int __attribute__((__cdecl__)) _getw(FILE *_File);


  void __attribute__((__cdecl__)) perror(const char *_ErrMsg);


  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _pclose(FILE *_File);
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _popen(const char *_Command,const char *_Mode);





  int __attribute__((__cdecl__)) putc(int _Ch,FILE *_File);
  int __attribute__((__cdecl__)) putchar(int _Ch);
  int __attribute__((__cdecl__)) puts(const char *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _putw(int _Word,FILE *_File);


  int __attribute__((__cdecl__)) remove(const char *_Filename);
  int __attribute__((__cdecl__)) rename(const char *_OldFilename,const char *_NewFilename);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _unlink(const char *_Filename);

  int __attribute__((__cdecl__)) unlink(const char *_Filename) ;


  void __attribute__((__cdecl__)) rewind(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _rmtmp(void);
  void __attribute__((__cdecl__)) setbuf(FILE * __restrict__ _File,char * __restrict__ _Buffer) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _setmaxstdio(int _Max);
  __attribute__ ((__dllimport__)) unsigned int __attribute__((__cdecl__)) _set_output_format(unsigned int _Format);
  __attribute__ ((__dllimport__)) unsigned int __attribute__((__cdecl__)) _get_output_format(void);
  int __attribute__((__cdecl__)) setvbuf(FILE * __restrict__ _File,char * __restrict__ _Buf,int _Mode,size_t _Size);

  __attribute__ ((__pure__))
  __attribute__((__format__ (__gnu_printf__, 1, 2))) __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) _scprintf(const char * __restrict__ _Format,...);
  __attribute__((__format__ (__gnu_scanf__, 3, 4))) __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) _snscanf(const char * __restrict__ _Src,size_t _MaxCount,const char * __restrict__ _Format,...) ;







  __attribute__ ((__pure__))
  __attribute__((__format__(__ms_printf__, 1,0))) __attribute__ ((__nonnull__ (1)))
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vscprintf(const char * __restrict__ _Format,va_list _ArgList);
  FILE *__attribute__((__cdecl__)) tmpfile(void) ;
  FILE *__attribute__((__cdecl__)) tmpfile64(void);
  char *__attribute__((__cdecl__)) tmpnam(char *_Buffer);
  int __attribute__((__cdecl__)) ungetc(int _Ch,FILE *_File);


  __attribute__((__format__ (__gnu_printf__, 3, 0))) __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) _vsnprintf(char * __restrict__ _Dest,size_t _Count,const char * __restrict__ _Format,va_list _Args) ;
  __attribute__((__format__ (__gnu_printf__, 3, 4))) __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) _snprintf(char * __restrict__ _Dest,size_t _Count,const char * __restrict__ _Format,...) ;
# 718 "C:/msys64/ucrt64/include/stdio.h" 3
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"

  __attribute__((__format__ (__gnu_printf__, 3, 0))) __attribute__ ((__nonnull__ (3)))
  int vsnprintf (char * __restrict__ __stream, size_t __n, const char * __restrict__ __format, va_list __local_argv);

  __attribute__((__format__ (__gnu_printf__, 3, 4))) __attribute__ ((__nonnull__ (3)))
  int snprintf (char * __restrict__ __stream, size_t __n, const char * __restrict__ __format, ...);
# 784 "C:/msys64/ucrt64/include/stdio.h" 3
#pragma GCC diagnostic pop
# 886 "C:/msys64/ucrt64/include/stdio.h" 3
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _set_printf_count_output(int _Value);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _get_printf_count_output(void);




                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vswscanf (const wchar_t * __restrict__ _Str,const wchar_t * __restrict__ Format,va_list argp);
                                                     __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_wscanf(const wchar_t * __restrict__ _Format,...);
                                                     __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_vwscanf(const wchar_t * __restrict__ Format, va_list argp);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vfwscanf (FILE * __restrict__ fp, const wchar_t * __restrict__ Format,va_list argp);

                                                      __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
                                                      __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_wprintf(const wchar_t * __restrict__ _Format,...);
                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __mingw_vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
                                                     __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __mingw_vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
                                                      __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __mingw_snwprintf (wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
                                                      __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __mingw_vsnwprintf (wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list);
                                                      __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __mingw_swprintf(wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , ...);
                                                      __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __mingw_vswprintf(wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ ,va_list);

                                                    __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...)
  __asm__("swscanf");
                                                    __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vswscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,va_list)
  __asm__("vswscanf");
                                                    __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_wscanf(const wchar_t * __restrict__ _Format,...)
  __asm__("wscanf");
                                                    __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_vwscanf(const wchar_t * __restrict__ _Format, va_list)
  __asm__("vwscanf");
                                                    __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...)
  __asm__("fwscanf");
                                                    __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vfwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list)
  __asm__("vfwscanf");

                                                     __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);


                                                     __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_wprintf(const wchar_t * __restrict__ _Format,...)
  __asm__("wprintf");
                                                    __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) __ms_vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList)
  __asm__("vfwprintf");
                                                    __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) __ms_vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList)
  __asm__("vwprintf");
                                                     __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __ms_swprintf(wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , ...)
  __asm__("swprintf");
                                                     __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __ms_vswprintf(wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ ,va_list)
  __asm__("vswprintf");
                                                     __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __ms_snwprintf(wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , ...)
  __asm__("snwprintf");
                                                     __attribute__ ((__nonnull__ (3)))
  int __attribute__((__cdecl__)) __ms_vsnwprintf(wchar_t * __restrict__ , size_t, const wchar_t * __restrict__ , va_list)
  __asm__("vsnwprintf");


  int __attribute__((__cdecl__)) __stdio_common_vswprintf(unsigned long long options, wchar_t *str, size_t len, const wchar_t *format, _locale_t locale, va_list valist);
  int __attribute__((__cdecl__)) __stdio_common_vfwprintf(unsigned long long options, FILE *file, const wchar_t *format, _locale_t locale, va_list valist);
  int __attribute__((__cdecl__)) __stdio_common_vswscanf(unsigned long long options, const wchar_t *input, size_t length, const wchar_t *format, _locale_t locale, va_list valist);
  int __attribute__((__cdecl__)) __stdio_common_vfwscanf(unsigned long long options, FILE *file, const wchar_t *format, _locale_t locale, va_list valist);
# 1080 "C:/msys64/ucrt64/include/stdio.h" 3
 
  int __attribute__((__cdecl__)) fwscanf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);

 
  int __attribute__((__cdecl__)) swscanf(const wchar_t * __restrict__ _Src,const wchar_t * __restrict__ _Format,...);

 
  int __attribute__((__cdecl__)) wscanf(const wchar_t * __restrict__ _Format,...);

  __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) vfwscanf(FILE *__stream, const wchar_t *__format, va_list __local_argv);

  __attribute__ ((__nonnull__ (2)))
  int __attribute__((__cdecl__)) vswscanf(const wchar_t * __restrict__ __source, const wchar_t * __restrict__ __format, va_list __local_argv);

  __attribute__ ((__nonnull__ (1)))
  int __attribute__((__cdecl__)) vwscanf(const wchar_t *__format, va_list __local_argv);

  int __attribute__((__cdecl__)) fwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,...);
  int __attribute__((__cdecl__)) wprintf(const wchar_t * __restrict__ _Format,...);
  int __attribute__((__cdecl__)) vfwprintf(FILE * __restrict__ _File,const wchar_t * __restrict__ _Format,va_list _ArgList);
  int __attribute__((__cdecl__)) vwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);

  int __attribute__((__cdecl__)) swprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,...);
  int __attribute__((__cdecl__)) vswprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,va_list _Args);
  int __attribute__((__cdecl__)) snwprintf(wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, ...);
  int __attribute__((__cdecl__)) vsnwprintf(wchar_t * __restrict__ s, size_t n, const wchar_t * __restrict__ format, va_list arg);
# 1116 "C:/msys64/ucrt64/include/stdio.h" 3
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _wfsopen(const wchar_t *_Filename,const wchar_t *_Mode,int _ShFlag);


  wint_t __attribute__((__cdecl__)) fgetwc(FILE *_File);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _fgetwchar(void);
  wint_t __attribute__((__cdecl__)) fputwc(wchar_t _Ch,FILE *_File);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _fputwchar(wchar_t _Ch);
  wint_t __attribute__((__cdecl__)) getwc(FILE *_File);
  wint_t __attribute__((__cdecl__)) getwchar(void);
  wint_t __attribute__((__cdecl__)) putwc(wchar_t _Ch,FILE *_File);
  wint_t __attribute__((__cdecl__)) putwchar(wchar_t _Ch);
  wint_t __attribute__((__cdecl__)) ungetwc(wint_t _Ch,FILE *_File);
  wchar_t *__attribute__((__cdecl__)) fgetws(wchar_t * __restrict__ _Dst,int _SizeInWords,FILE * __restrict__ _File);
  int __attribute__((__cdecl__)) fputws(const wchar_t * __restrict__ _Str,FILE * __restrict__ _File);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _getws(wchar_t *_String) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _putws(const wchar_t *_Str);

  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _scwprintf(const wchar_t * __restrict__ _Format,...);




  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,...) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vsnwprintf(wchar_t * __restrict__ _Dest,size_t _Count,const wchar_t * __restrict__ _Format,va_list _Args) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vscwprintf(const wchar_t * __restrict__ _Format,va_list _ArgList);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _swprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _vswprintf(wchar_t * __restrict__ _Dest,const wchar_t * __restrict__ _Format,va_list _Args);


# 1 "C:/msys64/ucrt64/include/swprintf.inl" 1 3
# 1146 "C:/msys64/ucrt64/include/stdio.h" 2 3
# 1159 "C:/msys64/ucrt64/include/stdio.h" 3
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wtempnam(const wchar_t *_Directory,const wchar_t *_FilePrefix);



  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _snwscanf(const wchar_t * __restrict__ _Src,size_t _MaxCount,const wchar_t * __restrict__ _Format,...);
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _wfdopen(int _FileHandle ,const wchar_t *_Mode);
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _wfopen(const wchar_t * __restrict__ _Filename,const wchar_t *__restrict__ _Mode) ;
  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _wfreopen(const wchar_t * __restrict__ _Filename,const wchar_t * __restrict__ _Mode,FILE * __restrict__ _OldFile) ;



  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _wperror(const wchar_t *_ErrMsg);

  __attribute__ ((__dllimport__)) FILE *__attribute__((__cdecl__)) _wpopen(const wchar_t *_Command,const wchar_t *_Mode);




  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wremove(const wchar_t *_Filename);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wtmpnam(wchar_t *_Buffer);

  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _fgetwc_nolock(FILE *_File);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _fputwc_nolock(wchar_t _Ch,FILE *_File);
  __attribute__ ((__dllimport__)) wint_t __attribute__((__cdecl__)) _ungetwc_nolock(wint_t _Ch,FILE *_File);
# 1206 "C:/msys64/ucrt64/include/stdio.h" 3
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fgetc_nolock(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fputc_nolock(int _Char, FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _getc_nolock(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _putc_nolock(int _Char, FILE *_File);





  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _lock_file(FILE *_File);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _unlock_file(FILE *_File);

  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fclose_nolock(FILE *_File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fflush_nolock(FILE *_File);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _fread_nolock(void * __restrict__ _DstBuf,size_t _ElementSize,size_t _Count,FILE * __restrict__ _File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fseek_nolock(FILE *_File,long _Offset,int _Origin);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _ftell_nolock(FILE *_File);
  __extension__ __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _fseeki64_nolock(FILE *_File,long long _Offset,int _Origin);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _ftelli64_nolock(FILE *_File);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _fwrite_nolock(const void * __restrict__ _DstBuf,size_t _Size,size_t _Count,FILE * __restrict__ _File);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _ungetc_nolock(int _Ch,FILE *_File);
# 1237 "C:/msys64/ucrt64/include/stdio.h" 3
  char *__attribute__((__cdecl__)) tempnam(const char *_Directory,const char *_FilePrefix) ;



  int __attribute__((__cdecl__)) fcloseall(void) ;
  FILE *__attribute__((__cdecl__)) fdopen(int _FileHandle,const char *_Format) ;
  int __attribute__((__cdecl__)) fgetchar(void) ;
  int __attribute__((__cdecl__)) fileno(FILE *_File) ;
  int __attribute__((__cdecl__)) flushall(void) ;
  int __attribute__((__cdecl__)) fputchar(int _Ch) ;
  int __attribute__((__cdecl__)) getw(FILE *_File) ;
  int __attribute__((__cdecl__)) putw(int _Ch,FILE *_File) ;
  int __attribute__((__cdecl__)) rmtmp(void) ;
# 1266 "C:/msys64/ucrt64/include/stdio.h" 3
int __attribute__((__cdecl__)) __mingw_str_wide_utf8 (const wchar_t * const wptr, char **mbptr, size_t * buflen);
# 1280 "C:/msys64/ucrt64/include/stdio.h" 3
int __attribute__((__cdecl__)) __mingw_str_utf8_wide (const char *const mbptr, wchar_t ** wptr, size_t * buflen);
# 1289 "C:/msys64/ucrt64/include/stdio.h" 3
void __attribute__((__cdecl__)) __mingw_str_free(void *ptr);






  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnl(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnle(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnlp(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnlpe(int _Mode,const wchar_t *_Filename,const wchar_t *_ArgList,...);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnv(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnve(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnvp(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _wspawnvpe(int _Mode,const wchar_t *_Filename,const wchar_t *const *_ArgList,const wchar_t *const *_Env);
# 1320 "C:/msys64/ucrt64/include/stdio.h" 3
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _spawnv(int _Mode,const char *_Filename,const char *const *_ArgList);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _spawnve(int _Mode,const char *_Filename,const char *const *_ArgList,const char *const *_Env);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _spawnvp(int _Mode,const char *_Filename,const char *const *_ArgList);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _spawnvpe(int _Mode,const char *_Filename,const char *const *_ArgList,const char *const *_Env);







       
       
       
       

#pragma pack(pop)

# 1 "C:/msys64/ucrt64/include/sec_api/stdio_s.h" 1 3
# 9 "C:/msys64/ucrt64/include/sec_api/stdio_s.h" 3
# 1 "C:/msys64/ucrt64/include/stdio.h" 1 3
# 10 "C:/msys64/ucrt64/include/sec_api/stdio_s.h" 2 3
# 29 "C:/msys64/ucrt64/include/sec_api/stdio_s.h" 3
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) clearerr_s(FILE *_File);

  size_t __attribute__((__cdecl__)) fread_s(void *_DstBuf,size_t _DstSize,size_t _ElementSize,size_t _Count,FILE *_File);


  int __attribute__((__cdecl__)) __stdio_common_vsprintf_s(unsigned long long _Options, char *_Str, size_t _Len, const char *_Format, _locale_t _Locale, va_list _ArgList);
  int __attribute__((__cdecl__)) __stdio_common_vsprintf_p(unsigned long long _Options, char *_Str, size_t _Len, const char *_Format, _locale_t _Locale, va_list _ArgList);
  int __attribute__((__cdecl__)) __stdio_common_vsnprintf_s(unsigned long long _Options, char *_Str, size_t _Len, size_t _MaxCount, const char *_Format, _locale_t _Locale, va_list _ArgList);
  int __attribute__((__cdecl__)) __stdio_common_vfprintf_s(unsigned long long _Options, FILE *_File, const char *_Format, _locale_t _Locale, va_list _ArgList);
  int __attribute__((__cdecl__)) __stdio_common_vfprintf_p(unsigned long long _Options, FILE *_File, const char *_Format, _locale_t _Locale, va_list _ArgList);

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vfscanf_s_l(FILE *_File, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vfscanf(0x0001ULL, _File, _Format, _Locale, _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vfscanf_s(FILE *_File, const char *_Format, va_list _ArgList)
  {
    return _vfscanf_s_l(_File, _Format, ((void *)0), _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vscanf_s_l(const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return _vfscanf_s_l((__acrt_iob_func(0)), _Format, _Locale, _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vscanf_s(const char *_Format, va_list _ArgList)
  {
    return _vfscanf_s_l((__acrt_iob_func(0)), _Format, ((void *)0), _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _fscanf_s_l(FILE *_File, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfscanf_s_l(_File, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) fscanf_s(FILE *_File, const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vfscanf_s_l(_File, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _scanf_s_l(const char *_Format, _locale_t _Locale ,...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfscanf_s_l((__acrt_iob_func(0)), _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) scanf_s(const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vfscanf_s_l((__acrt_iob_func(0)), _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vfscanf_l(FILE *_File, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vfscanf(0, _File, _Format, _Locale, _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vscanf_l(const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return _vfscanf_l((__acrt_iob_func(0)), _Format, _Locale, _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _fscanf_l(FILE *_File, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfscanf_l(_File, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _scanf_l(const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfscanf_l((__acrt_iob_func(0)), _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsscanf_s_l(const char *_Src, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vsscanf(0x0001ULL, _Src, (size_t)-1, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vsscanf_s(const char *_Src, const char *_Format, va_list _ArgList)
  {
    return _vsscanf_s_l(_Src, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _sscanf_s_l(const char *_Src, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vsscanf_s_l(_Src, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) sscanf_s(const char *_Src, const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vsscanf_s_l(_Src, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsscanf_l(const char *_Src, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vsscanf(0, _Src, (size_t)-1, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _sscanf_l(const char *_Src, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vsscanf_l(_Src, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }


  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snscanf_s_l(const char *_Src, size_t _MaxCount, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = __stdio_common_vsscanf(0x0001ULL, _Src, _MaxCount, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snscanf_s(const char *_Src, size_t _MaxCount, const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = __stdio_common_vsscanf(0x0001ULL, _Src, _MaxCount, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }


  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snscanf_l(const char *_Src, size_t _MaxCount, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = __stdio_common_vsscanf(0, _Src, _MaxCount, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }


  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vfprintf_s_l(FILE *_File, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vfprintf_s((*__local_stdio_printf_options()), _File, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vfprintf_s(FILE *_File, const char *_Format, va_list _ArgList)
  {
    return _vfprintf_s_l(_File, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vprintf_s_l(const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return _vfprintf_s_l((__acrt_iob_func(1)), _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vprintf_s(const char *_Format, va_list _ArgList)
  {
    return _vfprintf_s_l((__acrt_iob_func(1)), _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _fprintf_s_l(FILE *_File, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfprintf_s_l(_File, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _printf_s_l(const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfprintf_s_l((__acrt_iob_func(1)), _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) fprintf_s(FILE *_File, const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vfprintf_s_l(_File, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) printf_s(const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vfprintf_s_l((__acrt_iob_func(1)), _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsnprintf_c_l(char *_DstBuf, size_t _MaxCount, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vsprintf((*__local_stdio_printf_options()), _DstBuf, _MaxCount, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsnprintf_c(char *_DstBuf, size_t _MaxCount, const char *_Format, va_list _ArgList)
  {
    return _vsnprintf_c_l(_DstBuf, _MaxCount, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snprintf_c_l(char *_DstBuf, size_t _MaxCount, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vsnprintf_c_l(_DstBuf, _MaxCount, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snprintf_c(char *_DstBuf, size_t _MaxCount, const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vsnprintf_c_l(_DstBuf, _MaxCount, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsnprintf_s_l(char *_DstBuf, size_t _DstSize, size_t _MaxCount, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vsnprintf_s((*__local_stdio_printf_options()), _DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vsnprintf_s(char *_DstBuf, size_t _DstSize, size_t _MaxCount, const char *_Format, va_list _ArgList)
  {
    return _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsnprintf_s(char *_DstBuf, size_t _DstSize, size_t _MaxCount, const char *_Format, va_list _ArgList)
  {
    return _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snprintf_s_l(char *_DstBuf, size_t _DstSize, size_t _MaxCount, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snprintf_s(char *_DstBuf, size_t _DstSize, size_t _MaxCount, const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vsnprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsprintf_s_l(char *_DstBuf, size_t _DstSize, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vsprintf_s((*__local_stdio_printf_options()), _DstBuf, _DstSize, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vsprintf_s(char *_DstBuf, size_t _Size, const char *_Format, va_list _ArgList)
  {
    return _vsprintf_s_l(_DstBuf, _Size, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _sprintf_s_l(char *_DstBuf, size_t _DstSize, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vsprintf_s_l(_DstBuf, _DstSize, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) sprintf_s(char *_DstBuf, size_t _DstSize, const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vsprintf_s_l(_DstBuf, _DstSize, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vfprintf_p_l(FILE *_File, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vfprintf_p((*__local_stdio_printf_options()), _File, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vfprintf_p(FILE *_File, const char *_Format, va_list _ArgList)
  {
    return _vfprintf_p_l(_File, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vprintf_p_l(const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return _vfprintf_p_l((__acrt_iob_func(1)), _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vprintf_p(const char *_Format, va_list _ArgList)
  {
    return _vfprintf_p_l((__acrt_iob_func(1)), _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _fprintf_p_l(FILE *_File, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = __stdio_common_vfprintf_p((*__local_stdio_printf_options()), _File, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _fprintf_p(FILE *_File, const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vfprintf_p_l(_File, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _printf_p_l(const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfprintf_p_l((__acrt_iob_func(1)), _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _printf_p(const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vfprintf_p_l((__acrt_iob_func(1)), _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsprintf_p_l(char *_DstBuf, size_t _MaxCount, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vsprintf_p((*__local_stdio_printf_options()), _DstBuf, _MaxCount, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsprintf_p(char *_Dst, size_t _MaxCount, const char *_Format, va_list _ArgList)
  {
    return _vsprintf_p_l(_Dst, _MaxCount, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _sprintf_p_l(char *_DstBuf, size_t _MaxCount, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vsprintf_p_l(_DstBuf, _MaxCount, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _sprintf_p(char *_Dst, size_t _MaxCount, const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vsprintf_p_l(_Dst, _MaxCount, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vscprintf_p_l(const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vsprintf_p(0x0002ULL, ((void *)0), 0, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vscprintf_p(const char *_Format, va_list _ArgList)
  {
    return _vscprintf_p_l(_Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _scprintf_p_l(const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vscprintf_p_l(_Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _scprintf_p(const char *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vscprintf_p_l(_Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vfprintf_l(FILE *_File, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vfprintf((*__local_stdio_printf_options()), _File, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vprintf_l(const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return _vfprintf_l((__acrt_iob_func(1)), _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _fprintf_l(FILE *_File, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfprintf_l(_File, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _printf_l(const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfprintf_l((__acrt_iob_func(1)), _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsnprintf_l(char *_DstBuf, size_t _MaxCount, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vsprintf(0x0001ULL, _DstBuf, _MaxCount, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snprintf_l(char *_DstBuf, size_t _MaxCount, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vsnprintf_l(_DstBuf, _MaxCount, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsprintf_l(char *_DstBuf, const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return _vsnprintf_l(_DstBuf, (size_t)-1, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _sprintf_l(char *_DstBuf, const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vsprintf_l(_DstBuf, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vscprintf_l(const char *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vsprintf(0x0002ULL, ((void *)0), 0, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _scprintf_l(const char *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vscprintf_l(_Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
# 583 "C:/msys64/ucrt64/include/sec_api/stdio_s.h" 3
 
 
 
 
 

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) fopen_s(FILE **_File,const char *_Filename,const char *_Mode);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) freopen_s(FILE** _File, const char *_Filename, const char *_Mode, FILE *_Stream);

  __attribute__ ((__dllimport__)) char* __attribute__((__cdecl__)) gets_s(char*,rsize_t);
 

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) tmpfile_s(FILE **_File);

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) tmpnam_s(char*,rsize_t);
 




  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _getws_s(wchar_t *_Str,size_t _SizeInWords);
 


  int __attribute__((__cdecl__)) __stdio_common_vswprintf_s(unsigned long long _Options, wchar_t *_Str, size_t _Len, const wchar_t *_Format, _locale_t _Locale, va_list _ArgList);
  int __attribute__((__cdecl__)) __stdio_common_vsnwprintf_s(unsigned long long _Options, wchar_t *_Str, size_t _Len, size_t _MaxCount, const wchar_t *_Format, _locale_t _Locale, va_list _ArgList);
  int __attribute__((__cdecl__)) __stdio_common_vfwprintf_s(unsigned long long _Options, FILE *_File, const wchar_t *_Format, _locale_t _Locale, va_list _ArgList);

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vfwscanf_s_l(FILE *_File, const wchar_t *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vfwscanf((*__local_stdio_scanf_options()) | 0x0001ULL, _File, _Format, _Locale, _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vfwscanf_s(FILE* _File, const wchar_t *_Format, va_list _ArgList)
  {
    return _vfwscanf_s_l(_File, _Format, ((void *)0), _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vwscanf_s_l(const wchar_t *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return _vfwscanf_s_l((__acrt_iob_func(0)), _Format, _Locale, _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vwscanf_s(const wchar_t *_Format, va_list _ArgList)
  {
    return _vfwscanf_s_l((__acrt_iob_func(0)), _Format, ((void *)0), _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _fwscanf_s_l(FILE *_File, const wchar_t *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfwscanf_s_l(_File, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) fwscanf_s(FILE *_File, const wchar_t *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vfwscanf_s_l(_File, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _wscanf_s_l(const wchar_t *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfwscanf_s_l((__acrt_iob_func(0)), _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) wscanf_s(const wchar_t *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vfwscanf_s_l((__acrt_iob_func(0)), _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vswscanf_s_l(const wchar_t *_Src, const wchar_t *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vswscanf((*__local_stdio_scanf_options()) | 0x0001ULL, _Src, (size_t)-1, _Format, _Locale, _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vswscanf_s(const wchar_t *_Src, const wchar_t *_Format, va_list _ArgList)
  {
    return _vswscanf_s_l(_Src, _Format, ((void *)0), _ArgList);
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _swscanf_s_l(const wchar_t *_Src, const wchar_t *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vswscanf_s_l(_Src, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) swscanf_s(const wchar_t *_Src, const wchar_t *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vswscanf_s_l(_Src, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsnwscanf_s_l(const wchar_t *_Src, size_t _MaxCount, const wchar_t *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vswscanf((*__local_stdio_scanf_options()) | 0x0001ULL, _Src, _MaxCount, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snwscanf_s_l(const wchar_t *_Src, size_t _MaxCount, const wchar_t *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vsnwscanf_s_l(_Src, _MaxCount, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snwscanf_s(const wchar_t *_Src, size_t _MaxCount, const wchar_t *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vsnwscanf_s_l(_Src, _MaxCount, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vfwprintf_s_l(FILE *_File, const wchar_t *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vfwprintf_s((*__local_stdio_printf_options()), _File, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vwprintf_s_l(const wchar_t *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return _vfwprintf_s_l((__acrt_iob_func(1)), _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vfwprintf_s(FILE *_File, const wchar_t *_Format, va_list _ArgList)
  {
    return _vfwprintf_s_l(_File, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vwprintf_s(const wchar_t *_Format, va_list _ArgList)
  {
    return _vfwprintf_s_l((__acrt_iob_func(1)), _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _fwprintf_s_l(FILE *_File, const wchar_t *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfwprintf_s_l(_File, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _wprintf_s_l(const wchar_t *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vfwprintf_s_l((__acrt_iob_func(1)), _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) fwprintf_s(FILE *_File, const wchar_t *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vfwprintf_s_l(_File, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) wprintf_s(const wchar_t *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vfwprintf_s_l((__acrt_iob_func(1)), _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vswprintf_s_l(wchar_t *_DstBuf, size_t _DstSize, const wchar_t *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vswprintf_s((*__local_stdio_printf_options()), _DstBuf, _DstSize, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) vswprintf_s(wchar_t *_DstBuf, size_t _DstSize, const wchar_t *_Format, va_list _ArgList)
  {
    return _vswprintf_s_l(_DstBuf, _DstSize, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _swprintf_s_l(wchar_t *_DstBuf, size_t _DstSize, const wchar_t *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vswprintf_s_l(_DstBuf, _DstSize, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) swprintf_s(wchar_t *_DstBuf, size_t _DstSize, const wchar_t *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vswprintf_s_l(_DstBuf, _DstSize, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }

  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsnwprintf_s_l(wchar_t *_DstBuf, size_t _DstSize, size_t _MaxCount, const wchar_t *_Format, _locale_t _Locale, va_list _ArgList)
  {
    return __stdio_common_vsnwprintf_s((*__local_stdio_printf_options()), _DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _vsnwprintf_s(wchar_t *_DstBuf, size_t _DstSize, size_t _MaxCount, const wchar_t *_Format, va_list _ArgList)
  {
    return _vsnwprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, ((void *)0), _ArgList);
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snwprintf_s_l(wchar_t *_DstBuf, size_t _DstSize, size_t _MaxCount, const wchar_t *_Format, _locale_t _Locale, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Locale);
    _Ret = _vsnwprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, _Locale, _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
  static __attribute__ ((__unused__)) __inline__ __attribute__((__cdecl__)) int __attribute__((__cdecl__)) _snwprintf_s(wchar_t *_DstBuf, size_t _DstSize, size_t _MaxCount, const wchar_t *_Format, ...)
  {
    __builtin_va_list _ArgList;
    int _Ret;
    __builtin_va_start(_ArgList, _Format);
    _Ret = _vsnwprintf_s_l(_DstBuf, _DstSize, _MaxCount, _Format, ((void *)0), _ArgList);
    __builtin_va_end(_ArgList);
    return _Ret;
  }
# 862 "C:/msys64/ucrt64/include/sec_api/stdio_s.h" 3
 
 
 
 

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wfopen_s(FILE **_File,const wchar_t *_Filename,const wchar_t *_Mode);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wfreopen_s(FILE **_File,const wchar_t *_Filename,const wchar_t *_Mode,FILE *_OldFile);

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wtmpnam_s(wchar_t *_DstBuf,size_t _SizeInWords);
 
# 912 "C:/msys64/ucrt64/include/sec_api/stdio_s.h" 3
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _fread_nolock_s(void *_DstBuf,size_t _DstSize,size_t _ElementSize,size_t _Count,FILE *_File);
# 1339 "C:/msys64/ucrt64/include/stdio.h" 2 3
# 2 ".\\StringRoutine.c" 2
# 1 "C:/msys64/ucrt64/include/stdlib.h" 1 3
# 10 "C:/msys64/ucrt64/include/stdlib.h" 3
# 1 "C:/msys64/ucrt64/include/corecrt_wstdlib.h" 1 3
# 19 "C:/msys64/ucrt64/include/corecrt_wstdlib.h" 3
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wdupenv_s(wchar_t **_Buffer,size_t *_BufferSizeInWords,const wchar_t *_VarName);




  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _itow_s (int _Val,wchar_t *_DstBuf,size_t _SizeInWords,int _Radix);
 

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ltow_s (long _Val,wchar_t *_DstBuf,size_t _SizeInWords,int _Radix);
 

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ultow_s (unsigned long _Val,wchar_t *_DstBuf,size_t _SizeInWords,int _Radix);
 

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wgetenv_s(size_t *_ReturnSize,wchar_t *_DstBuf,size_t _DstSizeInWords,const wchar_t *_VarName);
 

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _i64tow_s(long long _Val,wchar_t *_DstBuf,size_t _SizeInWords,int _Radix);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ui64tow_s(unsigned long long _Val,wchar_t *_DstBuf,size_t _SizeInWords,int _Radix);

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wmakepath_s(wchar_t *_PathResult,size_t _SizeInWords,const wchar_t *_Drive,const wchar_t *_Dir,const wchar_t *_Filename,const wchar_t *_Ext);
 

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wputenv_s(const wchar_t *_Name,const wchar_t *_Value);

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wsearchenv_s(const wchar_t *_Filename,const wchar_t *_EnvVar,wchar_t *_ResultPath,size_t _SizeInWords);
 

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wsplitpath_s(const wchar_t *_FullPath,wchar_t *_Drive,size_t _DriveSizeInWords,wchar_t *_Dir,size_t _DirSizeInWords,wchar_t *_Filename,size_t _FilenameSizeInWords,wchar_t *_Ext,size_t _ExtSizeInWords);
 
# 11 "C:/msys64/ucrt64/include/stdlib.h" 2 3
# 1 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/limits.h" 1 3
# 34 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/limits.h" 3
# 1 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/syslimits.h" 1 3






 
# 7 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/syslimits.h" 3
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
# 1 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/limits.h" 1 3
# 210 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/limits.h" 3
# 1 "C:/msys64/ucrt64/include/limits.h" 1 3





# 1 "C:/msys64/ucrt64/include/crtdefs.h" 1 3
# 7 "C:/msys64/ucrt64/include/limits.h" 2 3
# 211 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/limits.h" 2 3
# 10 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/syslimits.h" 2 3
#pragma GCC diagnostic pop
# 35 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/limits.h" 2 3
# 12 "C:/msys64/ucrt64/include/stdlib.h" 2 3
# 26 "C:/msys64/ucrt64/include/stdlib.h" 3
#pragma pack(push,_CRT_PACKING)
# 50 "C:/msys64/ucrt64/include/stdlib.h" 3
  typedef int (__attribute__((__cdecl__)) *_onexit_t)(void);
# 60 "C:/msys64/ucrt64/include/stdlib.h" 3
  typedef struct _div_t {
    int quot;
    int rem;
  } div_t;

  typedef struct _ldiv_t {
    long quot;
    long rem;
  } ldiv_t;





#pragma pack(4)
  typedef struct {
    unsigned char ld[10];
  } _LDOUBLE;
#pragma pack()



  typedef struct {
    double x;
  } _CRT_DOUBLE;

  typedef struct {
    float f;
  } _CRT_FLOAT;

       


  typedef struct {
    long double x;
  } _LONGDOUBLE;

       

#pragma pack(4)
  typedef struct {
    unsigned char ld12[12];
  } _LDBL12;
#pragma pack()
# 113 "C:/msys64/ucrt64/include/stdlib.h" 3
__attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) ___mb_cur_max_func(void);
# 135 "C:/msys64/ucrt64/include/stdlib.h" 3
  typedef void (__attribute__((__cdecl__)) *_purecall_handler)(void);

  __attribute__ ((__dllimport__)) _purecall_handler __attribute__((__cdecl__)) _set_purecall_handler(_purecall_handler _Handler);
  __attribute__ ((__dllimport__)) _purecall_handler __attribute__((__cdecl__)) _get_purecall_handler(void);

  typedef void (__attribute__((__cdecl__)) *_invalid_parameter_handler)(const wchar_t *,const wchar_t *,const wchar_t *,unsigned int,uintptr_t);
  __attribute__ ((__dllimport__)) _invalid_parameter_handler __attribute__((__cdecl__)) _set_invalid_parameter_handler(_invalid_parameter_handler _Handler);
  __attribute__ ((__dllimport__)) _invalid_parameter_handler __attribute__((__cdecl__)) _get_invalid_parameter_handler(void);



  __attribute__ ((__dllimport__)) extern int *__attribute__((__cdecl__)) _errno(void);

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _set_errno(int _Value);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _get_errno(int *_Value);

  __attribute__ ((__dllimport__)) unsigned long *__attribute__((__cdecl__)) __doserrno(void);

  errno_t __attribute__((__cdecl__)) _set_doserrno(unsigned long _Value);
  errno_t __attribute__((__cdecl__)) _get_doserrno(unsigned long *_Value);
  __attribute__ ((__dllimport__)) char **__attribute__((__cdecl__)) __sys_errlist(void);
  __attribute__ ((__dllimport__)) int *__attribute__((__cdecl__)) __sys_nerr(void);



  __attribute__ ((__dllimport__)) char ***__attribute__((__cdecl__)) __p___argv(void);
  __attribute__ ((__dllimport__)) int *__attribute__((__cdecl__)) __p__fmode(void);
  __attribute__ ((__dllimport__)) int *__attribute__((__cdecl__)) __p___argc(void);
  __attribute__ ((__dllimport__)) wchar_t ***__attribute__((__cdecl__)) __p___wargv(void);
  __attribute__ ((__dllimport__)) char **__attribute__((__cdecl__)) __p__pgmptr(void);
  __attribute__ ((__dllimport__)) wchar_t **__attribute__((__cdecl__)) __p__wpgmptr(void);

  errno_t __attribute__((__cdecl__)) _get_pgmptr(char **_Value);
  errno_t __attribute__((__cdecl__)) _get_wpgmptr(wchar_t **_Value);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _set_fmode(int _Mode);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _get_fmode(int *_PMode);
# 221 "C:/msys64/ucrt64/include/stdlib.h" 3
  __attribute__ ((__dllimport__)) char ***__attribute__((__cdecl__)) __p__environ(void);
  __attribute__ ((__dllimport__)) wchar_t ***__attribute__((__cdecl__)) __p__wenviron(void);
# 234 "C:/msys64/ucrt64/include/stdlib.h" 3
  __attribute__ ((__dllimport__)) unsigned int *__attribute__((__cdecl__)) __p__osplatform(void);
  __attribute__ ((__dllimport__)) unsigned int *__attribute__((__cdecl__)) __p__osver(void);
  __attribute__ ((__dllimport__)) unsigned int *__attribute__((__cdecl__)) __p__winver(void);
  __attribute__ ((__dllimport__)) unsigned int *__attribute__((__cdecl__)) __p__winmajor(void);
  __attribute__ ((__dllimport__)) unsigned int *__attribute__((__cdecl__)) __p__winminor(void);
# 256 "C:/msys64/ucrt64/include/stdlib.h" 3
  errno_t __attribute__((__cdecl__)) _get_osplatform(unsigned int *_Value);
  errno_t __attribute__((__cdecl__)) _get_osver(unsigned int *_Value);
  errno_t __attribute__((__cdecl__)) _get_winver(unsigned int *_Value);
  errno_t __attribute__((__cdecl__)) _get_winmajor(unsigned int *_Value);
  errno_t __attribute__((__cdecl__)) _get_winminor(unsigned int *_Value);
# 274 "C:/msys64/ucrt64/include/stdlib.h" 3
  void __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) exit(int _Code) __attribute__ ((__noreturn__));
  void __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) _exit(int _Code) __attribute__ ((__noreturn__));

  void __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) quick_exit(int _Code) __attribute__ ((__noreturn__));




  void __attribute__((__cdecl__)) _Exit(int) __attribute__ ((__noreturn__));






       

  void __attribute__((__cdecl__)) __attribute__ ((__noreturn__)) abort(void);
       



  __attribute__ ((__dllimport__)) unsigned int __attribute__((__cdecl__)) _set_abort_behavior(unsigned int _Flags,unsigned int _Mask);



  int __attribute__((__cdecl__)) abs(int _X);
  long __attribute__((__cdecl__)) labs(long _X);


  __extension__ long long __attribute__((__cdecl__)) _abs64(long long);

  extern __inline__ __attribute__((__always_inline__,__gnu_inline__)) long long __attribute__((__cdecl__)) _abs64(long long x) {
    return __builtin_llabs(x);
  }


  int __attribute__((__cdecl__)) atexit(void (__attribute__((__cdecl__)) *)(void));

  int __attribute__((__cdecl__)) at_quick_exit(void (__attribute__((__cdecl__)) *)(void));



  double __attribute__((__cdecl__)) atof(const char *_String);
  double __attribute__((__cdecl__)) _atof_l(const char *_String,_locale_t _Locale);

  int __attribute__((__cdecl__)) atoi(const char *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atoi_l(const char *_Str,_locale_t _Locale);
  long __attribute__((__cdecl__)) atol(const char *_Str);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _atol_l(const char *_Str,_locale_t _Locale);


  void *__attribute__((__cdecl__)) bsearch(const void *_Key,const void *_Base,size_t _NumOfElements,size_t _SizeOfElements,int (__attribute__((__cdecl__)) *_PtFuncCompare)(const void *,const void *));
  void __attribute__((__cdecl__)) qsort(void *_Base,size_t _NumOfElements,size_t _SizeOfElements,int (__attribute__((__cdecl__)) *_PtFuncCompare)(const void *,const void *));

  unsigned short __attribute__((__cdecl__)) _byteswap_ushort(unsigned short _Short);
  unsigned long __attribute__((__cdecl__)) _byteswap_ulong (unsigned long _Long);
  __extension__ unsigned long long __attribute__((__cdecl__)) _byteswap_uint64(unsigned long long _Int64);
  div_t __attribute__((__cdecl__)) div(int _Numerator,int _Denominator);
  char *__attribute__((__cdecl__)) getenv(const char *_VarName) ;
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _itoa(int _Value,char *_Dest,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _i64toa(long long _Val,char *_DstBuf,int _Radix) ;
  __extension__ __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _ui64toa(unsigned long long _Val,char *_DstBuf,int _Radix) ;
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _atoi64(const char *_String);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _atoi64_l(const char *_String,_locale_t _Locale);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _strtoi64(const char *_String,char **_EndPtr,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _strtoi64_l(const char *_String,char **_EndPtr,int _Radix,_locale_t _Locale);
  __extension__ __attribute__ ((__dllimport__)) unsigned long long __attribute__((__cdecl__)) _strtoui64(const char *_String,char **_EndPtr,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) unsigned long long __attribute__((__cdecl__)) _strtoui64_l(const char *_String,char **_EndPtr,int _Radix,_locale_t _Locale);
  ldiv_t __attribute__((__cdecl__)) ldiv(long _Numerator,long _Denominator);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _ltoa(long _Value,char *_Dest,int _Radix) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) mblen(const char *_Ch,size_t _MaxCount);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _mblen_l(const char *_Ch,size_t _MaxCount,_locale_t _Locale);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _mbstrlen(const char *_Str);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _mbstrlen_l(const char *_Str,_locale_t _Locale);

  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _mbstrnlen(const char *_Str,size_t _MaxCount);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _mbstrnlen_l(const char *_Str,size_t _MaxCount,_locale_t _Locale);

  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) mbtowc(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SrcSizeInBytes);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _mbtowc_l(wchar_t * __restrict__ _DstCh,const char * __restrict__ _SrcCh,size_t _SrcSizeInBytes,_locale_t _Locale);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) mbstowcs(wchar_t * __restrict__ _Dest,const char * __restrict__ _Source,size_t _MaxCount);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _mbstowcs_l(wchar_t * __restrict__ _Dest,const char * __restrict__ _Source,size_t _MaxCount,_locale_t _Locale);
  int __attribute__((__cdecl__)) mkstemp(char *_TemplateName);
  char *__attribute__((__cdecl__)) mkdtemp(char *_TemplateName);
  int __attribute__((__cdecl__)) rand(void);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _set_error_mode(int _Mode);
  void __attribute__((__cdecl__)) srand(unsigned int _Seed);
# 388 "C:/msys64/ucrt64/include/stdlib.h" 3
  double __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) strtod(const char * __restrict__ _Str,char ** __restrict__ _EndPtr);
  float __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) strtof(const char * __restrict__ nptr, char ** __restrict__ endptr);

  long double __attribute__((__cdecl__)) __attribute__ ((__nothrow__)) strtold(const char * __restrict__ , char ** __restrict__ );


  extern double __attribute__((__cdecl__)) __attribute__ ((__nothrow__))
  __strtod (const char * __restrict__ , char ** __restrict__);







  float __attribute__((__cdecl__)) __mingw_strtof (const char * __restrict__, char ** __restrict__);
  double __attribute__((__cdecl__)) __mingw_strtod (const char * __restrict__, char ** __restrict__);
  long double __attribute__((__cdecl__)) __mingw_strtold(const char * __restrict__, char ** __restrict__);

  __attribute__ ((__dllimport__)) float __attribute__((__cdecl__)) _strtof_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,_locale_t _Locale);
  __attribute__ ((__dllimport__)) double __attribute__((__cdecl__)) _strtod_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,_locale_t _Locale);
  long __attribute__((__cdecl__)) strtol(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _strtol_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  unsigned long __attribute__((__cdecl__)) strtoul(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix);
  __attribute__ ((__dllimport__)) unsigned long __attribute__((__cdecl__)) _strtoul_l(const char * __restrict__ _Str,char ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);


  int __attribute__((__cdecl__)) system(const char *_Command);

  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _ultoa(unsigned long _Value,char *_Dest,int _Radix) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) wctomb(char *_MbCh,wchar_t _WCh) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wctomb_l(char *_MbCh,wchar_t _WCh,_locale_t _Locale) ;
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) wcstombs(char * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _MaxCount) ;
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _wcstombs_l(char * __restrict__ _Dest,const wchar_t * __restrict__ _Source,size_t _MaxCount,_locale_t _Locale) ;
# 455 "C:/msys64/ucrt64/include/stdlib.h" 3
  void *__attribute__((__cdecl__)) calloc(size_t _NumOfElements,size_t _SizeOfElements);
  void __attribute__((__cdecl__)) free(void *_Memory);
  void *__attribute__((__cdecl__)) malloc(size_t _Size);
  void *__attribute__((__cdecl__)) realloc(void *_Memory,size_t _NewSize);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _aligned_free(void *_Memory);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _aligned_malloc(size_t _Size,size_t _Alignment);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _aligned_offset_malloc(size_t _Size,size_t _Alignment,size_t _Offset);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _aligned_realloc(void *_Memory,size_t _Size,size_t _Alignment);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _aligned_offset_realloc(void *_Memory,size_t _Size,size_t _Alignment,size_t _Offset);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _recalloc(void *_Memory,size_t _Count,size_t _Size);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _aligned_recalloc(void *_Memory,size_t _Count,size_t _Size,size_t _Alignment);
  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _aligned_offset_recalloc(void *_Memory,size_t _Count,size_t _Size,size_t _Alignment,size_t _Offset);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _aligned_msize(void *_Memory,size_t _Alignment,size_t _Offset);


  size_t __attribute__((__cdecl__)) memalignment(const void *_Memory);
# 494 "C:/msys64/ucrt64/include/stdlib.h" 3
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _itow(int _Value,wchar_t *_Dest,int _Radix) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _ltow(long _Value,wchar_t *_Dest,int _Radix) ;
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _ultow(unsigned long _Value,wchar_t *_Dest,int _Radix) ;

  double __attribute__((__cdecl__)) __mingw_wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __attribute__((__cdecl__)) __mingw_wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);
  long double __attribute__((__cdecl__)) __mingw_wcstold(const wchar_t * __restrict__, wchar_t ** __restrict__);
# 513 "C:/msys64/ucrt64/include/stdlib.h" 3
  double __attribute__((__cdecl__)) wcstod(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr);
  float __attribute__((__cdecl__)) wcstof(const wchar_t * __restrict__ nptr, wchar_t ** __restrict__ endptr);


  long double __attribute__((__cdecl__)) wcstold(const wchar_t * __restrict__, wchar_t ** __restrict__);

  __attribute__ ((__dllimport__)) double __attribute__((__cdecl__)) _wcstod_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,_locale_t _Locale);
  __attribute__ ((__dllimport__)) float __attribute__((__cdecl__)) _wcstof_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,_locale_t _Locale);
  long __attribute__((__cdecl__)) wcstol(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _wcstol_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  unsigned long __attribute__((__cdecl__)) wcstoul(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix);
  __attribute__ ((__dllimport__)) unsigned long __attribute__((__cdecl__)) _wcstoul_l(const wchar_t * __restrict__ _Str,wchar_t ** __restrict__ _EndPtr,int _Radix,_locale_t _Locale);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wgetenv(const wchar_t *_VarName) ;


  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wsystem(const wchar_t *_Command);

  __attribute__ ((__dllimport__)) double __attribute__((__cdecl__)) _wtof(const wchar_t *_Str);
  __attribute__ ((__dllimport__)) double __attribute__((__cdecl__)) _wtof_l(const wchar_t *_Str,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wtoi(const wchar_t *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wtoi_l(const wchar_t *_Str,_locale_t _Locale);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _wtol(const wchar_t *_Str);
  __attribute__ ((__dllimport__)) long __attribute__((__cdecl__)) _wtol_l(const wchar_t *_Str,_locale_t _Locale);

  __extension__ __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _i64tow(long long _Val,wchar_t *_DstBuf,int _Radix) ;
  __extension__ __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _ui64tow(unsigned long long _Val,wchar_t *_DstBuf,int _Radix) ;
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _wtoi64(const wchar_t *_Str);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _wtoi64_l(const wchar_t *_Str,_locale_t _Locale);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _wcstoi64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) long long __attribute__((__cdecl__)) _wcstoi64_l(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);
  __extension__ __attribute__ ((__dllimport__)) unsigned long long __attribute__((__cdecl__)) _wcstoui64(const wchar_t *_Str,wchar_t **_EndPtr,int _Radix);
  __extension__ __attribute__ ((__dllimport__)) unsigned long long __attribute__((__cdecl__)) _wcstoui64_l(const wchar_t *_Str ,wchar_t **_EndPtr,int _Radix,_locale_t _Locale);


  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _putenv(const char *_EnvString);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _wputenv(const wchar_t *_EnvString);
# 557 "C:/msys64/ucrt64/include/stdlib.h" 3
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _fullpath(char *_FullPath,const char *_Path,size_t _SizeInBytes);




  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _ecvt(double _Val,int _NumOfDigits,int *_PtDec,int *_PtSign) ;
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _fcvt(double _Val,int _NumOfDec,int *_PtDec,int *_PtSign) ;
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _gcvt(double _Val,int _NumOfDigits,char *_DstBuf) ;
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atodbl(_CRT_DOUBLE *_Result,char *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atoldbl(_LDOUBLE *_Result,char *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atoflt(_CRT_FLOAT *_Result,char *_Str);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atodbl_l(_CRT_DOUBLE *_Result,char *_Str,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atoldbl_l(_LDOUBLE *_Result,char *_Str,_locale_t _Locale);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _atoflt_l(_CRT_FLOAT *_Result,char *_Str,_locale_t _Locale);
# 586 "C:/msys64/ucrt64/include/stdlib.h" 3
unsigned long __attribute__((__cdecl__)) _lrotl(unsigned long,int);
unsigned long __attribute__((__cdecl__)) _lrotr(unsigned long,int);





  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _makepath(char *_Path,const char *_Drive,const char *_Dir,const char *_Filename,const char *_Ext);
  _onexit_t __attribute__((__cdecl__)) _onexit(_onexit_t _Func);





       
       


  __extension__ unsigned long long __attribute__((__cdecl__)) _rotl64(unsigned long long _Val,int _Shift);
  __extension__ unsigned long long __attribute__((__cdecl__)) _rotr64(unsigned long long Value,int Shift);
       
       
       
       


  unsigned int __attribute__((__cdecl__)) _rotr(unsigned int _Val,int _Shift);
  unsigned int __attribute__((__cdecl__)) _rotl(unsigned int _Val,int _Shift);
       
       
  __extension__ unsigned long long __attribute__((__cdecl__)) _rotr64(unsigned long long _Val,int _Shift);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _searchenv(const char *_Filename,const char *_EnvVar,char *_ResultPath) ;
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _splitpath(const char *_FullPath,char *_Drive,char *_Dir,char *_Filename,char *_Ext) ;
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _swab(char *_Buf1,char *_Buf2,int _SizeInBytes);







  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wfullpath(wchar_t *_FullPath,const wchar_t *_Path,size_t _SizeInWords);



  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _wmakepath(wchar_t *_ResultPath,const wchar_t *_Drive,const wchar_t *_Dir,const wchar_t *_Filename,const wchar_t *_Ext);




  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _wsearchenv(const wchar_t *_Filename,const wchar_t *_EnvVar,wchar_t *_ResultPath) ;
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _wsplitpath(const wchar_t *_FullPath,wchar_t *_Drive,wchar_t *_Dir,wchar_t *_Filename,wchar_t *_Ext) ;


  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _beep(unsigned _Frequency,unsigned _Duration) __attribute__ ((__deprecated__));

  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _seterrormode(int _Mode) __attribute__ ((__deprecated__));
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _sleep(unsigned long _Duration) __attribute__ ((__deprecated__));
# 664 "C:/msys64/ucrt64/include/stdlib.h" 3
  char *__attribute__((__cdecl__)) ecvt(double _Val,int _NumOfDigits,int *_PtDec,int *_PtSign) ;
  char *__attribute__((__cdecl__)) fcvt(double _Val,int _NumOfDec,int *_PtDec,int *_PtSign) ;
  char *__attribute__((__cdecl__)) gcvt(double _Val,int _NumOfDigits,char *_DstBuf) ;
  char *__attribute__((__cdecl__)) itoa(int _Val,char *_DstBuf,int _Radix) ;
  char *__attribute__((__cdecl__)) ltoa(long _Val,char *_DstBuf,int _Radix) ;
  int __attribute__((__cdecl__)) putenv(const char *_EnvString) ;



  void __attribute__((__cdecl__)) swab(char *_Buf1,char *_Buf2,int _SizeInBytes) ;


  char *__attribute__((__cdecl__)) ultoa(unsigned long _Val,char *_Dstbuf,int _Radix) ;
  _onexit_t __attribute__((__cdecl__)) onexit(_onexit_t _Func);





  typedef struct { __extension__ long long quot, rem; } lldiv_t;

  __extension__ lldiv_t __attribute__((__cdecl__)) lldiv(long long, long long);

  __extension__ long long __attribute__((__cdecl__)) llabs(long long);




  __extension__ long long __attribute__((__cdecl__)) strtoll(const char * __restrict__, char ** __restrict, int);
  __extension__ unsigned long long __attribute__((__cdecl__)) strtoull(const char * __restrict__, char ** __restrict__, int);


  __extension__ long long __attribute__((__cdecl__)) atoll (const char *);


  __extension__ long long __attribute__((__cdecl__)) wtoll (const wchar_t *);
  __extension__ char *__attribute__((__cdecl__)) lltoa (long long, char *, int);
  __extension__ char *__attribute__((__cdecl__)) ulltoa (unsigned long long , char *, int);
  __extension__ wchar_t *__attribute__((__cdecl__)) lltow (long long, wchar_t *, int);
  __extension__ wchar_t *__attribute__((__cdecl__)) ulltow (unsigned long long, wchar_t *, int);
# 721 "C:/msys64/ucrt64/include/stdlib.h" 3
#pragma pack(pop)

# 1 "C:/msys64/ucrt64/include/sec_api/stdlib_s.h" 1 3
# 9 "C:/msys64/ucrt64/include/sec_api/stdlib_s.h" 3
# 1 "C:/msys64/ucrt64/include/stdlib.h" 1 3
# 10 "C:/msys64/ucrt64/include/sec_api/stdlib_s.h" 2 3
# 19 "C:/msys64/ucrt64/include/sec_api/stdlib_s.h" 3
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _dupenv_s(char **_PBuffer,size_t *_PBufferSizeInBytes,const char *_VarName);




  __attribute__ ((__dllimport__)) void * __attribute__((__cdecl__)) bsearch_s(const void *_Key,const void *_Base,rsize_t _NumOfElements,rsize_t _SizeOfElements,int (__attribute__((__cdecl__)) * _PtFuncCompare)(void *, const void *, const void *), void *_Context);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) getenv_s(size_t *_ReturnSize,char *_DstBuf,rsize_t _DstSize,const char *_VarName);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _itoa_s(int _Value,char *_DstBuf,size_t _Size,int _Radix);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _i64toa_s(long long _Val,char *_DstBuf,size_t _Size,int _Radix);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ui64toa_s(unsigned long long _Val,char *_DstBuf,size_t _Size,int _Radix);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ltoa_s(long _Val,char *_DstBuf,size_t _Size,int _Radix);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) mbstowcs_s(size_t *_PtNumOfCharConverted,wchar_t *_DstBuf,size_t _SizeInWords,const char *_SrcBuf,size_t _MaxCount);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _mbstowcs_s_l(size_t *_PtNumOfCharConverted,wchar_t *_DstBuf,size_t _SizeInWords,const char *_SrcBuf,size_t _MaxCount,_locale_t _Locale);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ultoa_s(unsigned long _Val,char *_DstBuf,size_t _Size,int _Radix);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) wctomb_s(int *_SizeConverted,char *_MbCh,rsize_t _SizeInBytes,wchar_t _WCh);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wctomb_s_l(int *_SizeConverted,char *_MbCh,size_t _SizeInBytes,wchar_t _WCh,_locale_t _Locale);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) wcstombs_s(size_t *_PtNumOfCharConverted,char *_Dst,size_t _DstSizeInBytes,const wchar_t *_Src,size_t _MaxCountInBytes);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _wcstombs_s_l(size_t *_PtNumOfCharConverted,char *_Dst,size_t _DstSizeInBytes,const wchar_t *_Src,size_t _MaxCountInBytes,_locale_t _Locale);
 


  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ecvt_s(char *_DstBuf,size_t _Size,double _Val,int _NumOfDights,int *_PtDec,int *_PtSign);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _fcvt_s(char *_DstBuf,size_t _Size,double _Val,int _NumOfDec,int *_PtDec,int *_PtSign);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _gcvt_s(char *_DstBuf,size_t _Size,double _Val,int _NumOfDigits);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _makepath_s(char *_PathResult,size_t _Size,const char *_Drive,const char *_Dir,const char *_Filename,const char *_Ext);
 
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _putenv_s(const char *_Name,const char *_Value);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _searchenv_s(const char *_Filename,const char *_EnvVar,char *_ResultPath,size_t _SizeInBytes);

  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _splitpath_s(const char *_FullPath,char *_Drive,size_t _DriveSize,char *_Dir,size_t _DirSize,char *_Filename,size_t _FilenameSize,char *_Ext,size_t _ExtSize);
 



  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) qsort_s(void *_Base,size_t _NumOfElements,size_t _SizeOfElements,int (__attribute__((__cdecl__)) *_PtFuncCompare)(void *,const void *,const void *),void *_Context);
# 724 "C:/msys64/ucrt64/include/stdlib.h" 2 3
# 1 "C:/msys64/ucrt64/include/malloc.h" 1 3
# 11 "C:/msys64/ucrt64/include/malloc.h" 3
#pragma pack(push,_CRT_PACKING)
# 52 "C:/msys64/ucrt64/include/malloc.h" 3
  typedef struct _heapinfo {
    int *_pentry;
    size_t _size;
    int _useflag;
  } _HEAPINFO;



  __attribute__ ((__dllimport__)) unsigned int *__attribute__((__cdecl__)) __p__amblksiz(void);
# 129 "C:/msys64/ucrt64/include/malloc.h" 3
void * __mingw_aligned_malloc (size_t _Size, size_t _Alignment);
void __mingw_aligned_free (void *_Memory);
void * __mingw_aligned_offset_realloc (void *_Memory, size_t _Size, size_t _Alignment, size_t _Offset);
void * __mingw_aligned_offset_malloc (size_t, size_t, size_t);
void * __mingw_aligned_realloc (void *_Memory, size_t _Size, size_t _Offset);
size_t __mingw_aligned_msize (void *memblock, size_t alignment, size_t offset);



# 1 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/mm_malloc.h" 1 3
# 29 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/mm_malloc.h" 3
# 1 "C:/msys64/ucrt64/include/errno.h" 1 3
# 30 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/mm_malloc.h" 2 3


static __inline__ void *
_mm_malloc (size_t __size, size_t __align)
{
  void * __malloc_ptr;
  void * __aligned_ptr;


  if (__align & (__align - 1))
    {

      (*_errno()) = 22;

      return ((void *) 0);
    }

  if (__size == 0)
    return ((void *) 0);





    if (__align < 2 * sizeof (void *))
      __align = 2 * sizeof (void *);

  __malloc_ptr = malloc (__size + __align);
  if (!__malloc_ptr)
    return ((void *) 0);


  __aligned_ptr = (void *) (((size_t) __malloc_ptr + __align)
       & ~((size_t) (__align) - 1));


  ((void **) __aligned_ptr)[-1] = __malloc_ptr;

  return __aligned_ptr;
}

static __inline__ void
_mm_free (void *__aligned_ptr)
{
  if (__aligned_ptr)
    free (((void **) __aligned_ptr)[-1]);
}
# 139 "C:/msys64/ucrt64/include/malloc.h" 2 3





  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _resetstkoflw (void);

  __attribute__ ((__dllimport__)) unsigned long __attribute__((__cdecl__)) _set_malloc_crt_max_wait(unsigned long _NewValue);







  __attribute__ ((__dllimport__)) void *__attribute__((__cdecl__)) _expand(void *_Memory,size_t _NewSize);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _msize(void *_Memory);
# 167 "C:/msys64/ucrt64/include/malloc.h" 3
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _get_sbh_threshold(void);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _set_sbh_threshold(size_t _NewValue);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _set_amblksiz(size_t _Value);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _get_amblksiz(size_t *_Value);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _heapadd(void *_Memory,size_t _Size);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _heapchk(void);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _heapmin(void);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _heapset(unsigned int _Fill);
  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _heapwalk(_HEAPINFO *_EntryInfo);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _heapused(size_t *_Used,size_t *_Commit);
  __attribute__ ((__dllimport__)) intptr_t __attribute__((__cdecl__)) _get_heap_handle(void);
# 190 "C:/msys64/ucrt64/include/malloc.h" 3
  static __inline void *_MarkAllocaS(void *_Ptr,unsigned int _Marker) {
    if(_Ptr) {
      *((unsigned int*)_Ptr) = _Marker;
      _Ptr = (char*)_Ptr + 16;
    }
    return _Ptr;
  }
# 218 "C:/msys64/ucrt64/include/malloc.h" 3
  static __inline void __attribute__((__cdecl__)) _freea(void *_Memory) {
    unsigned int _Marker;
    if(_Memory) {
      _Memory = (char*)_Memory - 16;
      _Marker = *(unsigned int *)_Memory;
      if(_Marker==0xDDDD) {
 free(_Memory);
      }





    }
  }
# 264 "C:/msys64/ucrt64/include/malloc.h" 3
#pragma pack(pop)
# 725 "C:/msys64/ucrt64/include/stdlib.h" 2 3
# 3 ".\\StringRoutine.c" 2
# 1 "C:/msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/16.1.0/include/stdbool.h" 1 3
# 4 ".\\StringRoutine.c" 2
# 1 "C:/msys64/ucrt64/include/time.h" 1 3
# 25 "C:/msys64/ucrt64/include/time.h" 3
# 1 "C:/msys64/ucrt64/include/sys/timeb.h" 1 3
# 15 "C:/msys64/ucrt64/include/sys/timeb.h" 3
#pragma pack(push,_CRT_PACKING)
# 53 "C:/msys64/ucrt64/include/sys/timeb.h" 3
  struct __timeb32 {
    __time32_t time;
    unsigned short millitm;
    short timezone;
    short dstflag;
  };


  struct timeb {
    time_t time;
    unsigned short millitm;
    short timezone;
    short dstflag;
  };


  struct __timeb64 {
    __time64_t time;
    unsigned short millitm;
    short timezone;
    short dstflag;
  };



  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _ftime64(struct __timeb64 *_Time);
  __attribute__ ((__dllimport__)) void __attribute__((__cdecl__)) _ftime32(struct __timeb32 *_Time);
# 95 "C:/msys64/ucrt64/include/sys/timeb.h" 3
struct _timespec32 {
  __time32_t tv_sec;
  long tv_nsec;
};

struct _timespec64 {
  __time64_t tv_sec;
  long tv_nsec;
};



struct timespec {
  time_t tv_sec;
  long tv_nsec;
};

struct itimerspec {
  struct timespec it_interval;
  struct timespec it_value;
};
# 125 "C:/msys64/ucrt64/include/sys/timeb.h" 3
  int __attribute__((__cdecl__)) ftime (struct timeb *) __asm__("ftime64");
# 136 "C:/msys64/ucrt64/include/sys/timeb.h" 3
#pragma pack(pop)

# 1 "C:/msys64/ucrt64/include/sec_api/sys/timeb_s.h" 1 3
# 10 "C:/msys64/ucrt64/include/sec_api/sys/timeb_s.h" 3
# 1 "C:/msys64/ucrt64/include/sys/timeb.h" 1 3
# 11 "C:/msys64/ucrt64/include/sec_api/sys/timeb_s.h" 2 3





  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ftime32_s(struct __timeb32 *_Time);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _ftime64_s(struct __timeb64 *_Time);
# 139 "C:/msys64/ucrt64/include/sys/timeb.h" 2 3
# 26 "C:/msys64/ucrt64/include/time.h" 2 3

#pragma pack(push,_CRT_PACKING)
# 63 "C:/msys64/ucrt64/include/time.h" 3
  typedef long clock_t;
# 100 "C:/msys64/ucrt64/include/time.h" 3
  struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
  };
# 119 "C:/msys64/ucrt64/include/time.h" 3
  __attribute__ ((__dllimport__)) int *__attribute__((__cdecl__)) __daylight(void);
  __attribute__ ((__dllimport__)) long *__attribute__((__cdecl__)) __dstbias(void);
  __attribute__ ((__dllimport__)) long *__attribute__((__cdecl__)) __timezone(void);
  __attribute__ ((__dllimport__)) char **__attribute__((__cdecl__)) __tzname(void);
# 138 "C:/msys64/ucrt64/include/time.h" 3
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _get_daylight(int *_Daylight);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _get_dstbias(long *_Daylight_savings_bias);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _get_timezone(long *_Timezone);
  __attribute__ ((__dllimport__)) errno_t __attribute__((__cdecl__)) _get_tzname(size_t *_ReturnValue,char *_Buffer,size_t _SizeInBytes,int _Index);
  char *__attribute__((__cdecl__)) asctime(const struct tm *_Tm) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) asctime_s (char *_Buf,size_t _SizeInWords,const struct tm *_Tm);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _ctime32(const __time32_t *_Time) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _ctime32_s (char *_Buf,size_t _SizeInBytes,const __time32_t *_Time);
  clock_t __attribute__((__cdecl__)) clock(void);
  __attribute__ ((__dllimport__)) double __attribute__((__cdecl__)) _difftime32(__time32_t _Time1,__time32_t _Time2);
  __attribute__ ((__dllimport__)) struct tm *__attribute__((__cdecl__)) _gmtime32(const __time32_t *_Time) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _gmtime32_s (struct tm *_Tm,const __time32_t *_Time);
  __attribute__ ((__dllimport__)) struct tm *__attribute__((__cdecl__)) _localtime32(const __time32_t *_Time) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _localtime32_s (struct tm *_Tm,const __time32_t *_Time);
  size_t __attribute__((__cdecl__)) strftime(char * __restrict__ _Buf,size_t _SizeInBytes,const char * __restrict__ _Format,const struct tm * __restrict__ _Tm) __attribute__((__format__ (gnu_strftime, 3, 0)));
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _strftime_l(char * __restrict__ _Buf,size_t _Max_size,const char * __restrict__ _Format,const struct tm * __restrict__ _Tm,_locale_t _Locale);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _strdate(char *_Buffer) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _strdate_s (char *_Buf,size_t _SizeInBytes);
 
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _strtime(char *_Buffer) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _strtime_s (char *_Buf ,size_t _SizeInBytes);
 
  __attribute__ ((__dllimport__)) __time32_t __attribute__((__cdecl__)) _time32(__time32_t *_Time);

  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _timespec32_get(struct _timespec32 *_Ts, int _Base);

  __attribute__ ((__dllimport__)) __time32_t __attribute__((__cdecl__)) _mktime32(struct tm *_Tm);
  __attribute__ ((__dllimport__)) __time32_t __attribute__((__cdecl__)) _mkgmtime32(struct tm *_Tm);


  void __attribute__((__cdecl__)) tzset(void) ;





  void __attribute__((__cdecl__)) _tzset(void);


  __attribute__ ((__dllimport__)) double __attribute__((__cdecl__)) _difftime64(__time64_t _Time1,__time64_t _Time2);
  __attribute__ ((__dllimport__)) char *__attribute__((__cdecl__)) _ctime64(const __time64_t *_Time) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _ctime64_s (char *_Buf,size_t _SizeInBytes,const __time64_t *_Time);
  __attribute__ ((__dllimport__)) struct tm *__attribute__((__cdecl__)) _gmtime64(const __time64_t *_Time) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _gmtime64_s (struct tm *_Tm,const __time64_t *_Time);
  __attribute__ ((__dllimport__)) struct tm *__attribute__((__cdecl__)) _localtime64(const __time64_t *_Time) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _localtime64_s (struct tm *_Tm,const __time64_t *_Time);
  __attribute__ ((__dllimport__)) __time64_t __attribute__((__cdecl__)) _mktime64(struct tm *_Tm);
  __attribute__ ((__dllimport__)) __time64_t __attribute__((__cdecl__)) _mkgmtime64(struct tm *_Tm);
  __attribute__ ((__dllimport__)) __time64_t __attribute__((__cdecl__)) _time64(__time64_t *_Time);

  __attribute__ ((__dllimport__)) int __attribute__((__cdecl__)) _timespec64_get(struct _timespec64 *_Ts, int _Base);

  unsigned __attribute__((__cdecl__)) _getsystime(struct tm *_Tm);
  unsigned __attribute__((__cdecl__)) _setsystime(struct tm *_Tm,unsigned _MilliSec);


  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wasctime(const struct tm *_Tm);
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wasctime_s (wchar_t *_Buf,size_t _SizeInWords,const struct tm *_Tm);
  wchar_t *__attribute__((__cdecl__)) _wctime32(const __time32_t *_Time) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wctime32_s (wchar_t *_Buf,size_t _SizeInWords,const __time32_t *_Time);
  size_t __attribute__((__cdecl__)) wcsftime(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm);
  __attribute__ ((__dllimport__)) size_t __attribute__((__cdecl__)) _wcsftime_l(wchar_t * __restrict__ _Buf,size_t _SizeInWords,const wchar_t * __restrict__ _Format,const struct tm * __restrict__ _Tm,_locale_t _Locale);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wstrdate(wchar_t *_Buffer) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wstrdate_s (wchar_t *_Buf,size_t _SizeInWords);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wstrtime(wchar_t *_Buffer) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wstrtime_s (wchar_t *_Buf,size_t _SizeInWords);
  __attribute__ ((__dllimport__)) wchar_t *__attribute__((__cdecl__)) _wctime64(const __time64_t *_Time) ;
  __attribute__((dllimport)) errno_t __attribute__((__cdecl__)) _wctime64_s (wchar_t *_Buf,size_t _SizeInWords,const __time64_t *_Time);




  wchar_t *__attribute__((__cdecl__)) _wctime(const time_t *_Time) __asm__("_wctime64") ;
# 219 "C:/msys64/ucrt64/include/time.h" 3
  errno_t __attribute__((__cdecl__)) _wctime_s (wchar_t *_Buffer,size_t _SizeInWords,const time_t *_Time) __asm__("_wctime64_s");
# 250 "C:/msys64/ucrt64/include/time.h" 3
time_t __attribute__((__cdecl__)) time(time_t *_Time) __asm__("_time64");

int __attribute__((__cdecl__)) timespec_get(struct timespec* _Ts, int _Base) __asm__("_timespec64_get");

double __attribute__((__cdecl__)) difftime(time_t _Time1,time_t _Time2) __asm__("_difftime64");
struct tm *__attribute__((__cdecl__)) localtime(const time_t *_Time) __asm__("_localtime64");
errno_t __attribute__((__cdecl__)) localtime_s(struct tm *_Tm,const time_t *_Time) __asm__("_localtime64_s");
struct tm *__attribute__((__cdecl__)) gmtime(const time_t *_Time) __asm__("_gmtime64");
errno_t __attribute__((__cdecl__)) gmtime_s(struct tm *_Tm, const time_t *_Time) __asm__("_gmtime64_s");
char *__attribute__((__cdecl__)) ctime(const time_t *_Time) __asm__("_ctime64");
errno_t __attribute__((__cdecl__)) ctime_s(char *_Buf,size_t _SizeInBytes,const time_t *_Time) __asm__("_ctime64_s");
time_t __attribute__((__cdecl__)) mktime(struct tm *_Tm) __asm__("_mktime64");
time_t __attribute__((__cdecl__)) _mkgmtime(struct tm *_Tm) __asm__("_mkgmtime64");
# 280 "C:/msys64/ucrt64/include/time.h" 3
  __attribute__ ((__dllimport__)) extern int daylight __attribute__ ((__deprecated__("Only provided for source compatibility; this variable might " "not always be accurate when linking to UCRT.")));
  __attribute__ ((__dllimport__)) extern long timezone __attribute__ ((__deprecated__("Only provided for source compatibility; this variable might " "not always be accurate when linking to UCRT.")));
  __attribute__ ((__dllimport__)) extern char *tzname[2] __attribute__ ((__deprecated__("Only provided for source compatibility; this variable might " "not always be accurate when linking to UCRT.")));
  void __attribute__((__cdecl__)) tzset(void) ;


# 1 "C:/msys64/ucrt64/include/_timeval.h" 1 3
# 10 "C:/msys64/ucrt64/include/_timeval.h" 3
struct timeval
{
 long tv_sec;
 long tv_usec;
};
# 287 "C:/msys64/ucrt64/include/time.h" 2 3



struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

  extern int __attribute__((__cdecl__)) mingw_gettimeofday (struct timeval *p, struct timezone *z);


#pragma pack(pop)
# 329 "C:/msys64/ucrt64/include/time.h" 3
# 1 "C:/msys64/ucrt64/include/pthread_time.h" 1 3
# 27 "C:/msys64/ucrt64/include/pthread_time.h" 3
# 1 "C:/msys64/ucrt64/include/pthread_compat.h" 1 3
# 91 "C:/msys64/ucrt64/include/pthread_compat.h" 3
typedef int clockid_t;





typedef unsigned short mode_t;
# 28 "C:/msys64/ucrt64/include/pthread_time.h" 2 3
# 77 "C:/msys64/ucrt64/include/pthread_time.h" 3
 int __attribute__((__cdecl__)) nanosleep32(const struct _timespec32 *request, struct _timespec32 *remain);
 int __attribute__((__cdecl__)) nanosleep64(const struct _timespec64 *request, struct _timespec64 *remain);
static __inline__ __attribute__((__always_inline__)) int __attribute__((__cdecl__)) nanosleep(const struct timespec *request, struct timespec *remain)
{



  return nanosleep64 ((struct _timespec64 *)request, (struct _timespec64 *)remain);

}

 int __attribute__((__cdecl__)) clock_nanosleep32(clockid_t clock_id, int flags, const struct _timespec32 *request, struct _timespec32 *remain);
 int __attribute__((__cdecl__)) clock_nanosleep64(clockid_t clock_id, int flags, const struct _timespec64 *request, struct _timespec64 *remain);
static __inline__ __attribute__((__always_inline__)) int __attribute__((__cdecl__)) clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain)
{



  return clock_nanosleep64 (clock_id, flags, (struct _timespec64 *)request, (struct _timespec64 *)remain);

}

 int __attribute__((__cdecl__)) clock_getres32(clockid_t clock_id, struct _timespec32 *res);
 int __attribute__((__cdecl__)) clock_getres64(clockid_t clock_id, struct _timespec64 *res);
static __inline__ __attribute__((__always_inline__)) int __attribute__((__cdecl__)) clock_getres(clockid_t clock_id, struct timespec *res)
{



  return clock_getres64 (clock_id, (struct _timespec64 *)res);

}

 int __attribute__((__cdecl__)) clock_gettime32(clockid_t clock_id, struct _timespec32 *tp);
 int __attribute__((__cdecl__)) clock_gettime64(clockid_t clock_id, struct _timespec64 *tp);
static __inline__ __attribute__((__always_inline__)) int __attribute__((__cdecl__)) clock_gettime(clockid_t clock_id, struct timespec *tp)
{



  return clock_gettime64 (clock_id, (struct _timespec64 *)tp);

}

 int __attribute__((__cdecl__)) clock_settime32(clockid_t clock_id, const struct _timespec32 *tp);
 int __attribute__((__cdecl__)) clock_settime64(clockid_t clock_id, const struct _timespec64 *tp);
static __inline__ __attribute__((__always_inline__)) int __attribute__((__cdecl__)) clock_settime(clockid_t clock_id, const struct timespec *tp)
{



  return clock_settime64 (clock_id, (struct _timespec64 *)tp);

}
# 330 "C:/msys64/ucrt64/include/time.h" 2 3
# 5 ".\\StringRoutine.c" 2


# 6 ".\\StringRoutine.c"
void reverse(char *s, int n) {
    for (int i = 0; i < n/2; i++) {
        char t = s[i];
        s[i] = s[n-1-i];
        s[n-1-i] = t;
    }
}
int main() {
    char buf[1000000];
    clock_t t0 = clock();
    for (int i=0;i<1000000;i++) buf[i]='a'+(i%26);
    for (int i=0;i<1000;i++) reverse(buf, 1000000);
    clock_t t1 = clock();
    printf("%f ms\n", (double)(t1-t0)*1000.0/
# 19 ".\\StringRoutine.c" 3
                                            1000
# 19 ".\\StringRoutine.c"
                                                          );
    return 0;
}
