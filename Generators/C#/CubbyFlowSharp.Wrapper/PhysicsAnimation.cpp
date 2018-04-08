// ----------------------------------------------------------------------------
// <auto-generated>
// This is autogenerated code by CppSharp.
// Do not edit this file or all your changes will be lost after re-generation.
// </auto-generated>
// ----------------------------------------------------------------------------
#include "PhysicsAnimation.h"
#include "Frame.h"

using namespace System;
using namespace System::Runtime::InteropServices;

CubbyFlowSharp::CubbyFlow::PhysicsAnimation::PhysicsAnimation(::CubbyFlow::PhysicsAnimation* native)
    : CubbyFlowSharp::CubbyFlow::Animation((::CubbyFlow::Animation*)native)
{
}

CubbyFlowSharp::CubbyFlow::PhysicsAnimation^ CubbyFlowSharp::CubbyFlow::PhysicsAnimation::__CreateInstance(::System::IntPtr native)
{
    return gcnew ::CubbyFlowSharp::CubbyFlow::PhysicsAnimation((::CubbyFlow::PhysicsAnimation*) native.ToPointer());
}

CubbyFlowSharp::CubbyFlow::PhysicsAnimation::~PhysicsAnimation()
{
    if (NativePtr)
    {
        auto __nativePtr = NativePtr;
        NativePtr = 0;
        delete (::CubbyFlow::PhysicsAnimation*) __nativePtr;
    }
}

CubbyFlowSharp::CubbyFlow::PhysicsAnimation::PhysicsAnimation()
    : CubbyFlowSharp::CubbyFlow::Animation((::CubbyFlow::Animation*)nullptr)
{
    __ownsNativeInstance = true;
}

void CubbyFlowSharp::CubbyFlow::PhysicsAnimation::AdvanceSingleFrame()
{
    ((::CubbyFlow::PhysicsAnimation*)NativePtr)->AdvanceSingleFrame();
}

CubbyFlowSharp::CubbyFlow::PhysicsAnimation::PhysicsAnimation(CubbyFlowSharp::CubbyFlow::PhysicsAnimation^ _0)
    : CubbyFlowSharp::CubbyFlow::Animation((::CubbyFlow::Animation*)nullptr)
{
    __ownsNativeInstance = true;
}

bool CubbyFlowSharp::CubbyFlow::PhysicsAnimation::IsUsingFixedSubTimeSteps::get()
{
    auto __ret = ((::CubbyFlow::PhysicsAnimation*)NativePtr)->GetIsUsingFixedSubTimeSteps();
    return __ret;
}

void CubbyFlowSharp::CubbyFlow::PhysicsAnimation::IsUsingFixedSubTimeSteps::set(bool isUsing)
{
    ((::CubbyFlow::PhysicsAnimation*)NativePtr)->SetIsUsingFixedSubTimeSteps(isUsing);
}

unsigned int CubbyFlowSharp::CubbyFlow::PhysicsAnimation::NumberOfFixedSubTimeSteps::get()
{
    auto __ret = ((::CubbyFlow::PhysicsAnimation*)NativePtr)->GetNumberOfFixedSubTimeSteps();
    return __ret;
}

void CubbyFlowSharp::CubbyFlow::PhysicsAnimation::NumberOfFixedSubTimeSteps::set(unsigned int numberOfSteps)
{
    ((::CubbyFlow::PhysicsAnimation*)NativePtr)->SetNumberOfFixedSubTimeSteps(numberOfSteps);
}

CubbyFlowSharp::CubbyFlow::Frame^ CubbyFlowSharp::CubbyFlow::PhysicsAnimation::CurrentFrame::get()
{
    auto __ret = ((::CubbyFlow::PhysicsAnimation*)NativePtr)->GetCurrentFrame();
    auto ____ret = new ::CubbyFlow::Frame(__ret);
    return (____ret == nullptr) ? nullptr : gcnew CubbyFlowSharp::CubbyFlow::Frame((::CubbyFlow::Frame*)____ret);
}

void CubbyFlowSharp::CubbyFlow::PhysicsAnimation::CurrentFrame::set(CubbyFlowSharp::CubbyFlow::Frame^ frame)
{
    if (ReferenceEquals(frame, nullptr))
        throw gcnew ::System::ArgumentNullException("frame", "Cannot be null because it is a C++ reference (&).");
    auto &__arg0 = *(::CubbyFlow::Frame*)frame->NativePtr;
    ((::CubbyFlow::PhysicsAnimation*)NativePtr)->SetCurrentFrame(__arg0);
}

double CubbyFlowSharp::CubbyFlow::PhysicsAnimation::CurrentTimeInSeconds::get()
{
    auto __ret = ((::CubbyFlow::PhysicsAnimation*)NativePtr)->GetCurrentTimeInSeconds();
    return __ret;
}

