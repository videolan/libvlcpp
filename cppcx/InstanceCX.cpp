/*****************************************************************************
* InstanceCX.cpp: Instance API
*****************************************************************************
* Copyright © 2014 the VideoLAN team
*
* Authors: Hugo Beauzée-Luyssen <hugo@beauzee.fr>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2.1 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
*****************************************************************************/

#include "InstanceCX.hpp"
#include "Helpers.h"
#include <collection.h>

//HACK HACK HACK
#include "MMDeviceLocator.h"
using namespace Windows::Graphics::Display;

namespace libVLCX
{
    Instance::Instance(Windows::Foundation::Collections::IVector<Platform::String^>^ argv, SwapChainPanel^ panel)
        : m_chainPanel(panel)
    {
        int extraArgs = 5;
        auto c_argv = new char*[argv->Size + extraArgs];
        unsigned int i = 0;
        for (auto arg : argv)
        {
            c_argv[i++] = _strdup((const char*) VLCString(arg));
        }
        InitializeHack(c_argv, i);
        m_instance = VLC::Instance::create(argv->Size + extraArgs, c_argv);
        for (i = 0; i < argv->Size + extraArgs; ++i)
            free(c_argv[i]);
        delete [] c_argv;
    }

    void Instance::Trim()
    {
        m_dxManager->Trim();
    }

    void Instance::InitializeHack(char** argv, unsigned int nbArgs)
    {
        m_dxManager.reset(new DirectXManger);
        m_dxManager->CreateSwapPanel(m_chainPanel);
        UpdateSize(m_chainPanel->ActualWidth, m_chainPanel->ActualHeight);

        ComPtr<MMDeviceLocator> audioReg = Make<MMDeviceLocator>();

        audioReg->m_AudioClient = NULL;
        audioReg->m_audioClientReady = CreateEventEx(NULL, TEXT("AudioClientReady"), 0, EVENT_ALL_ACCESS);
        audioReg->RegisterForWASAPI();

        void *addr = NULL;
        DWORD res;
        while ((res = WaitForSingleObjectEx(audioReg->m_audioClientReady, 1000, TRUE)) == WAIT_TIMEOUT) {
            OutputDebugStringW(L"Waiting for audio\n");
        }
        CloseHandle(audioReg->m_audioClientReady);
        if (res != WAIT_OBJECT_0) {
            OutputDebugString(TEXT("Failure while waiting for audio client"));
            return;
        }

        char ptr_astring[40];
        sprintf_s(ptr_astring, "--winstore-audioclient=0x%p", audioReg->m_AudioClient);
        argv[nbArgs++] = _strdup(ptr_astring);

        char ptr_d2dstring[40];
        sprintf_s(ptr_d2dstring, "--winrt-d2dcontext=0x%p", m_dxManager->cp_d2dContext);
        argv[nbArgs++] = _strdup(ptr_d2dstring);

        char ptr_scstring[40];
        sprintf_s(ptr_scstring, "--winrt-swapchain=0x%p", m_dxManager->cp_swapChain);
        argv[nbArgs++] = _strdup(ptr_scstring);

        char widthstring[40];
        sprintf_s(widthstring, "--winrt-width=0x%p", &m_width);
        argv[nbArgs++] = _strdup(widthstring);

        char heightstring[40];
        sprintf_s(heightstring, "--winrt-height=0x%p", &m_height);
        argv[nbArgs++] = _strdup(heightstring);
    }

    void Instance::UpdateSize(float x, float y)
    {
#if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
        const float scaleFactor = Windows::Graphics::Display::DisplayInformation::GetForCurrentView()->RawPixelsPerViewPixel;
#else
        const float scaleFactor = (float) DisplayProperties::ResolutionScale / 100.f;
#endif

        m_width = (uint32_t)(x * scaleFactor);
        m_height = (uint32_t)(y * scaleFactor);
    }

    int Instance::addIntf(Platform::String^ name)
    {
        return m_instance.addIntf(VLCString(name));
    }

    void Instance::setUserAgent(Platform::String^ name, Platform::String^ http)
    {
        m_instance.setUserAgent(VLCString(name), VLCString(http));
    }

    void Instance::setAppId(Platform::String^ id, Platform::String^ version, Platform::String^ icon)
    {
        m_instance.setAppId(VLCString(id), VLCString(version), VLCString(icon));
    }

    Windows::Foundation::Collections::IVector<ModuleDescription^>^ Instance::audioFilterList()
    {
        return MarshallVector<ModuleDescription, VLC::ModuleDescription>(m_instance.audioFilterList());
    }

    Windows::Foundation::Collections::IVector<ModuleDescription^>^ Instance::videoFilterList()
    {
        return MarshallVector<ModuleDescription, VLC::ModuleDescription>(m_instance.videoFilterList());
    }

    Windows::Foundation::Collections::IVector<AudioOutputDescription^>^ Instance::audioOutputList()
    {
        return MarshallVector<AudioOutputDescription, VLC::AudioOutputDescription>(m_instance.audioOutputList());
    }

    Windows::Foundation::Collections::IVector<AudioOutputDeviceDescription^>^ Instance::audioOutputDeviceList(Platform::String^ aout)
    {
        return MarshallVector<AudioOutputDeviceDescription, VLC::AudioOutputDeviceDescription>(m_instance.audioOutputDeviceList(VLCString(aout)));
    }

    Instance::~Instance()
    {
    }

} // namespace VLC

