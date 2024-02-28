
#ifdef DECLARE_FUNC
DECLARE_FUNC(a,test1,test2)
DECLARE_FUNC(b,test3)
#undef DECLARE_FUNC
#else
#error "define DECLARE_FUNC first"
#endif