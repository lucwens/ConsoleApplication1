#include "ParameterStore.h" // Include the custom header
#include <iostream>

// --- Example Usage ---
// To run this example:
// 1. Compile: g++ main.cpp ParameterStore.cpp -o ParameterStoreExample.exe -std=c++17
// 2. Run with JSON:
//    .\ParameterStoreExample.exe "{\"appName\":\"CommandLineApp\",\"version\":1.5,\"maxUsers\":50,\"debugMode\":true}"
// 3. Run without JSON:
//    .\ParameterStoreExample.exe
// 4. Run with invalid JSON:
//    .\ParameterStoreExample.exe "{invalid json}"
int main(int argc, char *argv[])
{
    std::cout << "--- Initializing ParameterStore from Command Line ---" << std::endl;
    ParameterStore parameters(argc, argv);

    if (parameters.isInitializedFromJson())
    {
        std::cout << "ParameterStore was successfully initialized from a valid JSON command-line argument." << std::endl;
    }
    else
    {
        std::cout << "ParameterStore was NOT initialized from a valid JSON command-line argument (either no argument or invalid JSON)." << std::endl;
    }

    std::cout << "\n--- Current Parameters (from command line or default) ---" << std::endl;
    std::cout << parameters.serialize() << std::endl << std::endl;

    // You can still set additional parameters after initialization
    parameters.set("newSetting", "someValue");
    std::cout << "Parameters after adding 'newSetting':\n" << parameters.serialize() << std::endl << std::endl;

    // Demonstrate retrieving parameters using default values
    std::cout << "--- Retrieving Parameters with Default Values ---" << std::endl;

    // Existing parameter, correct type
    std::cout << "App Name (exists, default 'DefaultApp'): " << parameters.getString("appName", "DefaultApp") << std::endl;
    std::cout << "Version (exists, default 0.0): " << parameters.getDouble("version", 0.0) << std::endl;

    // Non-existent parameter, providing a default
    std::cout << "Log File (non-existent, default 'app.log'): " << parameters.getString("logFile", "app.log") << std::endl;
    std::cout << "Timeout (non-existent, default 30): " << parameters.getInt("timeout", 30) << std::endl;
    std::cout << "Verbose (non-existent, default false): " << (parameters.getBool("verbose", false) ? "True" : "False") << std::endl;

    // Parameter exists but wrong type, will return default
    std::cout << "Max Users (wrong type, default 500): " << parameters.getString("maxUsers", "500_default") << std::endl; // maxUsers is int, asking for string
    std::cout << "Debug Mode (wrong type, default 999): " << parameters.getInt("debugMode", 999) << std::endl;            // debugMode is bool, asking for int

    // Retrieving complex type with default
    std::vector<std::string> defaultFeatures = {"defaultFeatureX", "defaultFeatureY"};
    auto                     features        = parameters.getGeneric<std::vector<std::string>>("enabledFeatures", defaultFeatures);
    std::cout << "Enabled Features (retrieved with default): ";
    for (const auto &feature : features)
    {
        std::cout << feature << " ";
    }
    std::cout << std::endl;

    // Non-existent complex type with default
    std::vector<std::string> defaultPlugins = {"pluginA", "pluginB"};
    auto                     plugins        = parameters.getGeneric<std::vector<std::string>>("enabledPlugins", defaultPlugins);
    std::cout << "Enabled Plugins (non-existent, retrieved with default): ";
    for (const auto &plugin : plugins)
    {
        std::cout << plugin << " ";
    }
    std::cout << std::endl;

    // Continue with other example usages from before, as they still apply
    // (e.g., saving to file, loading from file, removing parameters)
    std::cout << "\n--- Demonstrating other functionalities (as in previous version) ---" << std::endl;
    const std::string filename = "runtime_config.json"; // Changed filename to avoid conflict
    if (parameters.saveToFile(filename))
    {
        std::cout << "Parameters saved to " << filename << std::endl;
    }
    else
    {
        std::cout << "Failed to save parameters to " << filename << std::endl;
    }

    ParameterStore loadedConfig;
    if (loadedConfig.loadFromFile(filename))
    {
        std::cout << "Parameters loaded from " << filename << std::endl;
        std::cout << "Loaded config content:\n" << loadedConfig.serialize() << std::endl;
    }
    else
    {
        std::cout << "Failed to load parameters from " << filename << std::endl;
    }

    parameters.remove("newSetting");
    std::cout << "After removing 'newSetting':\n" << parameters.serialize() << std::endl;

    parameters.clear();
    std::cout << "After clear:\n" << parameters.serialize() << std::endl;

    return 0;
}
