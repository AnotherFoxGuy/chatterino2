#ifndef SCRIPTENGINE_HXX_
#define SCRIPTENGINE_HXX_

#ifdef USE_ANGELSCRIPT

#    include <angelscript.h>
#    include <string>
#    include "common/Singleton.hpp"

namespace chatterino {
class ScriptEngine : public Singleton
{
public:
    ScriptEngine() = default;
    ~ScriptEngine();

    /**
    * @brief Setup the ScriptEngine
    *
    */
    void initialize(Settings &settings, Paths &paths) override;

protected:
    /**
    * @brief The message callback used to receive information on errors in human readable form.
    *
    */
    void MessageCallback(const asSMessageInfo *msg);

    asIScriptEngine *engine{};    //!< instance of the scripting engine
    asIScriptContext *context{};  //!< context in which all scripting happens
};

}  // namespace chatterino
#endif

#endif
