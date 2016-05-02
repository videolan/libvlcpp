/*****************************************************************************
* EqualizerCX.cpp: Equalizer API
*****************************************************************************
* Copyright © 2015 libvlcpp authors & VideoLAN
*
* Authors: Hugo Beauzée-Luyssen <hugo@beauzee.fr>
*          Bastien Penavayre <bastienPenava@gmail.com>
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

#include "EqualizerCX.hpp"
#include "Helpers.h"

namespace libVLCX
{
    Equalizer::Equalizer()
    {
    }

    Equalizer::Equalizer(unsigned int index)
        : m_eq(index)
    {
    }

    int Equalizer::setPreamp(float preamp)
    {
        return m_eq.setPreamp(preamp);
    }

    float Equalizer::preamp()
    {
        return m_eq.preamp();
    }

    int Equalizer::setAmp(float amp, unsigned int band)
    {
        return m_eq.setAmp(amp, band);
    }

    float Equalizer::amp(unsigned int band)
    {
        return m_eq.amp(band);
    }

    unsigned int Equalizer::presetCount()
    {
        return VLC::Equalizer::presetCount();
    }

    Platform::String^ Equalizer::presetName(unsigned index)
    {
        return ToPlatformString(VLC::Equalizer::presetName(index));
    }

    unsigned int Equalizer::bandCount()
    {
        return VLC::Equalizer::bandCount();
    }

    float Equalizer::bandFrequency(unsigned int index)
    {
        return VLC::Equalizer::bandFrequency(index);
    }
}
