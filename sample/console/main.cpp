/* ********************************************************************** */
/**
 * @brief   ESM: sample && test application.
 * @author  eel3
 * @date    2017-10-18
 */
/* ********************************************************************** */

#include "command_reader.h"
#include "event_id.h"
#include "handler_public.h"
#include "mailbox.h"

#include "esm.h"
#include "esm_md.h"

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <future>
#include <map>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>

namespace {

/* ---------------------------------------------------------------------- */
/* Type Aliases */
/* ---------------------------------------------------------------------- */

/** "vector<string>" type. */
using VS = std::vector<std::string>;

/** Event ID maker function type. */
using EVENT_ID_MAKER = ESM_EVENT_ID (*)(const VS& argv);

/** Command entry: key type. */
using CE_KEY = std::string;
/** Command entry: value type. */
using CE_FIELD = std::tuple<EVENT_ID_MAKER, std::string, std::string>;
/** Command entry: "std::map::value_type" type. */
using CE_VALUE = std::pair<CE_KEY, CE_FIELD>;
/** Command entry type. */
using CE = std::map<CE_KEY, CE_FIELD>;

/* ---------------------------------------------------------------------- */
/* Data structures */
/* ---------------------------------------------------------------------- */

/** Module context type. */
struct MODULE_CTX {
    Mailbox<ESM_EVENT_ID> mailbox;
};

/* ---------------------------------------------------------------------- */
/* File scope variables */
/* ---------------------------------------------------------------------- */

/** Module context. */
MODULE_CTX module_ctx;

/* ---------------------------------------------------------------------- */
/* Private functions: test commands */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Convert from std::string to timer id bits.
 *
 * @param[in]  s          Input string.
 * @param[in]  threshold  Threshold value.
 * @param[out] id         Output place.
 *
 * @retval true  Exit success.
 * @retval false Exit failure.
 */
/* ====================================================================== */
bool
convert_to_timer_id(const std::string& s, const unsigned long threshold, EVENT_BIT& id)
{
    try {
        auto n = std::stoul(s);
        if (n < threshold) {
            id = (static_cast<EVENT_BIT>(n) << 16) & EVENT_BITMASK_TIMER_ID;
            return true;
        }
    } catch (...) {
        /*EMPTY*/
    }
    return false;
}

/* ====================================================================== */
/**
 * @brief  Convert from std::string to timeout value bits.
 *
 * @param[in]  s        Input string.
 * @param[out] timeout  Output place.
 *
 * @retval true  Exit success.
 * @retval false Exit failure.
 */
/* ====================================================================== */
bool
convert_to_timeout(const std::string& s, EVENT_BIT& timeout)
{
    try {
        auto n = std::stoul(s);
        if (n <= static_cast<unsigned long>(EVENT_BITMASK_TIMER_TIMEOUT)) {
            timeout = static_cast<EVENT_BIT>(n) & EVENT_BITMASK_TIMER_TIMEOUT;
            return true;
        }
    } catch (...) {
        /*EMPTY*/
    }
    return false;
}

/* ====================================================================== */
/**
 * @brief  Convert from std::string to repeat flag bit.
 *
 * @param[in]  s       Input string.
 * @param[out] repeat  Output place.
 *
 * @retval true  Exit success.
 * @retval false Exit failure.
 */
/* ====================================================================== */
bool
convert_to_repeat_flag(const std::string& s, EVENT_BIT& repeat)
{
    if (s == "repeat-off") {
        repeat = EVENT_BIT_TIMER_REPEAT_OFF;
    } else if (s == "repeat-on") {
        repeat = EVENT_BIT_TIMER_REPEAT_ON;
    } else {
        return false;
    }

    return true;
}

/* ====================================================================== */
/**
 * @brief  Returm event ID for "push-handler" command.
 *
 * @param[in] argv  Command name and arguments.
 *
 * @return Event ID.
 */
/* ====================================================================== */
ESM_EVENT_ID
command_next_handler(const VS& argv)
{
    if (argv.size() != 1) {
        return ESM_EVENT_ID_NONE;
    }

    auto bits = EVENT_BIT_CMD_NEXT_HANDLER;

    return static_cast<ESM_EVENT_ID>(bits);
}

/* ====================================================================== */
/**
 * @brief  Returm event ID for "set-timer" command.
 *
 * @param[in] argv  Command name and arguments.
 *
 * @return Event ID.
 */
/* ====================================================================== */
ESM_EVENT_ID
command_set_timer(const VS& argv)
{
    if (argv.size() != 4) {
        return ESM_EVENT_ID_NONE;
    }
    const auto& cmd = argv[0];
    const auto& opt_id = argv[1];
    const auto& opt_timeout = argv[2];
    const auto& opt_repeat = argv[3];

    EVENT_BIT id;
    if (!convert_to_timer_id(opt_id, ESM_CFG_MAX_TIMER, id)) {
        std::cerr << cmd << ": invalid id: " << opt_id << std::endl;
        return ESM_EVENT_ID_NONE;
    }

    EVENT_BIT timeout;
    if (!convert_to_timeout(opt_timeout, timeout)) {
        std::cerr << cmd << ": invalid timeout value: " << opt_timeout << std::endl;
        return ESM_EVENT_ID_NONE;
    }

    EVENT_BIT repeat;
    if (!convert_to_repeat_flag(opt_repeat, repeat)) {
        std::cerr << cmd << ": invalid repeat switch: " << opt_repeat<< std::endl;
        return ESM_EVENT_ID_NONE;
    }

    auto bits = EVENT_BIT_CMD_SET_TIMER | id | timeout | repeat;

    return static_cast<ESM_EVENT_ID>(bits);
}

/* ====================================================================== */
/**
 * @brief  Returm event ID for "kill-timer" command.
 *
 * @param[in] argv  Command name and arguments.
 *
 * @return Event ID.
 */
/* ====================================================================== */
ESM_EVENT_ID
command_kill_timer(const VS& argv)
{
    if (argv.size() != 2) {
        return ESM_EVENT_ID_NONE;
    }
    const auto& cmd = argv[0];
    const auto& opt_id = argv[1];

    EVENT_BIT id;
    if (!convert_to_timer_id(opt_id, ESM_CFG_MAX_TIMER, id)) {
        std::cerr << cmd << ": invalid id: " << opt_id << std::endl;
        return ESM_EVENT_ID_NONE;
    }

    auto bits = EVENT_BIT_CMD_KILL_TIMER | id;

    return static_cast<ESM_EVENT_ID>(bits);
}

/* ====================================================================== */
/**
 * @brief  Returm event ID for "set-gtimer" command.
 *
 * @param[in] argv  Command name and arguments.
 *
 * @return Event ID.
 */
/* ====================================================================== */
ESM_EVENT_ID
command_set_gtimer(const VS& argv)
{
    if (argv.size() != 4) {
        return ESM_EVENT_ID_NONE;
    }
    const auto& cmd = argv[0];
    const auto& opt_id = argv[1];
    const auto& opt_timeout = argv[2];
    const auto& opt_repeat = argv[3];

    EVENT_BIT id;
    if (!convert_to_timer_id(opt_id, ESM_CFG_MAX_GLOBAL_TIMER, id)) {
        std::cerr << cmd << ": invalid id: " << opt_id << std::endl;
        return ESM_EVENT_ID_NONE;
    }

    EVENT_BIT timeout;
    if (!convert_to_timeout(opt_timeout, timeout)) {
        std::cerr << cmd << ": invalid timeout value: " << opt_timeout << std::endl;
        return ESM_EVENT_ID_NONE;
    }

    EVENT_BIT repeat;
    if (!convert_to_repeat_flag(opt_repeat, repeat)) {
        std::cerr << cmd << ": invalid repeat switch: " << opt_repeat<< std::endl;
        return ESM_EVENT_ID_NONE;
    }

    auto bits = EVENT_BIT_CMD_SET_GTIMER | id | timeout | repeat;

    return static_cast<ESM_EVENT_ID>(bits);
}

/* ====================================================================== */
/**
 * @brief  Returm event ID for "kill-gtimer" command.
 *
 * @param[in] argv  Command name and arguments.
 *
 * @return Event ID.
 */
/* ====================================================================== */
ESM_EVENT_ID
command_kill_gtimer(const VS& argv)
{
    if (argv.size() != 2) {
        return ESM_EVENT_ID_NONE;
    }
    const auto& cmd = argv[0];
    const auto& opt_id = argv[1];

    EVENT_BIT id;
    if (!convert_to_timer_id(opt_id, ESM_CFG_MAX_GLOBAL_TIMER, id)) {
        std::cerr << cmd << ": invalid id: " << opt_id << std::endl;
        return ESM_EVENT_ID_NONE;
    }

    auto bits = EVENT_BIT_CMD_KILL_GTIMER | id;

    return static_cast<ESM_EVENT_ID>(bits);
}

/* ====================================================================== */
/**
 * @brief  Returm event ID for "post-msg-inner" command.
 *
 * @param[in] argv  Command name and arguments.
 *
 * @return Event ID.
 */
/* ====================================================================== */
ESM_EVENT_ID
command_post_msg(const VS& argv)
{
    if (argv.size() != 1) {
        return ESM_EVENT_ID_NONE;
    }

    auto bits = EVENT_BIT_CMD_POST_MESSAGE;

    return static_cast<ESM_EVENT_ID>(bits);
}

/* ====================================================================== */
/**
 * @brief  Dummy command function.
 *
 * @param[in] argv  Command name and arguments.
 *
 * @return Event ID.
 */
/* ====================================================================== */
ESM_EVENT_ID
command_nop(const VS&)
{
    return ESM_EVENT_ID_NONE;
}

/* ---------------------------------------------------------------------- */
/* Constants: test commands */
/* ---------------------------------------------------------------------- */

/** Test command entries. */
const CE COMMAND_ENTRY {
#define ENTRY(cmd, fn, arg, desc) \
    std::make_pair(#cmd, std::make_tuple(fn, arg, desc))

    ENTRY(next-handler,   command_next_handler, "(no option)",                       "Set next event handler."),
    ENTRY(set-timer,      command_set_timer,    "id timeout-millis repeat-[off|on]", "Start timer."),
    ENTRY(kill-timer,     command_kill_timer,   "id",                                "Kill timer."),
    ENTRY(set-gtimer,     command_set_gtimer,   "id timeout-millis repeat-[off|on]", "Start global timer."),
    ENTRY(kill-gtimer,    command_kill_gtimer,  "id",                                "Kill global timer."),
    ENTRY(post-msg-inner, command_post_msg,     "(no option)",                       "Post message (from main-loop() thread)"),

    ENTRY(post-msg-outer, command_nop,          "(no option)",                       "Post message (from main thread)"),

    ENTRY(exit,           command_nop,          "(no option)",                       "Exit program."),
    ENTRY(help,           command_nop,          "(no option)",                       "Show help message."),

#undef ENTRY
};

/* ---------------------------------------------------------------------- */
/* Private functions: etc. */
/* ---------------------------------------------------------------------- */

/* ====================================================================== */
/**
 * @brief  Main loop.
 *
 * @param[in,out] pr_init  A promise object to notify initialization result.
 * @param[in]     fu_fin   A future object to receive application terminate.
 */
/* ====================================================================== */
void
main_loop(std::promise<bool>& pr_init, std::future<void>& fu_fin)
{
    ESM_ERR err;

    err = esm_Initialize();
    if (err != ESM_E_OK) {
        pr_init.set_value(false);
        return;
    }

    ESM_PREPARE_PARAMS params { &state_1_event_handler };

    err = esm_PrepareBeforeMainLoop(&params);
    if (err != ESM_E_OK) {
        esm_Finalize();
        pr_init.set_value(false);
        return;
    }

    pr_init.set_value(true);

    std::chrono::milliseconds timeout { 10 };
    while (fu_fin.wait_for(timeout) == std::future_status::timeout) {
        (void) esm_ResumeAndYield();
    }

    (void) esm_CleanupAfterMainLoop();
    esm_Finalize();
}

/* ====================================================================== */
/**
 * @brief  Show command help message.
 *
 * @param[in] value  Command entry parameters.
 */
/* ====================================================================== */
void
show_command_help(const CE_VALUE& value)
{
    using std::get;

    const auto& field = value.second;
    std::cerr << value.first << "\t"
              << get<1>(field) << "\t"
              << get<2>(field) << std::endl;
}

/* ====================================================================== */
/**
 * @brief  Show help message.
 */
/* ====================================================================== */
void
show_help()
{
    const auto& ce = COMMAND_ENTRY;
    std::for_each(ce.cbegin(), ce.cend(), show_command_help);
}

/* ====================================================================== */
/**
 * @brief  Do "post-msg-outer" command.
 *
 * @retval true  Exit success.
 * @retval false Exit failure.
 */
/* ====================================================================== */
bool
post_message()
{
    static char prefix[] = "outer";

    ESM_MESSAGE msg = default_message;
    msg.user_data = (void *) prefix;
    return esm_PostMessage(&msg) == ESM_E_OK;
}

} // namespace

/* ---------------------------------------------------------------------- */
/* ESM: machdep implementation */
/* ---------------------------------------------------------------------- */

extern "C" {

/* ********************************************************************** */
/**
 * @brief  Peek event.
 *
 * @return  Event ID.
 */
/* ********************************************************************** */
ESM_EVENT_ID
esm_md_PeekEvent(void)
{
    auto& mc = module_ctx;
    ESM_EVENT_ID id;

    if (!mc.mailbox.pop(id)) {
        id = ESM_EVENT_ID_NONE;
    }

    return id;
}

} // extern "C"

/* ---------------------------------------------------------------------- */
/* Main routine */
/* ---------------------------------------------------------------------- */

/* ********************************************************************** */
/**
 * @brief  Application entry point.
 *
 * @retval EXIT_SUCCESS  Exit success.
 * @retval EXIT_FAILURE  Exit failure.
 */
/* ********************************************************************** */
int
main()
{
    using std::cerr;
    using std::endl;

    std::promise<bool> pr_init;
    auto fu_init = pr_init.get_future();

    std::promise<void> pr_fin;
    auto fu_fin = pr_fin.get_future();

    std::thread th([&pr_init, &fu_fin] { main_loop(pr_init, fu_fin); });    // XXX Workaround !

    const auto initialized = fu_init.get();
    if (!initialized) {
        th.join();
        cerr << "Failed to initialize esm module." << endl;
        return EXIT_FAILURE;
    }

    CommandReader cr;

    while (cr.read()) {
        assert(cr.argc() > 0);

        const auto& argv = cr.argv();

        auto p = COMMAND_ENTRY.find(argv[0]);
        if (p == COMMAND_ENTRY.end()) {
            cerr << "Command not found." << endl;
            continue;
        }

        const auto& command = p->first;

        if (command == "exit") {
            break;
        }
        if (command == "help") {
            show_help();
            continue;
        }
        if (command == "post-msg-outer") {
            if (!post_message()) {
                cerr << "Failed to post message from main thread" << endl;
            }
            continue;
        }

        const auto event_id = std::get<0>(p->second)(argv);
        if (event_id == ESM_EVENT_ID_NONE) {
            show_command_help(*p);
            continue;
        }
        auto& mc = module_ctx;
        mc.mailbox.push(event_id);
    }

    pr_fin.set_value();
    th.join();

    return EXIT_SUCCESS;
}
