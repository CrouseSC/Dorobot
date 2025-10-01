#pragma once
// tas_runtime.hpp — Dorobot mini-TAS runtime (event bus + tick scheduler + drop-ele helper)
//
// Drop this into your project, then:
//  (1) Call Events::emit(EventType::PrePhys/Render/PostPhys) from your engine hooks
//  (2) Call TickScheduler::onSimulationTick() exactly once per *physics* tick
//  (3) Implement InputBus to fire your engine inputs (+jump/+crouch/etc.)

#include <functional>
#include <vector>
#include <string>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <optional>
#include <sstream>

// ----------------------------- Utilities -------------------------------------

static inline float clampf(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }
static inline bool near_zero_axis(float v, float tol)   { return std::fabs(v) <= tol; }

// ----------------------------- Event Bus --------------------------------------

enum class EventType { Render, PrePhys, PostPhys };

struct EventHandle {
    EventType type;
    size_t index;
};

class Events {
public:
    using CB = std::function<void()>;

    static EventHandle on(EventType t, CB cb) {
        auto& vec = vecFor(t);
        vec.push_back(std::move(cb));
        return {t, vec.size() - 1};
    }

    static void off(const EventHandle& h) {
        auto& vec = vecFor(h.type);
        if (h.index < vec.size()) vec[h.index] = nullptr;
    }

    // Wire these 3 calls from your engine:
    static void emit(EventType t) {
        auto& vec = vecFor(t);
        for (auto& cb : vec) if (cb) cb();
    }

private:
    static std::vector<CB>& vecFor(EventType t) {
        static std::vector<CB> render, pre, post;
        switch (t) {
            case EventType::Render:  return render;
            case EventType::PrePhys: return pre;
            default:                 return post;
        }
    }
};

// -------------------------- Tick-Locked Scheduler -----------------------------

// Runs actions at exact simulation ticks (not milliseconds).
class TickScheduler {
public:
    using Action = std::function<void()>;

    // Call this EXACTLY once per physics tick.
    static void onSimulationTick() {
        ++s_tick;
        // execute due actions in tick order
        for (size_t i = 0; i < s_actions.size();) {
            if (s_actions[i].tick <= s_tick) {
                auto fn = std::move(s_actions[i].fn);
                s_actions.erase(s_actions.begin() + i);
                if (fn) fn();
            } else {
                ++i;
            }
        }
    }

    static uint64_t tick() { return s_tick; }

    // Schedule to run at an absolute tick
    static void at(uint64_t absTick, Action fn) {
        s_actions.push_back({absTick, std::move(fn)});
        // keep list roughly ordered (small n)
        std::sort(s_actions.begin(), s_actions.end(),
                  [](const Item& a, const Item& b){ return a.tick < b.tick; });
    }

    // Schedule relative to "now"
    static void in(uint64_t ticksFromNow, Action fn) {
        at(s_tick + ticksFromNow, std::move(fn));
    }

    static void clear() { s_actions.clear(); }

private:
    struct Item { uint64_t tick; Action fn; };
    static inline uint64_t s_tick = 0;
    static inline std::vector<Item> s_actions{};
};

// ---------------------------- Engine Facades ----------------------------------

// Minimal player snapshot you can fill each PrePhys or PostPhys.
struct PlayerState {
    float pos[3]      = {0,0,0};
    float vel[3]      = {0,0,0}; // units per second (Z used for gravity comp)
    float viewNormal[3] = {0,0,1}; // surface normal hit by your trace (if any)
    float traceFrac   = 1.0f;    // fraction from last trace (1.0 = no hit)
    bool  onGround    = false;
};

// You wire this to your existing input system.
struct InputBus {
    virtual ~InputBus() = default;
    virtual void pressJump()   = 0;
    virtual void pressCrouch() = 0;
    virtual void pressUse()    = 0;
    virtual void strafe(float x, float y) = 0; // optional micro-nudge
};

// You wire this to your trace; it should fill normal & fraction.
// Return true if a plane was hit.
using TraceFn = std::function<bool(PlayerState&)>;

// ---------------------------- Drop-Ele Helper ---------------------------------

struct DropEleConfig {
    // Axis snap
    float axisSnapTol      = 0.002f;   // how close to 0.000 we want (X *or* Y)
    float axisSnapStep     = 0.0015f;  // small strafe nudge per attempt (units)
    int   axisSnapMaxTicks = 12;       // attempts before giving up

    // Surface normal gate (wall-ish plane)
    float minWallDot       = 0.65f;    // dot(view->forward, planeNormal) gating

    // Base timing (ticks relative)
    int   preAlignTicks    = 3;        // settle before firing
    int   jumpTickOffset   = 7;
    int   crouchTickOffset = 9;

    // Gravity compensation (shift crouch later if falling fast)
    // crouch_final_offset = crouchTickOffset + clamp( velZ * scale, addMin, addMax )
    float gravScale        = -0.015f;  // note velZ is negative when falling
    int   gravAddMin       = 0;
    int   gravAddMax       = 4;

    // Retry
    int   maxRetries       = 2;        // small ±1 tick shuffle around crouch
};

struct DropEleResult {
    bool  fired     = false;
    bool  success   = false;   // set this yourself post-run if you can detect it
    int   usedJump  = 0;
    int   usedCrouch= 0;
    std::string reason;        // why we aborted
};

class ElebotDropEle {
public:
    ElebotDropEle(InputBus* input, TraceFn tracer, DropEleConfig cfg = {})
        : m_input(input), m_trace(std::move(tracer)), m_cfg(cfg) {}

    // Call this to attempt a drop-ele sequence. Non-blocking; uses TickScheduler.
    DropEleResult start(const PlayerState& ps0) {
        m_log.str(std::string()); m_log.clear();
        m_lastHUD.clear();
        m_result = {};

        if (!m_input || !m_trace) {
            m_result.reason = "No input/trace bound";
            return m_result;
        }

        m_ps = ps0;

        // Re-trace to ensure we have a plane & normal
        PlayerState tmp = m_ps;
        if (!m_trace(tmp) || tmp.traceFrac >= 1.0f) {
            m_result.reason = "No wall in trace";
            return m_result;
        }
        m_ps = tmp;

        // Normal gate as a cheap wall sanity check
        float nZ = std::fabs(m_ps.viewNormal[2]);
        if (nZ > (1.f - m_cfg.minWallDot)) {
            m_result.reason = "Bad wall normal";
            return m_result;
        }

        m_result.fired = true;
        m_stage = Stage::AlignAxis;
        m_retry = 0;

        // We latch the starting tick to keep timing deterministic.
        const uint64_t base = TickScheduler::tick();
        TickScheduler::in(0, [this]{ onTick(); }); // run immediately next tick
        // Also hook PrePhys to keep player state fresh and bump stages.
        if (!m_hooked) {
            m_preHandle = Events::on(EventType::PrePhys, [this]{ onPrePhys(); });
            m_hooked = true;
        }

        logf("Start@tick=%llu  vZ=%.3f  n=[%.3f %.3f %.3f]\n",
             (unsigned long long)TickScheduler::tick(), m_ps.vel[2],
             m_ps.viewNormal[0], m_ps.viewNormal[1], m_ps.viewNormal[2]);

        return m_result; // Immediate result just says we started; poll HUD/logs if needed.
    }

    // Call when you know attempt succeeded/failed (optional)
    void setSuccess(bool ok) { m_result.success = ok; }

    // You can display this in your HUD each Render
    std::string hudLine() const { return m_lastHUD; }

    // For file logging, dump once per attempt
    std::string attemptLog() const { return m_log.str(); }

private:
    enum class Stage { Idle, AlignAxis, Prime, Fire, Done };

    void onPrePhys() {
        // Update live state before each physics step
        PlayerState tmp = m_ps;
        (void)m_trace(tmp); // refresh normal/frac if your trace is cheap
        m_ps = tmp;
    }

    void onTick() {
        switch (m_stage) {
            case Stage::AlignAxis: stageAlignAxis(); break;
            case Stage::Prime:     stagePrime();     break;
            case Stage::Fire:      stageFire();      break;
            case Stage::Done:      cleanup();        break;
            case Stage::Idle:      default: break;
        }
        if (m_stage != Stage::Done) {
            // continue pumping next tick until finished
            TickScheduler::in(1, [this]{ onTick(); });
        }
    }

    void stageAlignAxis() {
        // Try to nudge X or Y toward 0.000. Pick the one closer already.
        const float x = m_ps.pos[0], y = m_ps.pos[1];
        const bool x_ok = near_zero_axis(x, m_cfg.axisSnapTol);
        const bool y_ok = near_zero_axis(y, m_cfg.axisSnapTol);

        if (x_ok || y_ok || m_alignTicks >= m_cfg.axisSnapMaxTicks) {
            logf("AlignAxis done: x=%.5f y=%.5f ticks=%d\n", x, y, m_alignTicks);
            m_stage = Stage::Prime;
            m_alignTicks = 0;
            return;
        }

        // Decide which axis to nudge
        const bool nudgeX = (std::fabs(x) > std::fabs(y));
        const float dir   = nudgeX ? (x > 0 ? -1.f : 1.f) : (y > 0 ? -1.f : 1.f);
        float nx = nudgeX ? (m_cfg.axisSnapStep * dir) : 0.f;
        float ny = nudgeX ? 0.f : (m_cfg.axisSnapStep * dir);

        // Very small micro-strafe. You can scale by dt if needed.
        m_input->strafe(nx, ny);
        ++m_alignTicks;

        m_lastHUD = "ELE: aligning axis…";
    }

    void stagePrime() {
        if (m_primeWait < m_cfg.preAlignTicks) {
            ++m_primeWait;
            m_lastHUD = "ELE: priming…";
            return;
        }
        // Compute adaptive crouch offset based on vertical velocity (falling negative).
        const float vz = m_ps.vel[2];
        const int gravAdd = clampi((int)std::round(vz * m_cfg.gravScale),
                                   m_cfg.gravAddMin, m_cfg.gravAddMax);
        m_usedJump   = m_cfg.jumpTickOffset;
        m_usedCrouch = m_cfg.crouchTickOffset + gravAdd;

        logf("Prime: vZ=%.3f  jump@+%d  crouch@+%d (gravAdd=%d)\n",
             vz, m_usedJump, m_usedCrouch, gravAdd);

        m_stage = Stage::Fire;
        m_lastHUD = "ELE: firing…";
    }

    void stageFire() {
        // Schedule jump/crouch relative to NOW (tick-locked)
        TickScheduler::in(m_usedJump,   [this]{ m_input->pressJump();   logf(" +jump\n"); });
        TickScheduler::in(m_usedCrouch, [this]{ m_input->pressCrouch(); logf(" +crouch\n"); });

        // Optional micro retry shuffle around crouch if first attempt fails.
        if (m_retry < m_cfg.maxRetries) {
            const int delta = (m_retry % 2 == 0) ? +1 : -1;
            TickScheduler::in(m_usedCrouch + delta, [this](){
                m_input->pressCrouch(); logf(" retry +crouch (delta=%d)\n", (m_retry%2==0)?+1:-1);
            });
        }

        // End this attempt shortly after last action; consumer can evaluate success.
        TickScheduler::in(std::max(m_usedCrouch, m_usedJump) + 3, [this](){
            m_stage = Stage::Done;
        });
        m_stage = Stage::Idle; // pause stage machine; we'll flip to Done via scheduled lambda
    }

    void cleanup() {
        m_lastHUD = "ELE: done.";
        logf("Done@tick=%llu\n", (unsigned long long)TickScheduler::tick());
        if (m_hooked) {
            Events::off(m_preHandle);
            m_hooked = false;
        }
        m_stage = Stage::Idle;
    }

    static int clampi(int v, int lo, int hi){ return (v<lo)?lo:((v>hi)?hi:v); }
    void logf(const char* fmt, ...) {
        char buf[256];
        va_list ap; va_start(ap, fmt);
#ifdef _MSC_VER
        vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, ap);
#else
        vsnprintf(buf, sizeof(buf), fmt, ap);
#endif
        va_end(ap);
        m_log << buf;
    }

private:
    InputBus*  m_input = nullptr;
    TraceFn    m_trace;
    DropEleConfig m_cfg;

    PlayerState m_ps{};
    Stage   m_stage = Stage::Idle;
    bool    m_hooked = false;
    EventHandle m_preHandle{};
    int     m_alignTicks = 0;
    int     m_primeWait  = 0;
    int     m_retry      = 0;

    int     m_usedJump   = 0;
    int     m_usedCrouch = 0;

    std::string m_lastHUD;
    std::ostringstream m_log;
};

// ------------------------------ Integration Notes -----------------------------
//
// 1) Wire engine events:
//      - Call Events::emit(EventType::PrePhys) right before your physics step
//      - Call Events::emit(EventType::PostPhys) right after your physics step
//      - Call Events::emit(EventType::Render) sometime each frame (for HUD)
// 2) Tick stepping:
//      - Call TickScheduler::onSimulationTick() exactly once per physics tick.
//        (If your physics runs at 125Hz, this is called 125 times per second.)
// 3) Implement InputBus:
//      struct MyInputs : InputBus {
//          void pressJump() override   { /* send +jump */ }
//          void pressCrouch() override { /* send +crouch */ }
//          void pressUse() override    { /* optional */ }
//          void strafe(float x,float y) override { /* tiny movement nudge */ }
//      };
// 4) Tracing:
//      - Provide a TraceFn that fills PlayerState.viewNormal[] and traceFrac.
//        Return true if you hit a plane (the wall). Use your existing cg_trace.
// 5) Using ElebotDropEle:
//      MyInputs inputs;
//      TraceFn tracer = [&](PlayerState& ps){ /* fill ps.viewNormal, ps.traceFrac; return hit; */ };
//      ElebotDropEle ele(&inputs, tracer, {/*optional cfg overrides*/});
//
//      // Right before the attempt, snapshot current state and start:
//      PlayerState ps = /* fill from engine */;
//      auto res = ele.start(ps);
//      // Then let the scheduler run. You can show ele.hudLine() in your UI.
//
// 6) Marking success:
//      If you can detect a successful elevator (e.g., sudden Z speed/pos jump,
//      or known step into air), call ele.setSuccess(true) for logging/telemetry.
//