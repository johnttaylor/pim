#ifndef COLONY_CONFIG_H_
#define COLONY_CONFIG_H_

#define USE_CPL_SYSTEM_TRACE

// For C tests
#define CPL_CONTAINER_ITEM_FATAL_ERROR_HANDLER myFatalErrorHandler
#ifdef __cplusplus
extern "C" {
#endif

    extern void myFatalErrorHandler( void* item, void* currentList, void* newList );

#ifdef __cplusplus
}
#endif


#endif
