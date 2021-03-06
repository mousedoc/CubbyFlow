/*************************************************************************
> File Name: Frame.cpp
> Project Name: CubbyFlow
> Author: Chan-Ho Chris Ohk
> Purpose: Frame class for CubbyFlow C# API.
> Created Time: 2018/04/11
> Copyright (c) 2018, Chan-Ho Chris Ohk
*************************************************************************/
#include <API/CSharp/Animation/Frame.h>

namespace CubbyFlowSharp
{
	Frame::Frame() : m_pFrame(new ::CubbyFlow::Frame)
	{
		// Do nothing
	}

	Frame::Frame(int newIndex, double newTimeIntervalInSeconds) :
		m_pFrame(new ::CubbyFlow::Frame(newIndex, newTimeIntervalInSeconds))
	{
		// Do nothing
	}

	::CubbyFlow::Frame* Frame::GetInteralPtr()
	{
		return m_pFrame;
	}

	double Frame::TimeInSeconds()
	{
		return m_pFrame->index * m_pFrame->timeIntervalInSeconds;
	}

	void Frame::Advance(int delta)
	{
		m_pFrame->Advance(delta);
	}
} 