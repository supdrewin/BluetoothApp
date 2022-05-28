#include <QDialog>
#include <QLabel>
#include <QString>

#include "Bluetooth.hh"

using QBtDeviceDiscoveryError = QBtDeviceDiscoveryAgent::Error;

using QBtError = QBtLocalDevice::Error;
using QBtHostMode = QBtLocalDevice::HostMode;
using QBtPairing = QBtLocalDevice::Pairing;

Bluetooth::Bluetooth(QWidget* parent)
    : QWidget { parent }
    , discovery_agent()
    , current_adapter()
    , global_layout()
    , adapter_options()
    , local_devices()
    , poweron_button { "Power on" }
    , discoverable_button { "Discoverable" }
    , search_button { "Search" }
    , device_options()
    , remote_devices()
    , connect_button { "Connect" }
    , disconnect_button { "Disconnect" }
{
    auto error_dialog = [](QString const& message) {
        QDialog error_dialog;

        QVBoxLayout layout;

        QLabel label { message };
        QPushButton ok_button { "OK" };

        connect(
            &ok_button, &QPushButton::clicked,
            &error_dialog, &QDialog::close);

        layout.addWidget(&label);
        layout.addWidget(&ok_button);

        error_dialog.setWindowTitle("Error Occurred");
        error_dialog.setLayout(&layout);

        error_dialog.exec();
    };

    {
        auto devices = QBtLocalDevice::allDevices();
        auto tooltip = "Select bluebooth adapter";

        if (devices.isEmpty()) {
            error_dialog("No bluetooth adapters found!");
        }

        for (auto const& device : devices) {
            auto addr = device.address().toString(),
                 name = device.name();

            if (name.isEmpty()) {
                name = addr;
            }

            local_devices.addItem(name, addr);
        }

        local_devices.setToolTip(tooltip);
    }

    {
        auto tooltip = "Power on this adapter";
        poweron_button.setToolTip(tooltip);
    }

    auto clear_remote_devices = [this]() {
        remote_devices.clear();

        connect_button.setDisabled(true);
        disconnect_button.setDisabled(true);
    };

    auto poweron = [this]() {
        QBtLocalDevice adapter {
            local_devices.currentData().value<QBtAddress>()
        };
        adapter.powerOn();
    };

    auto poweroff = [clear_remote_devices, this]() {
        QBtLocalDevice adapter {
            local_devices.currentData().value<QBtAddress>()
        };

        adapter.setHostMode(QBtHostMode::HostPoweredOff);
        clear_remote_devices();
    };

    connect(
        &poweron_button, &QCheckBox::clicked,
        this, [poweron, poweroff](bool checked) {
            checked ? poweron() : poweroff();
        });

    connect(
        &poweron_button, &QCheckBox::stateChanged,
        this, [this](int state) {
            discoverable_button.setEnabled(state);
            search_button.setEnabled(state);
        });

    connect(
        &search_button, &QPushButton::clicked,
        this, [clear_remote_devices, this]() {
            clear_remote_devices();
            discovery_agent.start();

            if (discovery_agent.isActive()) {
                search_button.setDisabled(true);
            }
        });

    connect(
        &discovery_agent, &QBtDeviceDiscoveryAgent::finished,
        this, [this]() { search_button.setEnabled(true); });

    {
        auto tooltip = "Make this adapter discoverable";

        discoverable_button.setToolTip(tooltip);
        discoverable_button.setDisabled(true);
    }

    {
        auto tooltip = "Search bluebooth devices";

        search_button.setToolTip(tooltip);
        search_button.setDisabled(true);
    }

    auto update_poweron_button = [this](QBtHostMode mode) {
        auto state { mode != QBtHostMode::HostPoweredOff };
        poweron_button.setChecked(state);
    };

    auto update_discoverable_button = [this](QBtHostMode mode) {
        auto state { mode == QBtHostMode::HostDiscoverable };
        discoverable_button.setChecked(state);
    };

    auto update_buttons = [update_poweron_button, update_discoverable_button](
                              QBtHostMode mode) {
        update_poweron_button(mode);
        update_discoverable_button(mode);
    };

    {
        QBtLocalDevice adapter {
            local_devices.currentData().value<QBtAddress>()
        };

        update_buttons(adapter.hostMode());
    }

    auto update_adapter = [error_dialog, update_buttons, this](int = 0) {
        current_adapter.clear();
        current_adapter = new QBtLocalDevice {
            local_devices.currentData().value<QBtAddress>()
        };

        connect(
            current_adapter, &QBtLocalDevice::hostModeStateChanged,
            current_adapter, update_buttons);

        connect(
            current_adapter, &QBtLocalDevice::deviceConnected,
            current_adapter, [this](QBtAddress const& addr) {
                if (addr.toString() == remote_devices.currentData()) {
                    connect_button.setVisible(false);

                    disconnect_button.setEnabled(true);
                    disconnect_button.setVisible(true);
                }
            });

        connect(
            current_adapter, &QBtLocalDevice::deviceDisconnected,
            current_adapter, [this](QBtAddress const& addr) {
                if (addr.toString() == remote_devices.currentData()) {
                    disconnect_button.setVisible(false);

                    connect_button.setEnabled(true);
                    connect_button.setVisible(true);
                }
            });

        connect(
            current_adapter, &QBtLocalDevice::errorOccurred,
            current_adapter, [error_dialog](QBtError error) {
                QString message;

                switch (error) {
                case QBtError::PairingError:
                    message = "Pairing Error!";
                    break;
                case QBtError::UnknownError:
                    message = "Unknown Error!";
                    break;
                case QBtError::NoError:
                    message = "No Error!";
                    break;
                }

                error_dialog(message);
            });
    };

    update_adapter();

    connect(
        &local_devices, &QComboBox::currentIndexChanged,
        this, update_adapter);

    connect(
        &discovery_agent, &QBtDeviceDiscoveryAgent::deviceDiscovered,
        this, [this](QBtDeviceInfo const& info) {
            auto addr = info.address().toString(),
                 name = info.name();

            if (name.isEmpty()) {
                name = addr;
            }

            if (poweron_button.isChecked()) {
                remote_devices.addItem(name, addr);
            }
        });

    connect(
        &discovery_agent, &QBtDeviceDiscoveryAgent::errorOccurred,
        this, [error_dialog, this](QBtDeviceDiscoveryError) {
            error_dialog(discovery_agent.errorString());
            search_button.setEnabled(true);
        });

    {
        auto tooltip = "Select bluetooth device";
        remote_devices.setToolTip(tooltip);
    }

    connect(
        &remote_devices, &QComboBox::currentIndexChanged,
        this, [this](int) {
            QBtLocalDevice adapter {
                local_devices.currentData().value<QBtAddress>()
            };
            QBtAddress addr {
                remote_devices.currentData().toString()
            };

            connect_button.setEnabled(true);
            disconnect_button.setEnabled(true);

            auto connected_devices = adapter.connectedDevices();
            auto connected = connected_devices.contains(addr);

            connect_button.setVisible(!connected);
            disconnect_button.setVisible(connected);
        });

    {
        auto tooltip = "Connect to this device";

        connect_button.setToolTip(tooltip);
        connect_button.setDisabled(true);
    }

    connect(
        &connect_button, &QPushButton::clicked,
        this, [this]() {
            QBtLocalDevice adapter {
                local_devices.currentData().value<QBtAddress>()
            };
            QBtAddress addr {
                remote_devices.currentData().toString()
            };

            adapter.requestPairing(addr, QBtPairing::Paired);
            connect_button.setDisabled(true);
        });

    {
        auto tooltip = "Disconnect to this device";

        disconnect_button.setToolTip(tooltip);
        disconnect_button.setDisabled(true);
    }

    connect(
        &disconnect_button, &QPushButton::clicked,
        this, [this]() {
            QBtLocalDevice adapter {
                local_devices.currentData().value<QBtAddress>()
            };
            QBtAddress addr {
                remote_devices.currentData().toString()
            };

            adapter.requestPairing(addr, QBtPairing::Unpaired);
            disconnect_button.setDisabled(true);
        });

    adapter_options.addWidget(&local_devices);
    adapter_options.addWidget(&poweron_button);
    adapter_options.addWidget(&discoverable_button);
    adapter_options.addWidget(&search_button);

    device_options.addWidget(&remote_devices);
    device_options.addWidget(&connect_button);
    device_options.addWidget(&disconnect_button);

    global_layout.addLayout(&adapter_options);
    global_layout.addLayout(&device_options);
    global_layout.addStretch();

    setLayout(&global_layout);
}

Bluetooth::~Bluetooth()
{
    current_adapter.clear();
}
