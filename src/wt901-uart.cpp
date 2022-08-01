#include "witmotion/wt901-uart.h"

namespace witmotion
{
namespace wt901
{

using namespace Qt;

const std::set<witmotion_packet_id> QWitmotionWT901Sensor::registered_types =
{
    pidAcceleration,
    pidAngularVelocity,
    pidAngles,
    pidMagnetometer
};

void QWitmotionWT901Sensor::UnlockConfiguration()
{
    witmotion_config_packet config_packet;
    config_packet.header_byte = WITMOTION_CONFIG_HEADER;
    config_packet.key_byte = WITMOTION_CONFIG_KEY;
    config_packet.address_byte = ridUnlockConfiguration;
    config_packet.setting.raw[0] = 0x88;
    config_packet.setting.raw[1] = 0xB5;
    ttyout << "Configuration ROM: lock removal started" << ENDL;
    emit SendConfig(config_packet);
    sleep(1);
}

const std::set<witmotion_packet_id> *QWitmotionWT901Sensor::RegisteredPacketTypes()
{
    return &registered_types;
}

void QWitmotionWT901Sensor::Start()
{
    ttyout << "Running reader thread" << ENDL;
    emit RunReader();
}

void QWitmotionWT901Sensor::Calibrate()
{
    witmotion_config_packet config_packet;
    config_packet.header_byte = WITMOTION_CONFIG_HEADER;
    config_packet.key_byte = WITMOTION_CONFIG_KEY;
    config_packet.address_byte = ridCalibrate;
    config_packet.setting.raw[0] = 0x01;
    config_packet.setting.raw[1] = 0x00;
    ttyout << "Entering spatial calibration, please hold the sensor in fixed position for 5 seconds" << ENDL;
    emit SendConfig(config_packet);
    sleep(5);
    config_packet.setting.raw[0] = 0x00;
    ttyout << "Exiting spatial calibration mode" << ENDL;
    emit SendConfig(config_packet);
    sleep(1);
}

void QWitmotionWT901Sensor::CalibrateMagnetometer()
{
    witmotion_config_packet config_packet;
    config_packet.header_byte = WITMOTION_CONFIG_HEADER;
    config_packet.key_byte = WITMOTION_CONFIG_KEY;
    config_packet.address_byte = ridCalibrate;
    config_packet.setting.raw[0] = 0x02;
    config_packet.setting.raw[1] = 0x00;
    ttyout << "Entering magnetic calibration, please hold the sensor in fixed position for 5 seconds" << ENDL;
    emit SendConfig(config_packet);
    sleep(5);
    config_packet.setting.raw[0] = 0x00;
    ttyout << "Exiting magnetic calibration mode" << ENDL;
    emit SendConfig(config_packet);
    sleep(1);
}

void QWitmotionWT901Sensor::SetBaudRate(const QSerialPort::BaudRate &rate)
{
    witmotion_config_packet config_packet;
    config_packet.header_byte = WITMOTION_CONFIG_HEADER;
    config_packet.key_byte = WITMOTION_CONFIG_KEY;
    config_packet.address_byte = ridPortBaudRate;
    port_rate = rate;
    config_packet.setting.raw[0] = witmotion_baud_rate(port_rate);
    config_packet.setting.raw[1] = 0x00;
    emit SendConfig(config_packet);
    sleep(1);
}

void QWitmotionWT901Sensor::SetPollingRate(const int32_t hz)
{
    witmotion_config_packet config_packet;
    config_packet.header_byte = WITMOTION_CONFIG_HEADER;
    config_packet.key_byte = WITMOTION_CONFIG_KEY;
    config_packet.address_byte = ridOutputFrequency;
    config_packet.setting.raw[0] = witmotion_output_frequency(hz);
    config_packet.setting.raw[1] = 0x00;
    emit SendConfig(config_packet);
    sleep(1);
}

void QWitmotionWT901Sensor::ConfirmConfiguration()
{
    witmotion_config_packet config_packet;
    config_packet.header_byte = WITMOTION_CONFIG_HEADER;
    config_packet.key_byte = WITMOTION_CONFIG_KEY;
    config_packet.address_byte = ridSaveSettings;
    config_packet.setting.raw[0] = 0x00;
    config_packet.setting.raw[1] = 0x00;
    emit SendConfig(config_packet);
    sleep(1);
}

void QWitmotionWT901Sensor::SetOrientation(const bool vertical)
{
    witmotion_config_packet config_packet;
    config_packet.header_byte = WITMOTION_CONFIG_HEADER;
    config_packet.key_byte = WITMOTION_CONFIG_KEY;
    config_packet.address_byte = ridInstallationDirection;
    config_packet.setting.raw[0] = vertical ? 0x01 : 0x00;
    config_packet.setting.raw[1] = 0x00;
    emit SendConfig(config_packet);
    sleep(1);
}

QWitmotionWT901Sensor::QWitmotionWT901Sensor(const QString device,
                                             const QSerialPort::BaudRate rate,
                                             const uint32_t polling_period):
    QAbstractWitmotionSensorController(device, rate)
{
    ttyout << "Creating multithreaded interface for Witmotion WT901 IMU sensor connected to "
           << port_name
           << " at "
           << static_cast<int32_t>(port_rate)
           << " baud"
           << ENDL;
    reader->SetSensorPollInterval(polling_period);
}

}
}
