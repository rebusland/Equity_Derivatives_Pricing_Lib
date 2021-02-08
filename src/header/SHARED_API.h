
#ifndef SHARED_API_H
#define SHARED_API_H

#ifdef SHARED_EXPORTS_BUILT_AS_STATIC
#  define SHARED_API
#  define EQPRICER_NO_EXPORT
#else
#  ifndef SHARED_API
#    ifdef EQPricer_EXPORTS
        /* We are building this library */
#      define SHARED_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define SHARED_API __declspec(dllimport)
#    endif
#  endif

#  ifndef EQPRICER_NO_EXPORT
#    define EQPRICER_NO_EXPORT 
#  endif
#endif

#ifndef EQPRICER_DEPRECATED
#  define EQPRICER_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef EQPRICER_DEPRECATED_EXPORT
#  define EQPRICER_DEPRECATED_EXPORT SHARED_API EQPRICER_DEPRECATED
#endif

#ifndef EQPRICER_DEPRECATED_NO_EXPORT
#  define EQPRICER_DEPRECATED_NO_EXPORT EQPRICER_NO_EXPORT EQPRICER_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef EQPRICER_NO_DEPRECATED
#    define EQPRICER_NO_DEPRECATED
#  endif
#endif

#endif /* SHARED_API_H */
