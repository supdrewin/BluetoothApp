#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtBluetooth>

using QBtAddress = QBluetoothAddress;
using QBtDeviceDiscoveryAgent = QBluetoothDeviceDiscoveryAgent;
using QBtDeviceInfo = QBluetoothDeviceInfo;
using QBtLocalDevice = QBluetoothLocalDevice;

class Bluetooth : public QWidget {

    Q_OBJECT

public:
    using QBtLocalDevicePtr = QPointer<QBtLocalDevice>;

    explicit Bluetooth(QWidget* = nullptr);
    ~Bluetooth();

private:
    QBtDeviceDiscoveryAgent discovery_agent;
    QBtLocalDevicePtr current_adapter;

    QVBoxLayout global_layout;

    QHBoxLayout adapter_options;

    QComboBox local_devices;
    QCheckBox poweron_button;
    QCheckBox discoverable_button;
    QPushButton search_button;

    QHBoxLayout device_options;

    QComboBox remote_devices;
    QPushButton connect_button;
    QPushButton disconnect_button;
};
