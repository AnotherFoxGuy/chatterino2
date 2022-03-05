#include "ScriptEngine.hpp"

// Include the definitions of the script library and the add-ons we'll use.
// The project settings may need to be configured to let the compiler where
// to find these headers. Don't forget to add the source modules for the
// add-ons to your project as well so that they will be compiled into the
// application.

#include <angelscript.h>
#include <scriptbuilder/scriptbuilder.h>
#include <scriptstdstring/scriptstdstring.h>
#include <cassert>
#include <string>
#include "Application.hpp"
#include "messages/MessageBuilder.hpp"
#include "singletons/Paths.hpp"
#include "singletons/WindowManager.hpp"
#include "widgets/splits/SplitContainer.hpp"

#include "singletons/Settings.hpp"
#include "util/SplitCommand.hpp"
#include "widgets/Window.hpp"
#include "widgets/dialogs/QualityPopup.hpp"
#include "widgets/splits/Split.hpp"

namespace chatterino {
void print_in_channel(const std::string &str)
{
    auto *currentPage = dynamic_cast<SplitContainer *>(
        getApp()->windows->getMainWindow().getNotebook().getSelectedPage());
    if (currentPage != nullptr)
    {
        if (auto currentSplit = currentPage->getSelectedSplit();
            currentSplit != nullptr)
        {
            currentSplit->getChannel()->addMessage(
                makeSystemMessage(QString::fromStdString(str)));
        }
    }
}

ScriptEngine::~ScriptEngine()
{
    // Clean up
    context->Release();
    engine->ShutDownAndRelease();
}

void ScriptEngine::initialize(Settings &settings, Paths &paths)
{
    // Create the script engine
    engine = asCreateScriptEngine();
    // Set the message callback to receive information on errors in human readable form.
    int r = engine->SetMessageCallback(asMETHOD(ScriptEngine, MessageCallback),
                                       this, asCALL_THISCALL);
    assert(r >= 0);

    // AngelScript doesn't have a built-in string type, as there is no definite standard
    // string type for C++ applications. Every developer is free to register its own string type.
    // The SDK do however provide a standard add-on for registering a string type, so it's not
    // necessary to implement the registration yourself if you don't want to.
    RegisterStdString(engine);
    // Register the function that we want the scripts to call
    r = engine->RegisterGlobalFunction("void print(const string &in)",
                                       asFUNCTION(print_in_channel), asCALL_CDECL);
    assert(r >= 0);

    // The CScriptBuilder helper is an add-on that loads the file,
    // performs a pre-processing pass if necessary, and then tells
    // the engine to build a script module.
    CScriptBuilder builder;
    r = builder.StartNewModule(engine, "CytopiaModule");
    if (r < 0)
    {
        // If the code fails here it is usually because there
        // is no more memory to allocate the module
        print_in_channel("Unrecoverable error while starting a new module.");
        return;
    }

    QString filePath = paths.miscDirectory + "/test.as";
    r = builder.AddSectionFromFile(filePath.toLocal8Bit().data());
    if (r < 0)
    {
        // The builder wasn't able to load the file. Maybe the file
        // has been removed, or the wrong name was given, or some
        // preprocessing commands are incorrectly written.
        print_in_channel("Please correct the errors in the script and try again.");
        return;
    }

    r = builder.BuildModule();
    if (r < 0)
    {
        // An error occurred. Instruct the script writer to fix the
        // compilation errors that were listed in the output stream.
        print_in_channel("Please correct the errors in the script and try again.");
        return;
    }

    // Find the function that is to be called.
    asIScriptModule *mod = engine->GetModule("Chatterino2Module");
    asIScriptFunction *func = mod->GetFunctionByDecl("void main()");
    if (func == nullptr)
    {
        // The function couldn't be found. Instruct the script writer
        // to include the expected function in the script.
        print_in_channel("The script must have the function 'void main()'. Please add it "
               "and try again.");
        return;
    }

    // Create our context, prepare it, and then execute
    context = engine->CreateContext();
    context->Prepare(func);
    r = context->Execute();
    if (r != asEXECUTION_FINISHED && r == asEXECUTION_EXCEPTION)
    {
        // The execution didn't complete as expected. Determine what happened.
        // An exception occurred, let the script writer know what happened so it can be corrected.
        print_in_channel("An exception '%s' occurred. Please correct the code and try "
               "again.");
               print_in_channel(context->GetExceptionString());
    }
}

void ScriptEngine::MessageCallback(const asSMessageInfo *msg)
{
    const char *type = "Error";
    if (msg->type == asMSGTYPE_INFORMATION)
        type = "Info";
    else if (msg->type == asMSGTYPE_WARNING)
        type = "Warning";

    print_in_channel(msg->message);
}
}  // namespace chatterino