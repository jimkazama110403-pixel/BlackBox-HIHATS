//
// Created by canyie on 2020/2/9.
//

#ifndef PINE_SCOPED_LOCAL_REF_H
#define PINE_SCOPED_LOCAL_REF_H

#include <cstddef>
#include "macros.h"
// Generic RAII wrapper for a JNI local reference.
template<typename T>
class ScopedLocalRef {
public:
    using value_type = T;

    explicit ScopedLocalRef(JNIEnv* env)
            : env_(env), local_(nullptr) {
    }

    ScopedLocalRef(JNIEnv* env, T ref)
            : env_(env), local_(ref) {
    }

    ~ScopedLocalRef() {
        Reset();
    }

    T Get() const {
        return local_;
    }

    void Reset(T newRef = nullptr) {
        if (local_ != newRef) {
            if (local_ != nullptr) {
                env_->DeleteLocalRef(local_);
            }
            local_ = newRef;
        }
    }

    T Release() __attribute__((warn_unused_result)) {
        T ref = local_;
        local_ = nullptr;
        return ref;
    }

    bool IsNull() const {
        return local_ == nullptr;
    }

    JNIEnv* Env() const {
        return env_;
    }

    bool operator==(std::nullptr_t) const {
        return IsNull();
    }

    bool operator!=(std::nullptr_t) const {
        return !IsNull();
    }

private:
    JNIEnv* env_;
    T       local_;

    DISALLOW_COPY_AND_ASSIGN(ScopedLocalRef);
};

// RAII wrapper for jclass
class ScopedLocalClassRef : public ScopedLocalRef<jclass> {
    using Base = ScopedLocalRef<jclass>;
public:
    using Base::Env;
    using Base::Get;
    using Base::IsNull;

    explicit ScopedLocalClassRef(JNIEnv* env)
            : Base(env) {
    }

    ScopedLocalClassRef(JNIEnv* env, jclass ref)
            : Base(env, ref) {
    }

    ScopedLocalClassRef(JNIEnv* env, const char* name)
            : Base(env, env->FindClass(name)) {
    }

    jmethodID FindMethodID(const char* name, const char* signature) {
        JNIEnv* env = Env();
        jmethodID method = env->GetMethodID(Get(), name, signature);
        if (LIKELY(method != nullptr)) {
            return method;
        }
        env->ExceptionClear();
        return nullptr;
    }
};

// RAII wrapper for jstring
class ScopedLocalUtfStringRef : public ScopedLocalRef<jstring> {
    using Base = ScopedLocalRef<jstring>;
public:
    using Base::Env;
    using Base::Get;
    using Base::IsNull;

    explicit ScopedLocalUtfStringRef(JNIEnv* env)
            : Base(env) {
    }

    ScopedLocalUtfStringRef(JNIEnv* env, jstring ref)
            : Base(env, ref) {
    }

    ScopedLocalUtfStringRef(JNIEnv* env, const char* content)
            : Base(env, env->NewStringUTF(content)) {
    }
};

#endif // PINE_SCOPED_LOCAL_REF_H
