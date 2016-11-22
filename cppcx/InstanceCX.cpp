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
using namespace Windows::Graphics::Display;

namespace libVLCX
{
    Instance::Instance(Windows::Foundation::Collections::IVector<Platform::String^>^ argv, SwapChainPanel^ panel)
        : m_chainPanel(panel)
    {
        int extraArgs = 2;
        auto c_argv = new char*[argv->Size + extraArgs];
        unsigned int i = 0;
        for (auto arg : argv)
        {
            c_argv[i++] = _strdup((const char*) VLCString(arg));
        }
        InitializeHack(c_argv, i);
        m_instance = VLC::Instance(i, c_argv);
        for (unsigned j = 0; j < i; ++j)
            free(c_argv[j]);
        delete [] c_argv;
    }

    void Instance::Trim()
    {
        m_dxManager->Trim();
    }

    void Instance::InitializeHack(char** argv, unsigned int & nbArgs)
    {
        m_dxManager.reset(new DirectXManager);
        m_dxManager->CreateSwapPanel(m_chainPanel);
        UpdateSize(m_chainPanel->ActualWidth * m_chainPanel->CompositionScaleX,
                   m_chainPanel->ActualHeight * m_chainPanel->CompositionScaleY);

        char ptr_d3dcstring[64];
        sprintf_s(ptr_d3dcstring, "--winrt-d3dcontext=0x%p", m_dxManager->cp_d3dContext);
        argv[nbArgs++] = _strdup(ptr_d3dcstring);

        char ptr_scstring[64];
        sprintf_s(ptr_scstring, "--winrt-swapchain=0x%p", m_dxManager->cp_swapChain);
        argv[nbArgs++] = _strdup(ptr_scstring);
    }

    static const GUID SWAPCHAIN_WIDTH  = { 0xf1b59347, 0x1643, 0x411a,{ 0xad, 0x6b, 0xc7, 0x80, 0x17, 0x7a, 0x6, 0xb6 } };
    static const GUID SWAPCHAIN_HEIGHT = { 0x6ea976a0, 0x9d60, 0x4bb7,{ 0xa5, 0xa9, 0x7d, 0xd1, 0x18, 0x7f, 0xc9, 0xbd } };

    void Instance::UpdateSize(float x, float y)
    {
        m_width  = (uint32_t)x;
        m_height = (uint32_t)y;
        m_dxManager->cp_swapChain->SetPrivateData(SWAPCHAIN_WIDTH, sizeof(uint32_t), &m_width);
        m_dxManager->cp_swapChain->SetPrivateData(SWAPCHAIN_HEIGHT, sizeof(uint32_t), &m_height);
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

    void Instance::logUnset()
    {
        m_instance.logUnset();
    }

    void Instance::logSet(LogCallback^ logCb)
    {
        m_instance.logSet([logCb](int logLevel, const libvlc_log_t* Log, std::string msgStr)
        {
            logCb(logLevel, ToPlatformString(msgStr));
        });
    }

    void Instance::setDialogHandlers(DialogCallback::Error ^ error, DialogCallback::Login ^ login, DialogCallback::Question ^ question, DialogCallback::DisplayProgress ^ dspProgress, DialogCallback::Cancel ^ cancel, DialogCallback::UpdateProgress ^ updtProgress)
    {
        m_instance.setDialogHandlers(
        [error](std::string &&title, std::string &&txt) {
            if (error)
            error(ToPlatformString(title), ToPlatformString(txt));
        },
        [login](VLC::Dialog&& dialog, std::string &&title, std::string &&text, std::string &&defaultUserName, bool askToStore) {
            if (login)
            login(ref new Dialog(std::move(dialog)), ToPlatformString(std::move(title)), ToPlatformString(std::move(text)), ToPlatformString(std::move(defaultUserName)), askToStore);
        },
        [question](VLC::Dialog &&dialog, std::string &&title, std::string &&text, VLC::Question qType, std::string &&cancel, std::string &&action1, std::string &&action2) {
            if (question)
            question(ref new Dialog(std::move(dialog)), ToPlatformString(std::move(title)), ToPlatformString(std::move(text)), (libVLCX::Question)qType, ToPlatformString(std::move(cancel)), ToPlatformString(std::move(action1)), ToPlatformString(std::move(action2)));
        },
        [dspProgress](VLC::Dialog &&dialog, std::string &&title, std::string &&text, bool intermediate, float position, std::string &&cancel) {
            if (dspProgress)
            dspProgress(ref new Dialog(std::move(dialog)), ToPlatformString(std::move(title)), ToPlatformString(std::move(text)), intermediate, position, ToPlatformString(std::move(cancel)));
        },
        [cancel](VLC::Dialog &&dialog) {
            if (cancel)
            cancel(ref new Dialog(std::move(dialog)));
        },
        [updtProgress](VLC::Dialog &&dialog, float position, std::string &&text) {
            if (updtProgress)
            updtProgress(ref new Dialog(std::move(dialog)), position, ToPlatformString(std::move(text)));
        });
    }

    void Instance::unsetDialogHandlers()
    {
        m_instance.unsetDialogHandlers();
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

    Windows::Foundation::Collections::IVector<MediaDiscovererDescription^>^ Instance::mediaDiscoverers(MediaDiscovererCategory category)
    {
        return MarshallVector<MediaDiscovererDescription, VLC::MediaDiscoverer::Description>(m_instance.mediaDiscoverers(static_cast<VLC::MediaDiscoverer::Category>(category)));
    }

    Instance::~Instance()
    {
    }

} // namespace VLC

