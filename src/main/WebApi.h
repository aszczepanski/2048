#ifndef SRC_MAIN_WEB_API_H_
#define SRC_MAIN_WEB_API_H_

#include "common/GameState.h"

// taken from: https://gcc.gnu.org/wiki/Visibility
#if defined _WIN32 || defined __CYGWIN__
	#ifdef BUILDING_DLL
		#ifdef __GNUC__
			#define DLL_PUBLIC __attribute__ ((dllexport))
		#else
			#define DLL_PUBLIC __declspec(dllexport)
		#endif
	#else
		#ifdef __GNUC__
			#define DLL_PUBLIC __attribute__ ((dllimport))
		#else
			#define DLL_PUBLIC __declspec(dllimport)
		#endif
	#endif
	#define DLL_LOCAL
	#else
	#if __GNUC__ >= 4
		#define DLL_PUBLIC __attribute__ ((visibility ("default")))
		#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
	#else
		#define DLL_PUBLIC
		#define DLL_LOCAL
	#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

DLL_PUBLIC void setEvalMultithreading(bool multithreading);
DLL_PUBLIC void setMaxDepth(int depth);
DLL_PUBLIC void setMaxTime(int time);
DLL_PUBLIC void setStrategy(const char* strategy);
DLL_PUBLIC void setUnzip(bool unzip);
DLL_PUBLIC bool loadTuplesDescriptor();
DLL_PUBLIC void initializeTables();
DLL_PUBLIC void initializeEvaluator();
DLL_PUBLIC int bestAction(GameState gameState);

#ifdef __cplusplus
}
#endif

const int ACTION_CODE_UP = 0;
const int ACTION_CODE_DOWN = 1;
const int ACTION_CODE_LEFT = 2;
const int ACTION_CODE_RIGHT = 3;

#endif  // SRC_MAIN_WEB_API_H_
