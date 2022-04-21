#include "witmotion/wt31n-uart.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <QtCore>
#include <QSerialPort>

using namespace witmotion::wt31n;

void handle_shutdown(int s)
{
    std::cout << std::endl;
    QCoreApplication::exit(0);
}

int main(int argc, char** args)
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = handle_shutdown;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    QCoreApplication app(argc, args);
    QCommandLineParser parser;
    parser.setApplicationDescription("WITMOTION WT31N EXAMPLE DATA MONITOR");
    parser.addHelpOption();
    QCommandLineOption BaudRateOption(QStringList() << "b" << "baudrate",
                                      "Baudrate to set up the port",
                                      "QSerialPort::BaudRate set of values",
                                      "9600");
    QCommandLineOption DeviceNameOption(QStringList() << "d" << "device",
                                        "Port serial device name, without \'/dev\'",
                                        "ttyUSB0",
                                        "ttyUSB0");
    parser.addOption(BaudRateOption);
    parser.addOption(DeviceNameOption);
    parser.process(app);

    QWitmotionWT31NSensor sensor(parser.value(DeviceNameOption),
                                 static_cast<QSerialPort::BaudRate>(parser.value(BaudRateOption).toInt()));
    sensor.setParent(dynamic_cast<QObject*>(&app));
    sensor.SetValidation(true); // Instructs the library to accept only the valid packets from sensor, to throw an error signal otherwise
    QObject::connect(&sensor, &QWitmotionWT31NSensor::ErrorOccurred, [](const QString& description){
        std::cout << "ERROR: " << description.toStdString() << std::endl;
        QCoreApplication::exit(1);
    }); // Connection of the error signal to the MOC compatible inlay lambda
    float ax = 0.0, ay = 0.0, az = 0.0, ex = 0.0, ey = 0.0, ez = 0.0;
    QObject::connect(&sensor, &QWitmotionWT31NSensor::AcquiredAccelerations, [&ax, &ay, &az](float& x, float& y, float& z, float& t)
    {
        ax = x;
        ay = y;
        az = z;
    }); // Connection of the acceleration acquire signal to the MOC compatible mutable lambda
    QObject::connect(&sensor, &QWitmotionWT31NSensor::AcquiredAngles, [&ax, &ay, &az, &ex, &ey, &ez](float& x, float& y, float& z, float& t)
    {
        ex = x;
        ey = y;
        ez = z;
        std::cout << "Acquired:" << std::endl
                  << "X: acceleration " << ax << " R angle " << ex * witmotion::DEG2RAD << std::endl
                  << "Y: acceleration " << ay << " P angle " << ey * witmotion::DEG2RAD << std::endl
                  << "Z: acceleration " << az << " Y angle " << ez * witmotion::DEG2RAD<< std::endl;
    }); // Connection of the Euler angle acquire signal to the MOC compatible mutable lambda
    sensor.Start();
    return app.exec();
}
