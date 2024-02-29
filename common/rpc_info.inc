
#ifndef BEGIN_MODULE
#define BEGIN_MODULE(...)
#endif
#ifndef END_MODULE
#define END_MODULE(...)
#endif
#ifndef MODULE_FUNCTION
#define MODULE_FUNCTION(...)
#endif
BEGIN_MODULE(a)
MODULE_FUNCTION(test1,char ,int  )
MODULE_FUNCTION(test2,test_2 *)

END_MODULE()
BEGIN_MODULE(b)
MODULE_FUNCTION(test3,const char *)
END_MODULE()
#undef BEGIN_MODULE
#undef END_MODULE
#undef MODULE_FUNCTION
