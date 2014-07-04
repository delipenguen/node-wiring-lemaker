#ifndef _ADDON_H_
#define _ADDON_H_
  
  #include <v8.h>
    
  using namespace v8;
  
  // We don't want to include the whole node headers :)
  namespace node {
    namespace Buffer {
      bool HasInstance(v8::Handle<v8::Value> val);
      bool HasInstance(v8::Handle<v8::Object> val);
      char* Data(v8::Handle<v8::Value> val);
      char* Data(v8::Handle<v8::Object> val);
      size_t Length(v8::Handle<v8::Value> val);
      size_t Length(v8::Handle<v8::Object> val);
    }
  }
  
  bool find_string(const char* string, const char* array[], size_t s);
  bool find_int(const int value, const int array[], size_t s);
  void throw_error(const char* fmt, ...);
    
  #define DECLARE(name) \
    namespace nodemodule { \
      static v8::Handle<v8::Value> name(const v8::Arguments& args); \
    }
    
  #define IMPLEMENT(name) \
    v8::Handle<v8::Value> nodemodule::name(const v8::Arguments& args)
  
  #define EXPORT_FUNCTION(name)  \
    target->Set(v8::String::NewSymbol(#name), \
      v8::FunctionTemplate::New(nodemodule::name)->GetFunction())
    
  #define EXPORT_CONSTANT_INT(name) \
    target->Set(v8::String::NewSymbol(#name), \
      v8::Int32::New(name), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
  
  #define EXPORT_CONSTANT_STRING(name) \
    target->Set(v8::String::NewSymbol(#name), \
      v8::String::New(name), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
    
  #define EXPORT_CONSTANT_INT_ARRAY(name, array, length) \
    { \
      v8::Local<v8::Array> arr = v8::Array::New(); \
      for (int i = 0; i < length; i++) { \
        arr->Set(i, v8::Int32::New(array[i])); \
      } \
      target->Set(v8::String::NewSymbol(#name), arr, static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete)); \
    }
    
  #define EXPORT_CONSTANT_STRING_ARRAY(name, array, length) \
    { \
      v8::Local<v8::Array> arr = v8::Array::New(); \
      for (int i = 0; i < length; i++) { \
        arr->Set(i, v8::Int32::New(array[i])); \
      } \
      target->Set(v8::String::NewSymbol(#name), arr, static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete)); \
    }
  
  #define NODE_MODULE_INIT() \
    namespace nodemodule { \
      void init(v8::Handle<v8::Object> target); \
    } \
    void nodemodule::init(v8::Handle<v8::Object> target)
  #define NODE_MODULE_DECLARE(name) NODE_MODULE(name, nodemodule::init)
  #define IMPLEMENT_EXPORT_INIT(name) void nodemodule::init##name(v8::Handle<v8::Object> target)
  #define DECLARE_EXPORT_INIT(name) \
    namespace nodemodule { \
      void init##name(v8::Handle<v8::Object> target); \
    }
  
  #define INIT(name) nodemodule::init##name(target);
  
  #define SCOPE_OPEN() v8::HandleScope scope
  #define SCOPE_CLOSE(obj) return scope.Close(obj)
  
  #define UNDEFINED() v8::Undefined()
  #define INT32(v) v8::Int32::New(v)
  #define UINT32(v) v8::Uint32::New(v)
  #define STRING(v) v8::String::New(v)
    
  #define THROW_ERROR(fmt, ...) \
    throw_error(fmt, __VA_ARGS__); \
    SCOPE_CLOSE(UNDEFINED())
  
  #define SET_ARGUMENT_NAME(id, name) static const char* arg##id = #name
  #define GET_ARGUMENT_NAME(id) arg##id
  
  #define CHECK_ARGUMENTS_LENGTH_EQUAL(length) \
    if (args.Length() != length) { \
      THROW_ERROR("%s: arguments.length => (%i === %i) === false", __func__, args.Length(), length); \
    }
    
  #define CHECK_ARGUMENT_TYPE(id, istype) \
    if (!args[id]->istype()) { \
      THROW_ERROR("%s: %s(arguments['%s']) === false", __func__, #istype, GET_ARGUMENT_NAME(id)); \
    }
  
  #define CHECK_ARGUMENT_TYPE_INT32(id) CHECK_ARGUMENT_TYPE(id, IsInt32)
  #define CHECK_ARGUMENT_TYPE_UINT32(id) CHECK_ARGUMENT_TYPE(id, IsUint32)
  #define CHECK_ARGUMENT_TYPE_NUMBER(id) CHECK_ARGUMENT_TYPE(id, IsNumber)
  #define CHECK_ARGUMENT_TYPE_STRING(id) CHECK_ARGUMENT_TYPE(id, IsString)
  #define CHECK_ARGUMENT_TYPE_FUNCTION(id) CHECK_ARGUMENT_TYPE(id, IsFunction)
  #define CHECK_ARGUMENT_TYPE_OBJECT(id) CHECK_ARGUMENT_TYPE(id, IsObject)
  #define CHECK_ARGUMENT_TYPE_NODE_BUFFER(id) \
    if (!(args[id]->IsObject() && node::Buffer::HasInstance(args[id]))) { \
      THROW_ERROR("%s: %s(arguments['%s']) === false", __func__, "isBuffer", GET_ARGUMENT_NAME(id)); \
    }
  
  #define GET_ARGUMENT_AS_TYPE(id, type) args[id]->type()
  
  #define GET_ARGUMENT_AS_INT32(id) GET_ARGUMENT_AS_TYPE(id, Int32Value)
  #define GET_ARGUMENT_AS_UINT32(id) GET_ARGUMENT_AS_TYPE(id, Uint32Value)
  #define GET_ARGUMENT_AS_NUMBER(id) GET_ARGUMENT_AS_TYPE(id, NumberValue)
  #define GET_ARGUMENT_AS_STRING(id) GET_ARGUMENT_AS_TYPE(id, ToString)
  #define GET_ARGUMENT_AS_LOCAL_FUNCTION(id) v8::Local<v8::Function>::Cast(args[id])
  #define GET_ARGUMENT_AS_PERSISTENT_FUNCTION(id) v8::Persistent<v8::Function>::New(GET_ARGUMENT_AS_LOCAL_FUNCTION(id))
  
  #define LIST(...) { __VA_ARGS__ }
  #define CHECK_ARGUMENT_IN_STRINGS(id, value, T) \
    { \
      static const char* strings[] = LIST T; \
      if (!find_string(*value, strings, sizeof(strings) / sizeof(char*))) { \
        THROW_ERROR("%s: arguments['%s'] => (\"%s\" in %s) === false", __func__, GET_ARGUMENT_NAME(id), *value, #T); \
      } \
    }
    
  #define CHECK_ARGUMENT_IN_INTS(id, value, T) \
    { \
      static const int ints[] = LIST T; \
      if (!find_int(value, ints, sizeof(ints) / sizeof(int))) { \
        THROW_ERROR("%s: arguments['%s'] => (%i in %s) === false", __func__, GET_ARGUMENT_NAME(id), value, #T); \
      } \
    }
    
  #define CHECK_ARGUMENT_IN_RANGE(id, value, min, max) \
    if (value < min || value > max) { \
      THROW_ERROR("%s: arguments['%s'] => inRange(%i, [%i, %i]) === false", __func__, GET_ARGUMENT_NAME(id), value, min, max); \
    }

#endif
