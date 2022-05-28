/// Bluetooth.hh - a part of BluetoothApp project
/// Copyright (C) 2022  supdrewin
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
