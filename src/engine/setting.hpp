/*
    Egaroucid Project

    @file setting.hpp
        Main settings of Egaroucid
    @date 2021-2024
    @author Takuto Yamana
    @license GPL-3.0 license
*/

/* Compile Options
    -DHAS_AVX512        : Use AVX-512
    -DHAS_NO_AVX2       : no AVX2
    -DHAS_ARM_PROCESSOR : ARM Processor
    -DHAS_32_BIT_OS     : 32bit environment
*/

#include <string>

/*
    @brief version settings
*/
#define EGAROUCID_ENGINE_VERSION "7.3"
#define USE_BETA_VERSION false


/*
    @brief GUI settings
*/
// GUI portable mode
#define GUI_PORTABLE_MODE true
// GUI Open Console?
#define GUI_OPEN_CONSOLE false


/*
    @brief Option
*/
// use SIMD
#ifndef HAS_NO_AVX2
    #define USE_SIMD true
    // #define HAS_AVX512
    #ifdef HAS_AVX512
        #define USE_AVX512 true
    #endif
#endif

// use ARM
#ifdef HAS_ARM_PROCESSOR
    #define USE_ARM true
#endif

#ifndef HAS_32_BIT_OS
    #define USE_64_BIT true
#endif



/*
    @brief Egaroucid engine settings
*/

// OS
#ifdef _WIN64 
    #define EGAROUCID_OS (std::string)"Windows"
#elif defined _WIN32
    #define EGAROUCID_OS (std::string)"Windows"
#elif defined __APPLE__
    #define EGAROUCID_OS (std::string)"Mac OSX"
#else
    #define EGAROUCID_OS (std::string)"Linux"
#endif

// CPU type
#if USE_ARM
    #if USE_64_BIT
        #define EGAROUCID_CPU (std::string)"ARM64"
    #else
        #define EGAROUCID_CPU (std::string)"ARM"
    #endif
#else
    #if USE_64_BIT
        #define EGAROUCID_CPU (std::string)"x64"
    #else
        #define EGAROUCID_CPU (std::string)"x86"
    #endif
#endif

// revision
#if USE_SIMD
    #if USE_AVX512
        #define EGAROUCID_REVISION (std::string)"AVX512"
    #else
        #define EGAROUCID_REVISION (std::string)"SIMD"
    #endif
#else
    #define EGAROUCID_REVISION (std::string)"Generic"
#endif

// compiler
#ifdef __clang_version__
    #define EGAROUCID_COMPILER (std::string)"Clang"
#elif defined __GNUC__
    #define EGAROUCID_COMPILER (std::string)"GCC"
#elif defined _MSC_VER
    #define EGAROUCID_COMPILER (std::string)"MSVC"
#else
    #define EGAROUCID_COMPILER (std::string)"Unknown Compiler"
#endif

#define EGAROUCID_ENGINE_ENV_VERSION (EGAROUCID_OS + " " + EGAROUCID_CPU + " " + EGAROUCID_REVISION + " (" + EGAROUCID_COMPILER + ")")


/*
    @brief performance settings
*/

#if USE_SIMD
    #if USE_ARM
        // vertical mirror
        #define USE_FAST_VERTICAL_MIRROR true

        // pop_count
        #define USE_BUILTIN_POPCOUNT true

        // NTZ
        #define USE_MINUS_NTZ false

        // use SIMD in evaluation (pattern) function
        #define USE_SIMD_EVALUATION true
    #else
        // vertical mirror
        #define USE_FAST_VERTICAL_MIRROR true

        // pop_count
        #define USE_BUILTIN_POPCOUNT true

        // NTZ
        #define USE_BUILTIN_NTZ true

        // next bit
        #define USE_FAST_NEXT_BIT true

        // use SIMD in evaluation (pattern) function
        #define USE_SIMD_EVALUATION true

        // use bit gather optimization
        #define USE_BIT_GATHER_OPTIMIZE true

        // use fast join_h_line
        #define USE_FAST_JOIN_H_LINE true
    #endif
#else
    // NTZ
    #define USE_MINUS_NTZ false
#endif





/*
    @brief search settings
*/
// parity ordering
#define USE_END_PO true

// stability cut
#define USE_END_SC true
#define USE_LAST4_SC false

// enhanced transposition cutoff
#define USE_MID_ETC true

// Multi-ProbCut
#define USE_MID_MPC true

// last parity ordering optimization
#define LAST_PO_OPTIMIZE true

// use parallel clog search
#define USE_PARALLEL_CLOG_SEARCH true

// MPC pre calculation
#define USE_MPC_PRE_CALCULATION true

// YBWC
#define USE_YBWC_NWS true
#define USE_YBWC_NEGASCOUT true
#define USE_YBWC_NEGASCOUT_ANALYZE false

// aspiration search in negascout
#define USE_ASPIRATION_NEGASCOUT false

// Hash level setting
#define USE_CHANGEABLE_HASH_LEVEL true

// Light Evaluation Function
#define USE_LIGHT_EVALUATION false

// transposition table stack / heap
// if false, USE_CHANGEABLE_HASH_LEVEL must be true
#define TT_USE_STACK true






/*
    @brief debug settings
*/

// search statistics
#define USE_SEARCH_STATISTICS false

// thread monitor
#define USE_THREAD_MONITOR false







/*
    @brief tuning
*/

// move ordering
#define TUNE_MOVE_ORDERING false

// probcut
#define TUNE_PROBCUT_MID false
#define TUNE_PROBCUT_END false


/*
    @brief test
*/

// endgame accuracy test
#define TEST_ENDGAME_ACCURACY false
