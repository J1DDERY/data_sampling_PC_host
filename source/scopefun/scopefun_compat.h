////////////////////////////////////////////////////////////////////////////////
// Compatibility shims to replace removed generator backend symbols.
// Provides a minimal `Osciloskop` declaration and related stubs so
// remaining code can compile. Implementations are no-ops where possible.
////////////////////////////////////////////////////////////////////////////////
#ifndef __SCOPEFUN_COMPAT_H__
#define __SCOPEFUN_COMPAT_H__

#include <SDL.h>

class WndMain;
class ThreadStub {
public:
    bool isOpen() { return false; }
    bool isFpga() { return false; }
    void openUSB(void*, int) {}
    void uploadFpga(void*) {}
    void function(int) {}
    void wait() {}
    int getVersion() { return 0; }
};

class CaptureBufferStub {
public:
    SDL_atomic_t m_frameIndex;
    SDL_atomic_t m_frameCount;
    SDL_atomic_t m_frameSize;
    CaptureBufferStub() { SDL_AtomicSet(&m_frameIndex,0); SDL_AtomicSet(&m_frameCount,0); SDL_AtomicSet(&m_frameSize,0); }
    void save(const char*) {}
    void load(const char*) {}
};

class SettingsStub { };

class CallbackStub {
public:
    void Add(const char*, void(*)(void*)) {}
    void SetHelp(const char*) {}
    struct PtrType { void onFunction(double,double,void*){} };
    PtrType* Ptr() { return nullptr; }
};

// `pOsciloscope` is the manager pointer (OsciloscopeManager*), not the GUI
// `Osciloskop` wxFrame class. Forward-declare the manager to avoid conflicts
// with GUI headers and allow linking to the real manager instance.
// Do NOT declare `pOsciloscope` here — the correct manager pointer and
// `OsciloscopeManager` type are provided by the normal project headers
// (see source/scopefun/osc/oscmng.h and source/core/manager/manager.h).
// Leaving `pOsciloscope` undefined here avoids conflicting forward
// declarations with GUI or manager headers.

// Signal mode constants used throughout the codebase. Provide both macros
// and a namespaced enum to match existing usages.
namespace SignalMode {
    enum : int {
        SIGNAL_MODE_PLAY = 0,
        SIGNAL_MODE_PAUSE = 1,
        SIGNAL_MODE_CAPTURE = 2,
        SIGNAL_MODE_SIMULATE_DEPRECATED = 3,  // removed — was SIMULATE
        SIGNAL_MODE_CLEAR = 4
    };
}

#define SIGNAL_MODE_PLAY    SignalMode::SIGNAL_MODE_PLAY
#define SIGNAL_MODE_PAUSE   SignalMode::SIGNAL_MODE_PAUSE
#define SIGNAL_MODE_CAPTURE SignalMode::SIGNAL_MODE_CAPTURE
#define SIGNAL_MODE_SIMULATE_DEPRECATED SignalMode::SIGNAL_MODE_SIMULATE_DEPRECATED
#define SIGNAL_MODE_CLEAR   SignalMode::SIGNAL_MODE_CLEAR

#endif
