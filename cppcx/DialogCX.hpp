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