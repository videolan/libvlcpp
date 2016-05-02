/*****************************************************************************
* DialogCX.hpp:  libvlcpp dialog API
*****************************************************************************
* Copyright © 2016 VLC authors and VideoLAN
*
* Authors: Bastien Penavayre <bastienPenava@gmail.com>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2.1 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program; if not, write to the Free Software Foundation,
* Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
*****************************************************************************/

#pragma once

namespace libVLCX
{
    public ref class Dialog sealed
    {
        VLC::Dialog _dialog;

        friend ref class Instance;

        /**
        * The standard constructor.
        *
        * Used only by setDialogHandlers in Instance.hpp.
        * \param other   native C++ instance of Dialog
        */
        Dialog(VLC::Dialog &&other);

    public:
        /**
        * Post a login answer.
        *
        * After this call, the instance won't be valid anymore
        *
        * \param username   valid non-empty string
        * \param password   valid string
        * \param store      if true stores the credentials
        * \return true if success, false otherwise
        */
        bool postLogin(Platform::String^ username, Platform::String^ password, bool store);

        /**
        * Post a question answer.
        *
        * After this call, this instance won't be valid anymore
        *
        * \see QuestionCb
        * \param actionIndex 1 for action1, 2 for action2
        * \return true on success, false otherwise
        */
        bool postAction(int actionIndex);

        /**
        * Dismiss a dialog.
        *
        * After this call, this instance won't be valid anymore
        *
        * \see CancelCb
        */
        bool dismiss();
    };
}