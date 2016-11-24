/*****************************************************************************
* InstanceCX.hpp: Instance API
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

#pragma once

#include "StructuresCX.hpp"
#include <collection.h>
#include <vlcpp/vlc.hpp>
#include "DialogCX.hpp"

//<ugly>
#include "DirectXManager.h"
//</ugly>

namespace libVLCX
{
    public delegate void LogCallback(int, Platform::String^);

    public enum class Question
    {
        normal = VLC::DialogType::normal,
        warning = VLC::DialogType::warning,
        critical = VLC::DialogType::critical
    };
        
    namespace DialogCallback
    {
        /**
        * Called when an error message needs to be displayed.
        *
        * \param title title of the dialog
        * \param text text of the dialog
        */
        public delegate void Error(Platform::String^ title, Platform::String^ text);

        /**
        *Called when a login dialog needs to be displayed.
        *
        *You can interact with this dialog by using the postLogin method on dialog to post an answer or the dismiss method to cancel this dialog.
        *
        *\note to receive this callack, CancelCb should not be NULL.
        *\param dialog used to interact with the dialog
        *\param title title of the dialog
        *\param text text of the dialog
        *\param defaultUserName user name that should be set on the user form
        *\param askToStore if true, ask the user if he wants to save the credentials
        */
        public delegate void Login(Dialog^ dialog, Platform::String^ title, Platform::String^ text, Platform::String^ defaultUserName, bool askToStore);

        /**
        * Called when a question dialog needs to be displayed
        *
        * You can interact with this dialog by using the postAction method on dialog
        * to post an answer or dismiss method to cancel this dialog.
        *
        * \note to receive this callack, CancelCb should not be
        * NULL.
        *
        * \param dialog used to interact with the dialog
        * \param title title of the diaog
        * \param text text of the dialog
        * \param qtype question type (or severity) of the dialog (\see Question)
        * \param cancel text of the cancel button
        * \param action1 text of the first button, if NULL, don't display this
        * button
        * \param action2 text of the second button, if NULL, don't display
        * this button
        */
        public delegate void Question(Dialog^ dialog, Platform::String^ title, Platform::String^ text, libVLCX::Question qType, Platform::String^ cancel, Platform::String^ action1, Platform::String^ action2);

        /**
        * Called when a progress dialog needs to be displayed
        *
        * If cancellable (cancel != NULL), you can cancel this dialog by
        * calling the dismiss method on dialog
        *
        * \note to receive this callack, CancelCb and
        * UpdtProgressCb should not be NULL.
        *
        * \param dialog used to interact with the dialog
        * \param title title of the diaog
        * \param text text of the dialog
        * \param indeterminate true if the progress dialog is indeterminate
        * \param position initial position of the progress bar (between 0.0 and
        * 1.0)
        * \param cancel text of the cancel button, if NULL the dialog is not
        * cancellable
        */
        public delegate void DisplayProgress(Dialog^ dialog, Platform::String^ title, Platform::String^ text, bool intermediate, float position, Platform::String^ cancel);

        /**
        * Called when a displayed dialog needs to be cancelled
        *
        * The implementation must call the method dismiss on dialog to really release
        * the dialog.
        *
        * \param dialog used to interact with the dialog
        */
        public delegate void Cancel(Dialog^ dialog);

        /**
        * Called when a progress dialog needs to be updated
        *
        * \param dialog used to interact with the dialog
        * \param position osition of the progress bar (between 0.0 and 1.0)
        * \param text new text of the progress dialog
        */
        public delegate void UpdateProgress(Dialog^ dialog, float position, Platform::String^ text);
    }


    public ref class Instance sealed
    {
    public:
        //<HACK HACK HACK>
        void UpdateSize(float x, float y);
        void Trim();
        //</HACK HACK HACK>

        /**
        * Create and initialize a libvlc instance. This functions accept a list
        * of "command line" arguments similar to the main(). These arguments
        * affect the LibVLC instance default configuration.
        *
        * \version Arguments are meant to be passed from the command line to
        * LibVLC, just like VLC media player does. The list of valid arguments
        * depends on the LibVLC version, the operating system and platform, and
        * set of available LibVLC plugins. Invalid or unsupported arguments will
        * cause the function to fail (i.e. return NULL). Also, some arguments
        * may alter the behaviour or otherwise interfere with other LibVLC
        * functions.
        *
        * \warning There is absolutely no warranty or promise of forward,
        * backward and cross-platform compatibility with regards to
        * Instance::Instance() arguments. We recommend that you do not use them,
        * other than when debugging.
        *
        * \param argc  the number of arguments (should be 0)
        *
        * \param argv  list of arguments (should be NULL)
        */
        Instance(Windows::Foundation::Collections::IVector<Platform::String^>^ argv, SwapChainPanel^ panel);

        /**
        * Try to start a user interface for the libvlc instance.
        *
        * \param name  interface name, or NULL for default
        *
        * \return 0 on success, -1 on error.
        */
        int addIntf(Platform::String^ name);

        /**
        * Sets the application name. LibVLC passes this as the user agent string
        * when a protocol requires it.
        *
        * \param name  human-readable application name, e.g. "FooBar player
        * 1.2.3"
        *
        * \param http  HTTP User Agent, e.g. "FooBar/1.2.3 Python/2.6.0"
        *
        * \version LibVLC 1.1.1 or later
        */
        void setUserAgent(Platform::String^ name, Platform::String^ http);

        /**
        * Sets some meta-information about the application. See also
        * Instance::setUserAgent() .
        *
        * \param id  Java-style application identifier, e.g. "com.acme.foobar"
        *
        * \param version  application version numbers, e.g. "1.2.3"
        *
        * \param icon  application icon name, e.g. "foobar"
        *
        * \version LibVLC 2.1.0 or later.
        */
        void setAppId(Platform::String^ id, Platform::String^ version, Platform::String^ icon);

        /**
        * Unsets the logging callback for a LibVLC instance. This is rarely
        * needed: the callback is implicitly unset when the instance is
        * destroyed. This function will wait for any pending callbacks
        * invocation to complete (causing a deadlock if called from within the
        * callback).
        *
        * \version LibVLC 2.1.0 or later
        */
        void logUnset();

        /**
        * Sets the logging callback for a LibVLC instance. This function is
        * thread-safe: it will wait for any pending callbacks invocation to
        * complete.
        *
        * \note Some log messages (especially debug) are emitted by LibVLC while
        * is being initialized. These messages cannot be captured with this
        * interface.
        *
        * \param logCb A delegate of the form void(int logLevel, String message)
        *
        * \warning A deadlock may occur if this function is called from the
        * callback.
        *
        * \version LibVLC 2.1.0 or later
        */
        void logSet(LogCallback^ logCb);

        /**
        * Replaces all the dialog callbacks for this Instance instance
        *
        * \param error  callback that will get called when an error message needs to be displayed.     \see DialogCallback::Error
        * \param login   callback that will get called when a login dialog needs to be displayed. \see DialogCallback::Login
        * \param question   callback that will get called when a question dialog needs to be displayed. \see DialogCallback::Question
        * \param dspProgress   callback that will get called when a progress dialog needs to be displayed. \see DialogCallback::DisplayProgress
        * \param cancel   callback that will get called when a displayed dialog needs to be cancelled. \see DialogCallback::Cancel
        * \param updtProgress   callback that will get called when a progress dialog needs to be updated. \see DialogCallback::UpdateProgress
        */
        void setDialogHandlers(DialogCallback::Error^ error, DialogCallback::Login^ login, DialogCallback::Question^ question, DialogCallback::DisplayProgress^ dspProgress, DialogCallback::Cancel^ cancel, DialogCallback::UpdateProgress^ updtProgress);

        /**
        * Unset all dialog callbacks
        */
        void unsetDialogHandlers();

        /**
        * Returns a list of audio filters that are available.
        *
        * \return a list of module descriptions. It should be freed with
        * ModuleDescription::moduleDescriptionListRelease() . In case of an
        * error, NULL is returned.
        *
        * \see ModuleDescription
        *
        * \see ModuleDescription::moduleDescriptionListRelease()
        */
        Windows::Foundation::Collections::IVector<ModuleDescription^>^ audioFilterList();

        /**
        * Returns a list of video filters that are available.
        *
        * \return a list of module descriptions. It should be freed with
        * ModuleDescription::moduleDescriptionListRelease() . In case of an
        * error, NULL is returned.
        *
        * \see ModuleDescription
        *
        * \see ModuleDescription::moduleDescriptionListRelease()
        */
        Windows::Foundation::Collections::IVector<ModuleDescription^>^ videoFilterList();

        /**
        * Gets the list of available audio output modules.
        *
        * \return list of available audio outputs. It must be freed it with
        *
        * \see AudioOutputDescription::audioOutputListRelease()
        *
        * \see AudioOutputDescription . In case of error, NULL is returned.
        */
        Windows::Foundation::Collections::IVector<AudioOutputDescription^>^ audioOutputList();

        /**
        * Gets a list of audio output devices for a given audio output module,
        *
        * \see Audio::outputDeviceSet() .
        *
        * \note Not all audio outputs support this. In particular, an empty
        * (NULL) list of devices does imply that the specified audio output does
        * not work.
        *
        * \note The list might not be exhaustive.
        *
        * \warning Some audio output devices in the list might not actually work
        * in some circumstances. By default, it is recommended to not specify
        * any explicit audio device.
        *
        * \param psz_aout  audio output name (as returned by
        * Instance::audioOutputList() )
        *
        * \return A vector containing all audio output devices for this module
        *
        * \version LibVLC 2.1.0 or later.
        */
        Windows::Foundation::Collections::IVector<AudioOutputDeviceDescription^>^ audioOutputDeviceList(Platform::String^ aout);

        Windows::Foundation::Collections::IVector<MediaDiscovererDescription^>^ mediaDiscoverers(MediaDiscovererCategory category);

    private:
        void InitializeHack(char** argv, unsigned int & nbArgs);

        ~Instance();

    internal:
        VLC::Instance m_instance;
        // <HACK HACK HACK!>
        std::unique_ptr<DirectXManager> m_dxManager;
        unsigned int m_width;
        unsigned int m_height;
        SwapChainPanel^ m_chainPanel;
        // </HACK HACK HACK>
    };

} // namespace VLC

