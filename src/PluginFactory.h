#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H

#include <map>
#include <string>
#include <functional>
#include "PluginBase.h"

class PluginFactory {
public:
  static PluginFactory& instance() {
    static PluginFactory instance;
    return instance;
  }
  
  using CreateFunc = std::function<PluginBase*()>;
  
  void registerPlugin(const std::string& name, CreateFunc func) {
    plugins[name] = func;
  }
  
  PluginBase* create(const std::string& name) {
    auto it = plugins.find(name);
    if (it != plugins.end()) {
      return it->second();
    }
    return nullptr;
  }
  
  // Register a plugin type with the factory
  template <typename T>
  static void registerType(const std::string& name) {
    instance().registerPlugin(name, []() -> PluginBase* { return new T(); });
  }
  
private:
  std::map<std::string, CreateFunc> plugins;
  PluginFactory() = default;
  PluginFactory(const PluginFactory&) = delete;
  PluginFactory& operator=(const PluginFactory&) = delete;
};

#define REGISTER_PLUGIN(NAME, TYPE)		\
  namespace {					\
    struct Register##TYPE {			\
      Register##TYPE() {				 \
	PluginFactory::registerType<TYPE>(NAME);	 \
      }							 \
    } register##TYPE;					 \
  }

#endif // PLUGINFACTORY_H
