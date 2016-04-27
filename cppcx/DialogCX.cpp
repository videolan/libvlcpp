#include "InstanceCX.hpp"

libVLCX::Dialog::Dialog(VLC::Dialog &&other) : _dialog(std::move(other))
{
}

bool libVLCX::Dialog::postLogin(Platform::String^ username, Platform::String^ password, bool store)
{
    return _dialog.postLogin(FromPlatformString(username), FromPlatformString(password), store);
}

bool libVLCX::Dialog::postAction(int actionIndex)
{
    return _dialog.postAction(actionIndex);
}

bool libVLCX::Dialog::dismiss()
{
    return _dialog.dismiss();
}