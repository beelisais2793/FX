local ffi=require('ffi')
ffi.cdef[[        
    char    *ctermid(char *);    
    void     clearerr(void *); 
    //int      dprintf(int, const char *, ...)
    int      fclose(void *);
    void    *fdopen(int, const char *);
    int      feof(void *);
    int      ferror(void *);
    int      fflush(void *);
    int      fgetc(void *);
    //int      fgetpos(void *, fpos_t *);
    char    *fgets(char *, int, void *);
    int      voidno(void *);
    void     flockvoid(void *);
    void    *fmemopen(void *, size_t, const char *);
    void    *fopen(const char *, const char *);
    int      fprintf(void *, const char *, ...);
    int      fputc(int, void *);
    int      fputs(const char *, void *);
    size_t   fread(void *, size_t, size_t, void *);
    void    *freopen(const char *, const char *, void *);
    int      fscanf(void *, const char *, ...);
    int      fseek(void *, long, int);
    //int      fseeko(void *, off_t, int);
    //int      fsetpos(void *, const fpos_t *);
    long     ftell(void *);
    //off_t    ftello(void *);
    int      ftrylockvoid(void *);
    void     funlockvoid(void *);
    size_t   fwrite(const void *, size_t, size_t, void *);
    int      getc(void *);
    int      getchar(void);
    int      getc_unlocked(void *);
    int      getchar_unlocked(void);
    ssize_t  getdelim(char **, size_t *, int, void *);
    ssize_t  getline(char **, size_t *, void *);
    int      pclose(void *);    
    char    *gets(char *);
    //void    *open_memstream(char **, size_t *);
    
    void     perror(const char *);
    //void    *popen(const char *, const char *);
    int      printf(const char *, ...);    
    int      putchar(int);    
    int      putchar_unlocked(int);
    int      puts(const char *);
    int      remove(const char *);
    int      rename(const char *, const char *);
    int      renameat(int, const char *, int, const char *);
    //int      putc(int, void *);
    //int      putc_unlocked(int, void *);
    //void     rewind(void *);
    //void     setbuf(void *, char *);
    //int      setvbuf(void *, char *, int, size_t);
    int      scanf(const char *, ...);    
    int      snprintf(char *, size_t, const char *, ...);
    int      sprintf(char *, const char *, ...);
    int      sscanf(const char *, const char *, ...);
    char    *tempnam(const char *, const char *);    
    char    *tmpnam(char *);    
    /*
    int      vdprintf(int, const char *, va_list);
    int      vprintf(const char *, va_list);
    int      vscanf(const char *, va_list);
    int      vsnprintf(char *, size_t, const char *,
                va_list);
    int      vsprintf(char *, const char *, va_list);
    int      vsscanf(const char *, const char *, va_list);
    */

    // ctype
    int   isalnum(int);
    int   isalpha(int);
    int   isascii(int);
    int   isblank(int);
    int   iscntrl(int);
    int   isdigit(int);
    int   isgraph(int);
    int   islower(int);
    int   isprint(int);
    int   ispunct(int);
    int   isspace(int);
    int   isupper(int);
    int   isxdigit(int);
    int   toascii(int);
    int   tolower(int);
    int   toupper(int);
    int   _toupper(int);
    int   _tolower(int);


    void    *memccpy(void *, const void *, int, size_t);
    void    *memchr(const void *, int, size_t);
    int      memcmp(const void *, const void *, size_t);
    void    *memcpy(void *, const void *, size_t);
    void    *memmove(void *, const void *, size_t);
    void    *memset(void *, int, size_t);
    char    *stpcpy(char *, const char *);
    char    *stpncpy(char *, const char *, size_t);
    char    *strcat(char *, const char *);
    char    *strchr(const char *, int);
    int      strcmp(const char *, const char *);
    int      strcoll(const char *, const char *);
    char    *strcpy(char *, const char *);
    size_t   strcspn(const char *, const char *);
    char    *strdup(const char *);
    char    *strerror(int);
    int      strerror_r(int, char *, size_t);
    size_t   strlen(const char *);
    char    *strncat(char *, const char *, size_t);
    int      strncmp(const char *, const char *, size_t);
    char    *strncpy(char *, const char *, size_t);
    char    *strndup(const char *, size_t);
    size_t   strnlen(const char *, size_t);
    char    *strpbrk(const char *, const char *);
    char    *strrchr(const char *, int);
    char    *strsignal(int);
    size_t   strspn(const char *, const char *);
    char    *strstr(const char *, const char *);
    char    *strtok(char *, const char *);
    char    *strtok_r(char *, const char *, char **);
    size_t   strxfrm(char *, const char *, size_t);

    
    void          _Exit(int);
    long          a64l(const char *);
    void          abort(void);
    int           abs(int);
    int           atexit(void (*)(void));
    double        atof(const char *);
    int           atoi(const char *);
    long          atol(const char *);
    long long     atoll(const char *);
    void         *bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
    void         *calloc(size_t, size_t);
    
    double        drand48(void);    
    double        erand48(unsigned short[3]);
    void          exit(int);    
    void          free(void *);    
    char         *getenv(const char *);
    int           getsubopt(char **, char *const *, char **);
    int           grantpt(int);
    char         *initstate(unsigned, char *, size_t);
    long          jrand48(unsigned short[3]);
    char         *l64a(long);
    long          labs(long);
    void          lcong48(unsigned short[7]);
    long long     llabs(long long);    
    long          lrand48(void);
    void         *malloc(size_t);
    int           mblen(const char *, size_t);
    size_t        mbstowcs(wchar_t *, const char *, size_t);
    int           mbtowc(wchar_t *, const char *, size_t);    
    int           mkstemp(char *);
    long          mrand48(void);
    long          nrand48(unsigned short[3]);
    int           posix_memalign(void **, size_t, size_t);
    int           posix_openpt(int);
    char         *ptsname(int);
    int           putenv(char *);
    void          qsort(void *, size_t, size_t, int (*)(const void *,const void *));
    int           rand(void);
    int           rand_r(unsigned *);
    long          random(void);
    void         *realloc(void *, size_t);
    char         *realpath(const char *, char *);
    unsigned short seed48(unsigned short[3]);
    int           setenv(const char *, const char *, int);
    void          setkey(const char *);
    char         *setstate(const char *);
    void          srand(unsigned);
    void          srand48(long);
    void          srandom(unsigned);
    double        strtod(const char *, char **);
    float         strtof(const char *, char **);
    long          strtol(const char *, char **, int);
    long double   strtold(const char *, char **);
    long long     strtoll(const char *, char **, int);
    unsigned long strtoul(const char *, char **, int);
    unsigned long long strtoull(const char *, char **, int);
    int           system(const char *);
    int           unlockpt(int);
    int           unsetenv(const char *);


    // time.h
    typedef struct tm {
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
    typedef int time_t;

    char *asctime(const struct tm *tm);
    char *asctime_r(const struct tm * tm, char * buf);

    char *ctime(const time_t *timep);
    char *ctime_r(const time_t * timep, char * buf);

    struct tm *gmtime(const time_t *timep);
    struct tm *gmtime_r(const time_t * timep,
                        struct tm * result);

    struct tm *localtime(const time_t *timep);
    struct tm *localtime_r(const time_t * timep,
                        struct tm * result);

    time_t mktime(struct tm *tm);
]]
ffi.C.printf("Hello %s!", "world")
f = ffi.C.fopen("test","w")
ffi.C.fprintf(f,"hi")
ffi.C.fclose(f)