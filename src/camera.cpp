#include "camera.h"

Camera::Camera(void) : remoteServiceBLEUUID{"8000FF00-FF00-FFFF-FFFF-FFFFFFFFFFFF"},
                       remoteService{remoteServiceBLEUUID},
                       remoteCommand{0xFF01},
                       remoteNotify{0xFF02}
{

    Bluefruit.begin(0, 1);

    Bluefruit.Central.setConnectCallback(connect_callback);
    Bluefruit.Central.setDisconnectCallback(disconnect_callback);
    Bluefruit.Security.setSecuredCallback(connection_secured_callback);
}