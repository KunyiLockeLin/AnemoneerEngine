#pragma once

#include "common/common.h"
#include "code_generator/generated_config_struct_enum.h"

BEGIN_NAMESPACE(ae)
BEGIN_NAMESPACE(common)

#define SINGLETON_OBJECT(class_name)    \
   private:                             \
    class_name();                       \
                                        \
   public:                              \
    static class_name &get_instance() { \
        static class_name instance;     \
        return instance;                \
    }

#define OBJECT_KEY(class_name, friend_name) \
    class DllExport class_name {            \
        friend class friend_name;           \
                                            \
       private:                             \
        class_name() {}                     \
    };

#define MANAGED_SINGLETON_OBJECT(class_name, key_name)                                   \
   public:                                                                               \
    class_name(const key_name &key);                                                     \
    static std::shared_ptr<class_name> get_instance(const key_name &key) {               \
        static std::shared_ptr<class_name> instance = std::make_shared<class_name>(key); \
        return instance;                                                                 \
    }

#define MANAGED_OBJECT(class_name, key_name) \
   public:                                   \
    class_name(const key_name &key);         \
    static std::shared_ptr<class_name> create(const key_name &key) { return std::make_shared<class_name>(key); }

// TODO: This marco is too large. Change it to inherit or something else.
#define INITIALIZE_MANAGER(mgr_name, key_name)                          \
    SINGLETON_OBJECT(mgr_name)                                          \
                                                                        \
   private:                                                             \
    const key_name key_;                                                \
                                                                        \
    template <typename class_name, typename dummy = int>                \
    struct ManageObject {};                                             \
                                                                        \
   public:                                                              \
    mgr_name(const mgr_name &) = delete;                                \
    mgr_name &operator=(const mgr_name &) = delete;                     \
                                                                        \
    template <typename T>                                               \
    AeResult get(std::shared_ptr<T> &obj) {                             \
        obj = T::get_instance(key_);                                    \
        return AE_SUCCESS;                                              \
    }                                                                   \
                                                                        \
    template <typename T>                                               \
    AeResult create(std::shared_ptr<T> &obj) {                          \
        auto active_map = this->*(ManageObject<T>::active_map());       \
        auto unactive_list = this->*(ManageObject<T>::unactive_list()); \
                                                                        \
        if (!unactive_list.empty()) {                                   \
            obj = unactive_list.back();                                 \
            unactive_list.pop_back();                                   \
        } else {                                                        \
            obj = T::create(key_);                                      \
        }                                                               \
        obj->initialize();                                              \
        active_map[obj->get_id()] = obj;                                \
        return AE_SUCCESS;                                              \
    }                                                                   \
                                                                        \
    template <typename T, typename K>                                   \
    AeResult find(const K &key, std::shared_ptr<T> &obj) {              \
        auto active_map = this->*(ManageObject<T>::active_map());       \
                                                                        \
        auto it = active_map.find(key);                                 \
        if (it != active_map.end()) {                                   \
            obj = it->second;                                           \
            return AE_SUCCESS;                                          \
        }                                                               \
        return AE_ERROR_UNKNOWN;                                        \
    }                                                                   \
                                                                        \
    template <typename T, typename K>                                   \
    AeResult remove(const K &key) {                                     \
        auto active_map = this->*(ManageObject<T>::active_map());       \
        auto unactive_list = this->*(ManageObject<T>::unactive_list()); \
                                                                        \
        auto it = active_map.find(key);                                 \
        if (it != active_map.end()) {                                   \
            it->second->cleanup();                                      \
            unactive_list.push_back(it->second);                        \
            active_map.erase(it);                                       \
            return AE_SUCCESS;                                          \
        }                                                               \
        return AE_ERROR_UNKNOWN;                                        \
    }                                                                   \
                                                                        \
    template <typename T>                                               \
    AeResult remove(const std::shared_ptr<T> &obj) {                    \
        return remove<T>(obj->get_id());                                \
    }                                                                   \
                                                                        \
    AeResult initialize();                                              \
    AeResult cleanup();

#define MANAGE_OBJECT(mgr_name, key_name, class_name)                                              \
   private:                                                                                        \
    std::unordered_map<key_name, std::shared_ptr<class_name>> active_##class_name##_map_;          \
    std::list<std::shared_ptr<class_name>> unactive_##class_name##_list_;                          \
                                                                                                   \
    template <typename dummy>                                                                      \
    struct ManageObject<class_name, dummy> {                                                       \
        static std::unordered_map<key_name, std::shared_ptr<class_name>> mgr_name::*active_map() { \
            return &mgr_name::active_##class_name##_map_;                                          \
        }                                                                                          \
        static std::list<std::shared_ptr<class_name>> mgr_name::*unactive_list() {                 \
            return &mgr_name::unactive_##class_name##_list_;                                       \
        }                                                                                          \
    };

static ID get_serial_number() {
    static ID serial_number = 0;
    ++serial_number;
    return serial_number;
}

// TODO: If it has a memory allocator, active_ might not be necessary.
class IObject {
   private:
    const ID id_;
    bool active_;

   protected:
    IObject() : id_(get_serial_number()), active_(false) {}

   public:
    IObject(const IObject &) = delete;
    IObject &operator=(const IObject &) = delete;

    ID get_id() { return id_; }
    bool active() { return active_; }

    virtual AeResult initialize() {
        active_ = true;
        return AE_SUCCESS;
    }
    virtual AeResult cleanup() {
        active_ = false;
        return AE_SUCCESS;
    }
};

END_NAMESPACE(common)
END_NAMESPACE(ae)
